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
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <TGUI/TGUI.hpp>
#include "ChessUtil.h"
#include "Variant.h"
#include <functional>

namespace Chess {

	struct DropPiece {

	public:
		char id;
		int count;
		std::vector<Chess::Square> dropSquares;
		sf::Sprite sprite, background, textBG;
		sf::Text text;

		DropPiece(char id, const sf::Texture& texture, const sf::Texture& backgroundTexture, const sf::Texture& textBackgroundTexture, const sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i);
		~DropPiece() {};

		void update();
		void move(float x, float y);
		void scale(float dropPieceSquareSize, sf::Sprite& dropPieceBackground, float bgStart, int i);
		
		bool mouseSelecting(sf::Vector2f mousePos) const;
		
		void setupSprites(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultplier, bool reversed);
		void setTexture(const sf::Texture& texture);
		void draw(sf::RenderWindow& window) const;
	};

	struct SDropPiece {
		char id;
		sf::Sprite sprite;
		std::vector<Chess::Square>* sDropSquares;

		SDropPiece(char id, const sf::Texture& texture);
		~SDropPiece() {};

		void setTexture(const sf::Texture& texture);
		void set(const sf::Texture& texture, std::vector<Chess::Square>& dropSquares, int id);
		void draw(sf::RenderWindow& window) const;
	};

	struct PromotePiece {
		sf::Sprite sprite, backgroundSprite;
		sf::Vector2f initialScale;
		Chess::PColor color;
		char id;
		bool self;

		PromotePiece(char id, sf::Vector2f initialScale, Chess::PColor color, const sf::Texture& pieceTexture, const sf::Texture& backgroundTexture, sf::Vector2i pos, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, sf::Vector2f boardScale, sf::Vector2u boardSquares, bool reversed, sf::Color promotionSquareColor, bool self);
		~PromotePiece() {};
	
		void setTexture(const sf::Texture& texture);

	};

	struct SavePosition {

	public:
		pieceVector pieceList;

		std::string move, moves, FEN;
		sf::Time whiteTime, blackTime;
		
		bool whiteToPlay, hasCheck, extraFlagMove, gameEnded;

		int halfMoves, fullMoves, whiteChecksLeft, blackChecksLeft;
		
		sf::Vector2i lastMoveStartLocal, lastMoveDestLocal;
		std::vector<sf::Vector2i> checkPositions;

		std::map<char, int> dropPieceCounts;

		std::optional<sf::Vector2i> enPassantTarget, enPassantTripleTarget;

		std::array<int, 4> castlePieces; // wK, wQ, bK, bQ
		std::array<bool, 4> castlingRights; // wK, wQ, bK, bQ

		SavePosition(pieceVector& pieceList, const std::string& FEN, const std::string& moves, bool gameEnded, bool whiteToPlay, std::optional<sf::Vector2i> enPassantTarget, std::optional<sf::Vector2i> enPassantTripleTarget, bool extraFlagMove, int halfMoves, int fullMoves, int whiteChecks, int blackChecks, bool hasCheck,
			sf::Vector2i lastMoveStartLocal, sf::Vector2i lastMoveDestLocal, const std::vector<sf::Vector2i>& checkPositions, sf::Time whiteTime, sf::Time blackTime, const std::array<int, 4>& castlePieces, const std::array<bool, 4>& castlingRights);
		SavePosition(const SavePosition& other);
		SavePosition& operator=(const SavePosition& other);

		bool operator==(const SavePosition& other);
	};

	std::map<char, sf::Texture> loadPieceSet(const std::string& path);
	std::map<char, sf::Texture> loadPieceSet(const std::string& path, const std::string& defaultPath);
	std::map<char, tgui::Texture> loadPieceSetSVG(const std::string& path);
	std::map<char, tgui::Texture> loadPieceSetSVG(const std::string& path, const std::string& defaultPath);
	sf::Texture getPieceFromSet(const std::string& path, const std::string& setName, const std::string& pieceName);
	sf::Texture loadBoard(const sf::Image& spriteSheet, int boardNumber, sf::Vector2u boardSquares);

}

struct BoardSettings {

	// Normal
	size_t variantIndex, variantTypeIndex, currentVariantIndex, currentVariantTypeIndex;
	int pieceSet, pieceSetSave, boardSet;
	bool AI_Time, AI, AI_Only, chess960, repeatFEN, timeEnabled;
	float xOffset, yOffset;
	std::optional<std::string> FEN, newPositionFEN, saveFEN;
	std::optional<bool> white, newPositionWhite;
	sf::Time whiteTime, blackTime, timeIncrement;
	const Chess::VariantList* variantList;

	// Advanced
	int whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID;
	bool keybindsEnabled, showOptionChanges, autoFlip, atomicExplosionEffect, endgamePosition, boardAnimated,
		overridePieceSpeed, instantPieces, isPaused, followMouse, scaleMouse, showMilliseconds, sharedTime, cycleSides;
	float pieceSpeed, boardScale;
	sf::Color promotionSquareColor;
	sf::Time startingWhiteTime, startingBlackTime;
	sf::Time millisecondsTime;
	std::function<bool(const sf::Time&)> millisecondsCondition;

	BoardSettings(const std::vector<Chess::VariantType>& types);
	BoardSettings(size_t variantIndex, size_t variantTypeIndex, int pieceSet, int boardSet, bool AI_Time, bool AI, bool AI_Only, bool chess960, bool endgamePosition, bool repeatFEN, bool timeEnabled, float xOffset, float yOffset, float boardScale,
		std::optional<std::string> FEN, std::optional<std::string> newPositionFEN, std::optional<bool> white, std::optional<bool> newPositionWhite, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement,
		bool keybindsEnabled, bool showOptionChanges, bool autoFlip, bool atomicExplosionEffect, bool boardAnimated, bool overridePieceSpeed, bool instantPieces, bool isPaused, bool followMouse, bool scaleMouse, bool showMilliseconds, float pieceSpeed, sf::Color promotionSquareColor,
		sf::Time startingWhiteTime, sf::Time startingBlackTime, sf::Time millisecondsTime, std::function<bool(const sf::Time&)> millisecondsCondition, int whiteUnit, int blackUnit, int incrUnit, int startingWhiteUnit, int startingBlackUnit, int millisecondsConditionID, bool sharedTime, bool cycleSides);
	~BoardSettings(){};
};