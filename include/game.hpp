#ifndef GAME_HPP
#define GAME_HPP

#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "cells.hpp"

class Game{
private:
	SDL_Window* m_window;
	int m_window_w;
	int m_window_h;

	SDL_Renderer* m_renderer;
	SDL_Color m_bg_col;
	SDL_Color m_fg_col;

	SDL_Event m_event;
	
	bool m_running;

	bool m_step;

	std::vector <std::vector<Cell>> cell;
	//std::vector <Cell> live_list;
public:
	bool getRunning();

	bool init(std::string cfg_file_path=".config");
	void handleEvents();
	void update();
	void render();

	~Game();
};

#endif
