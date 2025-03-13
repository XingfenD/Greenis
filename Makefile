# ./Makefile

CC = g++
CFLAGS = -Wall -Og $(INCLUDES) -Wextra -funroll-loops -march=native
LDFLAGS =
INCLUDES = -I./inc

LIB_DIR = lib
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)

SOURCES := $(wildcard $(LIB_DIR)/*.cpp)
OBJECTS := $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
SERVER_EXEC = $(BIN_DIR)/greenis_server.out
CLIENT_EXEC = $(BIN_DIR)/greenis_client.out

all: $(SERVER_EXEC) $(CLIENT_EXEC)

server: $(SERVER_EXEC)
	$(SERVER_EXEC)

client: $(CLIENT_EXEC)
	$(CLIENT_EXEC)

clean:
	rm -rf build

.PHONY: clean

$(SERVER_EXEC): $(OBJECTS) $(BUILD_DIR)/server.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_EXEC): $(OBJECTS) $(BUILD_DIR)/client.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/server.o: ./server.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/client.o: ./client.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir $@