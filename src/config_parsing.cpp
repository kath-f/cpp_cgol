#include "config_parsing.hpp"
#include <SDL3/SDL.h>
#include <algorithm>

//opens the file, if it can't open it sends an error message and sets the error flag
ParseFile::ParseFile(std::string file_path){
	file.open(file_path.c_str());
	if(!file.is_open()){
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not open file %s", file_path);
		error = true;
	}
	parse();
}

//just closes the file opened in the constructor
ParseFile::~ParseFile(){
	file.close();
}

//returns error status
bool ParseFile::getError(){
	return error;
}

//returns value associated with name given in parameter
//by default it returns the first item on the value vector,
//which is useful for single value params
std::string ParseFile::getValue(std::string name, int i){
	if(param_map.contains(name)){
		return param_map[name][i];
	}
	else{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Name [%s] not in map", name.c_str());
		error = true;
		return "WARNING: name not found";
	}
}

//parses the file opened saving names and values on the param_map
void ParseFile::parse(){
	std::string line;
	while(std::getline(file, line)){
		//basically tells to ignore the line if it starts with a # or a non printable character
		if(line[0]!='#' and (line[0]>=0x21 and line[0]<=0x7e)){

			//gets the parameter name as the part of the line before the = sign
			std::string name = line.substr(0, line.find('='));
			std::erase(name, ' '); //erases spaces from the name 

			//gets the value as the part of the string after the = sign
			std::string value = line.substr(line.find('=')+1, line.size());
			//ITS IMPORTANT TO NOT GET RID OF SPACES IF THE VALUE IS SUPPOSED TO BE A STRING
			if(value.contains(0x22)){ //if the value contains a quotation mark
				//sets the value as the text between the first and last quotation marks
				value = value.substr(value.find(0x22)+1, value.find_last_of(0x22)-2);
			}
			//if it doesnt have quotation marks - it's not a string, we should erase spaces
			else{
				std::erase(value, ' ');
			}
			
			//if the parameter has multiple values...
			if(value.contains(',')){
				std::vector <std::string> val_vec;
				//for 2 values
				if(std::ranges::count(value, ',')==1){
					std::string val;
					
					val = value.substr(0, value.find(','));
					std::erase(val, ',');
					val_vec.push_back(val);
					
					val = value.substr(value.find(','), value.size());
					std::erase(val, ',');
					val_vec.push_back(val);
				}
				//for 3 values
				else if(std::ranges::count(value, ',')==2){
					std::string val;

					val = value.substr(0, value.find(','));
					std::erase(val, ',');
					val_vec.push_back(val);

					//DISCLAIMER:: idk why it needs that -2 after the value.rfind, 
					//but it kept getting the wrong position otherwise
					val = value.substr(value.find(','), value.rfind(',') -2);
					std::erase(val, ',');
					val_vec.push_back(val);

					val = value.substr(value.rfind(','), value.size());
					std::erase(val, ',');
					val_vec.push_back(val);
				}
				//if there's more than 3 values, return an error please
				else{
					SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Parameter [%s] has more than 3 values associated", name.c_str());
					error = true;
				}
				param_map[name] = val_vec;
			}
			//if it has only one value
			else{
				param_map[name] = {value};
			}	
		}
	}
}
