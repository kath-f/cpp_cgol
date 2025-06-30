#include "game.hpp"
#include <fstream>
#include <string>
#include <algorithm>

bool Game::init(std::string cfg_file_path){
	
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing SDL...\n");
	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
		SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL\n");
		return false;
	}

	std::string window_title;
	int window_flags;
	
	std::ifstream cfg_file(cfg_file_path.c_str());
	if(!cfg_file.is_open()){
		SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "Could not open .config file in path: %s \n, please make sure it exists\n", cfg_file_path);
		return false;
	}
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, ".config file opened, parsing...");

	//PARSING the .config file -------------------------------------------------------------------------------
	std::string param;
	while(std::getline(cfg_file, param)){

		//ignores lines started with # (comments)
		//and that start with non printable characters
		if(param[0] != '#' and (param[0]>=0x21 and param[0]<=0x7e)){
			if(param.starts_with("window_title")){
				std::string name = "window_title";
				std::string value = param.substr(name.size()+1, param.size());
				//takes a substring from the parameter, 
				//starting from the parameter's name,
				//plus one to account for the space
				window_title = value;
			}

			//the next two do the same taking a substring thing,
			//but converts them to integers
			else if(param.starts_with("window_width")){
				std::string name = "window_width";
				std::string value = param.substr(name.size()+1, param.size());
				m_window_w = stoi(value);
			}
			else if(param.starts_with("window_height")){
				std::string name = "window_height";
				std::string value = param.substr(name.size()+1, param.size());
				m_window_h = stoi(value);
			}

			//this one sets the window as fullscreen or not based on the parameter in the config file
			else if(param.starts_with("window_fullscreen")){
				std::string name = "window_fullscreen";
				std::string value = param.substr(name.size()+1, param.size());
				if(value=="true"){
					window_flags = SDL_WINDOW_FULLSCREEN;
				}
				else if(value=="false"){
					window_flags = 0;
				}
				else{
					SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Invalid value for parameter window_fullscreen, ( %s )\n", value.c_str());
					window_flags = 0;
				}
			}

			//getting the params for the background and foreground colours
			else if(param.starts_with("background_color")){
				std::string name = "background_color";
				//here i define an array of values,
				//0 will be the full string of the values
				//1 will be the red value
				//2 will be the green value
				//3 will be the blue value
				std::string value[4];
				value[0] = param.substr(name.size()+1, param.size());
				value[1] = value[0].substr(0, 3);
				value[2] = value[0].substr(4, 3);
				value[3] = value[0].substr(8, 3);
				m_bg_col = {(Uint8)stoi(value[1]), (Uint8)stoi(value[2]), (Uint8)stoi(value[3]), 255};
			}
			else if(param.starts_with("foreground_color")){
				std::string name = "foreground_color";
				//here i define an array of values,
				//0 will be the full string of the values
				//1 will be the red value
				//2 will be the green value
				//3 will be the blue value
				std::string value[4];
				value[0] = param.substr(name.size()+1, param.size());
				value[1] = value[0].substr(0, 3);
				value[2] = value[0].substr(4, 3);
				value[3] = value[0].substr(8, 3);
				m_fg_col = {(Uint8)stoi(value[1]), (Uint8)stoi(value[2]), (Uint8)stoi(value[3]), 255};
			}


			//if the parameter doesn't match any of the parameters above, it's invalid
			else{
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Ivalid parameter ( %s )\n", param.c_str());
			}

		}
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	cfg_file.close();

	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, ".config parsed, params: \n");
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
		"\t window_title: %s, window_w: %i, window_h %i, window_flags: %i \n \t background_color: %i %i %i, foreground_color: %i %i %i\n", 
		 window_title.c_str(), m_window_w, m_window_h, window_flags, 
		 m_bg_col.r, m_bg_col.g, m_bg_col.b, m_fg_col.r, m_fg_col.g, m_fg_col.b);
	
	//initializing video --------------------------------------------------------------------------------------------------------------
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Initializing video\n");

	m_window = SDL_CreateWindow(window_title.c_str(), m_window_w, m_window_h, window_flags);
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
	float cell_size = 15.0f;
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
	while (SDL_PollEvent(&m_event)){
		if(m_event.type==SDL_QUIT) m_running = false;

		//all code below is for what happens if you press (any) mouse button -----------
		if(m_event.type==SDL_MOUSEBUTTONDOWN){
			int r = (int) m_event.button.y / (int)cell[0][0].getSize();
			int c = (int) m_event.button.x / (int)cell[0][0].getSize();
			SDL_Log("Cell clicked: %i, %i", r, c);
		
			//sets cell to alive or dead
			cell[r][c].setAlive(!cell[r][c].getAlive());
			/*
			//if cell is alive add it to live_list
			if(cell[r][c].getAlive()){
				//checks if cell is already on the list
				bool found = false;
				for(int i=0; i<live_list.size(); i++){
					if(live_list[i]==cell[r][c]){
						found = true;
						break;
					}
				}
				//if it's not on the list, add it
				if(!found){
					live_list.push_back(cell[r][c]);	
				}
			}
			//if cell isn't alive, remove it from the live list
			else{
				//checks if cell is on the list
				for(int i=0; i<live_list.size(); i++){
					//if it is, erase it
					if(live_list[i]==cell[r][c]){
						live_list.erase(live_list.begin() + i);
						break;
					}
				}
			}
			*/
		}
		//if you press enter it should step the simulation
		if(m_event.type == SDL_KEYDOWN){
			switch(m_event.key.keysym.sym){
				case SDLK_SPACE:
					m_step = true;
					break;
			}
		}
	}
	//---------------------------------------------------------------------

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

