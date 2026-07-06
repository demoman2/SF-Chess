/*
	SF Chess, a Chess GUI which supports many chess variants
	Copyright (C) 2026  demoman2 (https://github.com/demoman2)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once 
#include <SDKDDKVer.h>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include "HSV.h"
#include "Piece.h"
#include "Stockfish.h"
#include "ChessStructs.h"
#include "MoveSet.h"

class Board
{
	using BasicPosition = std::pair<std::vector<std::vector<char>>, bool>;

	// Stockfish
	bool stockfishEnabled, startedStockfish, ai, AI_Only, AI_OnlyT, playerSideWhite, isPaused;
	std::shared_ptr<Piece> selectedPiece, capturePiece, castleKing, castleRook, promotePiece, gatingPiece, gatingKing, gatingRook;
	Stockfish stockfish;

	// UI
	HSV boardHSV;
	bool scaleMode, mouseMode, optionMode, isFlipped, autoFlip, instantMove, instantMoveI;
	float sizeMultiplier, boardScale, pieceScale, startingScale, boardMultiplier, moveSpeed, moveSpeedI, captureThreshold, initialBoardScale;
	int pieceSet, boardSet;

	const std::set<std::string>* endgameVariants;
	const std::vector<std::map<char, sf::Texture>>* pieceTextures;
	const std::map<std::string, std::map<char, sf::Texture>>* variantPieceTextures;
	const std::map<char, sf::Texture>* currentPieceTextures;
	const std::vector<sf::Texture>* boardTextures;
	const sf::Font* textFont;
	const sf::Image* boardSheet;
	sf::Color promotionSquareColor, promotionSquareSelectedColor;
	std::vector<sf::RectangleShape> captureObjects;
	std::vector<Chess::PromotePiece> promotePieces, gatingPieces, gatingKingPieces, gatingRookPieces;
	sf::Vector2f boardOffset, promotePieceSize, dropPieceSquareSize, dropPieceWidth, dropPieceHeight, startingOffset, offset, boardSize, initialMousePos;
	sf::Vector2i selectedPos;
	sf::Vector2u windowSize;
	sf::Sprite boardSprite, selectedPieceBackground, lastMoveStart, lastMoveDest, kothBackground, kothShadow, rankBackground, rankShadowTop,
		whiteCheckCount, blackCheckCount, dropPieceBackgroundW, dropPieceBackgroundB, ghostSprite;
	std::vector<sf::Sprite> checkSprites;
	sf::Text whiteCheckText, blackCheckText, optionChangeText, gameEndText;
	sf::RectangleShape promotionOverlay, optionChangeOverlay, gameEndOverlay;
	sf::Texture boardTexture;
	sf::Clock animationClock, atomicClock, optionClock, gameEndClock;

	void setupDropSprites();
	void setupPromoteSprites();
	void setupGatingSprites();
	void setPieceSprites(bool makePieceSprites = false);
	void moveBy(float x, float y);
	void setLocation(sf::Vector2f pos);
	void scale(float scale);
	void setScale(float newScale);
	void setBoardTexture(const sf::Image& boardSheet, int set);
	void setPieceTextures();
	void setSpritePositions();

	void setOptionChange(const std::string& string);
	void setOptionChange(const std::string& string, bool v);
	void setOptionChange(const std::string& string, std::optional<bool> v);
	void setOptionChange(const std::string& string, int v);
	void setOptionChange(const std::string& string, float v);
	void setOptionChange(const std::string& string, const Chess::Variant& v);

	void setGameEndText(const std::string& string);

	// Position
	const Chess::VariantList* variantList;
	const Chess::Variant* variant;
	size_t variantIndex, variantTypeIndex;
	Chess::GameType gameType;
	Chess::Position position;
	std::optional<bool> white, newPositionWhite;
	bool calculatingPos, whiteToPlay, endgamePosition, hasCheck, isPromoting, isGating, playingMove, generatingMoves, doDebug,
		pieceMoving, isAnimated, animationFinished, shouldPostMove, repeatFEN, holdingPiece, mouseSelecting, mouseClicked, changingPosition, forwardMove, AI_Time, arrowLeft, arrowRight, cancelGating, autoNewPosition;
	int halfMoves, fullMoves, whiteChecksLeft, blackChecksLeft, movesPlayed;
	sf::Vector2i lastMoveStartLocal, lastMoveDestLocal;
	BasicPosition lastPositionAdded;
	std::string moves, currentmove, startingFEN;
	std::optional<std::string> newPositionFEN;
	std::map<BasicPosition, size_t> allPositionsPlayed;
	std::vector<Chess::SavePosition> positionHistory, positionHistoryF;
	std::vector<bool> positionChangeThreads;
	sf::Clock cClock, arrowClockL, arrowClockR;
	void getCastlingRights(const std::string& code);
	pieceVector generatePositionFromFENID(const std::string& code);
	void generateLegalMoves(bool init);
	void calculateDropPositions();
	void playMove(std::string& moveString, bool instantMove, bool promoteMove = false);
	void postMove(pieceVector movePieces, bool drop = false);
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
	size_t getMoveCount();
	BasicPosition savePosition();
	bool isRoyal(const std::shared_ptr<Piece>& piece) const;

	int whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID;
	bool keyBindsEnabled, atomicExplosionEffect, overridePieceSpeed, showMilliseconds, sharedTime, cycleSides;
	sf::Time millisecondsTime;
	std::function<bool(const sf::Time&)> millisecondsCondition;

	const sf::Texture& getTextureFromID(char id, Chess::PColor color) const;
	std::shared_ptr<Piece> getPieceFromID(char id, sf::Vector2i pos, Chess::PColor color, bool promoted, bool promotedPieceType) const;

	// Variants
	bool holdingDropPiece;
	Chess::SDropPiece selectedDropPiece;
	std::vector<Chess::DropPiece> whiteDropPieces, blackDropPieces;
	sf::Vector2u dropPieceCount;

	// Audio
	const std::vector<sf::SoundBuffer>* soundBuffers;
	sf::Sound moveSound, captureSound, gameEndSound, lowTimeSound;

public:
	// Update
	void handleEvent(const std::optional<sf::Event>& event, sf::Vector2f mousePos, const sf::Image& boardSpriteSheet, bool isFocused, bool controlClicked);
	void update(sf::Vector2f& mousePos, bool isFocused, float deltaTime);
	void updateS(sf::Vector2f& mousePos, bool isFocused); // Update for selected boards only
	void draw(sf::RenderWindow& window) const;
	void flipBoard();
	void resetPosition();
	void resetScale();
	std::string getCurrentFEN() const;
	BoardSettings getBoardSettings() const;
	const StockfishSettings& getStockfishSettings() const;
	void loadFromBoardArgs(const BoardSettings& bA, sf::Image& boardSheet);
	void setStockfishSettings(const StockfishSettings& newSettings);
	int getMouseCursor(bool& blockCursor) const;
	Stockfish& getEngine() { return stockfish; };

	void loadFromFEN(const std::optional<std::string>& FEN_ID, bool init, bool first = false);
	void loadFromPosition(const Chess::SavePosition& position);
	void setVariant(size_t variantIndex);

	void mouseModeOff();
	bool intersects(sf::Vector2f position) const;
	bool areKeybindsEnabled() const { return keyBindsEnabled; }

	const sf::Texture& getBoardTexture() const;
	int getBoardSet() const { return boardSet; };

	Board(const BoardSettings& bA, const StockfishSettings& stockfishSettings, sf::Vector2u windowSize, const std::vector<std::map<char, sf::Texture>>& pieceTextures, const std::map<std::string, std::map<char, sf::Texture>>& variantPieceTextures, const sf::Image& boardSheet, const sf::Font& textFont,
		const std::vector<sf::Texture>& boardTextures, const std::vector<sf::SoundBuffer>& soundBuffers, const std::set<std::string>& endgameVariants, bool init);
	~Board();

};