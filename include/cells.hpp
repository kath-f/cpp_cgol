#ifndef CELLS_HPP
#define CELLS_HPP

#include <SDL3/SDL.h>
#include <string>

class Cell{
private:
	float m_size;
	SDL_FRect m_rect;
	bool m_alive;

	std::string m_id;
public:
	int neighbour_count;

	void init(float size, float c, float r);
	void render(SDL_Renderer* renderer);

	float getSize();
	float getX();
	float getY();
	bool getAlive();
//	bool operator==(const Cell* cell);

	void setAlive(bool val);
};

#endif
