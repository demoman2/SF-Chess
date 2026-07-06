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

#include "ChessStructs.h"
#include "Piece.h"
#include <TGUI/Widgets/Picture.hpp>

// Drop Piece
Chess::DropPiece::DropPiece(char id, const sf::Texture& texture, const sf::Texture& backgroundTexture, const sf::Texture& textBackgroundTexture, const sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i)
	: id(id), count(0), sprite(texture), background(backgroundTexture), textBG(textBackgroundTexture), text(font, std::to_string(count), 45)
{
	sprite.setPosition({ dropPiecePosition.x, bgStart + dropPieceSquareSize * i });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale({ dropPieceSquareSize / texture.getSize().x, dropPieceSquareSize / texture.getSize().y });
	background.setOrigin(background.getLocalBounds().getCenter());
	background.setScale({ dropPieceSquareSize / backgroundTexture.getSize().x, dropPieceSquareSize / backgroundTexture.getSize().y });
	background.setPosition(sprite.getPosition());
	background.setColor({ 255, 255, 255, 0 });

	textBG.setOrigin(textBG.getLocalBounds().getCenter());
	textBG.setScale({ dropPieceSquareSize / textBackgroundTexture.getSize().x, dropPieceSquareSize / textBackgroundTexture.getSize().y });
	textBG.setPosition(sprite.getPosition());

	text.setCharacterSize(static_cast<unsigned int>(std::roundf(45.0f * (dropPieceSquareSize / 150.0f))));
	text.setOrigin({ text.getLocalBounds().position.x + text.getLocalBounds().size.x / 2.0f, text.getLocalBounds().position.y + text.getLocalBounds().size.y / 2.0f });
	text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });
}

void Chess::DropPiece::update()
{
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

void Chess::DropPiece::move(float x, float y) {
	sprite.move({ x, y });
	textBG.move({ x, y });
	text.move({ x, y });
	background.move({ x, y });
}

void Chess::DropPiece::scale(float dropPieceSquareSize, sf::Sprite& dropPieceBackground, float bgStart, int i)
{
	sprite.setScale({ dropPieceSquareSize / sprite.getTexture().getSize().x, dropPieceSquareSize / sprite.getTexture().getSize().y });
	textBG.setScale({ dropPieceSquareSize / textBG.getTexture().getSize().x, dropPieceSquareSize / textBG.getTexture().getSize().y});
	text.setCharacterSize(static_cast<unsigned int>(std::roundf(45.0f * (dropPieceSquareSize / 150.0f))));
	background.setScale({ dropPieceSquareSize / background.getTexture().getSize().x, dropPieceSquareSize / background.getTexture().getSize().y});
	sprite.setPosition({ dropPieceBackground.getPosition().x, bgStart + dropPieceSquareSize * i });
	textBG.setPosition(sprite.getPosition());
	text.setOrigin({ text.getLocalBounds().position.x + text.getLocalBounds().size.x / 2.0f, text.getLocalBounds().position.y + text.getLocalBounds().size.y / 2.0f });
	text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });
	background.setPosition(sprite.getPosition());
}

bool Chess::DropPiece::mouseSelecting(sf::Vector2f mousePos) const
{
	return count != 0 && sprite.getGlobalBounds().contains(mousePos);
}

void Chess::DropPiece::setupSprites(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultplier, bool reversed)
{
	for (auto& square : dropSquares) {
		square.setupSprite(texture, boardOffset, boardSize, boardSquares, boardMultplier, reversed);
	}
}

void Chess::DropPiece::setTexture(const sf::Texture& texture)
{
	sprite.setTexture(texture, true);
}

void Chess::DropPiece::draw(sf::RenderWindow& window) const
{
	window.draw(background);
	window.draw(sprite);
	window.draw(textBG);
	window.draw(text);
}

