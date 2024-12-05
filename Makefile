CC = clang++
CFLAGS = -std=c++1z -O2 -Wall -Wextra -Werror -pedantic -Wno-unused-result -Wconversion -Wvla
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET_NO_GRAPHICS = sokoban_nog
TARGET_GRAPHICS = sokoban_graph

all: $(TARGET_NO_GRAPHICS) $(TARGET_GRAPHICS)

$(TARGET_NO_GRAPHICS): main.cpp sokoban.hpp
	$(CC) $(CFLAGS) main.cpp -o $@

$(TARGET_GRAPHICS): show.cpp sokoban.hpp
	$(CC) $(CFLAGS) $(SFML_FLAGS) show.cpp -o $@


run_nog: $(TARGET_NO_GRAPHICS)
	./$(TARGET_NO_GRAPHICS) < testcases/in

run_graph: $(TARGET_GRAPHICS)
	./$(TARGET_GRAPHICS) < testcases/in

clean:
	rm -f $(TARGET_NO_GRAPHICS) $(TARGET_GRAPHICS)

.PHONY: all clean run_nog run_graph