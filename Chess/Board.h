#pragma once 
#include <SDKDDKVer.h>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include "HSV.h"
#include "Stockfish.h"
#include "ChessStructs.h"
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
	bool stockfishEnabled, startedStockfish, ai, AI_Only, AI_OnlyT, playerSideWhite, isPaused, gameEnded, gameShouldEnd;
	std::shared_ptr<Piece> selectedPiece, capturePiece, castleKing, castleRook, promotePiece;
	Stockfish stockfish;

	// UI
	HSV boardHSV;
	bool scaleMode, mouseMode, optionMode, isFlipped, autoFlip, instantMove, instantMoveI;
	float sizeMultiplier, boardScale, pieceScale, startingScale, boardMultiplier, moveSpeed, moveSpeedI, captureThreshold, initialBoardScale;
	int pieceSet, boardSet, pieceSize;

	sf::Color promotionSquareColor, promotionSquareSelectedColor;
	std::vector<sf::Texture>* boardTextures;
	sf::Font* textFont;
	std::vector<sf::Texture> pieceTextures;
	std::vector<sf::RectangleShape> captureObjects;
	std::vector<Chess::PromotePiece> promotePieces;
	sf::Vector2f boardOffset, dropPieceSquareSize, dropPieceWidth, dropPieceHeight, startingOffset, offset, boardSize, initialMousePos;
	sf::Vector2i selectedPos;
	sf::Vector2u windowSize;
	sf::Sprite boardSprite, selectedPieceBackground, checkSprite, lastMoveStart, lastMoveDest, kothBackground, kothShadow, rankBackground, rankShadowTop,
		whiteCheckCount, blackCheckCount, dropPieceBackgroundW, dropPieceBackgroundB, ghostSprite;
	sf::Text whiteCheckText, blackCheckText, optionChangeText;
	sf::RectangleShape promotionOverlay, optionChangeOverlay;
	sf::Texture boardTexture;
	sf::Clock animationClock, atomicClock, optionClock;
	void setupDropSprites();
	void setupPromoteSprites();
	void setPieceSprites(bool makePieceSprites = false);
	void moveBy(float x, float y);
	void setLocation(sf::Vector2f pos);
	void scale(float scale);
	void setScale(float newScale);
	void setBoardTexture(sf::Image& boardSheet, int set);
	void setPieceSheet(std::vector<sf::Image> sheets, int set);
	void setSpritePositions();
	void setOptionChange(std::string string);
	void setOptionChange(std::string string, bool v);
	void setOptionChange(std::string string, std::optional<bool> v);
	void setOptionChange(std::string string, int v);
	void setOptionChange(std::string string, float v);
	void setOptionChange(std::string string, Chess::Variant v);

	// Position
	Chess::Variant variant;
	Chess::GameType gameType;
	pieceVector pieceList;
	std::optional<bool> white, newPositionWhite;
	bool calculatingPos, whiteToPlay, promotionEnabled, endgamePosition, chess960Enabled, eighthRankWhite, hasCheck, isPromoting, atomicKings, playingMove, generatingMoves,
		pieceMoving, checksEnabled, castlingEnabled, dropsEnabled, isAnimated, animationFinished, shouldPostMove, repeatFEN, holdingPiece, mouseSelecting, mouseClicked, changingPosition, forwardMove, AI_Time, arrowLeft, arrowRight;
	int halfMoves, fullMoves, whiteChecks, blackChecks, movesPlayed;
	sf::Vector2i lastMoveStartLocal, lastMoveDestLocal;
	std::string moves, startingFEN;
	std::optional<std::string> newPositionFEN;
	std::vector<BasicPosition> allPositionsPlayed;
	std::vector<Chess::Position> positionHistory, positionHistoryF;
	std::vector<Chess::IPromotePiece> whitePromotePieces, blackPromotePieces;
	std::vector<bool> positionChangeThreads;
	sf::Clock cClock, arrowClockL, arrowClockR;
	pieceVector generatePositionFromFENID(std::string code);
	void generateLegalMoves(bool init);
	void calculateDropPositions();
	void playMove(std::string moveString, bool instantMove);
	void postMove(pieceVector movePieces);
	void setPreviousPosition();
	void setNextPosition();
	std::string getNewFEN() const;
	Chess::Endgame determineEnd();

	// Time
	bool timeEnabled;
	sf::Time startingWhiteTime, startingBlackTime, whiteTime, blackTime, timeIncrement;
	sf::Time startingPlayerTime, startingAITime, playerTime, AITime;
	sf::Sprite whiteTimeBG, blackTimeBG, whiteTimeOutline, blackTimeOutline;
	sf::Text whiteTimeText, blackTimeText;
	sf::Clock timeClock, delayClock;

	// Utility
	std::shared_ptr<Piece> getPieceFromCurrentPosition(sf::Vector2i position) const;
	sf::Vector2f getGlobalPosition(sf::Vector2i localPos) const;
	sf::Vector2f getGlobalPosition(sf::Vector2f localPos) const;
	sf::Vector2f getGlobalPosition(sf::Vector2i localPos, bool reversed) const;
	sf::Vector2f getGlobalPosition(sf::Vector2f localPos, bool reversed) const;
	sf::Vector2i getLocalPosition(sf::Vector2f globalPos) const;
	int getMoveCount();
	BasicPosition savePosition();

	int whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID;
	bool keyBindsEnabled, atomicExplosionEffect, overridePieceSpeed, showMilliseconds, sharedTime, cycleSides;
	sf::Time millisecondsTime;
	std::function<bool(const sf::Time&)> millisecondsCondition;

	sf::Texture& getTextureFromID(char id, Chess::PColor color);
	std::shared_ptr<Piece> getPieceFromID(char id, sf::Vector2i pos, Chess::PColor color, bool promoted);

	// Variants
	bool holdingDropPiece;
	Chess::SDropPiece selectedDropPiece;
	std::vector<std::shared_ptr<Chess::DropPiece>> whiteDropPieces, blackDropPieces;
	sf::Vector2u dropPieceCount;

	// Audio
	std::vector<sf::SoundBuffer>* soundBuffers;
	sf::Sound moveSound, captureSound, gameEndSound, lowTimeSound;

