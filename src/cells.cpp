#include "cells.hpp"

void Cell::init(float size, float c, float r){
	m_size = size;
	m_rect = {c*m_size, r*m_size, m_size, m_size};
	m_alive = false;
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
