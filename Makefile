

cgol: game.cpp.o cells.cpp.o main.cpp.o
	g++ -o $@ $^ -lSDL3

game.cpp.o: src/game.cpp include/game.hpp
	g++ -c -o $@ $< -I'include/' -std=c++20

cells.cpp.o: src/cells.cpp include/cells.hpp
	g++ -c -o $@ $< -I'include/' -std=c++20

main.cpp.o: src/main.cpp include/game.hpp
	g++ -c -o $@ $< -I'include/' -std=c++20

