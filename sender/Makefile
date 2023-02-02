BUILD_DIR = build
CFLAGS = g++ -std=c++11 -pthread
SOCKET_DIR = ../socket

OBJECTS = \
	$(BUILD_DIR)/Sender.o	\
	$(BUILD_DIR)/Main.o		\
	$(BUILD_DIR)/Socket.o	\

all: $(BUILD_DIR) sender.out

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Socket.o: $(SOCKET_DIR)/socket.cpp $(SOCKET_DIR)/socket.hpp
	$(CFLAGS) -c $(SOCKET_DIR)/socket.cpp -o $(BUILD_DIR)/Socket.o

$(BUILD_DIR)/Main.o: main.cpp sender.hpp $(SOCKET_DIR)/socket.hpp
	$(CFLAGS) -c main.cpp -o $(BUILD_DIR)/Main.o

$(BUILD_DIR)/Sender.o: sender.cpp sender.hpp $(SOCKET_DIR)/socket.hpp
	$(CFLAGS) -c sender.cpp -o $(BUILD_DIR)/Sender.o

sender.out: $(OBJECTS)
	$(CFLAGS) $(OBJECTS) -o sender.out

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o
	rm -rf $(BUILD_DIR) *.out