#include <iostream>

using std::string, std::cout;

int main(int argc, char** argv) {
    cout << "[Server] Server initialized\n";

    bool running = true;

    cout << "[Server] Press Enter to STOP\n";
    std::cin.get();
    running = false;

    // server.Stop();
    cout << "[Server] Shutdown.\n";
}