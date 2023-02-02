CFLAGS = g++ -std=c++11
BUILD_DIR = build
OBJECTS = \
	$(BUILD_DIR)/Graph.o	\
	$(BUILD_DIR)/Main.o		\
	$(BUILD_DIR)/Logger.o

all: $(BUILD_DIR) graph.out

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(BUILD_DIR)/Graph.o: graph.cpp graph.hpp logger.hpp
	$(CFLAGS) -c graph.cpp -o $(BUILD_DIR)/Graph.o

$(BUILD_DIR)/Main.o: main.cpp graph.hpp logger.hpp
	$(CFLAGS) -c main.cpp -o $(BUILD_DIR)/Main.o

$(BUILD_DIR)/Logger.o: logger.cpp logger.hpp
	$(CFLAGS) -c logger.cpp -o $(BUILD_DIR)/Logger.o

graph.out:	$(OBJECTS)
	$(CFLAGS) $(OBJECTS) -o graph.out

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o
	rm -rf $(BUILD_DIR) *.out
