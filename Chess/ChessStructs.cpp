#include "ChessStructs.h"
#include "Piece.h"
#include "King.h"
#include <TGUI/Widgets/Picture.hpp>

// Drop Piece
Chess::DropPiece::DropPiece(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i)
	: id(id), count(0), sprite(texture), background(backgroundTexture), textBG(textBackgroundTexture), text(font, std::to_string(count), 32)
{
	sprite.setPosition({ dropPiecePosition.x, bgStart + dropPieceSquareSize * i });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale({ dropPieceSquareSize / texture.getSize().x, dropPieceSquareSize / texture.getSize().y });
	background.setOrigin(background.getLocalBounds().getCenter());
	background.setScale(sprite.getScale());
	background.setPosition(sprite.getPosition());
	background.setColor({ 255, 255, 255, 0 });

	textBG.setOrigin(textBG.getLocalBounds().getCenter());
	textBG.setScale(sprite.getScale());
	textBG.setPosition(sprite.getPosition());

	text.setScale({ sprite.getScale().x * 3, sprite.getScale().y * 3 });
	text.setOrigin(text.getLocalBounds().getCenter());
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
	textBG.setScale(sprite.getScale());
	text.setScale({ sprite.getScale().x * 3, sprite.getScale().y * 3 });
	background.setScale(sprite.getScale());
	sprite.setPosition({ dropPieceBackground.getPosition().x, bgStart + dropPieceSquareSize * i });
	textBG.setPosition(sprite.getPosition());
	text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });
	background.setPosition(sprite.getPosition());
}

bool Chess::DropPiece::mouseSelecting(sf::Vector2f mousePos) const
{
	return count != 0 && sprite.getGlobalBounds().contains(mousePos);
}

void Chess::DropPiece::setupSprites(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultplier, float pieceScale, bool reversed)
{
	for (auto& square : dropSquares) {
		square.setupSprite(texture, boardOffset, boardSize, boardMultplier, pieceScale, reversed);
	}
}

void Chess::DropPiece::draw(sf::RenderWindow& window) const
{
	window.draw(background);
	window.draw(sprite);
	window.draw(textBG);
	window.draw(text);
}

// Promote Piece
Chess::PromotePiece::PromotePiece(Chess::IPromotePiece iPromotePiece, sf::Texture& pieceTexture, sf::Texture& backgroundTexture, sf::Vector2i pos, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, sf::Vector2f boardScale, bool reversed, sf::Color promotionSquareColor) :
	sprite(pieceTexture), backgroundSprite(backgroundTexture), id(iPromotePiece.id), initialScale(iPromotePiece.initialScale)
{
	backgroundSprite.setOrigin(backgroundSprite.getGlobalBounds().getCenter());
	backgroundSprite.setScale(boardScale);
	backgroundSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, reversed));
	backgroundSprite.setColor(promotionSquareColor);
	sprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, reversed));
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale(this->initialScale);
}

// Misc
std::vector<std::string> split(std::string str, char del) {
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < str.size(); i++) {
		if (str.at(i) == del) {
			res.push_back(temp);
			temp = "";
		}
		else {
			temp += str.at(i);
		}
	}
	res.push_back(temp);
	return res;
}

size_t findNthOf(std::string str, char find, int nth)
{
	int occ = 0;
	for (size_t i = 0; i < str.size(); i++) {
		if (str.at(i) == find) { occ++; }
		if (occ == nth) { return i; }
	}
	return 0;
}

// Bishop, King, Knight, Pawn, Queen, Rook
std::vector<sf::Texture> Chess::makePieceSet(sf::Image& spriteSheet, int pieceCount, int pieceSize) {
	int s = std::ceil((pieceCount / 4.0f));
	std::vector<sf::Texture> pieceTextures;
	for (int i = 0; i < pieceCount; i++) {
		int x = i % s;
		int y = std::floor(i / s);
		pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize * x, pieceSize * y }, { pieceSize, pieceSize })).setSmooth(true);
	}
	return pieceTextures;
};

void Chess::loadPieceSet(sf::Image& spriteSheet, std::vector<sf::Texture>& pieceTextures, int pieceSize) {
	int s = std::ceil((pieceTextures.size()) / 4.0f);
	for (int i = 0; i < pieceTextures.size(); i++) {
		int x = i % s;
		int y = std::floor(i / s);
		pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * x, pieceSize * y }, { pieceSize, pieceSize }));
	}
};

sf::Texture Chess::loadPieceSet(sf::Image& spriteSheet, int pieceSize) {
	return sf::Texture(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 1 }, { pieceSize, pieceSize }));
};

