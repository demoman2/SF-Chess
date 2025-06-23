#pragma once 
#include <SDKDDKVer.h>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>
#include "Stockfish.h"
#include "ChessUtil.h"
#include "Bishop.h"
#include "Knight.h"
#include "Rook.h"
#include "Queen.h"
#include "Pawn.h"
#include "King.h"

class Board
{
	using BasicPosition = std::pair<std::array<std::array<int, 8>, 8>, bool>;

	// Stockfish
	bool stockfishEnabled, AI_Only, playerSideWhite, isPaused, gameEnded, gameShouldEnd;
	std::shared_ptr<Piece> selectedPiece, capturePiece, castleKing, castleRook, promotePiece;
	Stockfish stockfish;

	// UI
	float sizeMultiplier, boardScale, pieceScale, startingScale, boardMultiplier, captureThreshold;
	int pieceSet, boardSet, boardTextureSize, pieceSize;
	bool scaleMode, mouseMode, optionMode, isFlipped, autoFlip;

	sf::Color promotionSquareColor{ 255, 255, 255 }, promotionSquareSelectedColor{ 255, 30, 0 };
	std::shared_ptr<std::vector<sf::Texture>> boardTextures;
	std::shared_ptr<sf::Font> textFont;
	std::vector<sf::Texture> pieceTextures;
	std::vector<sf::RectangleShape> captureObjects;
	std::vector<Chess::PromotePiece> promotePieces;
	sf::Vector2f boardOffset, dropPieceSquareSize, dropPieceWidth, dropPieceHeight, startingOffset, offset, boardSize;
	sf::Vector2i selectedPos;
	sf::Vector2u windowSize;
	sf::Sprite boardSprite, selectedPieceBackground, checkSprite, lastMoveStart, lastMoveDest, kothBackground, kothShadow, rankBackground, rankShadowTop,
		whiteCheckCount, blackCheckCount, dropPieceBackgroundW, dropPieceBackgroundB, ghostSprite;
	sf::Text whiteCheckText, blackCheckText, optionChangeText;
	sf::RectangleShape promotionOverlay, optionChangeOverlay;
	sf::Texture boardTexture;
	sf::Clock atomicClock, optionClock;
	void setupDropSprites();
	void setupPromoteSprites();
	void setExtraSprites();
	void moveBy(float x, float y);
	void setLocation(sf::Vector2f pos);
	void resetTransform();
	void scale(float scale);
	void setBoardTexture(sf::Image& boardSheet, int set);
	void setPieceSheet(std::vector<sf::Image> sheets, int set);
	void setSpritePositions();
	void flipBoard();
	void setOptionChange(std::string string, bool v);
	void setOptionChange(std::string string, int v);
	void setOptionChange(std::string string, Chess::Variant v);

	// Position
	Chess::Variant variant;
	pieceVector pieceList;
	bool calculatingPos, whiteToPlay, promotionEnabled, endgamePosition, chess960Enabled, eighthRankWhite, hasCheck, hasDoubleCheck, isPromoting, atomicKings, playingMove,
		pieceMoving, checksEnabled, castlingEnabled, dropsEnabled, isAnimated, animationFinished, shouldPostMove, repeatFEN, holdingPiece, mouseSelecting, mouseClicked, startedStockfish;
	int halfMoves, fullMoves, whiteChecks, blackChecks, promotionPieceCount;
	sf::Vector2i lastMoveStartLocal, lastMoveDestLocal;
	std::string moves, FEN;
	std::vector<BasicPosition> allPositionsPlayed;
	sf::Clock cClock;
	pieceVector generatePositionFromFENID(std::string code);
	void setupFEN(std::optional<std::string> FEN_ID, bool init);
	void setVariant(Chess::Variant variant);
	void calculateDropPositions();
	void playMove(std::string moveString, bool instantMove);
	void postMove(pieceVector movePieces);
	Chess::Endgame determineEnd();

	// Utility
	std::shared_ptr<Piece> getPieceFromCurrentPosition(sf::Vector2i position) const;
	sf::Vector2f getGlobalPosition(sf::Vector2i localPos) const;
	sf::Vector2f getGlobalPosition(sf::Vector2f localPos) const;
	sf::Vector2i getLocalPosition(sf::Vector2f globalPos) const;
	int getMoveCount();
	BasicPosition savePosition();

	sf::Texture& getTextureFromID(char id, Chess::PColor color);
	std::shared_ptr<Piece> getPieceFromID(char id, sf::Vector2i pos, Chess::PColor color, bool promoted);

	// Variants
	bool holdingDropPiece;
	Chess::SDropPiece selectedDropPiece;
	std::vector<Chess::Square> dropSquares;
	std::vector<Chess::DropPiece> whiteDropPieces, blackDropPieces;
	sf::Vector2u dropPieceCount;

	// Audio
	sf::SoundBuffer moveBuffer, captureBuffer, checkBuffer, gameEndBuffer, explosionBuffer;
	sf::Sound moveSound, captureSound, gameEndSound;

public:
	// Update
	void handleEvent(std::optional<sf::Event> event, sf::Image& boardSpriteSheet, std::vector<sf::Image>& pieceSpriteSheets, bool isFocused);
	void update(sf::Vector2f mousePos, bool isFocused);
	void updateS(sf::Vector2f mousePos, bool isFocused); // Update for selected boards only
	void draw(sf::RenderWindow& window);
	void generateLegalMoves(bool init);
	void setCalculating(bool c) { calculatingPos = c; }
	void mouseModeOff() { mouseMode = false; };
	bool intersects(sf::Vector2f position);
	int getMouseCursor(bool& hourGlass) const;

	bool generatingMoves;
	sf::Texture getBoardT() { return boardTexture; }
	Board(Chess::Variant variant, std::optional<std::string> FEN_ID, float xOffset, float yOffset, int boardSize, bool animated, sf::Vector2u windowSize, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
		bool AI, bool AI_Only, std::optional<bool> playerWhite, bool repeatFEN, bool chess960, bool endGamePosition, std::vector<sf::Texture>& pTextures, std::vector<sf::Texture>& boardTextures, bool init);
	~Board();

};