#pragma once
#include "Piece.h"

class King : public virtual Piece
{
public:
	bool inCheck, inDoubleCheck;
	int Krook, Qrook;

	King(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	~King();

	// Virtual Functions
	std::shared_ptr<Piece> clone() const override;
	void addAttacker() override;
	void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960, bool hasDoubleCheck) override;
	bool validatePosition(const pieceVector& pieceList) override;
	void updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting) override;

	std::shared_ptr<std::vector<Chess::Square>> getCastleSquares() { return castlePositions; }
	std::shared_ptr<std::vector<Chess::Square>> getCaptureCastleSquares() { return captureCastlePositions; }
	
private:
	bool canCastleK, canCastleQ;
	bool canNeverCastleK, canNeverCastleQ;
	std::shared_ptr<std::vector<Chess::Square>> castlePositions, captureCastlePositions;

	void calculateCastlingRights(pieceVector pieceList, bool checksEnabled);
	void addCastleSquare(sf::Vector2i square);
	void addCastleCaptureSquare(sf::Vector2i square, bool chess960, bool kingside);
	bool isValidMove(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled) override;
};

