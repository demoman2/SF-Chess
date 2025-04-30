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

typedef std::vector<std::shared_ptr<Piece>> pieceVector;
typedef std::vector<std::reference_wrapper<sf::Texture>> textureVector;
typedef std::pair<std::array<std::array<int, 8>, 8>, bool> basicBitboard;
using namespace Chess;
namespace bp = boost::process::v1;
class Board
{

public:
	sf::Color promotionSquareColor{ 255, 255, 255 }, promotionSquareSelectedColor{ 255, 30, 0 };
	Variant variant;
	pieceVector pieceList;
	textureVector pieceTextures;
	std::vector<sf::Image> pieceSpriteSheets;
	int boardSize, halfMoves, fullMoves, dropPieceCount, whiteChecks, blackChecks, wKRook, wQRook, bKRook, bQRook;
	float sizeMultiplier, boardScale, pieceScale, boardXOffset, boardYOffset, boardMultiplier, dropPieceSquareSize, dropPieceWidth, dropPieceHeight, threshold, pieceSize;
	bool whiteToPlay, playerSideWhite, unpromoting, kingPromotionEnabled, endgamePosition, Chess960Enabled, StockfishEnabled,
		calculatingPos, hourGlass, eighthRankWhite, movePlayed, mouseSelecting,
		mouseClicked, pieceMoving, check, promoting, animationFinished,
		pieceSelectionLock, standardPosition, checksEnabled, castlingEnabled, dropsEnabled, animated;
	sf::Image pieceSpriteSheet, boardSpriteSheet;
	sf::Sprite boardSprite, selectedPieceBackground, checkSprite, lastMoveStart, lastMoveDest, kothBackground, kothShadow, rankBackground, rankShadowTop,
		whiteCheckCount, blackCheckCount;
	sf::Text whiteCheckText, blackCheckText;
	sf::Texture boardTexture;
	sf::RectangleShape dropPieceBackground, promotionOverlay;
	std::string move, currentMove, moves, FEN, whiteDropPieces, blackDropPieces;
	std::vector<basicBitboard> allPositionsPlayed;
	std::shared_ptr<Piece> selectedPiece, capturePiece, castleKing, castleRook, promotePiece;
	std::shared_ptr<Pawn> enPassantPiece;
	std::vector<sf::Sprite> promoteBackgrounds, promoteSprites;
	sf::SoundBuffer moveBuffer, captureBuffer, checkBuffer, gameEndBuffer, explosionBuffer;
	sf::Sound moveSound, captureSound, gameEndSound;
	bp::ipstream is;
	bp::opstream os;
	bp::child c;

	Board(Variant variant, std::optional<std::string> FEN_ID, int boardSize, bool animated, sf::RenderWindow& window, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Texture& boardT, sf::Texture& selectBackgroundTexture, sf::Texture& checkTexture, sf::Texture& lastMoveTexture, sf::Texture& kothBackgroundTexture, sf::Texture& kothShadowTexture, sf::Texture& rankBackgroundTexture, sf::Texture& rankShadowTopTexture, sf::Texture& checkCountTexture, sf::Font& textFont,
		bool AI, std::optional<bool> playerWhite, bool chess960, bool endGamePosition, textureVector& pTextures, textureVector& extraTextures);
	void setupFEN(std::optional<std::string> FEN_ID, textureVector extraTextures);
	void calculatePositions(textureVector extraTextures, bool initial);
	void draw(sf::RenderWindow& window);
	~Board();

private:

};

