#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include "Piece.h"
#include "Bishop.h"
#include "King.h"
#include "Knight.h"
#include "Rook.h"
#include "Queen.h"
#include "Pawn.h"

using namespace Chess;
class Main {

public:
	static sf::Vector2f Interpolate(const sf::Vector2f& pointA, const sf::Vector2f& pointB, float factor) {
		factor = std::clamp(factor, 0.0f, 1.0f);
		if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 1.0f) {
			return pointB;
		}
		return pointA + (pointB - pointA) * factor;
	}

	static sf::Vector2f Interpolate(const sf::Vector2f& pointA, const sf::Vector2f& pointB, float factor, float threshold) {
		factor = std::clamp(factor, 0.0f, 1.0f);
		if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < threshold) {
			return pointB;
		}
		return pointA + (pointB - pointA) * factor;
	}

	// Bishop, King, Knight, Pawn, Queen, Rook
	static void loadPieceSet(sf::Image spriteSheet, std::vector<std::reference_wrapper<sf::Texture>>& pieceTextures, int pieceSize) {
		for (size_t i = 0; i < pieceTextures.size(); i++) {
			pieceTextures.at(i).get().setSmooth(true);
			switch (i) {
				// Black
			case 0:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
				break;
			case 1:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, 0 }, { pieceSize, pieceSize }));
				break;
			case 2:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, 0 }, { pieceSize, pieceSize }));
				break;
			case 3:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize }, { pieceSize, pieceSize }));
				break;
			case 4:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize }, { pieceSize, pieceSize }));
				break;
			case 5:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize }, { pieceSize, pieceSize }));
				break;
				// White
			case 6:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize * 2 }, { pieceSize, pieceSize }));
				break;
			case 7:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 2 }, { pieceSize, pieceSize }));
				break;
			case 8:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 2 }, { pieceSize, pieceSize }));
				break;
			case 9:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize * 3 }, { pieceSize, pieceSize }));
				break;
			case 10:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 3 }, { pieceSize, pieceSize }));
				break;
			case 11:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 3 }, { pieceSize, pieceSize }));
				break;
				// Default
			default:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
				break;
			}
		}
	};
	static void loadBoard(sf::Image spriteSheet, sf::Sprite& board, sf::Texture& boardTexture, int boardNumber, int boardSize) {
		int y = std::floor(boardNumber / 4.0f);
		int x = boardNumber % 4;
		boardTexture.loadFromImage(spriteSheet, false, sf::IntRect({ x * boardSize, y * boardSize }, { boardSize, boardSize }));
		board.setTexture(boardTexture);
	}

	// static std::shared_ptr<Piece>
	static std::shared_ptr<Piece> getPieceFromPosition(sf::Vector2i position, std::vector<std::shared_ptr<Piece>>& pieces)
	{
		for (auto& piece : pieces) {
			if (piece->position == position) {
				return piece;
			}
		}
		return nullptr;
	}

	static std::vector<std::string> split(std::string str, char del) {
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

	static sf::Vector2f getGlobalPosition(sf::Vector2i position, float boardOffset, float boardMultiplier) {
		return sf::Vector2f{ boardOffset + ((position.x - 0.5f) * boardMultiplier), (reverseY(position.y) - 0.5f) * boardMultiplier };
	}

	static sf::Vector2i getLocalPosition(sf::Vector2f position, float boardOffset, float boardMultiplier) {
		sf::Vector2f v = { std::ceil((position.x - boardOffset) / boardMultiplier), 9 - std::ceil((position.y / boardMultiplier)) };
		return (sf::Vector2i)v;
	}

	static sf::Vector2i convertChessNotationtoPosition(std::string notation) {
		int x = (notation.at(0) - 'a') + 1;
		int y = (notation.at(1) - '0');
		return { x, y };
	}

	static std::string convertPositiontoNotation(sf::Vector2i position) {
		std::string notation;
		int x = position.x;
		int y = position.y;
		notation += (x + 'a') - 1;
		notation += (y + '0');
		return notation;
	}

	static bool noPieceAt(std::string notation, std::vector<std::shared_ptr<Piece>>& pieces) {
		return getPieceFromPosition(convertChessNotationtoPosition(notation), pieces) == nullptr;
	}

	static int reverseY(int y) {
		return 9 - y;
	}

	static int convertXtoChar(int x) {
		return convertPositiontoNotation({ x + 1, 0 }).front();
	}

	static int convertChartoX(char c) {
		return c - 'a';
	}

	// Bishop, King, Knight, Pawn, Queen, Rook
	// Black --> White
	static std::vector<std::shared_ptr<Piece>> generatePositionFromFENID(std::string code, std::vector<std::reference_wrapper<sf::Texture>>& pieceTextures,
		float pieceScale, float boardOffset, float boardSquareOffset, bool& whiteToPlay, int& halfMoves, int& fullMoves, std::shared_ptr<Pawn>& enPassantPiece,
		sf::Sprite& checkSprite, bool& check, std::vector<std::reference_wrapper<sf::Texture>>& extraTextures, bool animated, bool& standardPosition, int& wKRook, int& wQRook, int& bKRook, int& bQRook) {
		std::vector<std::shared_ptr<Piece>> pieces;
		std::vector<std::string> splitString = split(code, ' ');
		// ========= MODIFIERS =========
		int whiteKingX = 0, blackKingX = 0;
		bool whiteCanNeverCastleK = true, whiteCanNeverCastleQ = true;
		bool blackCanNeverCastleK = true, blackCanNeverCastleQ = true;
		std::optional<sf::Vector2i> enPassantTarget{ };
		std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
		std::vector<std::string> ranks = split(splitString.front(), '/');
		std::vector<char> start, end;
		std::vector<char> whiteKingSideCharacters, whiteQueenSideCharacters, blackKingSideCharacters, blackQueenSideCharacters;
		if (ranks.front().front() == 'r' && ranks.front().at(4) == 'k' && ranks.front().back() == 'r') {
			if (ranks.back().front() == 'R' && ranks.back().at(4) == 'K' && ranks.back().back() == 'R') {
				standardPosition = true;
			}
		}


		for (int i = 0; i < modifiers.size(); i++) {
			std::string modifier = modifiers.at(i);
			switch (i) {
				// Side to Play
			case 0:
				if (modifier.front() == 'w') {
					whiteToPlay = true;
				}
				else if (modifier.front() == 'b') {
					whiteToPlay = false;
				}
				break;
				// Castling Rights
			case 1:
				if (modifier.front() == '-') {
					break;
				}
				for (int j = 0; j < ranks.front().size(); j++) {
					if (std::isdigit(ranks.front().at(j))) {
						int num = ranks.front().at(j) - '0';
						for (int k = 0; k < num; k++) {
							start.push_back('0');
						}
					}
					else {
						start.push_back(ranks.front().at(j));
					}
				}
				for (int j = 0; j < ranks.back().size(); j++) {
					if (std::isdigit(ranks.back().at(j))) {
						int num = ranks.back().at(j) - '0';
						for (int k = 0; k < num; k++) {
							end.push_back('0');
						}
					}
					else {
						end.push_back(ranks.back().at(j));
					}
				}
				for (int j = 0; j < end.size(); j++) {
					if (end.at(j) == 'K') {
						whiteKingX = j + 1;
					}
				}
				for (int j = 0; j < start.size(); j++) {
					if (start.at(j) == 'k') {
						blackKingX = j + 1;
					}
				}
				for (auto& l : modifier) {
					if (l == 'Q') {
						whiteCanNeverCastleQ = false;
						for (int k = 0; k < whiteKingX; k++) {
							if (end.at(k) == 'R') {
								wQRook = k + 1;
							}
						}
					}
					else if (l == 'K') {
						whiteCanNeverCastleK = false;
						for (int k = whiteKingX; k < 8; k++) {
							if (end.at(k) == 'R') {
								wKRook = k + 1;
							}
						}
					}
					else if (l == 'q') {
						blackCanNeverCastleQ = false;
						for (int k = 0; k < blackKingX; k++) {
							if (start.at(k) == 'r') {
								bQRook = k + 1;
							}
						}
					}
					else if (l == 'k') {
						blackCanNeverCastleK = false;
						for (int k = blackKingX; k < 8; k++) {
							if (start.at(k) == 'r') {
								bKRook = k + 1;
							}
						}
					}
				}
				// SHREDDER FEN
				for (int k = 0; k < whiteKingX - 1; k++) {
					whiteQueenSideCharacters.push_back(convertXtoChar(k));
				}
				for (int k = whiteKingX; k < 8; k++) {
					whiteKingSideCharacters.push_back(convertXtoChar(k));
				}
				for (int k = 0; k < blackKingX - 1; k++) {
					blackQueenSideCharacters.push_back(convertXtoChar(k));
				}
				for (int k = blackKingX; k < 8; k++) {
					blackKingSideCharacters.push_back(convertXtoChar(k));
				}

				for (int j = 0; j < modifier.size(); j++) {
					char letter = modifier.at(j);
					if (isupper(letter)) {
						if (std::any_of(whiteKingSideCharacters.begin(), whiteKingSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wKRook = x + 1;
								whiteCanNeverCastleK = false;
							}
						}
						else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wQRook = x + 1;
								whiteCanNeverCastleQ = false;
							}
						}
					}
					else {
						if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = convertChartoX(std::tolower(letter));
							if (start.at(x) == 'r') {
								bKRook = x + 1;
								blackCanNeverCastleK = false;
							}
						}
						else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = convertChartoX(std::tolower(letter));
							if (start.at(x) == 'r') {
								bQRook = x + 1;
								blackCanNeverCastleQ = false;
							}
						}
					}
				}
				break;
				// En Passant Target
			case 2:
				if (modifier.at(0) != '-') {
					enPassantTarget = convertChessNotationtoPosition(modifier);
				}
				else {
					enPassantTarget = {};
				}
				break;
				// Half Moves
			case 3:
				halfMoves = std::stoi(modifier);
				break;
				// Full Moves
			case 4:
				fullMoves = std::stoi(modifier);
				break;
			}
		}
		// ========= RANKS =============

		for (int i = 0; i < ranks.size(); i++) {
			int x = 1;
			int y = reverseY(i + 1);
			std::string rank = ranks.at(i);
			for (int j = 0; j < rank.size(); j++) {
				char letter = rank.at(j);
				if (std::isdigit(letter)) {
					x += letter - '0';
				}
				else {
					switch (letter) {
						// BLACK
					case 'b':
					{
						pieces.push_back(std::make_shared<Bishop>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(0), animated));
						break;
					}
					case 'k':
					{
						pieces.push_back(std::make_shared<King>(blackCanNeverCastleK, blackCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(1), animated));
						break;
					}
					case 'n':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(2), animated));
						break;
					}
					case 'p':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y - 1 }) {
							std::shared_ptr<Pawn> p = std::make_shared<Pawn>(true, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(3), animated);
							enPassantPiece = p;
							pieces.push_back(p);
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(3), animated));
						}
						break;
					}
					case 'q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(4), animated));
						break;
					}
					case 'r':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::Black, pieceTextures.at(5), animated));
						break;
					}
					// WHITE
					case 'B':
					{
						pieces.push_back(std::make_shared<Bishop>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(6), animated));
						break;
					}
					case 'K':
					{
						pieces.push_back(std::make_shared<King>(whiteCanNeverCastleK, whiteCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(7), animated));
						break;
					}
					case 'N':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(8), animated));
						break;
					}
					case 'P':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y + 1 }) {
							std::shared_ptr<Pawn> p = std::make_shared<Pawn>(true, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(9), animated);
							enPassantPiece = p;
							pieces.push_back(p);
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(9), animated));
						}
						break;
					}
					case 'Q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(10), animated));
						break;
					}
					case 'R':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, pieceScale, boardOffset, boardSquareOffset, PColor::White, pieceTextures.at(11), animated));
						break;
					}
					}
					x++;
				}
			}

		}
		for (auto& p : pieces) {
			if (p->name != "King") {
				Main::calculatePositions(p, pieces, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		// Check Detection
		for (auto& p : pieces) {
			if (p->name == "King") {
				std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
				k->inCheck = false;
			}
		}
		check = false;
		for (auto& p : pieces) {
			if (p->name == "King" && whiteToPlay == (p->color == PColor::White)) {
				for (auto& p2 : pieces) {
					if (p2->color != p->color) {
						std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
						k->inCheck = false;
						for (auto& pos : p2->getAvailableCapturePositions()) {
							if (pos == k->getLocalPosition()) {
								k->inCheck = true;
								if (animated) {
									checkSprite.setPosition(k->animationTarget.value());
								}
								else {
									checkSprite.setPosition(k->getGlobalPosition());
								}
								check = true;
								goto next;
							}
						}
					}
				}
			}
		}
	next:
		for (auto& p : pieces) {
			if (p->name == "King") {
				Main::calculatePositions(p, pieces, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		Main::setExtraSprites(pieces, extraTextures);
		return pieces;
	}

	// Simplify later
	static void calculateCastlingRights(std::shared_ptr<King>& king, std::vector<std::shared_ptr<Piece>>& pieces, bool whiteToPlay, int wKRook, int wQRook, int bKRook, int bQRook) {
		if (king != nullptr) {
			king->canCastleK = false;
			king->canCastleQ = false;
			// WHITE
			PColor color = king->color;
			if (!king->hasMoved && !king->inCheck && king->isWhite()) {
				// Kingside
				if (!king->canNeverCastleK && wKRook != -1) {
					if (getPieceFromPosition({ wKRook, 1 }, pieces) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ wKRook, 1 }, pieces);
						if (rook->name == "Rook" && rook->color == color && !rook->hasMoved) {
							std::vector<sf::Vector2i> castleSquares;
							if (king->x > rook->x) {
								for (int i = rook->x + 1; i < king->x; i++) {
									castleSquares.push_back({ i, 1 });
								}
							}
							else {
								for (int i = king->x + 1; i < rook->x; i++) {
									castleSquares.push_back({ i, 1 });
								}
							}
							auto find = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("f1"));
							if (find == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("f1"));
							}
							auto find2 = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("g1"));
							if (find2 == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("g1"));
							}
							bool capture = false;
							for (auto& sq : castleSquares) {
								if (getPieceFromPosition(sq, pieces) != nullptr) {
									if (sq != rook->getLocalPosition()) {
										if (sq != king->getLocalPosition()) {
											goto Wqueenside;
										}
									}
									else {
										capture = true;
									}
								}
							}
							for (auto& sq : castleSquares) {
								for (auto& piece3 : pieces) {
									if (piece3->color != color) {
										for (auto& square : piece3->availablePositions) {
											if (square.y == 1) {
												if (square == sq) {
													goto Wqueenside;
												}
											}
										}
										if (capture) {
											for (auto& square : piece3->availableCapturePositions) {
												if (square.y == 1) {
													if (square == sq) {
														goto Wqueenside;
													}
												}
											}
										}
									}
								}
							}
							king->canCastleK = true;
						}
					}
				}
			Wqueenside:
				if (!king->canNeverCastleQ && wQRook != -1) {
					if (getPieceFromPosition({ wQRook, 1 }, pieces) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ wQRook, 1 }, pieces);
						if (rook->name == "Rook" && rook->color == color && !rook->hasMoved) {
							std::vector<sf::Vector2i> castleSquares;
							if (king->x > rook->x) {
								for (int i = rook->x + 1; i < king->x; i++) {
									castleSquares.push_back({ i, 1 });
								}
							}
							else {
								for (int i = king->x + 1; i < rook->x; i++) {
									castleSquares.push_back({ i, 1 });
								}
							}
							auto find = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("c1"));
							if (find == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("c1"));
							}
							auto find2 = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("d1"));
							if (find2 == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("d1"));
							}
							bool capture = false;
							for (auto& sq : castleSquares) {
								if (getPieceFromPosition(sq, pieces) != nullptr) {
									if (sq != rook->getLocalPosition()) {
										if (sq != king->getLocalPosition()) {
											return;
										}
									}
									else {
										capture = true;
									}
								}
							}
							for (auto& sq : castleSquares) {
								for (auto& piece3 : pieces) {
									if (piece3->color != color) {
										for (auto& square : piece3->availablePositions) {
											if (square.y == 1) {
												if (square == sq) {
													return;
												}
											}
										}
										if (capture) {
											for (auto& square : piece3->availableCapturePositions) {
												if (square.y == 1) {
													if (square == sq) {
														return;
													}
												}
											}
										}
									}
								}
							}
							king->canCastleQ = true;
						}
					}
				}
			}
			// BLACK
			else if (!king->hasMoved && !king->inCheck && king->isBlack()) {
				// Kingside
				if (!king->canNeverCastleK && bKRook != -1) {
					if (getPieceFromPosition({ bKRook, 8 }, pieces) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ bKRook, 8 }, pieces);
						if (rook->name == "Rook" && rook->color == color && !rook->hasMoved) {
							std::vector<sf::Vector2i> castleSquares;
							if (king->x > rook->x) {
								for (int i = rook->x + 1; i < king->x; i++) {
									castleSquares.push_back({ i, 8 });
								}
							}
							else {
								for (int i = king->x + 1; i < rook->x; i++) {
									castleSquares.push_back({ i, 8 });
								}
							}
							auto find = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("f8"));
							if (find == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("f8"));
							}
							auto find2 = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("g8"));
							if (find2 == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("g8"));
							}
							bool capture = false;
							for (auto& sq : castleSquares) {
								if (getPieceFromPosition(sq, pieces) != nullptr) {
									if (sq != rook->getLocalPosition()) {
										if (sq != king->getLocalPosition()) {
											goto Bqueenside;
										}
									}
									else {
										capture = true;
									}
								}
							}
							for (auto& sq : castleSquares) {
								for (auto& piece3 : pieces) {
									if (piece3->color != color) {
										for (auto& square : piece3->availablePositions) {
											if (square.y == 8) {
												if (square == sq) {
													goto Bqueenside;
												}
											}
										}
										if (capture) {
											for (auto& square : piece3->availableCapturePositions) {
												if (square.y == 8) {
													if (square == sq) {
														goto Bqueenside;
													}
												}
											}
										}
									}
								}
							}
							king->canCastleK = true;
						}
					}
				}
			Bqueenside:
				if (!king->canNeverCastleQ && bQRook != -1) {
					if (getPieceFromPosition({ bQRook, 8 }, pieces) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ bQRook, 8 }, pieces);
						if (rook->name == "Rook" && rook->color == color && !rook->hasMoved) {
							std::vector<sf::Vector2i> castleSquares;
							if (king->x > rook->x) {
								for (int i = rook->x + 1; i < king->x; i++) {
									castleSquares.push_back({ i, 8 });
								}
							}
							else {
								for (int i = king->x + 1; i < rook->x; i++) {
									castleSquares.push_back({ i, 8 });
								}
							}
							auto find = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("c8"));
							if (find == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("c8"));
							}
							auto find2 = std::find(castleSquares.begin(), castleSquares.end(), convertChessNotationtoPosition("d8"));
							if (find2 == castleSquares.end()) {
								castleSquares.push_back(convertChessNotationtoPosition("d8"));
							}
							bool capture = false;
							for (auto& sq : castleSquares) {
								if (getPieceFromPosition(sq, pieces) != nullptr) {
									if (sq != rook->getLocalPosition()) {
										if (sq != king->getLocalPosition()) {
											return;
										}
									}
									else {
										capture = true;
									}
								}
							}
							for (auto& sq : castleSquares) {
								for (auto& piece3 : pieces) {
									if (piece3->color != color) {
										for (auto& square : piece3->availablePositions) {
											if (square.y == 8) {
												if (square == sq) {
													return;
												}
											}
										}
										if (capture) {
											for (auto& square : piece3->availableCapturePositions) {
												if (square.y == 8) {
													if (square == sq) {
														return;
													}
												}
											}
										}
									}
								}
							}
							king->canCastleQ = true;
						}
					}
				}
			}
		}
	}



	static bool isValidSquare(sf::Vector2i square) {
		return (square.x <= 8 && square.x >= 1 && square.y <= 8 && square.y >= 1);
	}

	static bool isValidPosition(std::vector<std::shared_ptr<Piece>>& position, PColor color) {
		for (auto& piece : position) {
			// Bishop + Queen
			if (piece->color != color) {
				if (piece->name == "Bishop" || piece->name == "Queen") {
					// Top Right Diag
					for (int i = 1; i < 8; i++) {
						int newX = piece->x + i, newY = piece->y + i;
						if (std::max(newX, newY) > 8) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Top Left Diag
					for (int i = 1; i < 8; i++) {
						int newX = piece->x - i, newY = piece->y + i;
						if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Bottom Right Diag
					for (int i = 1; i < 8; i++) {
						int newX = piece->x + i, newY = piece->y - i;
						if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Bottom Left Diag
					for (int i = 1; i < 8; i++) {
						int newX = piece->x - i, newY = piece->y - i;
						if (std::min(newX, newY) < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					if (piece->name == "Queen") {
						// Up
						for (int i = 1; i < 8; i++) {
							int newX = piece->x, newY = piece->y + i;
							if (newY > 8) { break; }
							sf::Vector2i newPos = { newX, newY };
							if (getPieceFromPosition(newPos, position) != nullptr) {
								std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
								else {
									break;
								}
							}
						}
						// Down
						for (int i = 1; i < 8; i++) {
							int newX = piece->x, newY = piece->y - i;
							if (newY < 1) { break; }
							sf::Vector2i newPos = { newX, newY };
							if (getPieceFromPosition(newPos, position) != nullptr) {
								std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
								else {
									break;
								}
							}
						}
						// Right
						for (int i = 1; i < 8; i++) {
							int newX = piece->x + i, newY = piece->y;
							if (newX > 8) { break; }
							sf::Vector2i newPos = { newX, newY };
							if (getPieceFromPosition(newPos, position) != nullptr) {
								std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
								else {
									break;
								}
							}
						}
						// Left
						for (int i = 1; i < 8; i++) {
							int newX = piece->x - i, newY = piece->y;
							if (newX < 1) { break; }
							sf::Vector2i newPos = { newX, newY };
							if (getPieceFromPosition(newPos, position) != nullptr) {
								std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
								else {
									break;
								}
							}
						}
					}
				}
				// Rook
				else if (piece->name == "Rook") {
					// Up
					for (int i = 1; i < 8; i++) {
						int newX = piece->x, newY = piece->y + i;
						if (newY > 8) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Down
					for (int i = 1; i < 8; i++) {
						int newX = piece->x, newY = piece->y - i;
						if (newY < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Right
					for (int i = 1; i < 8; i++) {
						int newX = piece->x + i, newY = piece->y;
						if (newX > 8) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
					// Left
					for (int i = 1; i < 8; i++) {
						int newX = piece->x - i, newY = piece->y;
						if (newX < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
							else {
								break;
							}
						}
					}
				}
				// Knight
				else if (piece->name == "Knight") {
					sf::Vector2i offsets[8] = { {-2, -1 }, {-2, 1 }, { 2, -1 }, { 2, 1 }, { -1, -2 }, { -1, 2 }, { 1, -2 }, { 1, 2 } };
					for (int i = 0; i < 8; i++) {
						sf::Vector2i newPos = { piece->x + offsets[i].x, piece->y + offsets[i].y };
						if (isValidSquare(newPos)) {
							if (getPieceFromPosition(newPos, position) != nullptr) {
								std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
							}
						}
					}
				}
				// Pawn
				else if (piece->name == "Pawn") {
					sf::Vector2i pos, pos2;
					if (piece->color == PColor::White) {
						pos = { piece->x - 1, piece->y + 1 }, pos2 = { piece->x + 1, piece->y + 1 };
					}
					else {
						pos = { piece->x - 1, piece->y - 1 }, pos2 = { piece->x + 1, piece->y - 1 };
					}
					if (isValidSquare(pos)) {
						if (getPieceFromPosition(pos, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(pos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
						}
					}
					if (isValidSquare(pos2)) {
						if (getPieceFromPosition(pos2, position) != nullptr) {
							std::shared_ptr<Piece> p = getPieceFromPosition(pos2, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
						}
					}
				}
				// King
				else if (piece->name == "King") {
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1; y++) {
							if (x != 0 || y != 0) {
								sf::Vector2i newPos = { piece->x + x, piece->y + y };
								if (isValidSquare(newPos)) {
									if (getPieceFromPosition(newPos, position) != nullptr) {
										std::shared_ptr<Piece> p = getPieceFromPosition(newPos, position);
										if (p->name == "King" && p->color != piece->color) {
											return false;
										}
									}
								}
							}
						}
					}
				}

			}
		}
		return true;
	}

	// Selection, Capture, Check, Last Move, Selection Hover
	static void setExtraSprites(std::vector<std::shared_ptr<Piece>>& pieces, std::vector<std::reference_wrapper<sf::Texture>>& extraTextures) {
		for (auto& piece : pieces) {
			piece->selectionSquares.clear();
			float pieceScale = piece->getScale();
			float boardOffset = piece->getBoardOffset();
			float boardMultiplier = piece->getBoardMultiplier();
			for (auto& square : piece->availablePositions) {
				sf::Sprite newSquare{ extraTextures.at(0) };
				float scale = (pieceScale * 320.0f) / 128.0f;
				newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
				newSquare.setScale(sf::Vector2f(scale, scale));
				newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
				piece->selectionSquares.push_back(newSquare);
			}
			piece->captureSquares.clear();
			for (auto& square : piece->availableCapturePositions) {
				sf::Sprite newSquare{ extraTextures.at(1) };
				float scale = (pieceScale * 320.0f) / 128.0f;
				newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
				newSquare.setScale(sf::Vector2f(scale, scale));
				newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
				piece->captureSquares.push_back(newSquare);
			}
			if (piece->name == "Pawn") {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
				pawn->enPassantSquares.clear();
				for (auto& square : pawn->enPassantPositions) {
					sf::Sprite newSquare{ extraTextures.at(0) };
					float scale = (pieceScale * 320.0f) / 128.0f;
					newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
					newSquare.setScale(sf::Vector2f(scale, scale));
					newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
					pawn->enPassantSquares.push_back(newSquare);
				}
			}
			if (piece->name == "King") {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				king->castleSquares.clear();
				king->castleCaptureSquares.clear();
				for (auto& square : king->castlePositions) {
					sf::Sprite newSquare{ extraTextures.at(0) };
					float scale = (pieceScale * 320.0f) / 128.0f;
					newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
					newSquare.setScale(sf::Vector2f(scale, scale));
					newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
					king->castleSquares.push_back(newSquare);
				}
				for (auto& square : king->captureCastlePositions) {
					sf::Sprite newSquare{ extraTextures.at(1) };
					float scale = (pieceScale * 320.0f) / 128.0f;
					newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
					newSquare.setScale(sf::Vector2f(scale, scale));
					newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
					king->castleCaptureSquares.push_back(newSquare);
				}
			}
		}
	}

	static void calculatePositions(std::shared_ptr<Piece> piece, std::vector<std::shared_ptr<Piece>>& position, bool whiteToPlay, bool standardPosition, int wKRook, int wQRook, int bKRook, int bQRook) {
		piece->canMove = false;
		// Clear all Position Vectors
		piece->availablePositions.clear();
		piece->availableCapturePositions.clear();
		if (piece->name == "King") {
			std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
			king->castlePositions.clear();
			king->captureCastlePositions.clear();
		}
		else if (piece->name == "Pawn") {
			std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
			pawn->enPassantPositions.clear();
		}

		sf::Vector2i pos = piece->position;
		{
			// Bishop + Queen
			if (piece->name == "Bishop" || piece->name == "Queen") {
				// Top Right Diag
				for (int i = 1; i < 8; i++) {
					int newX = piece->x + i, newY = piece->y + i;
					if (std::max(newX, newY) > 8) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);
						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);
					}
				}
				// Top Left Diag
				for (int i = 1; i < 8; i++) {
					int newX = piece->x - i, newY = piece->y + i;
					if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);
						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);
					}
				}
				// Bottom Right Diag
				for (int i = 1; i < 8; i++) {
					int newX = piece->x + i, newY = piece->y - i;
					if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
				// Bottom Left Diag
				for (int i = 1; i < 8; i++) {
					int newX = piece->x - i, newY = piece->y - i;
					if (std::min(newX, newY) < 1) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
				if (piece->name == "Queen") {
					// Up
					for (int i = 1; i < 8; i++) {
						int newX = piece->x, newY = piece->y + i;
						if (newY > 8) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							if (getPieceFromPosition(newPos, position)->color != piece->color) {
								piece->availableCapturePositions.push_back(newPos);

							}
							break;
						}
						else {
							piece->availablePositions.push_back(newPos);

						}
					}
					// Down
					for (int i = 1; i < 8; i++) {
						int newX = piece->x, newY = piece->y - i;
						if (newY < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							if (getPieceFromPosition(newPos, position)->color != piece->color) {
								piece->availableCapturePositions.push_back(newPos);

							}
							break;
						}
						else {
							piece->availablePositions.push_back(newPos);

						}
					}
					// Right
					for (int i = 1; i < 8; i++) {
						int newX = piece->x + i, newY = piece->y;
						if (newX > 8) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							if (getPieceFromPosition(newPos, position)->color != piece->color) {
								piece->availableCapturePositions.push_back(newPos);
							}
							break;
						}
						else {
							piece->availablePositions.push_back(newPos);

						}
					}
					// Left
					for (int i = 1; i < 8; i++) {
						int newX = piece->x - i, newY = piece->y;
						if (newX < 1) { break; }
						sf::Vector2i newPos = { newX, newY };
						if (getPieceFromPosition(newPos, position) != nullptr) {
							if (getPieceFromPosition(newPos, position)->color != piece->color) {
								piece->availableCapturePositions.push_back(newPos);

							}
							break;
						}
						else {
							piece->availablePositions.push_back(newPos);

						}
					}
				}
			}
			// Rook
			else if (piece->name == "Rook") {
				// Up
				for (int i = 1; i < 8; i++) {
					int newX = piece->x, newY = piece->y + i;
					if (newY > 8) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
				// Down
				for (int i = 1; i < 8; i++) {
					int newX = piece->x, newY = piece->y - i;
					if (newY < 1) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
				// Right
				for (int i = 1; i < 8; i++) {
					int newX = piece->x + i, newY = piece->y;
					if (newX > 8) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
				// Left
				for (int i = 1; i < 8; i++) {
					int newX = piece->x - i, newY = piece->y;
					if (newX < 1) { break; }
					sf::Vector2i newPos = { newX, newY };
					if (getPieceFromPosition(newPos, position) != nullptr) {
						if (getPieceFromPosition(newPos, position)->color != piece->color) {
							piece->availableCapturePositions.push_back(newPos);

						}
						break;
					}
					else {
						piece->availablePositions.push_back(newPos);

					}
				}
			}
			// Knight
			else if (piece->name == "Knight") {
				sf::Vector2i offsets[8] = { {-2, -1 }, {-2, 1 }, { 2, -1 }, { 2, 1 }, { -1, -2 }, { -1, 2 }, { 1, -2 }, { 1, 2 } };
				for (int i = 0; i < 8; i++) {
					sf::Vector2i newPos = { piece->x + offsets[i].x, piece->y + offsets[i].y };
					if (isValidSquare(newPos)) {
						if (getPieceFromPosition(newPos, position) != nullptr) {
							if (getPieceFromPosition(newPos, position)->color != piece->color) {
								piece->availableCapturePositions.push_back(newPos);

							}
						}
						else {
							piece->availablePositions.push_back(newPos);

						}
					}
				}
			}
			// Pawn
			else if (piece->name == "Pawn") {
				std::shared_ptr<Pawn> pawnPiece = std::dynamic_pointer_cast<Pawn>(piece);
				sf::Vector2i pos, pos2, pos3, pos4;
				if (piece->color == PColor::White) {
					pos = { pawnPiece->x, pawnPiece->y + 1 }, pos2 = { pawnPiece->x, pawnPiece->y + 2 };
					pos3 = { piece->x - 1, piece->y + 1 }, pos4 = { piece->x + 1, piece->y + 1 };
				}
				else {
					pos = { pawnPiece->x, pawnPiece->y - 1 }, pos2 = { pawnPiece->x, pawnPiece->y - 2 };
					pos3 = { piece->x - 1, piece->y - 1 }, pos4 = { piece->x + 1, piece->y - 1 };
				}

				if (isValidSquare(pos) && getPieceFromPosition(pos, position) == nullptr) {
					pawnPiece->availablePositions.push_back(pos);

					if (!pawnPiece->hasMoved) {
						if (isValidSquare(pos2) && getPieceFromPosition(pos2, position) == nullptr) {
							pawnPiece->availablePositions.push_back(pos2);
						}
					}
				}
				if (isValidSquare(pos3)) {
					if (getPieceFromPosition(pos3, position) != nullptr) {
						if (getPieceFromPosition(pos3, position)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos3);
						}
					}
				}
				if (isValidSquare(pos4)) {
					if (getPieceFromPosition(pos4, position) != nullptr) {
						if (getPieceFromPosition(pos4, position)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos4);
						}
					}
				}
				// En Passant
				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position) != nullptr) {
					std::shared_ptr<Piece> temp = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position);
					if (temp != nullptr) {
						if (temp->color != pawnPiece->color && temp->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(temp);
							if (pawn->enPassantTarget) {
								if (pawn->color == PColor::White) {
									pawnPiece->enPassantPositions.push_back(sf::Vector2i{ pawn->x, pawn->y - 1 });
								}
								else {
									pawnPiece->enPassantPositions.push_back(sf::Vector2i{ pawn->x, pawn->y + 1 });
								}
							}
						}
					}
				}
				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x - 1, pawnPiece->y }, position) != nullptr) {
					std::shared_ptr<Piece> temp = getPieceFromPosition(sf::Vector2i{ pawnPiece->x - 1, pawnPiece->y }, position);
					if (temp != nullptr) {
						if (temp->color != pawnPiece->color && temp->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(temp);
							if (pawn->enPassantTarget) {
								if (pawn->color == PColor::White) {
									pawnPiece->enPassantPositions.push_back(sf::Vector2i{ pawn->x, pawn->y - 1 });
								}
								else {
									pawnPiece->enPassantPositions.push_back(sf::Vector2i{ pawn->x, pawn->y + 1 });
								}
							}
						}
					}
				}
			}

			// King
			else if (piece->name == "King" && piece != nullptr) {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (x != 0 || y != 0) {
							sf::Vector2i newPos = { king->x + x, king->y + y };
							if (isValidSquare(newPos)) {
								if (getPieceFromPosition(newPos, position) != nullptr) {
									if (getPieceFromPosition(newPos, position)->color != king->color) {
										king->availableCapturePositions.push_back(newPos);
									}
								}
								else {
									king->availablePositions.push_back(newPos);
								}
							}
						}
					}
				}
			}

			{
				for (int i = 0; i < piece->availablePositions.size();) {
					std::vector<std::shared_ptr<Piece>> newV;
					newV.resize(position.size());
					for (size_t j = 0; j < position.size(); j++)
					{
						newV[j] = position[j]->clone();
						if (newV[j]->getLocalPosition() == piece->getLocalPosition()) {
							newV[j]->setLocalPosition(piece->availablePositions.at(i));
						}
					}
					if (!isValidPosition(newV, piece->color)) {
						piece->availablePositions.erase(piece->availablePositions.begin() + i);
					}
					else {
						i++;
					}
					newV.clear();
				}

				for (int i = 0; i < piece->availableCapturePositions.size();) {
					std::vector<std::shared_ptr<Piece>> newV;
					newV.resize(position.size());
					for (size_t j = 0; j < position.size(); j++)
					{
						newV[j] = position[j]->clone();
					}
					for (int j = 0; j < newV.size(); j++) {
						if (newV.at(j)->getLocalPosition() == piece->availableCapturePositions.at(i)) {
							newV.erase(newV.begin() + j);
							break;
						}
					}
					for (auto& v : newV) {
						if (v->getLocalPosition() == piece->getLocalPosition()) {
							v->setLocalPosition(piece->availableCapturePositions.at(i));
						}
					}
					if (!isValidPosition(newV, piece->color)) {
						piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
					}
					else {
						i++;
					}
					newV.clear();
				}
				if (piece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
					for (int i = 0; i < pawn->enPassantPositions.size();) {
						std::vector<std::shared_ptr<Piece>> newV{};
						newV.resize(position.size());
						for (size_t j = 0; j < position.size(); ++j)
						{
							newV[j] = position[j]->clone();
						}
						sf::Vector2i enPassantPos{ 0, 0 };
						if (pawn->color == PColor::White) {
							enPassantPos = { pawn->enPassantPositions.at(i).x, pawn->enPassantPositions.at(i).y - 1 };
						}
						else {
							enPassantPos = { pawn->enPassantPositions.at(i).x, pawn->enPassantPositions.at(i).y + 1 };
						}
						for (int j = 0; j < newV.size(); j++) {
							if (newV.at(j)->getLocalPosition() == enPassantPos) {
								newV.erase(newV.begin() + j);
								break;
							}
						}
						for (auto& v : newV) {
							if (v->getLocalPosition() == piece->getLocalPosition()) {
								v->setLocalPosition(pawn->enPassantPositions.at(i));
							}
						}
						if (!isValidPosition(newV, piece->color)) {
							pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
						}
						else {
							i++;
						}
						newV.clear();
					}
				}

				if (piece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					calculateCastlingRights(king, position, whiteToPlay, wKRook, wQRook, bKRook, bQRook);
					sf::Vector2i castlePos;
					if (king->canCastleK) {
						if (king->color == PColor::White) {
							sf::Vector2i capturePos = { wKRook, 1 };
							std::vector<std::shared_ptr<Piece>> newV;
							newV.resize(position.size());
							for (size_t j = 0; j < position.size(); j++)
							{
								newV[j] = position[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{wKRook, 1}) {
									v->setLocalPosition(convertChessNotationtoPosition("f1"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("g1"));
								}
							}
							if (isValidPosition(newV, piece->color)) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("g1");
									king->castlePositions.push_back(castlePos);
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { bKRook, 8 };
							std::vector<std::shared_ptr<Piece>> newV;
							newV.resize(position.size());
							for (size_t j = 0; j < position.size(); j++)
							{
								newV[j] = position[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ bKRook, 8 }) {
									v->setLocalPosition(convertChessNotationtoPosition("f8"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("g8"));
								}
							}
							if (isValidPosition(newV, piece->color)) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("g8");
									king->castlePositions.push_back(castlePos);
								}
							}
							newV.clear();
						}
						king->canMove = true;
					}
					if (king->canCastleQ) {
						if (king->color == PColor::White) {
							sf::Vector2i capturePos = { wQRook, 1 };
							std::vector<std::shared_ptr<Piece>> newV;
							newV.resize(position.size());
							for (size_t j = 0; j < position.size(); j++)
							{
								newV[j] = position[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ wQRook, 1 }) {
									v->setLocalPosition(convertChessNotationtoPosition("d1"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("c1"));
								}
							}
							if (isValidPosition(newV, piece->color)) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("c1");
									king->castlePositions.push_back(castlePos);
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { bQRook, 8 };
							std::vector<std::shared_ptr<Piece>> newV;
							newV.resize(position.size());
							for (size_t j = 0; j < position.size(); j++)
							{
								newV[j] = position[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ bQRook, 8 }) {
									v->setLocalPosition(convertChessNotationtoPosition("d8"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("c8"));
								}
							}
							if (isValidPosition(newV, piece->color)) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("c8");
									king->castlePositions.push_back(castlePos);
								}
							}
							newV.clear();
						}
						king->canMove = true;
					}
				}
				// Can Move
				if (!piece->availablePositions.empty()) {
					piece->canMove = true;
				}
				if (!piece->availableCapturePositions.empty()) {
					piece->canMove = true;
				}
				if (piece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
					if (!pawn->enPassantPositions.empty()) {
						pawn->canMove = true;
					}
				}
			}
		}
	}

	static int determineEnd(std::vector<std::shared_ptr<Piece>>& position, bool whiteToPlay, int halfMoves) {
		// Neither 0, Stalemate 1, Checkmate 2, Insufficient Material 3, 50 Move Rule 4
		if (halfMoves >= 100) {
			return 4;
		}
		if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : position) {
				if (whiteToPlay == (piece->color == PColor::White) && piece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					if (king->inCheck) {
						return 2;
					}
					else {
						return 1;
					}
				}
			}
		}
		else {
			if (position.size() == 2) {
				return 3;
			}
			if (position.size() == 3) {
				if (std::none_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
					return 3;
				}
			}
			else if (position.size() == 4) {
				for (auto& piece : position) {
					if (piece->name == "Pawn") {
						return 3;
					}
				}
				int whiteCount = 0, blackCount = 0;
				for (auto& piece : position) {
					if (piece->isWhite()) {
						whiteCount++;
					}
					else { blackCount++; }
				}
				if (whiteCount != blackCount) {
					if (std::none_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen"; })) {
						if (!std::all_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Bishop"; })) {
							return 3;
						}
					}
				}
				else {
					if (std::none_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen"; })) {
						return 3;
					}
				}
			}
		}
		return 0;
	}

	static std::pair<std::array<std::array<int, 8>, 8>, bool> savePosition(std::vector<std::shared_ptr<Piece>>& position, bool whiteToPlay) {
		std::array<std::array<int, 8>, 8> array{ 0 };
		for (auto& piece : position) {
			if (piece->isWhite()) {
				if (piece->name == "Bishop") {
					array.at(piece->x - 1).at(piece->y - 1) = 1;
				}
				else if (piece->name == "Knight") {
					array.at(piece->x - 1).at(piece->y - 1) = 2;
				}
				else if (piece->name == "King") {
					array.at(piece->x - 1).at(piece->y - 1) = 3;
				}
				else if (piece->name == "Queen") {
					array.at(piece->x - 1).at(piece->y - 1) = 4;
				}
				else if (piece->name == "Rook") {
					array.at(piece->x - 1).at(piece->y - 1) = 5;
				}
				else if (piece->name == "Pawn") {
					array.at(piece->x - 1).at(piece->y - 1) = 6;
				}
			}
			else {
				if (piece->name == "Bishop") {
					array.at(piece->x - 1).at(piece->y - 1) = 7;
				}
				else if (piece->name == "Knight") {
					array.at(piece->x - 1).at(piece->y - 1) = 8;
				}
				else if (piece->name == "King") {
					array.at(piece->x - 1).at(piece->y - 1) = 9;
				}
				else if (piece->name == "Queen") {
					array.at(piece->x - 1).at(piece->y - 1) = 10;
				}
				else if (piece->name == "Rook") {
					array.at(piece->x - 1).at(piece->y - 1) = 11;
				}
				else if (piece->name == "Pawn") {
					array.at(piece->x - 1).at(piece->y - 1) = 12;
				}
			}
		}
		return std::make_pair(array, whiteToPlay);
	}

	static int postMove(std::shared_ptr<Piece> piece, std::vector<std::shared_ptr<Piece>>& pieceList, std::vector<std::shared_ptr<Piece>>::iterator& it,
		float boardOffset, float boardMultiplier, bool& whiteToPlay, bool& check, bool& moving, int& fullMoves, int& halfMoves, sf::Sprite& checkSprite,
		std::vector<std::reference_wrapper<sf::Texture>> extraTextures, std::vector<std::pair<std::array<std::array<int, 8>, 8>, bool>> allPositionsPlayed,
		std::shared_ptr<Piece>& selectedPiece, std::shared_ptr<Piece>& capturePiece, sf::RenderWindow& window, sf::Sprite& board, sf::Sprite& lastMoveStart, sf::Sprite& lastMoveDest,
		int wKRook, int wQRook, int bKRook, int bQRook, bool standardPosition) {

		bool endCheck = false;
		if (capturePiece != nullptr) {
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i) != nullptr && pieceList.at(i) == capturePiece) {
					it = pieceList.erase(pieceList.begin() + i);
					halfMoves = 0;
					if (it == pieceList.end()) {
						endCheck = true;
					}
					break;
				}
			}
		}
		if (piece->name == "Pawn") {
			halfMoves = 0;
		}
		if (piece->isBlack()) {
			fullMoves++;
		}
		piece->setTarget({});
		piece->setLocalPosition(Main::getLocalPosition(piece->getGlobalPosition(), boardOffset, boardMultiplier));
		piece->setGlobalPosition(piece->getGlobalPosition());
		// Smooth Effect
		window.clear();
		window.draw(board);
		window.draw(lastMoveStart);
		window.draw(lastMoveDest);
		for (auto& p : pieceList) {
			p->draw(window);
		}
		window.display();
		if (!piece->hasMoved) { piece->hasMoved = true; }
		if (piece->name == "Pawn") {
			std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
			pawn->capturingEnPassant = false;
		}

		for (int i = 0; i < pieceList.size(); i++) {
			if (whiteToPlay == pieceList.at(i)->isWhite()) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
				}
			}
		}
		for (auto& p : pieceList) {
			if (p->name == "King") {
				std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
				k->inCheck = false;
			}
		}
		check = false;
		for (auto& p : pieceList) {
			if (p->name == "King" && whiteToPlay != (p->color == PColor::White)) {
				for (auto& p2 : pieceList) {
					if (p2->color != p->color) {
						std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
						k->inCheck = false;
						for (auto& pos : p2->getAvailableCapturePositions()) {
							if (pos == k->getLocalPosition()) {
								k->inCheck = true;
								checkSprite.setPosition(k->getGlobalPosition());
								check = true;
								goto next;
							}
						}
					}
				}
			}
		}
	next:
		if (check) {
			window.clear();
			window.draw(board);
			window.draw(lastMoveStart);
			window.draw(lastMoveDest);
			window.draw(checkSprite);
			for (auto& p : pieceList) {
				p->draw(window);
			}
			window.display();
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (whiteToPlay != pieceList.at(i)->isWhite()) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
				}
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name == "King") {
				Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		Main::setExtraSprites(pieceList, extraTextures);
		whiteToPlay = !whiteToPlay;
		std::cout << Main::determineEnd(pieceList, whiteToPlay, halfMoves) << std::endl;
		moving = false;
		auto pos = Main::savePosition(pieceList, whiteToPlay);
		int count = 1;
		for (auto& position : allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
			return 2;
		}
		allPositionsPlayed.push_back(pos);
		selectedPiece.reset();
		if (endCheck) { return 1; }
		return 0;
	}

	static int postMove(std::shared_ptr<Piece> piece, std::vector<std::shared_ptr<Piece>>& pieceList,
		float boardOffset, float boardMultiplier, bool& whiteToPlay, bool& check, bool& moving, int& fullMoves, int& halfMoves, sf::Sprite& checkSprite,
		std::vector<std::reference_wrapper<sf::Texture>> extraTextures, std::vector<std::pair<std::array<std::array<int, 8>, 8>, bool>> allPositionsPlayed,
		std::shared_ptr<Piece>& selectedPiece, std::shared_ptr<Piece>& capturePiece, sf::RenderWindow& window, sf::Sprite& board, sf::Sprite& lastMoveStart, sf::Sprite& lastMoveDest,
		int wKRook, int wQRook, int bKRook, int bQRook, bool standardPosition) {

		if (capturePiece != nullptr) {
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i) != nullptr && pieceList.at(i) == capturePiece) {
					pieceList.erase(pieceList.begin() + i);
					halfMoves = 0;
					break;
				}
			}
		}
		if (piece->name == "Pawn") {
			halfMoves = 0;
		}
		if (piece->isBlack()) {
			fullMoves++;
		}
		piece->setTarget({});
		piece->setLocalPosition(Main::getLocalPosition(piece->getGlobalPosition(), boardOffset, boardMultiplier));
		piece->setGlobalPosition(piece->getGlobalPosition());
		// Smooth Effect
		window.clear();
		window.draw(board);
		window.draw(lastMoveStart);
		window.draw(lastMoveDest);
		for (auto& p : pieceList) {
			p->draw(window);
		}
		window.display();
		if (!piece->hasMoved) { piece->hasMoved = true; }
		if (piece->name == "Pawn") {
			std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
			pawn->capturingEnPassant = false;
		}

		for (int i = 0; i < pieceList.size(); i++) {
			if (whiteToPlay == pieceList.at(i)->isWhite()) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
				}
			}
		}
		for (auto& p : pieceList) {
			if (p->name == "King") {
				std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
				k->inCheck = false;
			}
		}
		check = false;
		for (auto& p : pieceList) {
			if (p->name == "King" && whiteToPlay != (p->color == PColor::White)) {
				for (auto& p2 : pieceList) {
					if (p2->color != p->color) {
						std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
						for (auto& pos : p2->getAvailableCapturePositions()) {
							if (pos == k->getLocalPosition()) {
								k->inCheck = true;
								checkSprite.setPosition(k->getGlobalPosition());
								check = true;
								goto next;
							}
						}
					}
				}
			}
		}
	next:
		if (check) {
			window.clear();
			window.draw(board);
			window.draw(lastMoveStart);
			window.draw(lastMoveDest);
			window.draw(checkSprite);
			for (auto& p : pieceList) {
				p->draw(window);
			}
			window.display();
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (whiteToPlay != pieceList.at(i)->isWhite()) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
				}
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name == "King") {
				Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		Main::setExtraSprites(pieceList, extraTextures);
		whiteToPlay = !whiteToPlay;
		std::cout << Main::determineEnd(pieceList, whiteToPlay, halfMoves) << std::endl;
		moving = false;
		auto pos = Main::savePosition(pieceList, whiteToPlay);
		int count = 1;
		for (auto& position : allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
			return 2;
		}
		allPositionsPlayed.push_back(pos);
		selectedPiece.reset();
		return 0;
	}

	static int postCastle(std::shared_ptr<King> piece, std::shared_ptr<Piece> piece2, std::vector<std::shared_ptr<Piece>>& pieceList,
		float boardOffset, float boardMultiplier, bool& whiteToPlay, bool& check, bool& moving, int& fullMoves, int& halfMoves, sf::Sprite& checkSprite,
		std::vector<std::reference_wrapper<sf::Texture>> extraTextures, std::vector<std::pair<std::array<std::array<int, 8>, 8>, bool>> allPositionsPlayed,
		std::shared_ptr<Piece>& selectedPiece, sf::RenderWindow& window, sf::Sprite& board, sf::Sprite& lastMoveStart, sf::Sprite& lastMoveDest,
		int wKRook, int wQRook, int bKRook, int bQRook, bool standardPosition) {
		if (piece->isBlack()) {
			fullMoves++;
		}
		piece->canNeverCastleK = true;
		piece->canNeverCastleQ = true;
		piece->setTarget({});
		piece->setLocalPosition(Main::getLocalPosition(piece->getGlobalPosition(), boardOffset, boardMultiplier));
		piece->setGlobalPosition(piece->getGlobalPosition());
		piece2->setTarget({});
		piece2->setLocalPosition(Main::getLocalPosition(piece2->getGlobalPosition(), boardOffset, boardMultiplier));
		piece2->setGlobalPosition(piece2->getGlobalPosition());
		// Smooth Effect
		window.clear();
		window.draw(board);
		window.draw(lastMoveStart);
		window.draw(lastMoveDest);
		for (auto& p : pieceList) {
			p->draw(window);
		}
		window.display();
		if (!piece->hasMoved) { piece->hasMoved = true; }
		if (!piece2->hasMoved) { piece->hasMoved = true; }

		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name != "King") {
				Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name == "King") {
				Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook);
			}
		}
		Main::setExtraSprites(pieceList, extraTextures);
		whiteToPlay = !whiteToPlay;
		std::cout << Main::determineEnd(pieceList, whiteToPlay, halfMoves) << std::endl;
		moving = false;
		auto pos = Main::savePosition(pieceList, whiteToPlay);
		int count = 1;
		for (auto& position : allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
			return 2;
		}
		allPositionsPlayed.push_back(pos);
		selectedPiece.reset();
		return 0;
	}
};