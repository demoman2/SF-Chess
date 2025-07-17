#pragma once
#include "Piece.h"

class King : public virtual Piece
{
public:
	bool inCheck;
	bool canNeverCastleK, canNeverCastleQ;
	int Krook, Qrook;

	King(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	King(const King& other, bool copySprite = false, bool copySquares = true);
	~King();

	// Virtual Functions
	std::shared_ptr<Piece> clone(bool copySprite = false, bool copySquares = true) const override;
	void addAttacker() override;
	void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960) override;
	bool validatePosition(const pieceVector& pieceList) override;
	void updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting) override;

	std::vector<Chess::Square>& getCastleSquares() { return castlePositions; }
	std::vector<Chess::Square>& getCaptureCastleSquares() { return captureCastlePositions; }
	
private:
	bool canCastleK, canCastleQ;
	std::vector<Chess::Square> castlePositions, captureCastlePositions;

	void calculateCastlingRights(pieceVector pieceList, bool checksEnabled);
	void addCastleSquare(sf::Vector2i square);
	void addCastleCaptureSquare(sf::Vector2i square, bool chess960, bool kingside);
	bool isValidMove(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled) override;
};

