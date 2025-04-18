CXX = g++
CXXFLAGS = -std=c++20 -Wall -O3 -DCPPHTTPLIB_OPENSSL_SUPPORT -I/opt/homebrew/opt/openssl@3/include
CXXFLAGS += -Wno-deprecated-declarations
LDFLAGS = -lssl -lcrypto -lpthread -L/opt/homebrew/opt/openssl@3/lib

TARGET = src/main

.PHONY: all

all: $(TARGET)

$(TARGET): src/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

build: $(TARGET)

run: build
	./$(TARGET)

test: build
	./$(TARGET) --round wordtower-test-1 --turn 5

clean:
	rm -f $(TARGET) $(OBJS)