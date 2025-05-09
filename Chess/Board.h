#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "Piece.h"
#include "Bishop.h"
#include "King.h"
#include "Knight.h"
#include "Rook.h"
#include "Queen.h"
#include "Pawn.h"
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <map>

typedef std::vector<std::shared_ptr<Piece>> pieceVector;
typedef std::vector<std::reference_wrapper<sf::Texture>> textureVector;
typedef std::pair<std::array<std::array<int, 8>, 8>, bool> basicBitboard;
using namespace Chess;
namespace bp = boost::process::v1;

struct DropPiece {
	char id;
	int tex;
	int count;
	sf::Sprite sprite, textBG;
	sf::Text text;

	DropPiece(char id, int tex, textureVector& pieceTextures, textureVector& boardTextures, sf::Font& font, sf::Sprite& dropPieceBackground, float dropPieceSquareSize, float bgStart, size_t i)
		: id(id), tex(tex), count(0), sprite(pieceTextures.at(tex)), textBG(boardTextures.at(14)), text(font, std::to_string(count), 32)
	{
		sprite.setOrigin(sprite.getLocalBounds().getCenter());
		sprite.setScale({ dropPieceSquareSize / pieceTextures.at(tex).get().getSize().x, dropPieceSquareSize / pieceTextures.at(3).get().getSize().y });
		sprite.setPosition({ dropPieceBackground.getPosition().x, bgStart + dropPieceSquareSize * i });

		textBG.setOrigin(textBG.getLocalBounds().getCenter());
		textBG.setScale(sprite.getScale());
		textBG.setPosition(sprite.getPosition());

		text.setScale({ sprite.getScale().x * 3, sprite.getScale().y * 3 });
		text.setOrigin(text.getLocalBounds().getCenter());
		text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });

	}
	void update() {
		text.setString(std::to_string(count));
		if (count == 0) {
			sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 26));
			textBG.setColor(sf::Color(textBG.getColor().r, textBG.getColor().g, textBG.getColor().b, 0));
			text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, 0));
		}
		else {
			sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 255));
			textBG.setColor(sf::Color(textBG.getColor().r, textBG.getColor().g, textBG.getColor().b, 255));
			text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, 255));
		}
	}
	bool mouseSelecting(sf::Vector2i mousePos) const {
		return count != 0 && sprite.getGlobalBounds().contains((sf::Vector2f)mousePos);
	}
	void draw(sf::RenderWindow& window) const {
		window.draw(sprite);
		window.draw(textBG);
		window.draw(text);
	}
};

struct PromotePiece {

};

class Board
{

public:
	std::unique_ptr<textureVector> boardTextures;
	std::unique_ptr<sf::Font> textFont;
	sf::Color promotionSquareColor{ 255, 255, 255 }, promotionSquareSelectedColor{ 255, 30, 0 };
	Variant variant;
	pieceVector pieceList;
	textureVector pieceTextures;
	int boardSize, halfMoves, fullMoves, whiteChecks, blackChecks, wKRook, wQRook, bKRook, bQRook;
	float sizeMultiplier, boardScale, pieceScale, boardMultiplier, threshold, pieceSize;
	bool whiteToPlay, playerSideWhite, unpromoting, kingPromotionEnabled, endgamePosition, Chess960Enabled, StockfishEnabled, AI_Only,
		calculatingPos, hourGlass, eighthRankWhite, movePlayed, mouseSelecting,
		mouseClicked, pieceMoving, check, promoting, animationFinished,
		pieceSelectionLock, dropPieceLock, standardPosition, checksEnabled, castlingEnabled, dropsEnabled, animated;
	sf::Vector2f boardOffset, dropPieceSquareSize, dropPieceWidth, dropPieceHeight;
	sf::Vector2u dropPieceCount;
	sf::Sprite boardSprite, selectedPieceBackground, checkSprite, lastMoveStart, lastMoveDest, kothBackground, kothShadow, rankBackground, rankShadowTop,
		whiteCheckCount, blackCheckCount, dropPieceBackgroundW, dropPieceBackgroundB;
	sf::Text whiteCheckText, blackCheckText;
	sf::Texture boardTexture;
	sf::RectangleShape promotionOverlay;
	std::string move, currentMove, moves, FEN;
	std::pair<sf::Sprite, char> dropPiece;
	std::vector<basicBitboard> allPositionsPlayed;
	std::shared_ptr<Piece> selectedPiece, capturePiece, castleKing, castleRook, promotePiece;
	std::shared_ptr<Pawn> enPassantPiece;
	std::vector<sf::Sprite> promoteBackgrounds, promoteSprites, dropSquares;
	std::vector<sf::Text> dropPieceTextsW, dropPieceTextsB;
	std::vector<DropPiece> whiteDropPieces, blackDropPieces;
	sf::SoundBuffer moveBuffer, captureBuffer, checkBuffer, gameEndBuffer, explosionBuffer;
	sf::Sound moveSound, captureSound, gameEndSound;
	bp::ipstream is;
	bp::opstream os;
	bp::child c;

	Board(Variant variant, std::optional<std::string> FEN_ID, float xOffset, float yOffset, int boardSize, bool animated, sf::Vector2u windowSize, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
		bool AI, bool AI_Only, std::optional<bool> playerWhite, bool chess960, bool endGamePosition, textureVector& pTextures, textureVector& boardTextures);
	void setupFEN(std::optional<std::string> FEN_ID);
	void calculatePositions(bool init);
	void setupDropSprites();
	void calculateDropPositions();
	void draw(sf::RenderWindow& window);
	~Board();

private:
};

