#pragma once
#include <SDKDDKVer.h>
#include <iostream>
#include <boost/process.hpp>
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

	Stockfish(std::string path, const StockfishSettings& stockfishSettings);
	~Stockfish();
	void startStockfish();
	void loadSettings(const StockfishSettings& stockfishSettings);
	void setVariant(const Chess::Variant variant, bool chess960);
	void getBestMove(std::string fen, std::string moves);
	void getBestMoveT(std::string fen, std::string moves, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement);
	void stopCalculating();
	void stopCalculatingAndPlay();
	void printPosition();
	void clearHash();
	void runCommand(const std::string& command);
	void quit();
	std::string getFEN();
	int getLegalMoveCount(std::string fen, std::string moves);
	int getLegalMoveCount(std::string fen);
	std::vector<std::string> getLegalMoves(std::string fen, std::string moves); // Debug
	const StockfishSettings& getSettings();

	std::string getMove() const { return move; };
	void resetMove() { move.clear(); };
};