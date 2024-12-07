CC = g++
CFLAGS = -std=c++1z -O2 -Wall -Wextra -Werror -pedantic -Wno-unused-result -Wconversion -Wvla
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = sokoban

all: $(TARGET)

$(TARGET): main.cpp sokoban.hpp
	$(CC) main.cpp -o $@ $(SFML_FLAGS)

run: $(TARGET)
ifeq ($(TEST),)
	./$(TARGET) < testcases/in
else
	./$(TARGET) $(TEST) < testcases/big_$(TEST).in
endif

clean:
	rm -f $(TARGET)

.PHONY: all clean run
