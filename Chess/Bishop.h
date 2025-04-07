#pragma once
#include "Piece.h"

class Bishop : public virtual Piece
{
	
public:
	Bishop(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture);
	~Bishop();
	virtual std::shared_ptr<Piece> clone() const override;
};