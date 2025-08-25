#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include "ChessUtil.h"
#include <functional>

namespace Chess {

	struct DropPiece {
		char id;
		int count;
		std::vector<Chess::Square> dropSquares;
		sf::Sprite sprite, background, textBG;
		sf::Text text;

		DropPiece(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i);
		~DropPiece() {};

		virtual bool isValidSquare(sf::Vector2i square);
		void update();
		void move(float x, float y);
		void scale(float dropPieceSquareSize, sf::Sprite& dropPieceBackground, float bgStart, int i);
		bool mouseSelecting(sf::Vector2f mousePos) const;
		void setupSprites(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultplier, float pieceScale, bool reversed);
		void draw(sf::RenderWindow& window) const;
	};
	struct DropPawn : public virtual DropPiece {
		DropPawn(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i);
		~DropPawn() {};
		bool isValidSquare(sf::Vector2i square) override;

	};
	struct SDropPiece {
		char id;
		sf::Sprite sprite;
		std::vector<Chess::Square>* sDropSquares;

		SDropPiece(char id, sf::Texture& texture);
		~SDropPiece() {};
		void set(sf::Texture& texture, std::vector<Chess::Square>& dropSquares, int id);
		void draw(sf::RenderWindow& window);
	};

	struct IPromotePiece {
		sf::Vector2f initialScale;
		char id;

		IPromotePiece(char id, sf::Vector2f scale);
		~IPromotePiece() {};
	};
	struct PromotePiece {
		sf::Sprite sprite, backgroundSprite;
		sf::Vector2f initialScale;
		char id;

		PromotePiece(Chess::IPromotePiece iPromotePiece, sf::Texture& pieceTexture, sf::Texture& backgroundTexture, sf::Vector2i pos, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, sf::Vector2f boardScale, bool reversed, sf::Color promotionSquareColor);
		~PromotePiece() {};
	};

	struct Position {
		pieceVector pieceList;
		std::string move, moves, FEN;
		sf::Time whiteTime, blackTime;
		bool whiteToPlay, hasCheck, eighthRankWhite;
		int halfMoves, fullMoves, whiteChecks, blackChecks;
		sf::Vector2i lastMoveStartLocal, lastMoveDestLocal, checkPos;
		Position(pieceVector& pieceList, std::string FEN, std::string moves, bool whiteToPlay, bool eighthRankWhite, int halfMoves, int fullMoves, int whiteChecks, int blackChecks, bool hasCheck, sf::Vector2i lastMoveStartLocal, sf::Vector2i lastMoveDestLocal, sf::Vector2i checkPos, sf::Time whiteTime, sf::Time blackTime);
		Position(const Position& other);
		Position& operator=(const Position& other);
		bool operator==(const Position& other);
	};

	std::vector<sf::Texture> makePieceSet(sf::Image& spriteSheet, int pieceCount, int pieceSize);
	void loadPieceSet(sf::Image& spriteSheet, std::vector<sf::Texture>& pieceTextures, int pieceSize);
	sf::Texture loadPieceSet(sf::Image& spriteSheet, int pieceSize);
	sf::Texture loadBoard(sf::Image& spriteSheet, int boardNumber, int boardSize);
	sf::Texture loadBoard(sf::Image& spriteSheet, int boardNumber, int boardSize, int multiplier);
}

struct BoardSettings {

	// Normal
	Chess::Variant variant;
	int pieceSet, boardSet;
	bool AI_Time, AI, AI_Only, chess960, repeatFEN, timeEnabled;
	float xOffset, yOffset;
	std::optional<std::string> FEN, newPositionFEN;
	std::optional<bool> white, newPositionWhite;
	sf::Time whiteTime, blackTime, timeIncrement;

	// Advanced
	int whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID;
	bool keybindsEnabled, showOptionChanges, autoFlip, atomicExplosionEffect, endgamePosition, boardAnimated,
		overridePieceSpeed, instantPieces, isPaused, followMouse, scaleMouse, showMilliseconds, sharedTime, cycleSides;
	float pieceSpeed, boardScale;
	sf::Color promotionSquareColor;
	sf::Time startingWhiteTime, startingBlackTime;
	sf::Time millisecondsTime;
	std::function<bool(const sf::Time&)> millisecondsCondition;

	BoardSettings();
	BoardSettings(Chess::Variant variant, int pieceSet, int boardSet, bool AI_Time, bool AI, bool AI_Only, bool chess960, bool endgamePosition, bool repeatFEN, bool timeEnabled, float xOffset, float yOffset, float boardScale,
		std::optional<std::string> FEN, std::optional<std::string> newPositionFEN, std::optional<bool> white, std::optional<bool> newPositionWhite, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement,
		bool keybindsEnabled, bool showOptionChanges, bool autoFlip, bool atomicExplosionEffect, bool boardAnimated, bool overridePieceSpeed, bool instantPieces, bool isPaused, bool followMouse, bool scaleMouse, bool showMilliseconds, float pieceSpeed, sf::Color promotionSquareColor,
		sf::Time startingWhiteTime, sf::Time startingBlackTime, sf::Time millisecondsTime, std::function<bool(const sf::Time&)> millisecondsCondition, int whiteUnit, int blackUnit, int incrUnit, int startingWhiteUnit, int startingBlackUnit, int millisecondsConditionID, bool sharedTime, bool cycleSides);
	~BoardSettings(){};
};

std::vector<std::string> split(std::string str, char del);
size_t findNthOf(std::string str, char find, int nth);