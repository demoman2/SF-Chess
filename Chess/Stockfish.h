#pragma once
#include <SDKDDKVer.h>
#include <iostream>
#include <boost/process.hpp>
#include "Chess.h"

namespace bp = boost::process::v1;
class Stockfish {

private:
	int level;
	bp::ipstream is;
	bp::opstream os;
	bp::child c;

public:
	bool movePlayed;
	std::string move;

	Stockfish(std::string path, int level);
	void startStockfish();
	void setVariant(const Chess::Variant variant, bool chess960);
	void getBestMove(std::string fen, std::string moves);
	void printPosition();
	std::string getFEN();
	int getLegalMoveCount(std::string fen, std::string moves);
	int getLegalMoveCount(std::string fen);
	std::vector<std::string> getLegalMoves(std::string fen, std::string moves); // Debug

	std::string getMove() const { return move; };
	void resetMove() { move.clear(); };
};