public:
	// Update
	void handleEvent(std::optional<sf::Event> event, sf::Vector2f& mousePos, sf::Image& boardSpriteSheet, std::vector<sf::Image>& pieceSpriteSheets, bool isFocused, bool controlClicked);
	void update(sf::Vector2f& mousePos, bool isFocused, float deltaTime);
	void updateS(sf::Vector2f& mousePos, bool isFocused); // Update for selected boards only
	void draw(sf::RenderWindow& window);
	void flipBoard();
	void resetPosition();
	void resetScale();
	std::string getCurrentFEN();
	BoardSettings getBoardArgs();
	const StockfishSettings& getStockfishSettings();
	void loadFromBoardArgs(const BoardSettings& bA, std::vector<sf::Image>& pieceSheets, sf::Image& boardSheet);
	void setStockfishSettings(const StockfishSettings& newSettings);
	int getMouseCursor(bool& hourGlass) const;
	Stockfish& getEngine() { return stockfish; };

	void loadFromFEN(std::optional<std::string> FEN_ID, bool init, bool first = false);
	void loadFromPosition(const Chess::Position& position);
	void setVariant(Chess::Variant variant);

	void mouseModeOff();
	bool intersects(sf::Vector2f position);
	bool areKeybindsEnabled() const { return keyBindsEnabled; }

	const sf::Texture& getBoardT();
	int getBoardSet() const { return boardSet; };
	Board(const BoardSettings& bA, const StockfishSettings& stockfishSettings, sf::Vector2u windowSize, std::vector<sf::Image>& pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
		std::vector<sf::Texture>& pTextures, std::vector<sf::Texture>& boardTextures, std::vector<sf::SoundBuffer>& soundBuffers, bool init);
	~Board();

};