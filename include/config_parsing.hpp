#ifndef CONFIG_PARSING_HPP
#define CONFIG_PARSING_HPP

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

class ParseFile{
private:
	bool error;
	std::ifstream file;
	std::unordered_map<std::string, std::vector<std::string>> param_map;

	void parse();
public:
	ParseFile(std::string file_path);
	~ParseFile();

	bool getError();
	std::string getValue(std::string name, int i=0);
};

#endif
