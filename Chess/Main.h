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
	// Bishop, King, Knight, Pawn, Queen, Rook
	static void loadPieceSet(sf::Image spriteSheet, std::vector<std::reference_wrapper<sf::Texture>>& pieceTextures, int pieceSize) {
		for (size_t i = 0; i < pieceTextures.size(); i++) {
			switch (i) {
			// Black
			case 0:
				pieceTextures.at(i).get().loadFromImage(spriteSheet, false, sf::IntRect({0, 0}, {pieceSize, pieceSize}));
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

	static Piece* getPieceFromPosition(sf::Vector2i position, std::vector<std::shared_ptr<Piece>>& pieces)
	{
		for (auto& piece : pieces) {
			if (piece->position == position) {
				return piece.get();
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
						Bishop bishop{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(0)};
						pieces.push_back(std::make_shared<Bishop>(bishop));
						break;
					}
					case 'k':
					{
						King king{ blackCanNeverCastleK, blackCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(1) };
						pieces.push_back(std::make_shared<King>(king));
						break;
					}
					case 'n':
					{
						Knight knight{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(2) };
						pieces.push_back(std::make_shared<Knight>(knight));
						break;
					}
					case 'p':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{x, y - 1}) {
							Pawn pawn{ true, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(3) };
							pieces.push_back(std::make_shared<Pawn>(pawn));
						}
						else {
							Pawn pawn{ false, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(3) };
							pieces.push_back(std::make_shared<Pawn>(pawn));
						}
						break;
					}
					case 'q':
					{
						Queen queen{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(4) };
						pieces.push_back(std::make_shared<Queen>(queen));
						break;
					}
					case 'r':
					{
						Rook rook{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::Black, pieceTextures.at(5) };
						pieces.push_back(std::make_shared<Rook>(rook));
						break;
					}
					// WHITE
					case 'B':
					{
						Bishop bishop{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(6) };
						pieces.push_back(std::make_shared<Bishop>(bishop));
						break;
					}
					case 'K':
					{
						King king{ whiteCanNeverCastleK, whiteCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(7) };
						pieces.push_back(std::make_shared<King>(king));
						break;
					}
					case 'N':
					{
						Knight knight{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(8) };
						pieces.push_back(std::make_shared<Knight>(knight));
						break;
					}
					case 'P':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y + 1 }) {
							Pawn pawn{ true, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(9) };
							pieces.push_back(std::make_shared<Pawn>(pawn));
						}
						else {
							Pawn pawn{ false, x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(9) };
							pieces.push_back(std::make_shared<Pawn>(pawn));
						}
						break;
					}
					case 'Q':
					{
						Queen queen{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(10) };
						pieces.push_back(std::make_shared<Queen>(queen));
						break;
					}
					case 'R':
					{
						Rook rook{ x, y, pieceScale, boardOffset, boardSquareOffset, (int)pieces.size(), Piece::PColor::White, pieceTextures.at(11) };
						pieces.push_back(std::make_shared<Rook>(rook));
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
	static void calculateCastlingPossibilities(std::vector<std::shared_ptr<Piece>>& pieces) {
		for (auto& piece : pieces) {
			if (piece->name == "King") {
				King* king = dynamic_cast<King*>(piece.get());
				if (king != nullptr) {
					// WHITE
					Piece::PColor color = king->color;
					if (!king->hasMoved && !king->inCheck && king->isWhite()) {
						if (king->position == convertChessNotationtoPosition("e1")) {
							// Kingside
							if (!king->canNeverCastleK) {
								if (noPiece("f1", pieces) && noPiece("g1", pieces)) {
									for (auto& piece2 : pieces) {
										if (piece2->name == "Rook" && piece2->color == color) {
											Rook* rook = dynamic_cast<Rook*>(piece2.get());
											if (rook != nullptr && !rook->hasMoved && rook->position == convertChessNotationtoPosition("h1")) {
												for (auto& piece3 : pieces) {
													if (piece3->color != color) {
														for (auto& square : piece3->availablePositions) {
															if (square == convertChessNotationtoPosition("f1") || square == convertChessNotationtoPosition("g1")) {
																king->canCastleK = false;
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
										if (piece2->name == "Rook" && piece2->color == color) {
											Rook* rook = dynamic_cast<Rook*>(piece2.get());
											if (rook != nullptr && !rook->hasMoved && rook->position == convertChessNotationtoPosition("a1")) {
												for (auto& piece3 : pieces) {
													if (piece3->color != color) {
														for (auto& square : piece3->availablePositions) {
															if (square == convertChessNotationtoPosition("b1") || square == convertChessNotationtoPosition("c1") || square == convertChessNotationtoPosition("d1")) {
																king->canCastleQ = false;
																goto end;
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
										if (piece2->name == "Rook" && piece2->color == color) {
											Rook* rook = dynamic_cast<Rook*>(piece2.get());
											if (rook != nullptr && !rook->hasMoved && rook->position == convertChessNotationtoPosition("h8")) {
												for (auto& piece3 : pieces) {
													if (piece3->color != color) {
														for (auto& square : piece3->availablePositions) {
															if (square == convertChessNotationtoPosition("f8") || square == convertChessNotationtoPosition("g8")) {
																king->canCastleK = false;
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
										if (piece2->name == "Rook" && piece2->color == color) {
											Rook* rook = dynamic_cast<Rook*>(piece2.get());
											if (rook != nullptr && !rook->hasMoved && rook->position == convertChessNotationtoPosition("a8")) {
												for (auto& piece3 : pieces) {
													if (piece3->color != color) {
														for (auto& square : piece3->availablePositions) {
															if (square == convertChessNotationtoPosition("b8") || square == convertChessNotationtoPosition("c8") || square == convertChessNotationtoPosition("d8")) {
																king->canCastleQ = false;
																goto end;
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
			end:
			continue;
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
								Piece* p = getPieceFromPosition(newPos, position);
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
								Piece* p = getPieceFromPosition(newPos, position);
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
								Piece* p = getPieceFromPosition(newPos, position);
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
								Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
							Piece* p = getPieceFromPosition(newPos, position);
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
								Piece* p = getPieceFromPosition(newPos, position);
								if (p->name == "King" && p->color != piece->color) {
									return false;
								}
							}
						}
					}
				}
				// Pawn
				else if (piece->name == "Pawn") {
					sf::Vector2i pos = { piece->x - 1, piece->y + 1 }, pos2 = { piece->x + 1, piece->y + 1 };
					if (isValidSquare(pos)) {
						if (getPieceFromPosition(pos, position) != nullptr) {
							Piece* p = getPieceFromPosition(pos, position);
							if (p->name == "King" && p->color != piece->color) {
								return false;
							}
						}
					}
					if (isValidSquare(pos2)) {
						if (getPieceFromPosition(pos2, position) != nullptr) {
							Piece* p = getPieceFromPosition(pos2, position);
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
								if (getPieceFromPosition(newPos, position) != nullptr) {
									Piece* p = getPieceFromPosition(newPos, position);
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
		return true;
	}

	static void calculatePositions(std::shared_ptr<Piece>& piece, std::vector<std::shared_ptr<Piece>>& position) {
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
			// Pawn
			else if (piece->name == "Pawn") {
				Pawn* pawnPiece = dynamic_cast<Pawn*>(piece.get());
				sf::Vector2i pos = { pawnPiece->x - 1, pawnPiece->y + 1 }, pos2 = { pawnPiece->x + 1, pawnPiece->y + 1 };
				if (isValidSquare(pos)) {
					if (getPieceFromPosition(pos, position) != nullptr) {
						if (getPieceFromPosition(pos, position)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos);
						}
					}
					else {
						pawnPiece->availablePositions.push_back(pos);
					}
				}
				if (isValidSquare(pos2)) {
					if (getPieceFromPosition(pos2, position) != nullptr) {
						if (getPieceFromPosition(pos2, position)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos2);
						}
					}
					else {
						pawnPiece->availablePositions.push_back(pos2);
					}
				}
				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position) != nullptr) {
					Piece* p = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position);
					if (p != nullptr) {
						if (p->color != pawnPiece->color && p->name == "Pawn") {
							Pawn* pawn = dynamic_cast<Pawn*>(p);
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
					Piece* p = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, position);
					if (p != nullptr) {
						if (p->color != pawnPiece->color && p->name == "Pawn") {
							Pawn* pawn = dynamic_cast<Pawn*>(p);
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
			else if (piece->name == "King") {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (x != 0 || y != 0) {
							sf::Vector2i newPos = { piece->x + x, piece->y + y };
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
			}
		}

		{
			for (int i = 0; i < piece->availablePositions.size();) {
				std::vector<std::shared_ptr<Piece>> newV{ position.begin(), position.end()};
				newV.erase(newV.begin() + piece->index);
				std::shared_ptr<Piece> p2 = std::make_shared<Piece>(*piece);
				p2->setPosition(piece->availablePositions.at(i));
				newV.push_back(p2);
				if (!isValidPosition(newV, piece->color)) {
					piece->availablePositions.erase(piece->availablePositions.begin() + i);
				}
				else {
					i++;
				}
				p2->setPosition(pos);
				for (auto& v : newV) {
					v.reset();
				}
				newV.clear();
			}

			for (int i = 0; i < piece->availableCapturePositions.size();) {
				std::vector<std::shared_ptr<Piece>> newV{ position.begin(), position.end() };
				newV.erase(newV.begin() + piece->index);
				if (getPieceFromPosition(piece->availableCapturePositions.at(i), position) != nullptr) {
					for (int j = 0; j < newV.size(); j++) {
						if (newV.at(j)->position == piece->availableCapturePositions.at(i)) {
							newV.at(j).reset();
							newV.erase(newV.begin() + j);
							break;
						}
					}
				}
				std::shared_ptr<Piece> p2 = std::make_shared<Piece>(*piece);
				p2->setPosition(piece->availableCapturePositions.at(i));
				newV.push_back(p2);
				if (!isValidPosition(newV, piece->color)) {
					piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
				}
				else {
					i++;
				}
				p2->setPosition(pos);
				for (auto& v : newV) {
					v.reset();
				}
				newV.clear();
			}

			/*
			if (piece->name == "King") {
				King* king = dynamic_cast<King*>(piece.get());
				for (int i = 0; i < king->availableCapturePositions.size();) {
					std::vector<std::shared_ptr<Piece>> newV{ position.begin(), position.end() };
					newV.erase(newV.begin() + piece->index);
					std::shared_ptr<King> p2 = std::make_shared<King>(*king);
					if (king->isWhite()) {
						if (king->availableCastleSquares.at(i) == convertChessNotationtoPosition("g1")) {
							p2->setPosition(convertChessNotationtoPosition("g1"));
							Piece* r = getPieceFromPosition(convertChessNotationtoPosition("h1"), newV);
							sf::Vector2i rookPos = r->position;
							if (r->name == "Rook") {
								Rook* rook = dynamic_cast<Rook*>(r);
								rook->setPosition(convertChessNotationtoPosition("f1"));
								newV.erase(newV.begin() + r->index);
								if (!isValidPosition(newV, piece->color)) {
									king->availableCastleSquares.erase(king->availableCastleSquares.begin() + i);
								}
								else {
									i++;
								}
								p2->setPosition(pos);
								rook->setPosition(rookPos);
							}
						}
						if (king->availableCastleSquares.at(i) == convertChessNotationtoPosition("c1")) {
							p2->setPosition(convertChessNotationtoPosition("c1"));
							Piece* r = getPieceFromPosition(convertChessNotationtoPosition("a1"), newV);
							sf::Vector2i rookPos = r->position;
							if (r->name == "Rook") {
								Rook* rook = dynamic_cast<Rook*>(r);
								rook->setPosition(convertChessNotationtoPosition("d1"));
								newV.erase(newV.begin() + r->index);
								if (!isValidPosition(newV, piece->color)) {
									king->availableCastleSquares.erase(king->availableCastleSquares.begin() + i);
								}
								else {
									i++;
								}
								p2->setPosition(pos);
								rook->setPosition(rookPos);
							}
						}
					}
					else if (king->isBlack()) {
						if (king->availableCastleSquares.at(i) == convertChessNotationtoPosition("g8")) {
							p2->setPosition(convertChessNotationtoPosition("g8"));
							Piece* r = getPieceFromPosition(convertChessNotationtoPosition("h8"), newV);
							sf::Vector2i rookPos = r->position;
							if (r->name == "Rook") {
								Rook* rook = dynamic_cast<Rook*>(r);
								rook->setPosition(convertChessNotationtoPosition("f8"));
								newV.erase(newV.begin() + r->index);
								if (!isValidPosition(newV, piece->color)) {
									king->availableCastleSquares.erase(king->availableCastleSquares.begin() + i);
								}
								else {
									i++;
								}
								p2->setPosition(pos);
								rook->setPosition(rookPos);
							}
						}
						if (king->availableCastleSquares.at(i) == convertChessNotationtoPosition("c8")) {
							p2->setPosition(convertChessNotationtoPosition("c8"));
							Piece* r = getPieceFromPosition(convertChessNotationtoPosition("a8"), newV);
							sf::Vector2i rookPos = r->position;
							if (r->name == "Rook") {
								Rook* rook = dynamic_cast<Rook*>(r);
								rook->setPosition(convertChessNotationtoPosition("d8"));
								newV.erase(newV.begin() + r->index);
								if (!isValidPosition(newV, piece->color)) {
									king->availableCastleSquares.erase(king->availableCastleSquares.begin() + i);
								}
								else {
									i++;
								}
								p2->setPosition(pos);
								rook->setPosition(rookPos);
							}
						}
					}
					for (auto& v : newV) {
						v.reset();
					}
					newV.clear();
				}
			}
			*/

			if (piece->name == "Pawn") {
				Pawn* pawn = dynamic_cast<Pawn*>(piece.get());
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
					std::shared_ptr<Piece> p2 = std::make_shared<Piece>(*piece);
					p2->setPosition(pawn->enPassantPositions.at(i));
					newV.push_back(p2);
					if (!isValidPosition(newV, piece->color)) {
						pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
					}
					else {
						i++;
					}
					p2->setPosition(pos);
					for (auto& v : newV) {
						v.reset();
					}
					newV.clear();
				}
			}
		}
	}
};