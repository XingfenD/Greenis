# ./Makefile

CC = clang++
CFLAGS = -Wall -g $(PUB_INC) -Wextra -funroll-loops -march=native -std=c++11
LDFLAGS =

# include paths
PUB_INC = -I./inc
SERVER_INC = -I./inc/server
CLIENT_INC = -I./inc/client

# library paths
PUB_LIB = lib
SERVER_LIB = $(PUB_LIB)/server
CLIENT_LIB = $(PUB_LIB)/client

# build paths
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin


# source files
PUB_SRCS := $(wildcard $(PUB_LIB)/*.cpp)
SERVER_SRCS := $(wildcard $(SERVER_LIB)/*.cpp)
CLIENT_SRCS := $(wildcard $(CLIENT_LIB)/*.cpp)

# object files
PUB_OBJS := $(patsubst $(PUB_LIB)/%.cpp,$(BUILD_DIR)/%.o,$(PUB_SRCS))
SERVER_OBJS := $(patsubst $(SERVER_LIB)/%.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(CLIENT_LIB)/%.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRCS))

SERVER_EXEC = $(BIN_DIR)/server_greenis
CLIENT_EXEC = $(BIN_DIR)/client_greenis

all: $(SERVER_EXEC) $(CLIENT_EXEC)

server: $(SERVER_EXEC)
	$(SERVER_EXEC)

client: $(CLIENT_EXEC)
	$(CLIENT_EXEC)

remake: clean | all

clean:
	rm -rf build/*

.PHONY: clean

# link
$(SERVER_EXEC): $(PUB_OBJS) $(SERVER_OBJS) $(BUILD_DIR)/server.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_EXEC): $(PUB_OBJS) $(CLIENT_OBJS) $(BUILD_DIR)/client.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^


# compile main
$(BUILD_DIR)/server.o: ./server.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SERVER_INC) -c -o $@ $<

$(BUILD_DIR)/client.o: ./client.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CLIENT_INC) -c -o $@ $<

# compile library
$(BUILD_DIR)/%.o: $(PUB_LIB)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SERVER_LIB)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SERVER_INC) -c -o $@ $<

$(BUILD_DIR)/%.o: $(CLIENT_LIB)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CLIENT_INC) -c -o $@ $<

$(BIN_DIR): $(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

