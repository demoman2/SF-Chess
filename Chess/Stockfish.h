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
#include <SDKDDKVer.h>
#include <iostream>
#include <boost/process.hpp>
#include <boost/process/v1/windows.hpp>
#include <SFML/System/Time.hpp>
#include "ChessUtil.h"

namespace bp = boost::process::v1;

struct StockfishSettings {
	int sfThreads, sfMemory;
	bool usesFixedTime;
	sf::Time fixedTime;

	// Advanced
	bool ponder, UCI_Chess960, UCI_AnalyseMode, UCI_LimitStrength, UCI_ShowWDL, Syzygy50MoveRule, useNNUE, TsumeMode;
	int multiPV, skillLevel, moveOverhead, slowMover, nodestime, UCI_Elo, SyzygyProbeDepth, SyzygyProbeLimit;
	std::string debugLogFile, UCI_Variant, SyzygyPath, EvalFile, VariantPath;

	StockfishSettings();
	StockfishSettings(int sfThreads, int sfMemory, bool usesFixedTime, sf::Time fixedTime, bool ponder, bool UCI_Chess960, bool UCI_AnalyseMode, bool UCI_LimitStrength, bool UCI_ShowWDL, bool Syzygy50MoveRule, bool useNNUE, bool TsumeMode, int multiPV, int skillLevel, int moveOverhead, int slowMover, int nodestime, int UCI_Elo, int SyzygyProbeDepth, int SyzygyProbeLimit, const std::string& debugLogFile, const std::string& UCI_Variant, const std::string& SyzygyPath, const std::string& EvalFile, const std::string& VariantPath);
	~StockfishSettings() {};
};

class Stockfish {

	StockfishSettings settings;
	bool stoppedCalculating;
	bp::ipstream is;
	bp::opstream os;
	bp::child c;

public:
	bool isCalculating, movePlayed;
	std::string move;

	Stockfish(const std::string& path, const StockfishSettings& stockfishSettings);
	~Stockfish();
	void startStockfish();
	void loadSettings(const StockfishSettings& stockfishSettings);
	void setVariant(const std::string& vName, bool chess960);
	void getBestMove(const std::string& fen, const std::string& moves);
	void getBestMoveT(const std::string& fen, const std::string& moves, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement);
	void stopCalculating();
	void stopCalculatingAndPlay();
	void printPosition();
	void clearHash();
	void runCommand(const std::string& command);
	void quit();
	std::string getFEN();
	int getLegalMoveCount(const std::string& fen, const std::string& moves);
	int getLegalMoveCount(const std::string& fen);
	std::vector<std::string> getLegalMoves(const std::string& fen, const std::string& moves); // Debug
	std::vector<std::string> getLegalMoves();
	const StockfishSettings& getSettings() const;

	std::string getMove() const { return move; };
	void resetMove() { move.clear(); };
};