// Promote Piece
Chess::PromotePiece::PromotePiece(char id, sf::Vector2f initialScale, Chess::PColor color, const sf::Texture& pieceTexture, const sf::Texture& backgroundTexture, sf::Vector2i pos, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier,
	float pieceScale, sf::Vector2f boardScale, sf::Vector2u boardSquares, bool reversed, sf::Color promotionSquareColor, bool self) :
	sprite(pieceTexture), backgroundSprite(backgroundTexture), id(id), initialScale(initialScale), self(self), color(color)
{
	backgroundSprite.setOrigin(backgroundSprite.getGlobalBounds().getCenter());
	backgroundSprite.setScale(boardScale);
	backgroundSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
	backgroundSprite.setColor(promotionSquareColor);
	sprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale(this->initialScale);
}

void Chess::PromotePiece::setTexture(const sf::Texture& texture)
{
	sprite.setTexture(texture, true);
}

std::map<char, sf::Texture> Chess::loadPieceSet(const std::string& path) {
	std::map<char, sf::Texture> map;
	for (char c = 'a'; c <= 'z'; c++) {
		sf::Texture pieceTexture;
		if (pieceTexture.loadFromFile(path + "\\w" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png")) {
			pieceTexture.setSmooth(true);
			map.emplace(std::toupper(c), pieceTexture);
		}
		if (pieceTexture.loadFromFile(path + "\\b" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png")) {
			pieceTexture.setSmooth(true);
			map.emplace(c, pieceTexture);
		}
	}
	return map;
};

std::map<char, sf::Texture> Chess::loadPieceSet(const std::string& path, const std::string& defaultPath) {
	std::map<char, sf::Texture> map;
	for (char c = 'a'; c <= 'z'; c++) {
		sf::Texture pieceTexture;
		std::string newPath = path + "\\w" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png";
		if (pieceTexture.loadFromFile(newPath)) {
			pieceTexture.setSmooth(true);
			map.emplace(std::toupper(c), pieceTexture);
		}
		else {
			newPath = defaultPath + "\\w" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png";
			if (pieceTexture.loadFromFile(newPath)) {
				pieceTexture.setSmooth(true);
				map.emplace(std::toupper(c), pieceTexture);
			}
		}
		newPath = path + "\\b" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png";
		if (pieceTexture.loadFromFile(newPath)) {
			pieceTexture.setSmooth(true);
			map.emplace(c, pieceTexture);
		}
		else {
			newPath = defaultPath + "\\b" + std::string({ static_cast<char>(std::toupper(c)) }) + ".png";
			if (pieceTexture.loadFromFile(newPath)) {
				pieceTexture.setSmooth(true);
				map.emplace(c, pieceTexture);
			}
		}
	}
	return map;
};

std::map<char, tgui::Texture> Chess::loadPieceSetSVG(const std::string& path) {
	std::map<char, tgui::Texture> map;
	for (char l = 'a'; l <= 'z'; l++) {
		try {
			std::string white = path + "\\w" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
			map.emplace(std::toupper(l), white);
		}
		catch (std::exception e) {}
		try {
			std::string black = path + "\\b" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
			map.emplace(l, black);
		}
		catch (std::exception e) {}
	}
	return map;
};

std::map<char, tgui::Texture> Chess::loadPieceSetSVG(const std::string& path, const std::string& defaultPath) {
	std::map<char, tgui::Texture> map;
	for (char l = 'a'; l <= 'z'; l++) {
		std::string newPath = path + "\\w" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
		std::string otherPath = defaultPath + "\\w" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
		try {
			map.emplace(std::toupper(l), newPath);
		}
		catch (std::exception e) {
			try {
				map.emplace(std::toupper(l), otherPath);
			}
			catch (std::exception e) {}
		}

		newPath = path + "\\b" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
		otherPath = defaultPath + "\\b" + std::string({ static_cast<char>(std::toupper(l)) }) + ".svg";
		try {
			map.emplace(l, newPath);
		}
		catch (std::exception e) {
			try {
				map.emplace(l, otherPath);
			}
			catch (std::exception e) {}
		}
	}
	return map;
};


sf::Texture Chess::getPieceFromSet(const std::string& path, const std::string& setName, const std::string& pieceName) {
	sf::Texture pieceTexture;
	if (!pieceTexture.loadFromFile(path + "/" + setName + "/" + pieceName + ".png")) {
		std::cout << "Failed to load piece texture from " << path << "/" << setName << "/" << pieceName << ".png\n";
	}
	pieceTexture.setSmooth(true);
	return pieceTexture;
};

sf::Texture Chess::loadBoard(const sf::Image& spriteSheet, int boardNumber, sf::Vector2u boardSquares) {
	int bx = boardNumber % 4;
	int by = boardNumber / 4;
	sf::RenderTexture r{ sf::Vector2u{boardSquares.x * 128, boardSquares.y * 128} };
	r.clear(sf::Color::Transparent);
	sf::Texture light{ spriteSheet, false, sf::IntRect{{bx * 128 * 2, by * 128}, {128, 128}} };
	sf::Texture dark{ spriteSheet, false, sf::IntRect{{(bx * 128 * 2) + 128, by * 128}, {128, 128}} };
	for (int y = 0; y < boardSquares.y; y++) {
		for (int x = 0; x < boardSquares.x; x++) {
			if ((x + y) % 2 == 0) {
				sf::Sprite s{ dark };
				s.setPosition({ float(x * 128), float(y * 128) });
				r.draw(s);
			}
			else {
				sf::Sprite s{ light };
				s.setPosition({ float(x * 128), float(y * 128) });
				r.draw(s);
			}
		}
	}
	r.display();
	return r.getTexture();
}

Chess::SDropPiece::SDropPiece(char id, const sf::Texture& texture) : id(id), sprite(texture), sDropSquares(nullptr)
{
}

void Chess::SDropPiece::setTexture(const sf::Texture& texture)
{
	sprite.setTexture(texture);
}

void Chess::SDropPiece::set(const sf::Texture& texture, std::vector<Chess::Square>& dropSquares, int id)
{
	sprite.setTexture(texture);
	sDropSquares = &dropSquares;
	this->id = id;
}

void Chess::SDropPiece::draw(sf::RenderWindow& window) const
{
	if (sDropSquares) {
		for (auto& square : *sDropSquares) {
			sf::Sprite& sprite = *square.sprite;
			window.draw(*square.sprite);
		}
	}
	window.draw(sprite);
}

// Square
Chess::Square::Square(sf::Sprite& sprite, sf::Vector2i position, const Chess::PieceType& validPromotionTypes, bool promoteSquare) :
	pos(position), moveString(""), sprite(std::make_unique<sf::Sprite>(sprite)), validPromotionTypes(validPromotionTypes), promoteSquare(promoteSquare)
{
}

Chess::Square::Square(sf::Vector2i position, std::string moveString, const Chess::PieceType& validPromotionTypes, bool promoteSquare) :
	pos(position), moveString(moveString), sprite(nullptr), validPromotionTypes(validPromotionTypes), promoteSquare(promoteSquare)
{
}
	
Chess::Square::Square(const Chess::Square& other) : sprite(nullptr), pos(other.pos), moveString(other.moveString), validPromotionTypes(other.validPromotionTypes), promoteSquare(other.promoteSquare)
{
}

Chess::Square& Chess::Square::operator=(const Chess::Square& other)
{
	if (this == &other) {
		return *this;
	}
	sprite = nullptr;
	pos = other.pos;
	moveString = other.moveString;
	validPromotionTypes = other.validPromotionTypes;
	promoteSquare = other.promoteSquare;
	return *this;
}

void Chess::Square::setupSprite(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, bool reversed) {
	sf::Sprite sSprite{ texture };
	sSprite.setOrigin(sSprite.getLocalBounds().getCenter());
	sSprite.setScale(sf::Vector2f(boardMultiplier / static_cast<float>(texture.getSize().x), boardMultiplier / static_cast<float>(texture.getSize().x)));
	sSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
	sprite = std::make_unique<sf::Sprite>(std::move(sSprite));
}

void Chess::Square::setTexture(const sf::Texture& texture) {
	if (sprite) {
		sprite->setTexture(texture);
	}
}

Chess::SavePosition::SavePosition(pieceVector& pieceList, const std::string& FEN, const std::string& moves, bool gameEnded, bool whiteToPlay, std::optional<sf::Vector2i> enPassantTarget, std::optional<sf::Vector2i> enPassantTripleTarget, bool extraFlagMove, int halfMoves, int fullMoves, int whiteChecksLeft,
	int blackChecksLeft, bool hasCheck, sf::Vector2i lastMoveStartLocal, sf::Vector2i lastMoveDestLocal, const std::vector<sf::Vector2i>& checkPositions, sf::Time whiteTime, sf::Time blackTime, const std::array<int, 4>& castlePieces, const std::array<bool, 4>& castlingRights)
	: whiteToPlay(whiteToPlay), extraFlagMove(extraFlagMove), FEN(FEN), moves(moves), halfMoves(halfMoves), fullMoves(fullMoves), whiteChecksLeft(whiteChecksLeft), blackChecksLeft(blackChecksLeft), hasCheck(hasCheck), lastMoveStartLocal(lastMoveStartLocal), lastMoveDestLocal(lastMoveDestLocal), checkPositions(checkPositions), whiteTime(whiteTime), blackTime(blackTime),
	pieceList(Chess::copyPieceVec(pieceList, false, true, true)), enPassantTarget(enPassantTarget), enPassantTripleTarget(enPassantTripleTarget), castlePieces(castlePieces), castlingRights(castlingRights), gameEnded(gameEnded)
{
	if (!moves.empty()) {
		size_t last = 0;
		for (size_t i = 0; i < moves.size(); i++) {
			if (moves.at(i) == ' ' && i != (moves.size() - 1)) {
				last = i;
			}
		}
		if (moves.size() > (last + 1)) {
			move = moves.substr((last + 1));
		}
	}
}

Chess::SavePosition::SavePosition(const SavePosition& other) : whiteToPlay(other.whiteToPlay), extraFlagMove(other.extraFlagMove), FEN(other.FEN), moves(other.moves), halfMoves(other.halfMoves), fullMoves(other.fullMoves), whiteChecksLeft(other.whiteChecksLeft), blackChecksLeft(other.blackChecksLeft), hasCheck(other.hasCheck),
	lastMoveStartLocal(other.lastMoveStartLocal), lastMoveDestLocal(other.lastMoveDestLocal), checkPositions(other.checkPositions), whiteTime(other.whiteTime), blackTime(other.blackTime), gameEnded(other.gameEnded),
	pieceList(Chess::copyPieceVec(other.pieceList, false, true, true)), move(other.move), enPassantTarget(other.enPassantTarget), enPassantTripleTarget(other.enPassantTripleTarget), castlePieces(other.castlePieces), castlingRights(other.castlingRights)
{
}

Chess::SavePosition& Chess::SavePosition::operator=(const SavePosition& other)
{
	if (this == &other) {
		return *this;
	}
	whiteToPlay = other.whiteToPlay;
	extraFlagMove = other.extraFlagMove;
	moves = other.moves;
	halfMoves = other.halfMoves;
	fullMoves = other.fullMoves;
	whiteChecksLeft = other.whiteChecksLeft;
	blackChecksLeft = other.blackChecksLeft;
	hasCheck = other.hasCheck;
	lastMoveStartLocal = other.lastMoveStartLocal;
	lastMoveDestLocal = other.lastMoveDestLocal;
	checkPositions = other.checkPositions;
	whiteTime = other.whiteTime;
	blackTime = other.blackTime;
	FEN = other.FEN;
	move = other.move;
	enPassantTarget = other.enPassantTarget;
	enPassantTripleTarget = other.enPassantTripleTarget;
	castlePieces = other.castlePieces;
	castlingRights = other.castlingRights;
	gameEnded = other.gameEnded;
	pieceList = Chess::copyPieceVec(other.pieceList, false, true, true);
	return *this;
}

bool Chess::SavePosition::operator==(const SavePosition& other)
{
    return FEN == other.FEN && moves == other.moves;
}

BoardSettings::BoardSettings(const std::vector<Chess::VariantType>& types) : variantIndex(0), variantTypeIndex(0), currentVariantIndex(0), currentVariantTypeIndex(0), pieceSet(0), boardSet(19), AI_Time(false), AI(false), AI_Only(false), chess960(false),
	endgamePosition(false), boardAnimated(true), repeatFEN(false), timeEnabled(false), xOffset(0), yOffset(0), boardScale(1.0f), FEN(std::nullopt), newPositionFEN(std::nullopt), white(true), newPositionWhite(true),
	whiteTime(sf::seconds(300)), blackTime(sf::seconds(300)), timeIncrement(sf::seconds(5)), keybindsEnabled(true),
	showOptionChanges(true), autoFlip(false), atomicExplosionEffect(true), overridePieceSpeed(false), instantPieces(false), isPaused(false), followMouse(false), scaleMouse(false), showMilliseconds(true),
	pieceSpeed(1.0f), promotionSquareColor({ 255, 30, 0 }), startingWhiteTime(sf::seconds(300)), startingBlackTime(sf::seconds(300)), millisecondsTime(sf::seconds(10)), variantList(&types.front().variants), saveFEN({}),
	millisecondsCondition([millisecondsTime = millisecondsTime](const sf::Time& t) { return t < millisecondsTime; }), whiteUnit(1), blackUnit(1), incrUnit(0), startingWhiteUnit(1), startingBlackUnit(1), millisecondsConditionID(0), sharedTime(false), cycleSides(false), pieceSetSave(0)
{
}

BoardSettings::BoardSettings(size_t variantIndex, size_t variantTypeIndex, int pieceSet, int boardSet, bool AI_Time, bool AI, bool AI_Only, bool chess960, bool endgamePosition, bool repeatFEN, bool timeEnabled, float xOffset, float yOffset, float boardScale,
	std::optional<std::string> FEN, std::optional<std::string> newPositionFEN, std::optional<bool> white, std::optional<bool> newPositionWhite, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement,
	bool keybindsEnabled, bool showOptionChanges, bool autoFlip, bool atomicExplosionEffect, bool boardAnimated, bool overridePieceSpeed, bool instantPieces, bool isPaused, bool followMouse, bool scaleMouse, bool showMilliseconds, float pieceSpeed, sf::Color promotionSquareColor,
	sf::Time startingWhiteTime, sf::Time startingBlackTime, sf::Time millisecondsTime, std::function<bool(const sf::Time&)> millisecondsCondition, int whiteUnit, int blackUnit, int incrUnit, int startingWhiteUnit, int startingBlackUnit, int millisecondsConditionID, bool sharedTime, bool cycleSides)
	: variantIndex(variantIndex), pieceSet(pieceSet), boardSet(boardSet), AI_Time(AI_Time), overridePieceSpeed(overridePieceSpeed), AI(AI), AI_Only(AI_Only), chess960(chess960), showMilliseconds(showMilliseconds),
	endgamePosition(endgamePosition), repeatFEN(repeatFEN), timeEnabled(timeEnabled), xOffset(xOffset), yOffset(yOffset), boardScale(boardScale), FEN(FEN),
	newPositionFEN(newPositionFEN), white(white), newPositionWhite(newPositionWhite), whiteTime(whiteTime), blackTime(blackTime), timeIncrement(timeIncrement),
	keybindsEnabled(keybindsEnabled), showOptionChanges(showOptionChanges), autoFlip(autoFlip), atomicExplosionEffect(atomicExplosionEffect), boardAnimated(boardAnimated), instantPieces(instantPieces), saveFEN(FEN),
	isPaused(isPaused), followMouse(followMouse), scaleMouse(scaleMouse), pieceSpeed(pieceSpeed), promotionSquareColor(promotionSquareColor), startingWhiteTime(startingWhiteTime), variantTypeIndex(variantTypeIndex), currentVariantIndex(variantIndex), currentVariantTypeIndex(variantTypeIndex), pieceSetSave(pieceSet),
	startingBlackTime(startingBlackTime), millisecondsTime(millisecondsTime), millisecondsCondition(millisecondsCondition), whiteUnit(whiteUnit), blackUnit(blackUnit), incrUnit(incrUnit), startingWhiteUnit(startingWhiteUnit), startingBlackUnit(startingBlackUnit), millisecondsConditionID(millisecondsConditionID), sharedTime(sharedTime), cycleSides(cycleSides)
{
}