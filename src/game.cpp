#include "game.hpp"
#include "config_parsing.hpp"

#include <fstream>
#include <string>

bool Game::init(std::string cfg_file_path){
	
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing SDL...\n");
	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
		SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL\n");
		return false;
	}

	ParseFile cfg(".config");
	m_window_w = std::stoi(cfg.getValue("window_resolution", 0));
	m_window_h = std::stoi(cfg.getValue("window_resolution", 1));

	m_bg_col = {	(Uint8)std::stoi(cfg.getValue("background_color", 0)),
			(Uint8)std::stoi(cfg.getValue("background_color", 1)),
			(Uint8)std::stoi(cfg.getValue("background_color", 2)),
			255
		   };

	m_fg_col = {	(Uint8)std::stoi(cfg.getValue("foreground_color", 0)),
			(Uint8)std::stoi(cfg.getValue("foreground_color", 1)),
			(Uint8)std::stoi(cfg.getValue("foreground_color", 2)),
			255
		   };

	//initializing video --------------------------------------------------------------------------------------------------------------
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing video\n");

	m_window = SDL_CreateWindow(cfg.getValue("window_title").c_str(), m_window_w, m_window_h, 0);
	if(m_window == NULL){
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Could not create window\n");
		return false;
	}
	
	m_renderer = SDL_CreateRenderer(m_window, NULL);
	if(m_renderer == NULL){
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Could not create renderer\n");
		return false;
	}

	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Video initialized\n");
	//--------------------------------------------------------------------------------------------------------------------------------
	
	//Initializing Cells ------------------------------------------------------------------------------------------------------------
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing cells\n");

	//Reszing the vector:
	float cell_size = std::stof(cfg.getValue("cell_size"));
	cell.resize(m_window_h/cell_size);
	for(int r=0; r<cell.size(); r++){
		cell[r].resize(m_window_w/cell_size);
	}
	SDL_Log("Column size: %i, Row size: %i", cell.size(), cell[0].size());

	//Now initializing cells:
	for(int r=0; r<cell.size(); r++){
		for(int c=0; c<cell[r].size(); c++){
			//SDL_Log("Initializing cell %i, %i", c,  r);
			cell[r][c].init(cell_size, (float)c, (float)r);
		}
	}

	//if all went well it will set running to true
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing finished, entering main game loop");
	m_running = true;
	return true;
}

void Game::handleEvents(){
	SDL_PollEvent(&m_event);
	if(m_event.type==SDL_EVENT_QUIT) m_running = false;

	//all code below is for what happens if you press (any) mouse button -----------
	if(m_event.button.down==true){ 
		int r = (int) m_event.button.y / (int)cell[0][0].getSize();
		int c = (int) m_event.button.x / (int)cell[0][0].getSize();
		
		//sets cell to alive or dead
		cell[r][c].setAlive(!cell[r][c].getAlive());
	}
	//---------------------------------------------------------------------

	//if you press space it should step the simulation
	if(m_event.key.type == SDL_EVENT_KEY_DOWN){
		switch(m_event.key.key){
			case SDLK_SPACE:
				m_step = true;
				break;
		}
	}
}

void Game::update(){

	//error handling
	const char* error = SDL_GetError();
	if(*error){
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", error);
		SDL_ClearError();
	}
	
	//code for stepping the simulation
	if(m_step==true){
		//first it calculates the amount of live neighbours for each cell --------------------
		for(int r=0; r<cell.size(); r++){
			for(int c=0; c<cell[r].size(); c++){
				cell[r][c].neighbour_count = 0;
				if(r-1 >= 0 && c-1 >= 0 && r+1 < cell.size() && c+1 < cell[r].size()){
					//left
					if(cell[r][c-1].getAlive())cell[r][c].neighbour_count ++;
					//right
					if(cell[r][c+1].getAlive())cell[r][c].neighbour_count ++;
					//up
					if(cell[r-1][c].getAlive())cell[r][c].neighbour_count ++;
					//down
					if(cell[r+1][c].getAlive())cell[r][c].neighbour_count ++;
					//left-up
					if(cell[r-1][c-1].getAlive())cell[r][c].neighbour_count ++;
					//left-down
					if(cell[r+1][c-1].getAlive())cell[r][c].neighbour_count ++;
					//right-up
					if(cell[r-1][c+1].getAlive())cell[r][c].neighbour_count ++;
					//right-down
					if(cell[r+1][c+1].getAlive())cell[r][c].neighbour_count ++;
				}
				//if(cell[r][c].neighbour_count > 0) SDL_Log("Cell[%i][%i] has %i neighbours", r, c, cell[r][c].neighbour_count);
			}
		}
		//then apply the game rules...
		for(int r=0; r<cell.size(); r++){
			for(int c=0; c<cell[r].size(); c++){
				//SDL_Log("Applying rules cell[%i][%i]", r, c);
				if(cell[r][c].neighbour_count < 2) cell[r][c].setAlive(false); // underpopulation
				if(cell[r][c].neighbour_count == 3) cell[r][c].setAlive(true); // birth
				if(cell[r][c].neighbour_count > 3) cell[r][c].setAlive(false); // overpopulation
			}
		}
	}
	m_step = false;
}

void Game::render(){
	//set colour to bg_col and clear the renderer
	SDL_SetRenderDrawColor(m_renderer, m_bg_col.r, m_bg_col.g, m_bg_col.b, m_bg_col.a);
	SDL_RenderClear(m_renderer);

		//set colour to fg_col and render the cells
	SDL_SetRenderDrawColor(m_renderer, m_fg_col.r, m_fg_col.g, m_fg_col.b, m_fg_col.a);
	for(int r=0; r<cell.size(); r++){
		for(int c=0; c<cell[r].size(); c++){
			cell[r][c].render(m_renderer);
		}
	}

	//show what was rendered
	SDL_RenderPresent(m_renderer);
}

Game::~Game(){
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
}

bool Game::getRunning(){
	return m_running;
}

