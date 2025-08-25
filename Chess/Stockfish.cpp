#include "Stockfish.h"

Stockfish::Stockfish(std::string path, const StockfishSettings& stockfishSettings) : c(path, bp::std_in < os, bp::std_out > is), move(""), movePlayed(false), isCalculating(false), stoppedCalculating(false), settings(stockfishSettings)
{
	os.boolalpha;
}

Stockfish::~Stockfish()
{
}

void Stockfish::startStockfish() {
	std::string line;
	os << "uci" << std::endl;
	os << "setoption name Clear Hash" << std::endl;
	loadSettings(settings);
	os << "isready" << std::endl;
	// Check for Readiness
}

void Stockfish::loadSettings(const StockfishSettings& stockfishSettings) {
	settings = stockfishSettings;
	os << "setoption name Threads value " << std::to_string(stockfishSettings.sfThreads) << std::endl;
	os << "setoption name Hash value " << std::to_string(stockfishSettings.sfMemory) << std::endl;
	os << "setoption name Skill Level value " << stockfishSettings.skillLevel << std::endl;
	os << "setoption name Debug Log File value " << stockfishSettings.debugLogFile << std::endl;
	os << "setoption name Threads value " << stockfishSettings.sfThreads << std::endl;
	os << "setoption name Hash value " << stockfishSettings.sfMemory << std::endl;
	os << "setoption name Clear Hash value 0" << std::endl;
	os << "setoption name Ponder value " << stockfishSettings.ponder << std::endl;
	os << "setoption name MultiPV value " << stockfishSettings.multiPV << std::endl;
	os << "setoption name Skill Level value " << stockfishSettings.skillLevel << std::endl;
	os << "setoption name Move Overhead value " << stockfishSettings.moveOverhead << std::endl;
	os << "setoption name Slow Mover value " << stockfishSettings.slowMover << std::endl;
	os << "setoption name nodestime value " << stockfishSettings.nodestime << std::endl;
	os << "setoption name UCI_Chess960 value " << stockfishSettings.UCI_Chess960 << std::endl;
	os << "setoption name UCI_Variant value " << stockfishSettings.UCI_Variant << std::endl;
	os << "setoption name UCI_AnalyseMode value " << stockfishSettings.UCI_AnalyseMode << std::endl;
	os << "setoption name UCI_LimitStrength value " << stockfishSettings.UCI_LimitStrength << std::endl;
	os << "setoption name UCI_Elo value " << stockfishSettings.UCI_Elo << std::endl;
	os << "setoption name UCI_ShowWDL value " << stockfishSettings.UCI_ShowWDL << std::endl;
	os << "setoption name SyzygyPath value " << stockfishSettings.SyzygyPath << std::endl;
	os << "setoption name SyzygyProbeDepth value " << stockfishSettings.SyzygyProbeDepth << std::endl;
	os << "setoption name Syzygy50MoveRule value " << stockfishSettings.Syzygy50MoveRule << std::endl;
	os << "setoption name SyzygyProbeLimit value " << stockfishSettings.SyzygyProbeLimit << std::endl;
	os << "setoption name Use NNUE value " << stockfishSettings.useNNUE << std::endl;
	os << "setoption name EvalFile value " << stockfishSettings.EvalFile << std::endl;
	os << "setoption name TsumeMode value " << stockfishSettings.TsumeMode << std::endl;
	os << "setoption name VariantPath value " << stockfishSettings.VariantPath << std::endl;
}

void Stockfish::setVariant(const Chess::Variant variant, bool chess960) {
	std::string chess960String = chess960 ? "true" : "false";
	os << "setoption name UCI_Variant value " << Chess::toString(variant) << std::endl;
	os << "setoption name UCI_Chess960 value " << chess960String << std::endl;
	os << "ucinewgame" << std::endl;
}

void Stockfish::getBestMove(std::string fen, std::string moves)
{
	stoppedCalculating = false;
	std::string line;
	std::string move_string;
	os << "isready" << std::endl;
	os << "position fen " + fen + " moves" + moves << std::endl;
	os << "go movetime " << settings.fixedTime.asMilliseconds() << std::endl;

	while (getline(is, line)) {
		if (stoppedCalculating) {
			if (!line.compare(0, 8, "bestmove")) {
				break;
			}
		}
		else if (!line.compare(0, 8, "bestmove")) {
			move_string = line;
			break;
		}
	}
	if (!stoppedCalculating) {
		if (move_string.empty()) { std::cout << "Stockfish Returned Empty Move!" << std::endl; return; }
		move_string = move_string.substr(9, move_string.size() - 9);
		std::vector<std::string> mv;
		boost::split(mv, move_string, boost::is_any_of(" "));
		move = mv.at(0);
		movePlayed = true;
	}
	isCalculating = false;
}

void Stockfish::getBestMoveT(std::string fen, std::string moves, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncrement)
{
	stoppedCalculating = false;
	std::string line;
	std::string move_string;
	os << "isready" << std::endl;
	os << "position fen " + fen + " moves" + moves << std::endl;
	if (settings.usesFixedTime) {
		os << "go movetime " << settings.fixedTime.asMilliseconds() << std::endl;
	}
	else {
		os << "go wtime " << whiteTime.asMilliseconds() << " btime " << blackTime.asMilliseconds() << " winc " << timeIncrement.asMilliseconds() << " binc " << timeIncrement.asMilliseconds() << std::endl;
	}

	while (getline(is, line)) {
		if (stoppedCalculating) {
			if (!line.compare(0, 8, "bestmove")) {
				break;
			}
		}
		else if (!line.compare(0, 8, "bestmove")) {
			move_string = line;
			break;
		}
	}
	if (!stoppedCalculating) {
		if (move_string.empty()) { std::cout << "Stockfish Returned Empty Move!" << std::endl; return; }
		move_string = move_string.substr(9, move_string.size() - 9);
		std::vector<std::string> mv;
		boost::split(mv, move_string, boost::is_any_of(" "));
		move = mv.at(0);
		movePlayed = true;
	}
	isCalculating = false;
}

