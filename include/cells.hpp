#ifndef CELLS_HPP
#define CELLS_HPP

#include <SDL3/SDL.h>
#include <string>

class Cell{
private:
	float m_size;
	SDL_FRect m_rect;
	bool m_alive;

public:
	int neighbour_count;

	void init(float size, float c, float r);
	void render(SDL_Renderer* renderer);

	float getSize();
	float getX();
	float getY();
	bool getAlive();

	void setAlive(bool val);
};

#endif
