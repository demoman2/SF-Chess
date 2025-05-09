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
#include "Board.h"

typedef std::vector<std::shared_ptr<Piece>> pieceVector;
typedef std::vector<std::reference_wrapper<sf::Texture>> textureVector;
typedef std::pair<std::array<std::array<int, 8>, 8>, bool> basicBitboard;
using namespace Chess;
namespace bp = boost::process::v1;
class Main {

public:

	static void playMove(std::string move, Board& board) {
		if (move.empty()) { return; }
		if (move.at(1) == '@') {
			sf::Vector2i dest = convertChessNotationtoPosition(move.substr(2, 2));
			PColor col = board.whiteToPlay ? White : Black;
			int offset = board.whiteToPlay ? 0 : -6;
			board.moveSound.play();
			board.lastMoveStart.setPosition({ -1000, -1000 });
			board.lastMoveDest.setPosition(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
			if (board.whiteToPlay) {
				for (auto& piece : board.whiteDropPieces) {
					if (piece.id == std::tolower(move.at(0))) {
						piece.count--;
						break;
					}
				}
			}
			else {
				for (auto& piece : board.blackDropPieces) {
					if (piece.id == std::tolower(move.at(0))) {
						piece.count--;
						break;
					}
				}
			}
			switch (std::tolower(move.at(0))) {
				// Bishop, King, Knight, Pawn, Queen, Rook
				// Black --> White
			case 'b':
			{
				std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(6 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
				board.pieceList.push_back(bishop);
				board.calculatingPos = true;
				std::thread postMoveF(Main::postMove, bishop, std::ref(board));
				postMoveF.detach();
				break;
			}
			case 'q':
			{
				std::shared_ptr<Queen> queen = std::make_shared<Queen>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(10 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
				board.pieceList.push_back(queen);
				board.calculatingPos = true;
				std::thread postMoveF(Main::postMove, queen, std::ref(board));
				postMoveF.detach();
				break;
			}
			case 'p':
			{
				std::shared_ptr<Pawn> pawn = std::make_shared<Pawn>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(9 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
				board.pieceList.push_back(pawn);
				board.calculatingPos = true;
				std::thread postMoveF(Main::postMove, pawn, std::ref(board));
				postMoveF.detach();
				break;
			}
			case 'n':
			{
				std::shared_ptr<Knight> knight = std::make_shared<Knight>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(8 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
				board.pieceList.push_back(knight);
				board.calculatingPos = true;
				std::thread postMoveF(Main::postMove, knight, std::ref(board));
				postMoveF.detach();
				break;
			}
			case 'r':
			{
				std::shared_ptr<Rook> rook = std::make_shared<Rook>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(11 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
				board.pieceList.push_back(rook);
				board.calculatingPos = true;
				std::thread postMoveF(Main::postMove, rook, std::ref(board));
				postMoveF.detach();
				break;
			}
			}
		}
		else {
			int midPos = 0;
			for (int i = 0; i < move.size() - 1; i++) {
				if (i != 0 && !std::isdigit(move.at(i)) && i != move.size()) {
					midPos = i;
				}
			}
			sf::Vector2i start = convertChessNotationtoPosition(move.substr(0, midPos));
			sf::Vector2i dest;
			std::shared_ptr<Piece> piece = getPieceFromPosition(start, board.pieceList);
			if (piece == nullptr) { return; }
			std::shared_ptr<Piece> capture = nullptr;
			if (!std::isdigit(move.back()) && move.back() != '\r') {
				std::string t{ move.begin() + midPos, move.end() - 1 };
				dest = convertChessNotationtoPosition(t);
				capture = getPieceFromPosition(dest, board.pieceList);
				board.lastMoveStart.setPosition(piece->getGlobalPosition());
				board.lastMoveDest.setPosition(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
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
					std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, piece->color, board.pieceTextures.at(6 + offset), false);
					bishop->promoted = true;
					bishop->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
					board.pieceList.push_back(bishop);
					break;
				}
				case 'q':
				{
					std::shared_ptr<Queen> queen = std::make_shared<Queen>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, piece->color, board.pieceTextures.at(10 + offset), false);
					queen->promoted = true;
					queen->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
					board.pieceList.push_back(queen);
					break;
				}
				case 'k':
				{
					std::shared_ptr<King> king = std::make_shared<King>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, piece->color, board.pieceTextures.at(7 + offset), false);
					king->promoted = true;
					king->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
					board.pieceList.push_back(king);
					break;
				}
				case 'n':
				{
					std::shared_ptr<Knight> knight = std::make_shared<Knight>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, piece->color, board.pieceTextures.at(8 + offset), false);
					knight->promoted = true;
					knight->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
					board.pieceList.push_back(knight);
					break;
				}
				case 'r':
				{
					std::shared_ptr<Rook> rook = std::make_shared<Rook>(dest.x, dest.y, board.pieceScale, board.boardOffset, board.boardMultiplier, piece->color, board.pieceTextures.at(11 + offset), false);
					rook->promoted = true;
					rook->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
					board.pieceList.push_back(rook);
					break;
				}
				}
				for (size_t i = 0; i < board.pieceList.size(); i++) {
					if (board.pieceList.at(i)->position == start) {
						board.pieceList.erase(board.pieceList.begin() + i);
						break;
					}
				}
			}
			else {
				dest = convertChessNotationtoPosition(move.substr(midPos));
				capture = getPieceFromPosition(dest, board.pieceList);
				bool closeCastle = false;
				if (capture != nullptr) {
					if (capture->color == piece->color) {
						if (piece->name == "King" && capture->name == "Rook") {
							closeCastle = true;
						}
						capture.reset();
					}
				}
				board.lastMoveStart.setPosition(piece->getGlobalPosition());
				board.lastMoveDest.setPosition(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
				if (board.check) { board.check = false; }
				// En Passant
				if (board.enPassantPiece != nullptr) {
					board.enPassantPiece->enPassantTarget = false;
					board.enPassantPiece.reset();
				}
				piece->setTarget(getGlobalPosition(dest, board.boardOffset, board.boardMultiplier));
				if (piece->name == "Pawn") {
					if (piece->color == PColor::White) {
						if (dest.y == piece->getLocalPosition().y + 2) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							pawn->enPassantTarget = true;
							board.enPassantPiece = pawn;
						}
						if (dest.x != piece->getLocalPosition().x && capture == nullptr) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							for (auto& piece : board.pieceList) {
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
							board.enPassantPiece = pawn;
						}
						if (dest.x != piece->getLocalPosition().x && capture == nullptr) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							for (auto& piece : board.pieceList) {
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
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wKRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
							else if (dest.x < pieceX - 1) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wQRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
						}
						else {
							if (dest.x > pieceX + 1) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bKRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
							else if (dest.x < pieceX - 1) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bQRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
						}
					}
					else {
						if (piece->isWhite()) {
							if (dest.x > pieceX) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wKRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
							else if (dest.x < pieceX) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wQRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
						}
						else {
							if (dest.x > pieceX) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bKRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 6, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 7, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
							else if (dest.x < pieceX) {
								std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bQRook, piece->getLocalPosition().y }, board.pieceList);
								if (rook != nullptr) {
									rook->setTarget(getGlobalPosition({ 4, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									piece->setTarget(getGlobalPosition({ 3, piece->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
									board.castleKing = piece;
									board.castleRook = rook;
								}
							}
						}
					}
				}

			}
			if (capture != nullptr) {
				board.captureSound.play();
				board.capturePiece = capture;
				capture->setGhostSpriteVisible(true, false);
			}
			else {
				board.moveSound.play();
			}
		}
	}

	static void getBestMove(std::string fen, std::string moves, bp::opstream& os, bp::ipstream& is, bool& move, std::string& result) {
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
		result = mv.at(0);
		move = true;
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

	static sf::Vector2f getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, float boardMultiplier) {
		sf::Vector2f r{ ((position.x - 0.5f) * boardMultiplier), (reverseY(position.y) - 0.5f) * boardMultiplier };
		return r + boardOffset;
	}

	static sf::Vector2f getGlobalPositionF(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier) {
		sf::Vector2f r{ ((position.x - 0.5f) * boardMultiplier), ((9 - position.y) - 0.5f) * boardMultiplier };
		return r + boardOffset;
	}

	static sf::Vector2i getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier) {
		sf::Vector2f v = { std::ceil((position.x - boardOffset.x) / boardMultiplier), 9 - std::ceil((position.y - boardOffset.y) / boardMultiplier) };
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
	static pieceVector generatePositionFromFENID(std::string code, Board& board) {

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
		if (!board.Chess960Enabled) {
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
						board.standardPosition = true;
					}
				}
			}
		}
		for (int i = 0; i < modifiers.size(); i++) {
			std::string modifier = modifiers.at(i);
			if (board.variant == ThreeCheck || board.variant == FiveCheck) {
				switch (i) {
					// Side to Play
				case 0:
					if (modifier.front() == 'w') {
						board.whiteToPlay = true;
					}
					else if (modifier.front() == 'b') {
						board.whiteToPlay = false;
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
									board.wQRook = k + 1;
								}
							}
						}
						else if (l == 'K') {
							whiteCanNeverCastleK = false;
							for (int k = whiteKingX; k < 8; k++) {
								if (end.at(k) == 'R') {
									board.wKRook = k + 1;
								}
							}
						}
						else if (l == 'q') {
							blackCanNeverCastleQ = false;
							for (int k = 0; k < blackKingX; k++) {
								if (start.at(k) == 'r') {
									board.bQRook = k + 1;
								}
							}
						}
						else if (l == 'k') {
							blackCanNeverCastleK = false;
							for (int k = blackKingX; k < 8; k++) {
								if (start.at(k) == 'r') {
									board.bKRook = k + 1;
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
									board.wKRook = x + 1;
									whiteCanNeverCastleK = false;
								}
							}
							else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
								int x = convertChartoX(std::tolower(letter));
								if (end.at(x) == 'R') {
									board.wQRook = x + 1;
									whiteCanNeverCastleQ = false;
								}
							}
						}
						else {
							if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
								int x = convertChartoX(std::tolower(letter));
								if (start.at(x) == 'r') {
									board.bKRook = x + 1;
									blackCanNeverCastleK = false;
								}
							}
							else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
								int x = convertChartoX(std::tolower(letter));
								if (start.at(x) == 'r') {
									board.bQRook = x + 1;
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
						if (board.variant == ThreeCheck) {
							board.blackChecks = 3 - std::stoi(modifier.substr(0, plus));
							board.whiteChecks = 3 - std::stoi(modifier.substr(plus));
						}
						else if (board.variant == FiveCheck) {
							board.blackChecks = 5 - std::stoi(modifier.substr(0, plus));
							board.whiteChecks = 5 - std::stoi(modifier.substr(plus));
						}
					}
					break;
					// Half Moves
				case 4:
					board.halfMoves = std::stoi(modifier);
					break;
					// Full Moves
				case 5:
					board.fullMoves = std::stoi(modifier);
					break;
				}
			}
			else {
				switch (i) {
					// Side to Play
				case 0:
					if (modifier.front() == 'w') {
						board.whiteToPlay = true;
					}
					else if (modifier.front() == 'b') {
						board.whiteToPlay = false;
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
									board.wQRook = k + 1;
								}
							}
						}
						else if (l == 'K') {
							whiteCanNeverCastleK = false;
							for (int k = whiteKingX; k < 8; k++) {
								if (end.at(k) == 'R') {
									board.wKRook = k + 1;
								}
							}
						}
						else if (l == 'q') {
							blackCanNeverCastleQ = false;
							for (int k = 0; k < blackKingX; k++) {
								if (start.at(k) == 'r') {
									board.bQRook = k + 1;
								}
							}
						}
						else if (l == 'k') {
							blackCanNeverCastleK = false;
							for (int k = blackKingX; k < 8; k++) {
								if (start.at(k) == 'r') {
									board.bKRook = k + 1;
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
									board.wKRook = x + 1;
									whiteCanNeverCastleK = false;
								}
							}
							else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
								int x = convertChartoX(std::tolower(letter));
								if (end.at(x) == 'R') {
									board.wQRook = x + 1;
									whiteCanNeverCastleQ = false;
								}
							}
						}
						else {
							if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
								int x = convertChartoX(std::tolower(letter));
								if (start.at(x) == 'r') {
									board.bKRook = x + 1;
									blackCanNeverCastleK = false;
								}
							}
							else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
								int x = convertChartoX(std::tolower(letter));
								if (start.at(x) == 'r') {
									board.bQRook = x + 1;
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
					board.halfMoves = std::stoi(modifier);
					break;
					// Full Moves
				case 4:
					board.fullMoves = std::stoi(modifier);
					break;
				case 5:
					// 3check or 5check
					if (modifier.front() == '+' && modifier.size() >= 4) {
						size_t plus = modifier.find('+', 1);
						board.blackChecks = std::stoi(modifier.substr(1, plus));
						board.whiteChecks = std::stoi(modifier.substr(plus));
					}
					break;
				}
			}
		}
		// ========= RANKS =============
		std::string back = ranks.back();
		if (board.dropsEnabled) {
			size_t dropBegin = back.find('['), dropEnd = back.find(']');
			if (dropBegin != std::string::npos && dropEnd != std::string::npos) {
				std::string drop = ranks.back().substr(dropBegin + 1, dropEnd);
				drop.pop_back();
				for (auto& l : drop) {
					if (std::isupper(l)) {
						for (auto& piece : board.whiteDropPieces) {
							if (piece.id == std::tolower(l)) {
								piece.count += 1;
								break;
							}
						}
					}
					else {
						for (auto& piece : board.blackDropPieces) {
							if (piece.id == l) {
								piece.count += 1;
								break;
							}
						}
					}
				}
				ranks.back() = ranks.back().substr(0, dropBegin);
			}
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
						pieces.push_back(std::make_shared<Bishop>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(0), board.animated));
						break;
					}
					case 'k':
					{
						pieces.push_back(std::make_shared<King>(blackCanNeverCastleK, blackCanNeverCastleQ, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(1), board.animated));
						break;
					}
					case 'n':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(2), board.animated));
						break;
					}
					case 'p':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y - 1 }) {
							std::shared_ptr<Pawn> p = std::make_shared<Pawn>(true, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(3), board.animated);
							board.enPassantPiece = p;
							pieces.push_back(p);
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(3), board.animated));
						}
						break;
					}
					case 'q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(4), board.animated));
						break;
					}
					case 'r':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::Black, board.pieceTextures.at(5), board.animated));
						break;
					}
					// WHITE
					case 'B':
					{
						pieces.push_back(std::make_shared<Bishop>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(6), board.animated));
						break;
					}
					case 'K':
					{
						pieces.push_back(std::make_shared<King>(whiteCanNeverCastleK, whiteCanNeverCastleQ, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(7), board.animated));
						break;
					}
					case 'N':
					{
						pieces.push_back(std::make_shared<Knight>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(8), board.animated));
						break;
					}
					case 'P':
					{
						if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y + 1 }) {
							std::shared_ptr<Pawn> p = std::make_shared<Pawn>(true, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(9), board.animated);
							board.enPassantPiece = p;
							pieces.push_back(p);
						}
						else {
							pieces.push_back(std::make_shared<Pawn>(false, x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(9), board.animated));
						}
						break;
					}
					case 'Q':
					{
						pieces.push_back(std::make_shared<Queen>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(10), board.animated));
						break;
					}
					case 'R':
					{
						pieces.push_back(std::make_shared<Rook>(x, y, board.pieceScale, board.boardOffset, board.boardMultiplier, PColor::White, board.pieceTextures.at(11), board.animated));
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
	static void calculateCastlingRights(std::shared_ptr<King>& king, Board& board) {
		if (king != nullptr) {
			king->canCastleK = false;
			king->canCastleQ = false;
			// WHITE
			PColor color = king->color;
			if (!king->hasMoved && !king->inCheck && king->isWhite()) {
				// Kingside
				if (!king->canNeverCastleK && board.wKRook != -1) {
					if (getPieceFromPosition({ board.wKRook, 1 }, board.pieceList) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wKRook, 1 }, board.pieceList);
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
								if (getPieceFromPosition(sq, board.pieceList) != nullptr) {
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
								for (auto& piece3 : board.pieceList) {
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
				if (!king->canNeverCastleQ && board.wQRook != -1) {
					if (getPieceFromPosition({ board.wQRook, 1 }, board.pieceList) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ board.wQRook, 1 }, board.pieceList);
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
								if (getPieceFromPosition(sq, board.pieceList) != nullptr) {
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
								for (auto& piece3 : board.pieceList) {
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
				if (!king->canNeverCastleK && board.bKRook != -1) {
					if (getPieceFromPosition({ board.bKRook, 8 }, board.pieceList) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bKRook, 8 }, board.pieceList);
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
								if (getPieceFromPosition(sq, board.pieceList) != nullptr) {
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
								for (auto& piece3 : board.pieceList) {
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
				if (!king->canNeverCastleQ && board.bQRook != -1) {
					if (getPieceFromPosition({ board.bQRook, 8 }, board.pieceList) != nullptr) {
						std::shared_ptr<Piece> rook = getPieceFromPosition({ board.bQRook, 8 }, board.pieceList);
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
								if (getPieceFromPosition(sq, board.pieceList) != nullptr) {
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
								for (auto& piece3 : board.pieceList) {
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
	static void setExtraSprites(pieceVector& pieces, float pieceScale, sf::Vector2f boardOffset, float boardMultiplier, textureVector boardTextures) {
		for (auto& piece : pieces) {
			piece->selectionSquares.clear();
			for (auto& square : piece->availablePositions) {
				sf::Sprite newSquare{ boardTextures.at(0) };
				float scale = (pieceScale * 320.0f) / 128.0f;
				newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
				newSquare.setScale(sf::Vector2f(scale, scale));
				newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
				piece->selectionSquares.push_back(newSquare);
			}
			piece->captureSquares.clear();
			for (auto& square : piece->availableCapturePositions) {
				sf::Sprite newSquare{ boardTextures.at(1) };
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
					sf::Sprite newSquare{ boardTextures.at(0) };
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
					sf::Sprite newSquare{ boardTextures.at(0) };
					float scale = (pieceScale * 320.0f) / 128.0f;
					newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
					newSquare.setScale(sf::Vector2f(scale, scale));
					newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
					king->castleSquares.push_back(newSquare);
				}
				for (auto& square : king->captureCastlePositions) {
					sf::Sprite newSquare{ boardTextures.at(1) };
					float scale = (pieceScale * 320.0f) / 128.0f;
					newSquare.setOrigin(newSquare.getLocalBounds().getCenter());
					newSquare.setScale(sf::Vector2f(scale, scale));
					newSquare.setPosition(getGlobalPosition(square, boardOffset, boardMultiplier));
					king->castleCaptureSquares.push_back(newSquare);
				}
			}
		}
	}

	static void calculatePositions(std::shared_ptr<Piece> piece, Board& board) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
						if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
							if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
						if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
							if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
						if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
							if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
						if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
							if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
					if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
						if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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
						if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
							if (getPieceFromPosition(newPos, board.pieceList)->color != piece->color) {
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

				if (isValidSquare(pos) && getPieceFromPosition(pos, board.pieceList) == nullptr) {
					pawnPiece->availablePositions.push_back(pos);

					if (!pawnPiece->hasMoved) {
						if (isValidSquare(pos2) && getPieceFromPosition(pos2, board.pieceList) == nullptr) {
							pawnPiece->availablePositions.push_back(pos2);
						}
					}
				}
				if (isValidSquare(pos3)) {
					if (getPieceFromPosition(pos3, board.pieceList) != nullptr) {
						if (getPieceFromPosition(pos3, board.pieceList)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos3);
						}
					}
				}
				if (isValidSquare(pos4)) {
					if (getPieceFromPosition(pos4, board.pieceList) != nullptr) {
						if (getPieceFromPosition(pos4, board.pieceList)->color != pawnPiece->color) {
							pawnPiece->availableCapturePositions.push_back(pos4);
						}
					}
				}
				// En Passant
				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, board.pieceList) != nullptr) {
					std::shared_ptr<Piece> temp = getPieceFromPosition(sf::Vector2i{ pawnPiece->x + 1, pawnPiece->y }, board.pieceList);
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
				if (getPieceFromPosition(sf::Vector2i{ pawnPiece->x - 1, pawnPiece->y }, board.pieceList) != nullptr) {
					std::shared_ptr<Piece> temp = getPieceFromPosition(sf::Vector2i{ pawnPiece->x - 1, pawnPiece->y }, board.pieceList);
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
								if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
									if (getPieceFromPosition(newPos, board.pieceList)->color != king->color) {
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

			if (board.checksEnabled) {
				bool atomicKings = false;
				if (board.variant == Atomic) {
					for (int i = 0; i < board.pieceList.size(); i++) {
						if (board.pieceList.at(i)->name == "King") {
							std::shared_ptr<Piece> king1 = board.pieceList.at(i);
							for (int x = -1; x <= 1; x++) {
								for (int y = -1; y <= 1; y++) {
									if (x != 0 || y != 0) {
										sf::Vector2i newPos = { king1->x + x, king1->y + y };
										if (isValidSquare(newPos)) {
											if (getPieceFromPosition(newPos, board.pieceList) != nullptr && getPieceFromPosition(newPos, board.pieceList)->name == "King") {
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
						newV.resize(board.pieceList.size());
						for (size_t j = 0; j < board.pieceList.size(); j++)
						{
							newV[j] = board.pieceList[j]->clone();
							if (newV[j]->getLocalPosition() == piece->getLocalPosition()) {
								newV[j]->setLocalPosition(piece->availablePositions.at(i));
							}
						}
						if (!isValidPosition(newV, piece->color, board.variant)) {
							if (board.variant == Atomic && piece->name == "King" && enemyKingOccupies(board.pieceList, piece->availablePositions.at(i), piece->color)) {
								i++;
							}
							else {
								piece->availablePositions.erase(piece->availablePositions.begin() + i);
							}
						}
						else if (board.variant == RacingKings && !isValidPosition(newV, !piece->color, board.variant)) {
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
						newV.resize(board.pieceList.size());
						for (size_t j = 0; j < board.pieceList.size(); j++)
						{
							newV[j] = board.pieceList[j]->clone();
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
						if (!isValidPosition(newV, piece->color, board.variant)) {
							if (board.variant == Atomic && (canCaptureEnemyKing(board.pieceList, piece->availableCapturePositions.at(i), piece->color) && isValidAtomicCapture(board.pieceList, piece->availableCapturePositions.at(i), piece->color))) {
								i++;
							}
							else {
								piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
							}
						}
						else if (board.variant == Atomic && !isValidAtomicCapture(board.pieceList, piece->availableCapturePositions.at(i), piece->color)) {
							piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
						}
						else if (board.variant == RacingKings && !isValidPosition(newV, !piece->color, board.variant)) {
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
							newV.resize(board.pieceList.size());
							for (size_t j = 0; j < board.pieceList.size(); ++j)
							{
								newV[j] = board.pieceList[j]->clone();
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
							if (!isValidPosition(newV, pawn->color, board.variant)) {
								if (board.variant == Atomic && (canCaptureEnemyKing(board.pieceList, pawn->enPassantPositions.at(i), pawn->color) && isValidAtomicCapture(board.pieceList, piece->availableCapturePositions.at(i), piece->color))) {
									i++;
								}
								else {
									pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
								}
							}
							else if (board.variant == Atomic && !isValidAtomicCapture(board.pieceList, pawn->enPassantPositions.at(i), pawn->color)) {
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
						if (!isValidAtomicCapture(board.pieceList, piece->availableCapturePositions.at(i), piece->color)) {
							piece->availableCapturePositions.erase(piece->availableCapturePositions.begin() + i);
						}
						else {
							i++;
						}
					}
					if (piece->name == "Pawn") {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
						for (int i = 0; i < pawn->enPassantPositions.size();) {
							if (!isValidAtomicCapture(board.pieceList, pawn->enPassantPositions.at(i), piece->color)) {
								pawn->enPassantPositions.erase(pawn->enPassantPositions.begin() + i);
							}
							else {
								i++;
							}
						}
					}
				}
			}
			if (board.castlingEnabled) {
				if (piece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					calculateCastlingRights(king, board);
					sf::Vector2i castlePos;
					if (king->canCastleK) {
						if (king->color == PColor::White) {
							sf::Vector2i capturePos = { board.wKRook, 1 };
							pieceVector newV;
							newV.resize(board.pieceList.size());
							for (size_t j = 0; j < board.pieceList.size(); j++)
							{
								newV[j] = board.pieceList[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ board.wKRook, 1 }) {
									v->setLocalPosition(convertChessNotationtoPosition("f1"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("g1"));
								}
							}
							if (!board.checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (board.standardPosition) {
									castlePos = convertChessNotationtoPosition("g1");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, board.variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (board.standardPosition) {
										castlePos = convertChessNotationtoPosition("g1");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { board.bKRook, 8 };
							pieceVector newV;
							newV.resize(board.pieceList.size());
							for (size_t j = 0; j < board.pieceList.size(); j++)
							{
								newV[j] = board.pieceList[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ board.bKRook, 8 }) {
									v->setLocalPosition(convertChessNotationtoPosition("f8"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("g8"));
								}
							}
							if (!board.checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (board.standardPosition) {
									castlePos = convertChessNotationtoPosition("g8");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, board.variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (board.standardPosition) {
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
							sf::Vector2i capturePos = { board.wQRook, 1 };
							pieceVector newV;
							newV.resize(board.pieceList.size());
							for (size_t j = 0; j < board.pieceList.size(); j++)
							{
								newV[j] = board.pieceList[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ board.wQRook, 1 }) {
									v->setLocalPosition(convertChessNotationtoPosition("d1"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("c1"));
								}
							}
							if (!board.checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (board.standardPosition) {
									castlePos = convertChessNotationtoPosition("c1");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, board.variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (board.standardPosition) {
										castlePos = convertChessNotationtoPosition("c1");
										king->castlePositions.push_back(castlePos);
									}
								}
							}
							newV.clear();
						}
						else {
							sf::Vector2i capturePos = { board.bQRook, 8 };
							pieceVector newV;
							newV.resize(board.pieceList.size());
							for (size_t j = 0; j < board.pieceList.size(); j++)
							{
								newV[j] = board.pieceList[j]->clone();
							}
							for (auto& v : newV) {
								// Rook
								if (v->getLocalPosition() == sf::Vector2i{ board.bQRook, 8 }) {
									v->setLocalPosition(convertChessNotationtoPosition("d8"));
								}
								// King
								if (v->getLocalPosition() == king->getLocalPosition()) {
									v->setLocalPosition(convertChessNotationtoPosition("c8"));
								}
							}
							if (!board.checksEnabled) {
								king->captureCastlePositions.push_back(capturePos);
								if (board.standardPosition) {
									castlePos = convertChessNotationtoPosition("c8");
									king->castlePositions.push_back(castlePos);
								}
							}
							else {
								if (isValidPosition(newV, piece->color, board.variant)) {
									king->captureCastlePositions.push_back(capturePos);
									if (board.standardPosition) {
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

	static int determineEnd(Board& board) {
		bool whiteToPlay = board.whiteToPlay;
		// Neither 0, Stalemate 1, Checkmate 2, Insufficient Material 3, 50 Move Rule 4
		if (board.halfMoves >= 100) {
			return 4;
		}
		if (board.variant == Standard) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : board.pieceList) {
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
				if (board.pieceList.size() == 2) {
					return 3;
				}
				if (board.pieceList.size() == 3) {
					if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
						return 3;
					}
				}
				else if (board.pieceList.size() == 4) {
					for (auto& piece : board.pieceList) {
						if (piece->name == "Pawn") {
							return 0;
						}
					}
					int whiteCount = 0, blackCount = 0;
					for (auto& piece : board.pieceList) {
						if (piece->isWhite()) {
							whiteCount++;
						}
						else { blackCount++; }
					}
					if (whiteCount != blackCount) {
						if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen"; })) {
							int knights = 0;
							for (auto& piece : board.pieceList) {
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
						if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
							return 3;
						}
					}
				}
			}
		}
		else if (board.variant == Crazyhouse) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : board.pieceList) {
					if (whiteToPlay == (piece->color == PColor::White) && piece->name == "King") {
						std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
						if (king->inCheck) {
							return 2;
						}
						else {
							if (board.whiteDropPieces.empty() && board.blackDropPieces.empty()) {
								return 1;
							}
						}
					}
				}
			}
			else {
				if (board.whiteDropPieces.empty() && board.blackDropPieces.empty()) {
					if (board.pieceList.size() == 2) {
						return 3;
					}
					if (board.pieceList.size() == 3) {
						if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
							return 3;
						}
					}
					else if (board.pieceList.size() == 4) {
						for (auto& piece : board.pieceList) {
							if (piece->name == "Pawn") {
								return 0;
							}
						}
						int whiteCount = 0, blackCount = 0;
						for (auto& piece : board.pieceList) {
							if (piece->isWhite()) {
								whiteCount++;
							}
							else { blackCount++; }
						}
						if (whiteCount != blackCount) {
							if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen"; })) {
								int knights = 0;
								for (auto& piece : board.pieceList) {
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
							if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Rook" || piece->name == "Queen" || piece->name == "Pawn"; })) {
								return 3;
							}
						}
					}
				}
			}
		}
		else if (board.variant == Antichess) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
				return 2;
			}
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				return 2;
			}
			if (board.pieceList.size() == 2) {
				if (std::all_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->name == "Bishop"; })) {
					int n = board.pieceList.at(0)->x + board.pieceList.at(0)->y % 2;
					if (board.pieceList.at(1)->x + board.pieceList.at(1)->y % 2 != n) {
						return 3;
					}
				}
			}
		}
		else if (board.variant == Atomic) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->name == "King"; })) {
				return 2;
			}
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : board.pieceList) {
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
		else if (board.variant == Horde) {
			if (!whiteToPlay) {
				if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
					for (auto& piece : board.pieceList) {
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
				if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
					return 2;
				}
				if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
					return 1;
				}
			}
		}
		else if (board.variant == ThreeCheck) {
			if (whiteToPlay) {
				if (board.whiteChecks >= 3) {
					return 2;
				}
			}
			else {
				if (board.blackChecks >= 3) {
					return 2;
				}
			}
		}
		else if (board.variant == FiveCheck) {
			if (whiteToPlay) {
				if (board.whiteChecks >= 5) {
					return 2;
				}
			}
			else {
				if (board.blackChecks >= 5) {
					return 2;
				}
			}
		}
		else if (board.variant == KOTH) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : board.pieceList) {
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
			if (std::any_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay != piece->isWhite()) && piece->name == "King" && isInCenter(piece->getLocalPosition()); })) {
				return 2;
			}
		}
		else if (board.variant == RacingKings) {
			if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				return 2;
			}
			bool white = false, black = false;
			for (auto& piece : board.pieceList) {
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
				for (auto& piece : board.pieceList) {
					if (piece->name == "King" && piece->isBlack()) {
						if (std::any_of(piece->availablePositions.begin(), piece->availablePositions.end(), [](sf::Vector2i pos) { return pos.y == 8; }) ||
							std::any_of(piece->availableCapturePositions.begin(), piece->availableCapturePositions.end(), [](sf::Vector2i pos) { return pos.y == 8; })) {
							if (!board.eighthRankWhite) {
								board.eighthRankWhite = true;
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

	static void postMove(std::shared_ptr<Piece> piece, Board& board) {
		board.whiteToPlay = !board.whiteToPlay;
		if (piece->isBlack()) {
			board.fullMoves++;
		}
		bool capturedAtomic = false;
		if (board.capturePiece != nullptr) {
			if (board.variant == Atomic) {
				board.halfMoves = 0;
				capturedAtomic = true;
				for (auto it2 = board.pieceList.begin(); it2 != board.pieceList.end(); it2++) {
					std::shared_ptr<Piece> p = *it2;
					if (p->position == piece->position) {
						board.pieceList.erase(it2);
						break;
					}
				}
				sf::Vector2i cpos = board.capturePiece->getLocalPosition();
				std::vector<sf::Vector2i> piecePositions{ cpos };
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						sf::Vector2i newPos = { board.capturePiece->x + x, board.capturePiece->y + y };
						if (isValidSquare(newPos)) {
							if (getPieceFromPosition(newPos, board.pieceList) != nullptr) {
								if (x != 0 || y != 0) {
									std::shared_ptr<Piece> p = getPieceFromPosition(newPos, board.pieceList);
									if (p->name != "Pawn") {
										piecePositions.push_back(newPos);
									}
								}
							}
						}
					}
				}
				for (auto& pos : piecePositions) {
					for (auto it2 = board.pieceList.begin(); it2 != board.pieceList.end(); it2++) {
						std::shared_ptr<Piece> p = *it2;
						if (p->position == pos) {
							board.pieceList.erase(it2);
							break;
						}
					}
				}
				board.capturePiece.reset();
			}
			else {
				if (board.dropsEnabled) {
					if (piece->isWhite()) {
						if (board.capturePiece->promoted) {
							for (auto& t : board.whiteDropPieces) {
								if (t.id == 'p') {
									t.count += 1;
									break;
								}
							}
						}
						else {
							for (auto& t : board.whiteDropPieces) {
								if (t.id == board.capturePiece->id) {
									t.count += 1;
									break;
								}
							}
						}
					}
					else {
						if (board.capturePiece->promoted) {
							for (auto& t : board.blackDropPieces) {
								if (t.id == 'p') {
									t.count += 1;
									break;
								}
							}
						}
						else {
							for (auto& t : board.blackDropPieces) {
								if (t.id == board.capturePiece->id) {
									t.count += 1;
									break;
								}
							}
						}
					}
				}
				for (int i = 0; i < board.pieceList.size(); i++) {
					if (board.pieceList.at(i) != nullptr && board.pieceList.at(i) == board.capturePiece) {
						board.pieceList.erase(board.pieceList.begin() + i);
						board.halfMoves = 0;
						break;
					}
				}
				board.capturePiece.reset();
			}
		}
		if (!capturedAtomic) {
			if (piece->name == "Pawn") {
				board.halfMoves = 0;
			}
			piece->setTarget({});
			piece->setLocalPosition(Main::getLocalPosition(piece->getGlobalPosition(), board.boardOffset, board.boardMultiplier));
			piece->setGlobalPosition(piece->getGlobalPosition(), board.boardOffset, board.boardMultiplier);
			if (!piece->hasMoved) { piece->hasMoved = true; }
			if (piece->name == "Pawn") {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
				pawn->capturingEnPassant = false;
			}
		}

		board.calculatePositions(false);
		std::cout << Main::determineEnd(board) << std::endl;
		auto pos = Main::savePosition(board.pieceList, board.whiteToPlay);
		int count = 1;
		for (auto& position : board.allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
		}
		board.allPositionsPlayed.push_back(pos);
		board.calculatingPos = false;
	}

	static void postCastle(std::shared_ptr<King> king, std::shared_ptr<Piece> rook, Board& board) {
		board.whiteToPlay = !board.whiteToPlay;
		if (king->isBlack()) {
			board.fullMoves++;
		}
		king->canNeverCastleK = true;
		king->canNeverCastleQ = true;
		king->setTarget({});
		king->setLocalPosition(Main::getLocalPosition(king->getGlobalPosition(), board.boardOffset, board.boardMultiplier));
		king->setGlobalPosition(king->getGlobalPosition(), board.boardOffset, board.boardMultiplier);
		rook->setTarget({});
		rook->setLocalPosition(Main::getLocalPosition(rook->getGlobalPosition(), board.boardOffset, board.boardMultiplier));
		rook->setGlobalPosition(rook->getGlobalPosition(), board.boardOffset, board.boardMultiplier);
		if (!king->hasMoved) { king->hasMoved = true; }
		if (!rook->hasMoved) { rook->hasMoved = true; }

		board.calculatePositions(false);
		std::cout << Main::determineEnd(board) << std::endl;
		auto pos = Main::savePosition(board.pieceList, board.whiteToPlay);
		int count = 1;
		for (auto& position : board.allPositionsPlayed) {
			if (position == pos) {
				count++;
			}
		}
		if (count == 3) {
			// Threefold Repetition
		}
		board.allPositionsPlayed.push_back(pos);
		board.calculatingPos = false;
	}

	static void block_until_gained_focus(sf::Window& window) {
		while (const std::optional event = window.waitEvent()) {
			if (event->is<sf::Event::FocusGained>()) {
				return;
			}
		}
	}
};