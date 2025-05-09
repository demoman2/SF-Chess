#pragma once
#include "Piece.h"

class Bishop : public virtual Piece
{
	
public:
	Bishop(int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~Bishop();
	virtual std::shared_ptr<Piece> clone() const override;
};