void Stockfish::stopCalculating()
{
	if (isCalculating) {
		os << "stop" << std::endl;
		stoppedCalculating = true;
	}
}

void Stockfish::stopCalculatingAndPlay()
{
	if (isCalculating) {
		os << "stop" << std::endl;
	}
}

void Stockfish::printPosition()
{
	std::string line;
	os << "d" << std::endl;
	while (getline(is, line)) {
		std::cout << line << std::endl;
		if (line.compare(0, 4, "Key:") == 0) {
			break;
		}
	}
	std::cout << "Done" << std::endl;
}

void Stockfish::clearHash()
{
	os << "setoption name Clear Hash" << std::endl;
}

void Stockfish::runCommand(const std::string& command)
{
	os << command << std::endl;
}

void Stockfish::quit()
{
	os << "quit" << std::endl;
}

std::string Stockfish::getFEN()
{
	std::string line;
	os << "d" << std::endl;
	while (getline(is, line)) {
		if (line.compare(0, 4, "Fen:") == 0) {
			return line.substr(line.find_first_of(':') + 1, line.size());
		}
	}
	return "";
}

int Stockfish::getLegalMoveCount(std::string fen, std::string moves)
{
	std::string line;
	os << "position fen " + fen + " moves" + moves << std::endl;
	os << "go perft 1" << std::endl;
	while (getline(is, line)) {
		if (line.compare(0, 5, "Nodes") == 0) {
			return std::stoi(line.substr(line.find_first_of(':') + 2, line.size()));
		}
	}
	return -1;
}

int Stockfish::getLegalMoveCount(std::string fen)
{
	std::string line;
	os << "position fen " + fen << std::endl;
	os << "go perft 1" << std::endl;
	while (getline(is, line)) {
		if (line.compare(0, 5, "Nodes") == 0) {
			return std::stoi(line.substr(line.find_first_of(':') + 2, line.size()));
		}
	}
	return -1;
}


std::vector<std::string> Stockfish::getLegalMoves(std::string fen, std::string moves)
{
	std::string line;
	std::vector<std::string> lines;
	os << "position fen " + fen + " moves" + moves << std::endl;
	os << "go perft 1" << std::endl;
	while (getline(is, line)) {
		lines.push_back(line);
		if (line.compare(0, 5, "Nodes") == 0) {
			break;
		}
	}
	return lines;
}

const StockfishSettings& Stockfish::getSettings()
{
	return settings;
}

StockfishSettings::StockfishSettings() :
	sfThreads((std::thread::hardware_concurrency() / 2) - 1), sfMemory(16), usesFixedTime(false), fixedTime(sf::seconds(2)), ponder(false),
	multiPV(1), UCI_Chess960(false), UCI_AnalyseMode(false), UCI_LimitStrength(false), UCI_ShowWDL(false), Syzygy50MoveRule(true), useNNUE(true),
	TsumeMode(false), skillLevel(20), moveOverhead(10), slowMover(100), nodestime(0), UCI_Elo(1350), SyzygyProbeDepth(1), SyzygyProbeLimit(7),
	debugLogFile(""), UCI_Variant("chess"), SyzygyPath(""), EvalFile(""), VariantPath("")
{
}

StockfishSettings::StockfishSettings(int sfThreads, int sfMemory, bool usesFixedTime, sf::Time fixedTime, bool ponder, bool UCI_Chess960, bool UCI_AnalyseMode, bool UCI_LimitStrength, bool UCI_ShowWDL, bool Syzygy50MoveRule, bool useNNUE, bool TsumeMode, int multiPV, int skillLevel, int moveOverhead, int slowMover, int nodestime, int UCI_Elo, int SyzygyProbeDepth, int SyzygyProbeLimit, const std::string& debugLogFile, const std::string& UCI_Variant, const std::string& SyzygyPath, const std::string& EvalFile, const std::string& VariantPath) :
	sfThreads(sfThreads), sfMemory(sfMemory), usesFixedTime(usesFixedTime), fixedTime(fixedTime), ponder(ponder), multiPV(multiPV), UCI_Chess960(UCI_Chess960), UCI_AnalyseMode(UCI_AnalyseMode),
	UCI_LimitStrength(UCI_LimitStrength), UCI_ShowWDL(UCI_ShowWDL), Syzygy50MoveRule(Syzygy50MoveRule), useNNUE(useNNUE), TsumeMode(TsumeMode), skillLevel(skillLevel), moveOverhead(moveOverhead),
	slowMover(slowMover), nodestime(nodestime), UCI_Elo(UCI_Elo), SyzygyProbeDepth(SyzygyProbeDepth), SyzygyProbeLimit(SyzygyProbeLimit), debugLogFile(debugLogFile), UCI_Variant(UCI_Variant), SyzygyPath(SyzygyPath),
	EvalFile(EvalFile), VariantPath(VariantPath)
{
}