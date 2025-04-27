#pragma once
#include <SDKDDKVer.h>
#include <iostream>
#include <fstream>
#include <future>
#include <thread>
#include <vector>
#include <algorithm>
#include <iomanip>
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
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

typedef std::vector<std::shared_ptr<Piece>> pieceVector;
typedef std::vector<std::reference_wrapper<sf::Texture>> textureVector;
typedef std::pair<std::array<std::array<int, 8>, 8>, bool> basicBitboard;
using namespace Chess;
namespace bp = boost::process::v1;
class Main {

public:

	static void playMove(std::string move, pieceVector& pieces, std::shared_ptr<Piece>& capturePiece, std::shared_ptr<Piece>& castleKing, std::shared_ptr<Piece>& castleRook, std::shared_ptr<Piece>& promotePiece,
		std::shared_ptr<Pawn>& enPassantPiece, sf::Sprite& lastMoveStart, sf::Sprite& lastMoveDest, sf::Sound& moveSound, sf::Sound& captureSound, float boardOffset, float boardMultiplier, bool& check,
		int wKRook, int wQRook, int bKRook, int bQRook, float pieceScale, textureVector& pieceTextures) {
		if (move.empty()) { return; }
		int midPos = 0;
		for (int i = 0; i < move.size() - 1; i++) {
			if (i != 0 && !std::isdigit(move.at(i)) && i != move.size()) {
				midPos = i;
			}
		}
		sf::Vector2i start = convertChessNotationtoPosition(move.substr(0, midPos));
		sf::Vector2i dest;
		std::shared_ptr<Piece> piece = getPieceFromPosition(start, pieces);
		if (piece == nullptr) { return; }
		std::shared_ptr<Piece> capture = nullptr;
		if (!std::isdigit(move.back()) && move.back() != '\r') {
			std::string t{ move.begin() + midPos, move.end() - 1 };
			dest = convertChessNotationtoPosition(t);
			capture = getPieceFromPosition(dest, pieces);
			lastMoveStart.setPosition(piece->getGlobalPosition());
			lastMoveDest.setPosition(getGlobalPosition(dest, boardOffset, boardMultiplier));
			if (capture != nullptr && capture->color == piece->color) {
				capture.reset();
			}
			int offset = 0;
			if (piece->isBlack()) {
				offset = -6;
			}
			char c = move.back();
			switch (c) {
				// Bishop, King, Knight, Pawn, Queen, Rook
				// Black --> White
			case 'b':
			{
				std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(dest.x, dest.y, pieceScale, boardOffset, boardMultiplier, piece->color, pieceTextures.at(6 + offset), false);
				bishop->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
				pieces.push_back(bishop);
				break;
			}
			case 'q':
			{
				std::shared_ptr<Queen> queen = std::make_shared<Queen>(dest.x, dest.y, pieceScale, boardOffset, boardMultiplier, piece->color, pieceTextures.at(10 + offset), false);
				queen->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
				pieces.push_back(queen);
				break;
			}
			case 'k':
			{
				std::shared_ptr<King> king = std::make_shared<King>(dest.x, dest.y, pieceScale, boardOffset, boardMultiplier, piece->color, pieceTextures.at(7 + offset), false);
				king->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
				pieces.push_back(king);
				break;
			}
			case 'n':
			{
				std::shared_ptr<Knight> knight = std::make_shared<Knight>(dest.x, dest.y, pieceScale, boardOffset, boardMultiplier, piece->color, pieceTextures.at(8 + offset), false);
				knight->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
				pieces.push_back(knight);
				break;
			}
			case 'r':
			{
				std::shared_ptr<Rook> rook = std::make_shared<Rook>(dest.x, dest.y, pieceScale, boardOffset, boardMultiplier, piece->color, pieceTextures.at(11 + offset), false);
				rook->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
				pieces.push_back(rook);
				break;
			}
			}
			for (size_t i = 0; i < pieces.size(); i++) {
				if (pieces.at(i)->position == start) {
					pieces.erase(pieces.begin() + i);
					break;
				}
			}
		}
		else {
			dest = convertChessNotationtoPosition(move.substr(midPos));
			capture = getPieceFromPosition(dest, pieces);
			bool closeCastle = false;
			if (capture != nullptr) {
				if (capture->color == piece->color) {
					if (piece->name == "King" && capture->name == "Rook") {
						closeCastle = true;
					}
					capture.reset();
				}
			}
			lastMoveStart.setPosition(piece->getGlobalPosition());
			lastMoveDest.setPosition(getGlobalPosition(dest, boardOffset, boardMultiplier));
			if (check) { check = false; }
			// En Passant
			if (enPassantPiece != nullptr) {
				enPassantPiece->enPassantTarget = false;
				enPassantPiece.reset();
			}
			piece->setTarget(getGlobalPosition(dest, boardOffset, boardMultiplier));
			if (piece->name == "Pawn") {
				if (piece->color == PColor::White) {
					if (dest.y == piece->getLocalPosition().y + 2) {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						pawn->enPassantTarget = true;
						enPassantPiece = pawn;
					}
					if (dest.x != piece->getLocalPosition().x && capture == nullptr) {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						for (auto& piece : pieces) {
							if (piece->getLocalPosition() == (dest - sf::Vector2i(0, 1))) {
								capture = piece;
								pawn->capturingEnPassant = true;
								break;
							}
						}
					}
				}
				else {
					if (dest.y == piece->getLocalPosition().y - 2) {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						pawn->enPassantTarget = true;
						enPassantPiece = pawn;
					}
					if (dest.x != piece->getLocalPosition().x && capture == nullptr) {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						for (auto& piece : pieces) {
							if (piece->getLocalPosition() == (dest + sf::Vector2i(0, 1))) {
								capture = piece;
								pawn->capturingEnPassant = true;
								break;
							}
						}
					}
				}
			}
			// Castles
			else if (piece->name == "King") {
				int pieceX = piece->getLocalPosition().x;
				if (!closeCastle) {
					if (piece->isWhite()) {
						if (dest.x > pieceX + 1) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ wKRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX - 1) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ wQRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
					else {
						if (dest.x > pieceX + 1) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ bKRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX - 1) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ bQRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
				}
				else {
					if (piece->isWhite()) {
						if (dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ wKRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ wQRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
					else {
						if (dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ bKRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromPosition({ bQRook, piece->getLocalPosition().y }, pieces);
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, boardOffset, boardMultiplier));
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
				}
			}

		}
		if (capture != nullptr) {
			captureSound.play();
			capturePiece = capture;
			capture->setGhostSpriteVisible(true, false);
		}
		else {
			moveSound.play();
		}
	}

	static void getBestMove(std::string fen, std::string moves, const bp::child& c, bp::opstream& os, bp::ipstream& is, bool& calculatingPos, std::promise<std::string>& result) {
		std::string line;
		std::string move_string;
		os << "isready" << std::endl;
		os << "position fen " + fen + " moves" + moves << std::endl;
		os << "go movetime 1500" << std::endl;

		while (getline(is, line)) {
			if (!line.compare(0, 8, "bestmove")) {
				move_string = line;
				break;
			}
		}

		move_string = move_string.substr(9, move_string.size() - 9);
		std::vector<std::string> mv;
		boost::split(mv, move_string, boost::is_any_of(" "));
		result.set_value(mv.at(0));
		calculatingPos = false;
	}

	static std::string startStockfish(std::string fen, const bp::child& c, bp::opstream& os, bp::ipstream& is, const Variant variant, bool Chess960) {
		std::string line, variantString = "chess", chess960String = "false";
		switch (variant) {
		case Antichess:
			variantString = "antichess";
			break;
		case Atomic:
			variantString = "atomic";
			break;
		case Horde:
			variantString = "horde";
			break;
		case RacingKings:
			variantString = "racingkings";
			break;
		case KOTH:
			variantString = "kingofthehill";
			break;
		case Crazyhouse:
			variantString = "crazyhouse";
			break;
		case ThreeCheck:
			variantString = "3check";
			break;
		case FiveCheck:
			variantString = "5check";
			break;
		}
		if (Chess960) { chess960String = "true"; }
		os << "uci" << std::endl;
		os << "setoption name Threads value 11" << std::endl;
		os << "setoption name Hash value 2048" << std::endl; // ?
		os << "setoption name VariantPath value assets/other/variants.ini" << std::endl;
		os << "setoption name UCI_Variant value " << variantString << std::endl;
		os << "setoption name UCI_Chess960 value " << chess960String << std::endl;
		os << "setoption name Slow Mover value 1000" << std::endl;
		os << "setoption name Skill Level value 0" << std::endl;
		os << "isready" << std::endl;
		os << "position fen " + fen << std::endl;
		os << "ucinewgame" << std::endl;
		os << "go movetime 1000" << std::endl;

		std::string move_string;
		while (getline(is, line)) {
			if (!line.compare(0, 8, "bestmove")) {
				move_string = line;
				break;
			}
		}

		move_string = move_string.substr(9, move_string.size() - 9);
		std::vector<std::string> mv;
		boost::split(mv, move_string, boost::is_any_of(" "));
		return mv.at(0);
	}

	static sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor) {
		factor = std::clamp(factor, 0.0f, 1.0f);
		if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 1.0f) {
			return pointB;
		}
		return pointA + (pointB - pointA) * factor;
	}

