#pragma once
#include "Piece.h"

class Bishop : public virtual Piece
{
	
public:
	Bishop(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Bishop();
	void a();
};