#include "cells.hpp"

void Cell::init(float size, float c, float r){
	m_size = size;
	m_rect = {c*m_size, r*m_size, m_size, m_size};
	m_alive = false;

	m_id = std::to_string((int)r) + " " + std::to_string((int)c);
	//SDL_Log(m_id.c_str());

	//SDL_Log("size: %f, x: %f, y: %f, alive: %b\n", m_rect.w, m_rect.x, m_rect.y, m_alive);
}

void Cell::render(SDL_Renderer* renderer){
	if(m_alive){
		SDL_RenderFillRect(renderer, &m_rect);
	}
	else{
		SDL_RenderRect(renderer, &m_rect);
	}
}

float Cell::getSize(){return m_size;}

float Cell::getX(){return m_rect.x;}
float Cell::getY(){return m_rect.y;}

bool Cell::getAlive(){return m_alive;}
void Cell::setAlive(bool val){m_alive = val;}

/*
bool Cell::operator==(const Cell* cell){
	if(this->m_id == cell->m_id){
		return true;
	}
	return false;
}
*/
