CXX = g++
CXXFLAGS = -std=c++20 -Wall -O3 -DCPPHTTPLIB_OPENSSL_SUPPORT -I/opt/homebrew/opt/openssl@3/include
LDFLAGS = -lssl -lcrypto -lpthread -L/opt/homebrew/opt/openssl@3/lib

TARGET = src/main

.PHONY: all

all: $(TARGET)

$(TARGET): src/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)

