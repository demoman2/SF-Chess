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

class Main {

public:

	static sf::Vector2f Interpolate(const sf::Vector2f& pointA, const sf::Vector2f& pointB, float factor) {
		factor = std::clamp(factor, 0.0f, 1.0f);
		if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 0.1f) {
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

	static bool noPiece(std::string notation, std::vector<std::shared_ptr<Piece>>& pieces) {
		return getPieceFromPosition(convertChessNotationtoPosition(notation), pieces) == nullptr;
	}

	static int reverseY(int y) {
		return 9 - y;
	}

	// Bishop, King, Knight, Pawn, Queen, Rook
	// Black --> White
	static std::vector<std::shared_ptr<Piece>> generatePositionFromFENID(std::string code, std::vector<std::reference_wrapper<sf::Texture>>& pieceTextures, float pieceScale, float boardOffset, float boardSquareOffset, bool& whiteToPlay, int& halfMoves, int& fullMoves) {
		std::vector<std::shared_ptr<Piece>> pieces;
		std::vector<std::string> splitString = split(code, ' ');
		// ========= MODIFIERS =========
		bool whiteCanNeverCastleK = true, whiteCanNeverCastleQ = true;
		bool blackCanNeverCastleK = true, blackCanNeverCastleQ = true;
		std::optional<sf::Vector2i> enPassantTarget{ };
		std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
		for (int i = 0; i < modifiers.size(); i++) {
			std::string modifier = modifiers.at(i);

			switch (i) {
				// Side to Play
			case 0:
				if (modifier.at(0) == 'w') {
					whiteToPlay = true;
				}
				else if (modifier.at(0) == 'b') {
					whiteToPlay = false;
				}
				break;
				// Castling Rights
			case 1:;
				for (int j = 0; j < modifier.size(); j++) {
					if (modifier.at(j) == 'K') {
						whiteCanNeverCastleK = false;
					}
					else if (modifier.at(j) == 'Q') {
						whiteCanNeverCastleQ = false;
					}
					else if (modifier.at(j) == 'k') {
						blackCanNeverCastleK = false;
					}
					else if (modifier.at(j) == 'q') {
						blackCanNeverCastleQ = false;
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
		std::vector<std::string> ranks = split(splitString.at(0), '/');
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
						pieces.push_back(std::make_shared<Bishop>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(0)));
						break;
					}
					case 'k':
					{
						pieces.push_back(std::make_shared<King>(blackCanNeverCastleK, blackCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(1)));
						break;
					}
					case 'n':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(2)));
						break;
					}
					case 'p':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y - 1 }) {
							pieces.push_back(std::make_shared<Pawn>(true, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(3)));
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(3)));
						}
						break;
					}
					case 'q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(4)));
						break;
					}
					case 'r':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::Black, pieceTextures.at(5)));
						break;
					}
					// WHITE
					case 'B':
					{
						pieces.push_back(std::make_shared<Bishop>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(6)));
						break;
					}
					case 'K':
					{
						pieces.push_back(std::make_shared<King>(whiteCanNeverCastleK, whiteCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(7)));
						break;
					}
					case 'N':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(8)));
						break;
					}
					case 'P':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y + 1 }) {
							pieces.push_back(std::make_shared<Pawn>(true, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(9)));
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(9)));
						}
						break;
					}
					case 'Q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(10)));
						break;
					}
					case 'R':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, pieceScale, boardOffset, boardSquareOffset, pieces.size(), Piece::PColor::White, pieceTextures.at(11)));
						break;
					}
					}
					x++;
				}
			}

		}
		return pieces;
	}

	// Simplify later
	static void calculateCastlingPossibilities(std::shared_ptr<King> const& king, std::vector<std::shared_ptr<Piece>>& pieces) {
		if (king != nullptr) {
			king->canCastleK = false;
			king->canCastleQ = false;
			// WHITE
			Piece::PColor color = king->color;
			if (!king->hasMoved && !king->inCheck && king->isWhite()) {
				if (king->position == convertChessNotationtoPosition("e1")) {
					// Kingside
					if (!king->canNeverCastleK) {
						if (noPiece("f1", pieces) && noPiece("g1", pieces)) {
							for (auto& piece2 : pieces) {
								if (piece2->name == "Rook" && piece2->color == color && piece2 != nullptr) {
									std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece2);
									if (!rook->hasMoved && rook->position == convertChessNotationtoPosition("h1")) {
										for (auto& piece3 : pieces) {
											if (piece3->color != color) {
												for (auto& square : piece3->availablePositions) {
													if (square == convertChessNotationtoPosition("f1") || square == convertChessNotationtoPosition("g1")) {
														goto Wqueenside;
													}
												}
											}
										}
										king->canCastleK = true;
									}
								}
							}
						}
					}
				Wqueenside:
					if (!king->canNeverCastleQ) {
						if (noPiece("b1", pieces) && noPiece("c1", pieces) && noPiece("d1", pieces)) {
							for (auto& piece2 : pieces) {
								if (piece2->name == "Rook" && piece2->color == color && piece2 != nullptr) {
									std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece2);
									if (!rook->hasMoved && rook->position == convertChessNotationtoPosition("a1")) {
										for (auto& piece3 : pieces) {
											if (piece3->color != color) {
												for (auto& square : piece3->availablePositions) {
													if (square == convertChessNotationtoPosition("b1") || square == convertChessNotationtoPosition("c1") || square == convertChessNotationtoPosition("d1")) {
														return;
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
			}
			// BLACK
			else if (!king->hasMoved && !king->inCheck && king->isBlack()) {
				if (king->position == convertChessNotationtoPosition("e8")) {
					// Kingside
					if (!king->canNeverCastleK) {
						if (noPiece("f8", pieces) && noPiece("g8", pieces)) {
							for (auto& piece2 : pieces) {
								if (piece2->name == "Rook" && piece2->color == color && piece2 != nullptr) {
									std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece2);
									if (!rook->hasMoved && rook->position == convertChessNotationtoPosition("h8")) {
										for (auto& piece3 : pieces) {
											if (piece3->color != color) {
												for (auto& square : piece3->availablePositions) {
													if (square == convertChessNotationtoPosition("f8") || square == convertChessNotationtoPosition("g8")) {
														goto Bqueenside;
													}
												}
											}
										}
										king->canCastleK = true;
									}
								}
							}
						}
					}
				Bqueenside:
					if (!king->canNeverCastleQ) {
						if (noPiece("b8", pieces) && noPiece("c8", pieces) && noPiece("d8", pieces)) {
							for (auto& piece2 : pieces) {
								if (piece2->name == "Rook" && piece2->color == color && piece2 != nullptr) {
									std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece2);
									if (!rook->hasMoved && rook->position == convertChessNotationtoPosition("a8")) {
										for (auto& piece3 : pieces) {
											if (piece3->color != color) {
												for (auto& square : piece3->availablePositions) {
													if (square == convertChessNotationtoPosition("b8") || square == convertChessNotationtoPosition("c8") || square == convertChessNotationtoPosition("d8")) {
														return;
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
			}
		}
	}



	static bool isValidSquare(sf::Vector2i square) {
		return (square.x <= 8 && square.x >= 1 && square.y <= 8 && square.y >= 1);
	}

	static bool isValidPosition(std::vector<std::shared_ptr<Piece>>& position, Piece::PColor color) {
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
					if (piece->color == Piece::PColor::White) {
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

	static void calculatePositions(std::shared_ptr<Piece> const& piece, std::vector<std::shared_ptr<Piece>>& position) {
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
				if (piece->color == Piece::PColor::White) {
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

				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position) != nullptr) {
					std::shared_ptr<Piece> p = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position);
					if (p != nullptr) {
						if (p->color != pawnPiece->color && p->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							if (pawn->enPassantTarget) {
								if (pawn->color == Piece::PColor::White) {
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
					std::shared_ptr<Piece> p = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position);
					if (p != nullptr) {
						if (p->color != pawnPiece->color && p->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							if (pawn->enPassantTarget) {
								if (pawn->color == Piece::PColor::White) {
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
										king->addCaptureSquare(newPos);
									}
								}
								else {

								}
							}
						}
					}
				}
			}



			{
				for (int i = 0; i < piece->availablePositions.size();) {
					std::vector<std::shared_ptr<Piece>> newV{};
					newV.resize(position.size());
					std::cout << "Deep copy:";
					for (size_t j = 0; j < position.size(); ++j)
					{
						newV[j] = position[j]->clone();
						if (j == piece->index) {
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
					std::vector<std::shared_ptr<Piece>> newV{};
					newV.resize(position.size());
					for (size_t j = 0; j < position.size(); ++j)
					{
						newV[j] = position[j]->clone();
						if (j == piece->index) {
							newV[j]->setLocalPosition(piece->availableCapturePositions.at(i));
						}
					}
					for (int j = 0; j < newV.size(); j++) {
						if (newV.at(j)->position == piece->availableCapturePositions.at(i)) {
							newV.at(j).reset();
							newV.erase(newV.begin() + j);
							break;
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
						std::vector<std::shared_ptr<Piece>> newV{ position.begin(), position.end() };
						newV.erase(newV.begin() + pawn->index);
						sf::Vector2i pos{ 0, 0 };
						if (pawn->color == Piece::PColor::White) {
							pos = { pawn->enPassantPositions.at(i).x, pawn->enPassantPositions.at(i).y - 1 };
						}
						else {
							pos = { pawn->enPassantPositions.at(i).x, pawn->enPassantPositions.at(i).y + 1 };
						}
						if (getPieceFromPosition(pos, position) != nullptr) {
							for (int j = 0; j < newV.size(); j++) {
								if (newV.at(j)->position == piece->availableCapturePositions.at(i)) {
									newV.at(j).reset();
									newV.erase(newV.begin() + j);
									break;
								}
							}
						}
						piece->setLocalPosition(pawn->enPassantPositions.at(i));
						newV.push_back(piece);
						if (!isValidPosition(newV, piece->color)) {
							pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
						}
						else {
							i++;
						}
						piece->setLocalPosition(pos);
						newV.clear();
					}
				}

				if (piece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					calculateCastlingPossibilities(king, position);
					sf::Vector2i castlePos, capturePos;
					if (king->canCastleK) {
						if (king->color == Piece::PColor::White) {
							castlePos = convertChessNotationtoPosition("g1");
							capturePos = convertChessNotationtoPosition("h1");
							king->castlePositions.push_back(castlePos);
							king->captureCastlePositions.push_back(capturePos);
						}
						else {
							castlePos = convertChessNotationtoPosition("g8");
							capturePos = convertChessNotationtoPosition("h8");
							king->castlePositions.push_back(castlePos);
							king->captureCastlePositions.push_back(capturePos);
						}
					}
					if (king->canCastleQ) {
						if (king->color == Piece::PColor::White) {
							castlePos = convertChessNotationtoPosition("c1");
							capturePos = convertChessNotationtoPosition("a1");
							king->castlePositions.push_back(castlePos);
							king->captureCastlePositions.push_back(capturePos);
						}
						else {
							castlePos = convertChessNotationtoPosition("c8");
							capturePos = convertChessNotationtoPosition("a8");
							king->castlePositions.push_back(castlePos);
							king->captureCastlePositions.push_back(capturePos);
						}
					}
				}
			}
		}
	}
};