sf::Texture Chess::loadBoard(sf::Image& spriteSheet, int boardNumber, int boardSize) {
	int x = boardNumber % 4;
	int y = std::floor(boardNumber / 4.0f);
	return sf::Texture(spriteSheet, false, sf::IntRect({ x * boardSize, y * boardSize }, { boardSize, boardSize }));
}

sf::Texture Chess::loadBoard(sf::Image& spriteSheet, int boardNumber, int boardSize, int multiplier) {
	int y = std::floor(boardNumber / 4.0f);
	int x = boardNumber % 4;
	return sf::Texture(spriteSheet, false, sf::IntRect({ x * boardSize, y * boardSize }, { 128 * multiplier, 128 * multiplier }));
}

Chess::SDropPiece::SDropPiece(char id, sf::Texture& texture) : id(id), sprite(texture), sDropSquares(nullptr)
{
}

void Chess::SDropPiece::set(sf::Texture& texture, std::vector<Chess::Square>& dropSquares, int id)
{
	sprite.setTexture(texture);
	sDropSquares = &dropSquares;
	this->id = id;
}

void Chess::SDropPiece::draw(sf::RenderWindow& window)
{
	if (sDropSquares) {
		for (auto& square : *sDropSquares) {
			sf::Sprite& sprite = *square.sprite;
			window.draw(*square.sprite);
		}
	}
	window.draw(sprite);
}

bool Chess::DropPiece::isValidSquare(sf::Vector2i square)
{
	return true;
}

Chess::DropPawn::DropPawn(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i) :
	Chess::DropPiece(id, texture, backgroundTexture, textBackgroundTexture, font, dropPiecePosition, dropPieceSquareSize, bgStart, i)
{
}

bool Chess::DropPawn::isValidSquare(sf::Vector2i square)
{
	return square.y != 1 && square.y != 8;
}

// Square
Chess::Square::Square(sf::Sprite& sprite, sf::Vector2i position) : pos(position), moveString(""), sprite(std::make_unique<sf::Sprite>(sprite))
{
}

Chess::Square::Square(sf::Vector2i position, std::string moveString) : pos(position), moveString(moveString), sprite(nullptr)
{
}
	
Chess::Square::Square(const Chess::Square& other) : sprite(nullptr), pos(other.pos), moveString(other.moveString)
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
	return *this;
}

void Chess::Square::setupSprite(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed) {
	float scale = (pieceScale * 320.0f) / 128.0f;
	sf::Sprite sSprite{ texture };
	sSprite.setOrigin(sSprite.getLocalBounds().getCenter());
	sSprite.setScale(sf::Vector2f(scale, scale));
	sSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, reversed));
	sprite = std::make_unique<sf::Sprite>(sSprite);
}

void Chess::Square::setTexture(sf::Texture& texture) {
	if (sprite) {
		sprite->setTexture(texture);
	}
}

Chess::Position::Position(pieceVector& pieceList, std::string FEN, std::string moves, bool whiteToPlay, bool eighthRankWhite, int halfMoves, int fullMoves, int whiteChecks, int blackChecks, bool hasCheck, sf::Vector2i lastMoveStartLocal, sf::Vector2i lastMoveDestLocal, sf::Vector2i checkPos, sf::Time whiteTime, sf::Time blackTime)
	: whiteToPlay(whiteToPlay), eighthRankWhite(eighthRankWhite), FEN(FEN), moves(moves), halfMoves(halfMoves), fullMoves(fullMoves), whiteChecks(whiteChecks), blackChecks(blackChecks), hasCheck(hasCheck), lastMoveStartLocal(lastMoveStartLocal), lastMoveDestLocal(lastMoveDestLocal), checkPos(checkPos), whiteTime(whiteTime), blackTime(blackTime),
	pieceList(Chess::copyPieceVec(pieceList, false, true))
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

Chess::Position::Position(const Position& other) : 	whiteToPlay(other.whiteToPlay), eighthRankWhite(other.eighthRankWhite), FEN(other.FEN), moves(other.moves), halfMoves(other.halfMoves), fullMoves(other.fullMoves), whiteChecks(other.whiteChecks), blackChecks(other.blackChecks), hasCheck(other.hasCheck),
	lastMoveStartLocal(other.lastMoveStartLocal), lastMoveDestLocal(other.lastMoveDestLocal), checkPos(other.checkPos), whiteTime(other.whiteTime), blackTime(other.blackTime),
	pieceList(Chess::copyPieceVec(other.pieceList, false, true)), move(other.move)
{
}

