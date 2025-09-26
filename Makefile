# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Bibliotecas
LIBS_CLIENT = -lraylib -lm -lpthread -ldl -lX11
LIBS_SERVER = -lm -lpthread

# Diretórios
OBJ_DIR = obj
BIN_DIR = bin

# Includes
INCLUDE_COMMON = -Icommon/include
INCLUDE_CLIENT = -Iclient/include -Icommon/include
INCLUDE_SERVER = -Iserver/include -Icommon/include

# Função para pegar todos os .cpp de um diretório
define get_cpp
$(shell find $(1) -type f -name "*.cpp")
endef

# Sources
CLIENT_SRCS = $(call get_cpp,client/src) $(call get_cpp,common/src)
SERVER_SRCS = $(call get_cpp,server/src) $(call get_cpp,common/src)

# Objetos
CLIENT_OBJS = $(CLIENT_SRCS:%.cpp=$(OBJ_DIR)/%.o)
SERVER_OBJS = $(SERVER_SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Executáveis
CLIENT_EXEC = $(BIN_DIR)/client/Brawlblaze
SERVER_EXEC = $(BIN_DIR)/server/Brawlblaze

# Alvo padrão
all: client server

# Compila apenas o client
client: $(CLIENT_EXEC)

# Compila apenas o server
server: $(SERVER_EXEC)

# Linka client
$(CLIENT_EXEC): $(CLIENT_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CLIENT_OBJS) -o $@ $(LIBS_CLIENT)

# Linka server
$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(SERVER_OBJS) -o $@ $(LIBS_SERVER)

# Compilação de objetos client
$(OBJ_DIR)/client/src/%.o: client/src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE_CLIENT) -c $< -o $@

# Compilação de objetos server
$(OBJ_DIR)/server/src/%.o: server/src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE_SERVER) -c $< -o $@

# Compilação de objetos comuns
$(OBJ_DIR)/common/src%.o: common/src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE_CLIENT) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)