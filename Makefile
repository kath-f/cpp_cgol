CC=g++
LIB_FLAGS=-lSDL3

cgol: game.cpp.o cells.cpp.o main.cpp.o
	$(CC) -o $@ $^ $(LIB_FLAGS)

game.cpp.o: src/game.cpp include/game.hpp
	$(CC) -c -o $@ $< -I'include/' -std=c++20

cells.cpp.o: src/cells.cpp include/cells.hpp
	$(CC) -c -o $@ $< -I'include/' -std=c++20

main.cpp.o: src/main.cpp include/game.hpp
	$(CC) -c -o $@ $< -I'include/' -std=c++20

