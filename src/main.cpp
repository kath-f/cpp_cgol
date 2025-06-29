#include "game.hpp"

int main(int argc, char* argv[]){
	Game game;
	if(!game.init()) return -1;

	while(game.getRunning()){
		game.handleEvents();
		game.update();
		game.render();
	}

	return 0;
}