	static sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold) {
		factor = std::clamp(factor, 0.0f, 1.0f);
		if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < threshold) {
			return pointB;
		}
		return pointA + (pointB - pointA) * factor;
	}

	// Bishop, King, Knight, Pawn, Queen, Rook
	static void loadPieceSet(sf::Image spriteSheet, textureVector& pieceTextures, int pieceSize) {
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
	static std::shared_ptr<Piece> getPieceFromPosition(sf::Vector2i position, pieceVector& pieces)
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
		return { boardOffset + ((position.x - 0.5f) * boardMultiplier), (reverseY(position.y) - 0.5f) * boardMultiplier };
	}

	static sf::Vector2f getGlobalPositionF(sf::Vector2f position, float boardOffset, float boardMultiplier) {
		return { boardOffset + ((position.x - 0.5f) * boardMultiplier), ((9 - position.y) - 0.5f) * boardMultiplier };
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

	static bool noPieceAt(std::string notation, pieceVector& pieces) {
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
	static pieceVector generatePositionFromFENID(std::string code, textureVector& pieceTextures,
		float pieceScale, float boardOffset, float boardSquareOffset, bool& whiteToPlay, int& halfMoves, int& fullMoves, std::shared_ptr<Pawn>& enPassantPiece,
		sf::Sprite& checkSprite, bool& check, textureVector& extraTextures, bool animated, bool& standardPosition, int& wKRook, int& wQRook, int& bKRook, int& bQRook,
		bool checksEnabled, bool castlingEnabled, bool chess960, int& whiteChecks, int& blackChecks, std::string& whiteDropPieces, std::string& blackDropPieces, Variant variant) {

		pieceVector pieces;
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
		std::string tempfront, tempback;
		if (!chess960) {
			for (int j = 0; j < ranks.front().size(); j++) {
				if (std::isdigit(ranks.front().at(j))) {
					int num = ranks.front().at(j) - '0';
					for (int k = 0; k < num; k++) {
						tempfront.push_back('0');
					}
				}
				else {
					tempfront.push_back(ranks.front().at(j));
				}
			}
			for (int j = 0; j < ranks.back().size(); j++) {
				if (std::isdigit(ranks.back().at(j))) {
					int num = ranks.back().at(j) - '0';
					for (int k = 0; k < num; k++) {
						tempback.push_back('0');
					}
				}
				else {
					tempback.push_back(ranks.back().at(j));
				}
			}
			if (tempfront.front() == 'r' && tempfront.at(7) == 'r') {
				if (tempback.front() == 'R' && tempback.at(7) == 'R') {
					if (tempfront.at(4) == 'k' && tempback.at(4) == 'K') {
						standardPosition = true;
					}
				}
			}
		}
		for (int i = 0; i < modifiers.size(); i++) {
			std::string modifier = modifiers.at(i);
			if (variant == ThreeCheck || variant == FiveCheck) {
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
				case 3:
					if (modifier.size() >= 3) {
						size_t plus = modifier.find('+');
						if (variant == ThreeCheck) {
							blackChecks = 3 - std::stoi(modifier.substr(0, plus));
							whiteChecks = 3 - std::stoi(modifier.substr(plus));
						}
						else if (variant == FiveCheck) {
							blackChecks = 5 - std::stoi(modifier.substr(0, plus));
							whiteChecks = 5 - std::stoi(modifier.substr(plus));
						}
					}
					break;
					// Half Moves
				case 4:
					halfMoves = std::stoi(modifier);
					break;
					// Full Moves
				case 5:
					fullMoves = std::stoi(modifier);
					break;
				}
			}
			else {
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
				case 5:
					// 3check or 5check
					if (modifier.front() == '+' && modifier.size() >= 4) {
						size_t plus = modifier.find('+', 1);
						blackChecks = std::stoi(modifier.substr(1, plus));
						whiteChecks = std::stoi(modifier.substr(plus));
					}
					break;
				}
			}
		}
		// ========= RANKS =============
		std::string back = ranks.back();
		size_t dropBegin = back.find('['), dropEnd = back.find(']');
		if (dropBegin != std::string::npos && dropEnd != std::string::npos) {
			std::string drop = ranks.back().substr(dropBegin + 1, dropEnd);
			drop.pop_back();
			for (auto& l : drop) {
				if (std::isupper(l)) {
					whiteDropPieces += l;
				}
				else {
					blackDropPieces += l;
				}
			}
			ranks.back() = ranks.back().substr(0, dropBegin);
		}
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
				Main::calculatePositions(p, pieces, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
			}
		}
		// Check Detection
		if (checksEnabled) {
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
		}
	next:
		for (auto& p : pieces) {
			if (p->name == "King") {
				Main::calculatePositions(p, pieces, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
			}
		}
		Main::setExtraSprites(pieces, extraTextures);
		return pieces;
	}

	// Simplify later
	static void calculateCastlingRights(std::shared_ptr<King>& king, pieceVector& pieces, bool whiteToPlay, int wKRook, int wQRook, int bKRook, int bQRook) {
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

	static bool isValidPosition(pieceVector& position, PColor color, Variant variant) {
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
				if (piece->name == "King") {
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

	static bool isValidAtomicCapture(pieceVector& pieceList, sf::Vector2i square, PColor color) {
		std::vector<sf::Vector2i> piecePositions{};
		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				sf::Vector2i newPos = { square.x + x, square.y + y };
				if (isValidSquare(newPos)) {
					if (getPieceFromPosition(newPos, pieceList) != nullptr) {
						std::shared_ptr<Piece> p = getPieceFromPosition(newPos, pieceList);
						if (p->name == "King" && p->color == color) {
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	static bool canCaptureEnemyKing(pieceVector& pieceList, sf::Vector2i square, PColor color) {
		std::vector<sf::Vector2i> piecePositions{};
		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				sf::Vector2i newPos = { square.x + x, square.y + y };
				if (isValidSquare(newPos)) {
					if (getPieceFromPosition(newPos, pieceList) != nullptr) {
						std::shared_ptr<Piece> p = getPieceFromPosition(newPos, pieceList);
						if (p->name == "King" && p->color != color) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	static bool enemyKingOccupies(pieceVector& pieceList, sf::Vector2i square, PColor color) {
		for (auto& piece : pieceList) {
			if (piece->name == "King" && piece->color != color) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (x != 0 || y != 0) {
							sf::Vector2i newPos = { piece->x + x, piece->y + y };
							if (isValidSquare(newPos)) {
								if (newPos == square) {
									return true;
								}
							}
						}
					}
				}
			}
		}
		return false;
	}

	// Selection, Capture, Check, Last Move, Selection Hover
	static void setExtraSprites(pieceVector& pieces, textureVector& extraTextures) {
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

	static void calculatePositions(std::shared_ptr<Piece> piece, pieceVector& position, bool whiteToPlay, bool standardPosition, int wKRook, int wQRook, int bKRook, int bQRook, bool checksEnabled, bool castlingEnabled, Variant variant) {
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

			if (checksEnabled) {
				bool atomicKings = false;
				if (variant == Atomic) {
					for (int i = 0; i < position.size(); i++) {
						if (position.at(i)->name == "King") {
							std::shared_ptr<Piece> king1 = position.at(i);
							for (int x = -1; x <= 1; x++) {
								for (int y = -1; y <= 1; y++) {
									if (x != 0 || y != 0) {
										sf::Vector2i newPos = { king1->x + x, king1->y + y };
										if (isValidSquare(newPos)) {
											if (getPieceFromPosition(newPos, position) != nullptr && getPieceFromPosition(newPos, position)->name == "King") {
												atomicKings = true;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
				if (!atomicKings || piece->name == "King") {
					for (int i = 0; i < piece->availablePositions.size();) {
						pieceVector newV;
						newV.resize(position.size());
						for (size_t j = 0; j < position.size(); j++)
						{
							newV[j] = position[j]->clone();
							if (newV[j]->getLocalPosition() == piece->getLocalPosition()) {
								newV[j]->setLocalPosition(piece->availablePositions.at(i));
							}
						}
						if (!isValidPosition(newV, piece->color, variant)) {
							if (variant == Atomic && piece->name == "King" && enemyKingOccupies(position, piece->availablePositions.at(i), piece->color)) {
								i++;
							}
							else {
								piece->availablePositions.erase(piece->availablePositions.begin() + i);
							}
						}
						else if (variant == RacingKings && !isValidPosition(newV, !piece->color, variant)) {
							piece->availablePositions.erase(piece->availablePositions.begin() + i);
						}
						else {
							i++;
						}
						newV.clear();
					}
				}
				if (!atomicKings) {
					for (int i = 0; i < piece->availableCapturePositions.size();) {
						pieceVector newV;
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
						if (!isValidPosition(newV, piece->color, variant)) {
							if (variant == Atomic && (canCaptureEnemyKing(position, piece->availableCapturePositions.at(i), piece->color) && isValidAtomicCapture(position, piece->availableCapturePositions.at(i), piece->color))) {
								i++;
							}
							else {
								piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
							}
						}
						else if (variant == Atomic && !isValidAtomicCapture(position, piece->availableCapturePositions.at(i), piece->color)) {
							piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
						}
						else if (variant == RacingKings && !isValidPosition(newV, !piece->color, variant)) {
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
							pieceVector newV{};
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
							if (!isValidPosition(newV, pawn->color, variant)) {
								if (variant == Atomic && (canCaptureEnemyKing(position, pawn->enPassantPositions.at(i), pawn->color) && isValidAtomicCapture(position, piece->availableCapturePositions.at(i), piece->color))) {
									i++;
								}
								else {
									pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
								}
							}
							else if (variant == Atomic && !isValidAtomicCapture(position, pawn->enPassantPositions.at(i), pawn->color)) {
								pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
							}
							else {
								i++;
							}
							newV.clear();
						}
					}
				}
				else {
					for (int i = 0; i < piece->availableCapturePositions.size();) {
						if (!isValidAtomicCapture(position, piece->availableCapturePositions.at(i), piece->color)) {
							piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
						}
						else {
							i++;
						}
					}
					if (piece->name == "Pawn") {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						for (int i = 0; i < pawn->enPassantPositions.size();) {
							if (!isValidAtomicCapture(position, pawn->enPassantPositions.at(i), piece->color)) {
								pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
							}
							else {
								i++;
							}
						}
					}
				}
			}
			if (castlingEnabled) {
				if (piece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					calculateCastlingRights(king, position, whiteToPlay, wKRook, wQRook, bKRook, bQRook);
					sf::Vector2i castlePos;
					if (king->canCastleK) {
						if (king->color == PColor::White) {
							sf::Vector2i capturePos = { wKRook, 1 };
							pieceVector newV;
							newV.resize(position.size());
							for (size_t j = 0; j < position.size(); j++)
							{
								newV[j] = position[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ wKRook, 1 }) {
									v->setLocalPosition(convertChessNotationtoPosition("f1"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("g1"));
								}
							}
							if (!checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("g1");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (standardPosition) {
										castlePos = convertChessNotationtoPosition("g1");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { bKRook, 8 };
							pieceVector newV;
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
							if (!checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("g8");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (standardPosition) {
										castlePos = convertChessNotationtoPosition("g8");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						king->canMove = true;
					}
					if (king->canCastleQ) {
						if (king->color == PColor::White) {
							sf::Vector2i capturePos = { wQRook, 1 };
							pieceVector newV;
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
							if (!checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("c1");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (standardPosition) {
										castlePos = convertChessNotationtoPosition("c1");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { bQRook, 8 };
							pieceVector newV;
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
							if (!checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (standardPosition) {
									castlePos = convertChessNotationtoPosition("c8");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (standardPosition) {
										castlePos = convertChessNotationtoPosition("c8");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						king->canMove = true;

					}
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

	static bool isInCenter(sf::Vector2i pos) {
		for (int x = 4; x <= 5; x++) {
			for (int y = 4; y <= 5; y++) {
				if (pos == sf::Vector2i{ x, y }) {
					return true;
				}
			}
		}
		return false;
	}

	static int determineEnd(pieceVector& position, bool whiteToPlay, int halfMoves, int& whiteChecks, int& blackChecks, bool& eighthRankWhite, Variant variant) {
		// Neither 0, Stalemate 1, Checkmate 2, Insufficient Material 3, 50 Move Rule 4
		if (halfMoves >= 100) {
			return 4;
		}
		if (variant == Standard || variant == Crazyhouse) {
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
							return 0;
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
							int knights = 0;
							for (auto& piece : position) {
								if (piece->name == "Knight") {
									knights++;
								}
							}
							if (knights == 2) {
								return 3;
							}
						}
					}
					else {
						if (std::none_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
							return 3;
						}
					}
				}
			}
		}
		else if (variant == Antichess) {
			if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
				return 2;
			}
			if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				return 2;
			}
			if (position.size() == 2) {
				if (std::all_of(position.begin(), position.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Bishop"; })) {
					int n = position.at(0)->x + position.at(0)->y % 2;
					if (position.at(1)->x + position.at(1)->y % 2 != n) {
						return 3;
					}
				}
			}
		}
		else if (variant == Atomic) {
			if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->name == "King"; })) {
				return 2;
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
		}
		else if (variant == Horde) {
			if (!whiteToPlay) {
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
			}
			else {
				if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
					return 2;
				}
				if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
					return 1;
				}
			}
		}
		else if (variant == ThreeCheck) {
			if (whiteToPlay) {
				if (whiteChecks >= 3) {
					return 2;
				}
			}
			else {
				if (blackChecks >= 3) {
					return 2;
				}
			}
		}
		else if (variant == FiveCheck) {
			if (whiteToPlay) {
				if (whiteChecks >= 5) {
					return 2;
				}
			}
			else {
				if (blackChecks >= 5) {
					return 2;
				}
			}
		}
		else if (variant == KOTH) {
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
			if (std::any_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay != piece->isWhite()) && piece->name == "King" && isInCenter(piece->getLocalPosition()); })) {
				return 2;
			}
		}
		else if (variant == RacingKings) {
			if (std::none_of(position.begin(), position.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				return 2;
			}
			bool white = false, black = false;
			for (auto& piece : position) {
				if (piece->name == "King" && piece->isWhite() && piece->getLocalPosition().y == 8) {
					white = true;
					if (black) { break; }
				}
				if (piece->name == "King" && !piece->isWhite() && piece->getLocalPosition().y == 8) {
					black = true;
					if (white) { break; }
				}
			}
			if (white && black) {
				return 1;
			}
			if (black) {
				return 2;
			}
			if (white) {
				for (auto& piece : position) {
					if (piece->name == "King" && piece->isBlack()) {
						if (std::any_of(piece->availablePositions.begin(), piece->availablePositions.end(), [](sf::Vector2i pos) { return pos.y == 8; }) ||
							std::any_of(piece->availableCapturePositions.begin(), piece->availableCapturePositions.end(), [](sf::Vector2i pos) { return pos.y == 8; })) {
							if (!eighthRankWhite) {
								eighthRankWhite = true;
								return 0;
							}
							else {
								return 2;
							}
						}
						else {
							return 2;
						}
					}
				}
			}
		}
		return 0;
	}

	static basicBitboard savePosition(pieceVector& position, bool whiteToPlay) {
		std::array<std::array<int, 8>, 8> array{ 0 };
		for (auto& piece : position) {
			if (piece != nullptr) {
				if (piece->isWhite()) {
					if (piece->name == "Bishop") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 1;
					}
					else if (piece->name == "Knight") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 2;
					}
					else if (piece->name == "King") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 3;
					}
					else if (piece->name == "Queen") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 4;
					}
					else if (piece->name == "Rook") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 5;
					}
					else if (piece->name == "Pawn") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 6;
					}
				}
				else {
					if (piece->name == "Bishop") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 7;
					}
					else if (piece->name == "Knight") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 8;
					}
					else if (piece->name == "King") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 9;
					}
					else if (piece->name == "Queen") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 10;
					}
					else if (piece->name == "Rook") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 11;
					}
					else if (piece->name == "Pawn") {
						array.at(static_cast<std::array<std::array<int, 8Ui64>, 8Ui64>::size_type>(piece->x) - 1).at(static_cast<std::array<int, 8Ui64>::size_type>(piece->y) - 1) = 12;
					}
				}
			}
		}
		return std::make_pair(array, whiteToPlay);
	}

	static void postMove(std::shared_ptr<Piece> piece, pieceVector& pieceList,
		float boardOffset, float boardMultiplier, bool& whiteToPlay, bool& check, int& fullMoves, int& halfMoves, sf::Sprite& checkSprite,
		textureVector extraTextures, std::vector<basicBitboard>& allPositionsPlayed, std::shared_ptr<Piece>& capturePiece, int wKRook, int wQRook, int bKRook, int bQRook,
		bool standardPosition, bool checksEnabled, bool castlingEnabled, int& whiteChecks, int& blackChecks, bool& calculatingPos, bool& eighthRankWhite, Variant variant) {

		bool capturedAtomic = false;
		if (capturePiece != nullptr) {
			if (variant == Atomic) {
				halfMoves = 0;
				capturedAtomic = true;
				for (auto it2 = pieceList.begin(); it2 != pieceList.end(); it2++) {
					std::shared_ptr<Piece> p = *it2;
					if (p->position == piece->position) {
						pieceList.erase(it2);
						break;
					}
				}
				sf::Vector2i cpos = capturePiece->getLocalPosition();
				std::vector<sf::Vector2i> piecePositions{ cpos };
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						sf::Vector2i newPos = { capturePiece->x + x, capturePiece->y + y };
						if (isValidSquare(newPos)) {
							if (getPieceFromPosition(newPos, pieceList) != nullptr) {
								if (x != 0 || y != 0) {
									std::shared_ptr<Piece> p = getPieceFromPosition(newPos, pieceList);
									if (p->name != "Pawn") {
										piecePositions.push_back(newPos);
									}
								}
							}
						}
					}
				}
				for (auto& pos : piecePositions) {
					for (auto it2 = pieceList.begin(); it2 != pieceList.end(); it2++) {
						std::shared_ptr<Piece> p = *it2;
						if (p->position == pos) {
							pieceList.erase(it2);
							break;
						}
					}
				}
				capturePiece.reset();
			}
			else {
				for (int i = 0; i < pieceList.size(); i++) {
					if (pieceList.at(i) != nullptr && pieceList.at(i) == capturePiece) {
						pieceList.erase(pieceList.begin() + i);
						halfMoves = 0;
						break;
					}
				}
			}
		}
		if (!capturedAtomic) {
			if (piece->name == "Pawn") {
				halfMoves = 0;
			}
			if (piece->isBlack()) {
				fullMoves++;
			}
			piece->setTarget({});
			piece->setLocalPosition(Main::getLocalPosition(piece->getGlobalPosition(), boardOffset, boardMultiplier));
			piece->setGlobalPosition(piece->getGlobalPosition());
			if (!piece->hasMoved) { piece->hasMoved = true; }
			if (piece->name == "Pawn") {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
				pawn->capturingEnPassant = false;
			}
		}

		if (checksEnabled) {
			for (auto& p : pieceList) {
				if (whiteToPlay == p->isWhite()) {
					if (p->name != "King") {
						Main::calculatePositions(p, pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
					}
				}
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
									if (whiteToPlay) {
										blackChecks++;
									}
									else {
										whiteChecks++;
									}
									goto next;
								}
							}
						}
					}
					break;
				}
			}
		next:
			for (auto& p : pieceList) {
				if (whiteToPlay != p->isWhite()) {
					if (p->name != "King") {
						Main::calculatePositions(p, pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
					}
				}
			}
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name == "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
		}
		else {
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name == "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
		}
		if (variant == Antichess) {
			bool hasCapture = false;
			for (auto& p : pieceList) {
				if (p != nullptr && whiteToPlay != p->isWhite()) {
					if (!p->availableCapturePositions.empty()) {
						hasCapture = true;
						break;
					}
					else if (p->name == "Pawn") {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(p);
						if (!pawn->enPassantPositions.empty()) {
							hasCapture = true;
							break;
						}
					}
				}
			}
			if (hasCapture) {
				for (auto& p : pieceList) {
					if (whiteToPlay != p->isWhite()) {
						p->availablePositions.clear();
					}
				}
			}
		}
		Main::setExtraSprites(pieceList, extraTextures);
		whiteToPlay = !whiteToPlay;
		std::cout << Main::determineEnd(pieceList, whiteToPlay, halfMoves, whiteChecks, blackChecks, eighthRankWhite, variant) << std::endl;
		auto pos = Main::savePosition(pieceList, whiteToPlay);
		int count = 1;
		for (auto& position : allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
		}
		allPositionsPlayed.push_back(pos);
		calculatingPos = false;
	}

	static void postCastle(std::shared_ptr<King> king, std::shared_ptr<Piece> rook, pieceVector& pieceList,
		float boardOffset, float boardMultiplier, bool& whiteToPlay, bool& check, int& fullMoves, int& halfMoves, sf::Sprite& checkSprite,
		textureVector extraTextures, std::vector<basicBitboard>& allPositionsPlayed,
		int wKRook, int wQRook, int bKRook, int bQRook, bool standardPosition, bool checksEnabled, bool castlingEnabled, int& whiteChecks, int& blackChecks, bool& calculatingPos, bool& eighthRankWhite, Variant variant) {
		if (king->isBlack()) {
			fullMoves++;
		}
		king->canNeverCastleK = true;
		king->canNeverCastleQ = true;
		king->setTarget({});
		king->setLocalPosition(Main::getLocalPosition(king->getGlobalPosition(), boardOffset, boardMultiplier));
		king->setGlobalPosition(king->getGlobalPosition());
		rook->setTarget({});
		rook->setLocalPosition(Main::getLocalPosition(rook->getGlobalPosition(), boardOffset, boardMultiplier));
		rook->setGlobalPosition(rook->getGlobalPosition());
		if (!king->hasMoved) { king->hasMoved = true; }
		if (!rook->hasMoved) { rook->hasMoved = true; }
		if (checksEnabled) {
			for (auto& p : pieceList) {
				if (whiteToPlay == p->isWhite()) {
					if (p->name != "King") {
						Main::calculatePositions(p, pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
					}
				}
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
									if (whiteToPlay) {
										blackChecks++;
									}
									else {
										whiteChecks++;
									}
									goto next;
								}
							}
						}
					}
					break;
				}
			}
		next:
			for (auto& p : pieceList) {
				if (whiteToPlay != p->isWhite()) {
					if (p->name != "King") {
						Main::calculatePositions(p, pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
					}
				}
			}
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name == "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
		}
		else {
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name != "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->name == "King") {
					Main::calculatePositions(pieceList.at(i), pieceList, whiteToPlay, standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, variant);
				}
			}
		}
		Main::setExtraSprites(pieceList, extraTextures);
		whiteToPlay = !whiteToPlay;
		std::cout << Main::determineEnd(pieceList, whiteToPlay, halfMoves, whiteChecks, blackChecks, eighthRankWhite, variant) << std::endl;
		auto pos = Main::savePosition(pieceList, whiteToPlay);
		int count = 1;
		for (auto& position : allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
		}
		allPositionsPlayed.push_back(pos);
		calculatingPos = false;
	}

	static void block_until_gained_focus(sf::Window& window) {
		while (const std::optional event = window.waitEvent()) {
			if (event->is<sf::Event::FocusGained>()) {
				return;
			}
		}
	}
};