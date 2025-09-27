#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "network/Server.hpp"

#include "../../common/include/EntityManager.hpp"

using std::string, std::cout;

int main(int argc, char** argv) {
    cout << "[Server] Server starting...\n";

    Server server;
    if (!server.Start(8080)) return 1;

    EntityManager entityManager;

    bool running = true;

    std::thread inputThread([&]() {
        while (running) {
            auto packet = server.Receive();
            if (packet.type == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            try {
                switch (static_cast<ClientPacketType>(packet.type)) {
                    case ClientPacketType::Connect: {
                        auto payload = server.ParsePayload<ConnectPayload>(packet);
                        cout << "[Server] " << payload.nickname << " joined the game\n";
                        Entity newPlayer;
                        {
                            newPlayer = entityManager.CreateEntity();
                            entityManager.AddComponent(newPlayer.id, Type(EntityType::Player));
                            entityManager.AddComponent(newPlayer.id, Position(0.0f, 0.0f));
                        }

                        {
                            server.RegisterClient(newPlayer.id, packet.addr);
                        }

                        InitPayload initPayload{};
                        initPayload.entityID = newPlayer.id;
                        if (auto* position = entityManager.TryGetComponent<Position>(newPlayer.id)) {
                            initPayload.x = position->x;
                            initPayload.y = position->y;
                        }
                        for (int i = 0; i < 1000; i++) server.Send(ServerPacketType::Init, initPayload, packet.addr);
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[Server] Failed to parse packet: " << e.what() << "\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    cout << "[Server] Press Enter to STOP\n";
    std::cin.get();
    running = false;

    inputThread.join();

    server.Stop();
    cout << "[Server] Shutdown.\n";
}