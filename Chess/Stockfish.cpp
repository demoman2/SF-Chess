#include "Stockfish.h"

Stockfish::Stockfish(std::string path, int level) : c(path, bp::std_in < os, bp::std_out > is), level(std::clamp(level, -20, 20)), move(""), movePlayed(false) {}

void Stockfish::startStockfish() {
	std::string line;
	os << "uci" << std::endl;
	os << "setoption name Threads value 11" << std::endl;
	os << "setoption name VariantPath value assets/other/variants.ini" << std::endl;
	// os << "setoption name Slow Mover value 1000" << std::endl;
	os << "setoption name Skill Level value " << std::to_string(level) << std::endl;
	os << "isready" << std::endl;
	// Check for Readiness
}

void Stockfish::setVariant(const Chess::Variant variant, bool chess960) {
	std::string chess960String = chess960 ? "true" : "false";
	os << "setoption name UCI_Variant value " << Chess::toString(variant) << std::endl;
	os << "setoption name UCI_Chess960 value " << chess960String << std::endl;
	os << "ucinewgame" << std::endl;
}

void Stockfish::getBestMove(std::string fen, std::string moves) {
	std::string line;
	std::string move_string;
	os << "isready" << std::endl;
	os << "position fen " + fen + " moves" + moves << std::endl;
	os << "go movetime 200" << std::endl;

	while (getline(is, line)) {
		if (!line.compare(0, 8, "bestmove")) {
			move_string = line;
			break;
		}
	}
	if (move_string.empty()) { std::cout << "Stockfish Returned Empty Move!" << std::endl; return; }
	move_string = move_string.substr(9, move_string.size() - 9);
	std::vector<std::string> mv;
	boost::split(mv, move_string, boost::is_any_of(" "));
	move = mv.at(0);
	movePlayed = true;
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