Chess::Position& Chess::Position::operator=(const Position& other)
{
	if (this == &other) {
		return *this;
	}
	whiteToPlay = other.whiteToPlay;
	eighthRankWhite = other.eighthRankWhite;
	moves = other.moves;
	halfMoves = other.halfMoves;
	fullMoves = other.fullMoves;
	whiteChecks = other.whiteChecks;
	blackChecks = other.blackChecks;
	hasCheck = other.hasCheck;
	lastMoveStartLocal = other.lastMoveStartLocal;
	lastMoveDestLocal = other.lastMoveDestLocal;
	checkPos = other.checkPos;
	whiteTime = other.whiteTime;
	blackTime = other.blackTime;
	FEN = other.FEN;
	move = other.move;
	pieceList = Chess::copyPieceVec(other.pieceList, false, true);
	return *this;
}

bool Chess::Position::operator==(const Position& other)
{
    return FEN == other.FEN && moves == other.moves;
}

Chess::IPromotePiece::IPromotePiece(char id, sf::Vector2f scale) : id(id), initialScale(scale)
{
}

BoardSettings::BoardSettings() : variant(Chess::Variant::Standard), pieceSet(1), boardSet(19), AI_Time(false), AI(false), AI_Only(false), chess960(false),
	endgamePosition(false), boardAnimated(true), repeatFEN(false), timeEnabled(false), xOffset(0), yOffset(0), boardScale(1.0f), FEN(std::nullopt), newPositionFEN(std::nullopt), white(true), newPositionWhite(true),
	whiteTime(sf::seconds(300)), blackTime(sf::seconds(300)), timeIncrement(sf::seconds(5)), keybindsEnabled(true),
	showOptionChanges(true), autoFlip(false), atomicExplosionEffect(true), overridePieceSpeed(false), instantPieces(false), isPaused(false), followMouse(false), scaleMouse(false), showMilliseconds(true),
	pieceSpeed(1.0f), promotionSquareColor({ 255, 30, 0 }), startingWhiteTime(sf::seconds(300)), startingBlackTime(sf::seconds(300)), millisecondsTime(sf::seconds(10)),
	millisecondsCondition([millisecondsTime = millisecondsTime](const sf::Time& t) { return t < millisecondsTime; }), whiteUnit(1), blackUnit(1), incrUnit(0), startingWhiteUnit(1), startingBlackUnit(1), millisecondsConditionID(0), sharedTime(false), cycleSides(false)
{
}

BoardSettings::BoardSettings(Chess::Variant variant, int pieceSet, int boardSet, bool AI_Time, bool AI, bool AI_Only, bool chess960, bool endgamePosition, bool repeatFEN, bool timeEnabled, float xOffset, float yOffset, float boardScale,
	std::optional<std::string> FEN, std::optional<std::string> newPositionFEN, std::optional<bool> white, std::optional<bool> newPositionWhite, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement,
	bool keybindsEnabled, bool showOptionChanges, bool autoFlip, bool atomicExplosionEffect, bool boardAnimated, bool overridePieceSpeed, bool instantPieces, bool isPaused, bool followMouse, bool scaleMouse, bool showMilliseconds, float pieceSpeed, sf::Color promotionSquareColor,
	sf::Time startingWhiteTime, sf::Time startingBlackTime, sf::Time millisecondsTime, std::function<bool(const sf::Time&)> millisecondsCondition, int whiteUnit, int blackUnit, int incrUnit, int startingWhiteUnit, int startingBlackUnit, int millisecondsConditionID, bool sharedTime, bool cycleSides)
	: variant(variant), pieceSet(pieceSet), boardSet(boardSet), AI_Time(AI_Time), overridePieceSpeed(overridePieceSpeed), AI(AI), AI_Only(AI_Only), chess960(chess960), showMilliseconds(showMilliseconds),
	endgamePosition(endgamePosition), repeatFEN(repeatFEN), timeEnabled(timeEnabled), xOffset(xOffset), yOffset(yOffset), boardScale(boardScale), FEN(FEN),
	newPositionFEN(newPositionFEN), white(white), newPositionWhite(newPositionWhite), whiteTime(whiteTime), blackTime(blackTime), timeIncrement(timeIncrement),
	keybindsEnabled(keybindsEnabled), showOptionChanges(showOptionChanges), autoFlip(autoFlip), atomicExplosionEffect(atomicExplosionEffect), boardAnimated(boardAnimated), instantPieces(instantPieces),
	isPaused(isPaused), followMouse(followMouse), scaleMouse(scaleMouse), pieceSpeed(pieceSpeed), promotionSquareColor(promotionSquareColor), startingWhiteTime(startingWhiteTime),
	startingBlackTime(startingBlackTime), millisecondsTime(millisecondsTime), millisecondsCondition(millisecondsCondition), whiteUnit(whiteUnit), blackUnit(blackUnit), incrUnit(incrUnit), startingWhiteUnit(startingWhiteUnit), startingBlackUnit(startingBlackUnit), millisecondsConditionID(millisecondsConditionID), sharedTime(sharedTime), cycleSides(cycleSides)
{
}