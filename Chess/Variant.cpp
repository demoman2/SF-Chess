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

#include "Variant.h"

const std::regex Chess::Variant::customPiecePattern{ "customPiece\\d{1,2}" };

Chess::Variant::Variant(const std::string& variantStr, const std::vector<std::shared_ptr<Chess::Variant>>& inheritList) :
	nChecksWhite(0), nChecksBlack(0), nMoveRule(50), nFoldRule(3), flagPieceCount(1), extinctionPieceCount(0), startingFEN(""), boardSize(8, 8), dropNoDoubledCount(1),
	startKRookW(1), startQRookW(8), startKRookB(1), startQRookB(8),

	hasPromotion(false), allowsChess960(false), firstRankPawnDrops(false), promotionZonePawnDrops(false), extinctionPseudoRoyal(false), piecePromotionOnCapture(false), gating(false), seirawanGating(false),
	checksEnabled(true), blockChecks(false), castlingEnabled(true), dropsEnabled(false), atomicExplosions(false), dropPromotedState(false), flyingGeneral(false),

	capturesToHand(false), forceCapture(false), forceDrop(false), forcePawnPromotion(true), immobilityIllegal(false), pieceDemotion(false),
	forcePiecePromotion(false), pawnDoubleStep(true), extraFlagMove(false), nCheckCounting(false), dropLoop(false), dropOppositeColoredBishop(false), castlingDroppedPiece(false),

	enPassantPieces("p"), pawnPieces("p"), castlingKingPiece("k"), castlingRookPieces("r"),
	pieceTypes({ {"pawn", 'p'}, {"king", 'k'}, {"queen", 'q'}, {"rook", 'r'}, {"bishop", 'b'}, {"knight", 'n'}}),

	whiteDoubleStepRegion("*2"), blackDoubleStepRegion("*7"), whitePromotionRegion("*8"), blackPromotionRegion("*1"),
	whiteDropRegion("*"), blackDropRegion("*"), 

	castleRank(1), castleKDestination(7), castleQDestination(3),
	checkmateValue(Chess::WinValue::Loss), nFoldValue(Chess::WinValue::Draw),
	stalemateValue(Chess::WinValue::Draw), extinctionValue(Chess::WinValue::None)
{
	loadFromStr(variantStr, inheritList);
}

void Chess::Variant::loadFromStr(const std::string& variantStr, const std::vector<std::shared_ptr<Chess::Variant>>& inheritList)
{
	VariantSetters vSetters = getSetters();
	// set main variables
	std::string modifiedStr, nonPieceStr, vLine;
	std::stringstream stream(variantStr);
	while (std::getline(stream, vLine, '\n')) {
		if (vLine.front() == '[') {
			modifiedStr += vLine + '\n';
		}
		else {
			std::string key;
			try {
				key = vLine.substr(0, vLine.find(" = "));
			}
			catch (std::exception e) { std::cout << e.what() << " " << vLine << " " << key << '\n'; }
			if (pieceStrings.count(key) > 0 || std::regex_match(key, customPiecePattern)) {
				modifiedStr += vLine + '\n';
			}
			else { nonPieceStr += vLine + '\n'; }
		}
	}
	modifiedStr += nonPieceStr;
	std::stringstream stream_2(modifiedStr);
	while (std::getline(stream_2, vLine, '\n')) {
		if (vLine.empty()) { continue; }

		// name, inherit
		if (vLine.front() == '[') {
			if (vLine.find(':') != vLine.npos) {
				std::string vName = vLine.substr(vLine.find('[') + 1, vLine.find(':') - vLine.find('[') - 1);
				inherit = vLine.substr(vLine.find(':') + 1, vLine.find(']') - vLine.find(':') - 1);
				for (auto& v : inheritList) {
					if (v->name == inherit) {
						*this = *v;
						break;
					}
				}
				name = vName;
			}
			else {
				name = vLine.substr(vLine.find('[') + 1, vLine.find(']') - vLine.find('[') - 1);
			}
		}
		else {
			std::string key, value;
			try {
				key = vLine.substr(0, vLine.find(" = "));
				value = vLine.substr(vLine.find(" = ") + 3);
			}
			catch (std::exception e) { std::cout << e.what() << " " << vLine << " " << key << " " << value << '\n'; }

			setVar(vSetters, key, value);
		}
	}

	// drop pieces
	if (capturesToHand) {
		for (auto& p : pieceTypes) {
			bool find = false;
			for (auto& t : promotedPieceTypes) {
				if (t.second == p.second) {
					find = true;
				}
			}
			if (!royalPieces.has(p.second) && (dropLoop || !find)) {
				whiteDropPieces.add(p.second);
				blackDropPieces.add(p.second);
			}
		}
	}
	// promotion
	if (!whitePromotePieces.empty() || !blackPromotePieces.empty()) {
		hasPromotion = true;
	}
	if (nCheckCounting) {
		if (startingFEN.find('+') != startingFEN.npos) {
			size_t start = (findNthOf(startingFEN, ' ', 4)) + 1,
				plus = startingFEN.find('+'),
				end = (findNthOf(startingFEN, ' ', 5));
			nChecksWhite = std::stoi(startingFEN.substr(start, plus - start + 1));
			nChecksBlack = std::stoi(startingFEN.substr(plus + 1, end - plus));
		}
	}
	if (extinctionPseudoRoyal) {
		for (auto& piece : whiteExtinctionPieces) {
			castlingRookPieces.remove(piece);
		}
		for (auto& piece : blackExtinctionPieces) {
			castlingRookPieces.remove(piece);
		}
	}
	if (castlingEnabled) {
		std::vector<std::string> splitString = split(startingFEN, ' ');

		int whiteKingX = -1, blackKingX = -1;
		std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
		std::vector<std::string> ranks = split(splitString.front(), '/');
		if (ranks.size() > 1 && Chess::reverseY(castleRank, boardSize) - 1 < ranks.size()) {
			std::vector<char> start, end;
			int cRank = castleRank - 1;
			int cRankReverse = Chess::reverseY(castleRank, boardSize) - 1;
			for (int j = 0; j < ranks.at(cRank).size(); j++) {
				char c = ranks.at(cRank).at(j);
				if (std::isdigit(c)) {
					int k = j;
					for (; k < ranks.at(cRank).size(); k++) {
						if (!std::isdigit(ranks.at(cRank).at(k))) {
							k--;
							break;
						}
					}
					int num = std::stoi(ranks.at(cRank).substr(j, k - j + 1));
					j = k;
					for (int k = 0; k < num; k++) {
						start.push_back('0');
					}
				}
				else if (std::isalpha(c)) {
					start.push_back(c);
				}
			}
			for (int j = 0; j < ranks.at(cRankReverse).size(); j++) {
				char c = ranks.at(cRankReverse).at(j);
				if (c == '[') { break; }
				if (std::isdigit(c)) {
					int k = j;
					for (; k < ranks.at(cRankReverse).size(); k++) {
						if (!std::isdigit(ranks.at(cRankReverse).at(k))) {
							k--;
							break;
						}
					}
					int num = std::stoi(ranks.at(cRankReverse).substr(j, k - j + 1));
					j = k;
					for (int k = 0; k < num; k++) {
						end.push_back('0');
					}
				}
				else if (std::isalpha(c)) {
					end.push_back(c);
				}
			}

			if (modifiers.size() > 1 && modifiers.at(1) != "-") {
				std::string modifier = modifiers.at(1);
				for (int j = 0; j < end.size(); j++) {
					if (castlingKingPiece.has(std::tolower(end.at(j))) && std::isupper(end.at(j))) {
						whiteKingX = j + 1;
					}
				}
				for (int j = 0; j < start.size(); j++) {
					if (castlingKingPiece.has(start.at(j)) && std::islower(start.at(j))) {
						blackKingX = j + 1;
					}
				}
				if (whiteKingX != -1) {
					for (int k = 0; k < whiteKingX; k++) {
						if (castlingRookPieces.has(std::tolower(end.at(k)))) {
							startQRookW = k + 1;
						}
					}

					for (int k = whiteKingX; k < boardSize.x; k++) {
						if (castlingRookPieces.has(std::tolower(end.at(k)))) {
							startKRookW = k + 1;
						}
					}
				}
				if (blackKingX != -1) {
					for (int k = 0; k < blackKingX; k++) {
						if (castlingRookPieces.has(start.at(k))) {
							startQRookB = k + 1;
						}
					}
					for (int k = blackKingX; k < boardSize.x; k++) {
						if (castlingRookPieces.has(start.at(k))) {
							startKRookB = k + 1;
						}
					}
				}
			}
		}
	}
	std::vector<std::string> splitStartFEN = split(startingFEN, ' ');
	std::vector<std::string> startFENRanks = split(splitStartFEN.front(), '/');
	while (startFENRanks.size() < boardSize.y) {
		startFENRanks.push_back(std::to_string(boardSize.x));
	}
	sRanks.clear();
	for (auto& rank : startFENRanks) {
		std::string r;
		for (int j = 0; j < rank.size(); j++) {
			char c = rank.at(j);
			if (std::isdigit(c)) {
				int k = j;
				for (; k < rank.size(); k++) {
					if (!std::isdigit(rank.at(k))) {
						k--;
						break;
					}
				}
				int num = std::stoi(rank.substr(j, k - j + 1));
				j = k;
				for (int k = 0; k < num; k++) {
					r.push_back('0');
				}
			}
			else if (c == '+' && j != rank.size() - 1) {
				auto find = promotedPieceTypes.find((char)std::tolower(rank.at(j + 1)));
				if (find != promotedPieceTypes.end()) {
					r.push_back(find->second);
					j++;
				}
			}
			else if (std::isalpha(c)) {
				r.push_back(c);
			}
		}
		while (r.size() < boardSize.x) {
			r.push_back('0');
		}
		sRanks.push_back(r);
	}
	std::reverse(sRanks.begin(), sRanks.end());

	checksEnabled = !royalPieces.empty() || ((!whiteExtinctionPieces.empty() || !blackExtinctionPieces.empty()) && extinctionPseudoRoyal);
}

void Chess::Variant::addPiece(const std::string& pieceStr, const std::string& defaultMoveStr, const std::string& pieceName) {
	if (!pieceStr.empty()) {
		char pieceID = std::tolower(pieceStr.front());
		if (pieceID == '-') {
			removePiece(pieceName);
		}
		else {
			auto find = std::find_if(pieceTypes.begin(), pieceTypes.end(), [pieceID](auto& p) { return p.second == pieceID; });
			if (find != pieceTypes.end() && find->first != pieceName) {
				removePiece(pieceID);
			}
			if (pieceName == "janggiCannon") {
				janggiCannons.add(pieceID);
			}
			pieceTypes.erase(pieceName);
			pieceTypes.emplace(pieceName, pieceID);
			pieceMoves.erase(pieceID);
			if (pieceStr.find(':') != pieceStr.npos) {
				pieceMoves.emplace(pieceID, Chess::getMoveSets(pieceStr.substr(2)));
			}
			else {
				pieceMoves.emplace(pieceID, Chess::getMoveSets(defaultMoveStr));
			}

		}
	}
}

void Chess::Variant::removePiece(const std::string& pieceName)
{
	if (pieceTypes.find(pieceName) != pieceTypes.end()) {
		char id = pieceTypes.find(pieceName)->second;
		pawnPiecesBlastImmunity.remove(id);
		mutuallyImmunePieces.remove(id);
		atomicImmunePieces.remove(id);
		whiteExtinctionPieces.remove(id);
		blackExtinctionPieces.remove(id);
		whiteFlagPieces.remove(id);
		blackFlagPieces.remove(id);
		castlingKingPiece.remove(id);
		castlingRookPieces.remove(id);
		royalPieces.remove(id);
		promotionPawnPieces.remove(id);
		enPassantPieces.remove(id);
		nMoveRulePieces.remove(id);
		whiteDropPieces.remove(id);
		blackDropPieces.remove(id);
		whitePromotePieces.remove(id);
		blackPromotePieces.remove(id);
		pawnPieces.remove(id);
		pieceMoves.erase(id);
		pieceTypes.erase(pieceName);
	}
}

void Chess::Variant::removePiece(char id)
{
	pawnPiecesBlastImmunity.remove(id);
	mutuallyImmunePieces.remove(id);
	atomicImmunePieces.remove(id);
	whiteExtinctionPieces.remove(id);
	blackExtinctionPieces.remove(id);
	whiteFlagPieces.remove(id);
	blackFlagPieces.remove(id);
	castlingKingPiece.remove(id);
	castlingRookPieces.remove(id);
	royalPieces.remove(id);
	promotionPawnPieces.remove(id);
	enPassantPieces.remove(id);
	nMoveRulePieces.remove(id);
	whiteDropPieces.remove(id);
	blackDropPieces.remove(id);
	whitePromotePieces.remove(id);
	blackPromotePieces.remove(id);
	pawnPieces.remove(id);
	for (auto it = pieceTypes.begin(); it != pieceTypes.end();) {
		if (it->second == id) {
			it = pieceTypes.erase(it);
		}
		else { it++; }
	}
}

Chess::Variant::VariantSetters Chess::Variant::getSetters()
{
	VariantSetters vSetters;
	vSetters["startFen"] = [&](const std::string& str) { startingFEN = str; };
	vSetters["chess960"] = [&](const std::string& str) { allowsChess960 = strToBool(str); };
	vSetters["nMoveRule"] = [&](const std::string& str) { nMoveRule = std::stoi(str); };
	vSetters["nFoldRule"] = [&](const std::string& str) { nFoldRule = std::stoi(str); };
	vSetters["flagPieceCount"] = [&](const std::string& str) { flagPieceCount = std::stoi(str); };
	vSetters["extinctionPieceCount"] = [&](const std::string& str) { extinctionPieceCount = std::stoi(str); };
	vSetters["dropNoDoubledCount"] = [&](const std::string& str) { dropNoDoubledCount = std::stoi(str); };
	vSetters["maxRank"] = [&](const std::string& str) {
		boardSize.y = std::stoi(str);
		};
	vSetters["maxFile"] = [&](const std::string& str) {
		boardSize.x = std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); }) ? std::stoi(str) : convertChartoX(str.front()) + 1;
		};
	vSetters["firstRankPawnDrops"] = [&](const std::string& str) { firstRankPawnDrops = strToBool(str); };
	vSetters["promotionZonePawnDrops"] = [&](const std::string& str) { promotionZonePawnDrops = strToBool(str); };
	vSetters["checking"] = [&](const std::string& str) { blockChecks = !strToBool(str); };
	vSetters["castling"] = [&](const std::string& str) { castlingEnabled = strToBool(str); };
	vSetters["pieceDrops"] = [&](const std::string& str) { dropsEnabled = strToBool(str); };
	vSetters["blastOnCapture"] = [&](const std::string& str) { atomicExplosions = strToBool(str); };
	vSetters["dropPromoted"] = [&](const std::string& str) { dropPromotedState = strToBool(str); };
	vSetters["capturesToHand"] = [&](const std::string& str) { capturesToHand = strToBool(str); };
	vSetters["mustCapture"] = [&](const std::string& str) { forceCapture = strToBool(str); };
	vSetters["mustDrop"] = [&](const std::string& str) { forceDrop = strToBool(str); };
	vSetters["flyingGeneral"] = [&](const std::string& str) { flyingGeneral = strToBool(str); };
	vSetters["immobilityIllegal"] = [&](const std::string& str) { immobilityIllegal = strToBool(str); };
	vSetters["mandatoryPiecePromotion"] = [&](const std::string& str) { forcePiecePromotion = strToBool(str); };
	vSetters["pieceDemotion"] = [&](const std::string& str) { pieceDemotion = strToBool(str); };
	vSetters["gating"] = [&](const std::string& str) { gating = strToBool(str); };
	vSetters["seirawanGating"] = [&](const std::string& str) { seirawanGating = strToBool(str); };
	vSetters["mandatoryPawnPromotion"] = [&](const std::string& str) {
		forcePawnPromotion = strToBool(str);
		};
	vSetters["doubleStep"] = [&](const std::string& str) { pawnDoubleStep = strToBool(str); };
	vSetters["flagMove"] = [&](const std::string& str) { extraFlagMove = strToBool(str); };
	vSetters["checkCounting"] = [&](const std::string& str) { nCheckCounting = strToBool(str); };
	vSetters["dropLoop"] = [&](const std::string& str) { dropLoop = strToBool(str); };
	vSetters["castlingDroppedPiece"] = [&](const std::string& str) { castlingDroppedPiece = strToBool(str); };
	vSetters["dropOppositeColoredBishop"] = [&](const std::string& str) { dropOppositeColoredBishop = strToBool(str); };
	vSetters["extinctionPseudoRoyal"] = [&](const std::string& str) { extinctionPseudoRoyal = strToBool(str); };
	vSetters["castlingRank"] = [&](const std::string& str) { castleRank = std::stoi(str); };
	vSetters["castlingKingsideFile"] = [&](const std::string& str) {
		castleKDestination = std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); }) ? std::stoi(str) : convertChartoX(str.front()) + 1; };
	vSetters["castlingQueensideFile"] = [&](const std::string& str) {
		castleQDestination = std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); }) ? std::stoi(str) : convertChartoX(str.front()) + 1;
		};
	vSetters["castlingKingFile"] = [&](const std::string& str) {
		castlingKingFile = std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); }) ? std::stoi(str) : convertChartoX(str.front()) + 1;
		};
	vSetters["mutuallyImmuneTypes"] = [&](const std::string& str) { mutuallyImmunePieces.set(str); };
	vSetters["blastImmuneTypes"] = [&](const std::string& str) { atomicImmunePieces.set(str); };
	vSetters["dropNoDoubled"] = [&](const std::string& str) { dropNoDoubledPieces.set(str); };
	vSetters["extinctionPieceTypes"] = [&](const std::string& str) {
		whiteExtinctionPieces.set(str);
		blackExtinctionPieces.set(str);
		};
	vSetters["flagPiece"] = [&](const std::string& str) {
		whiteFlagPieces.set(str);
		blackFlagPieces.set(str);
		};
	vSetters["flagPieceWhite"] = [&](const std::string& str) { whiteFlagPieces.set(str); };
	vSetters["flagPieceBlack"] = [&](const std::string& str) { blackFlagPieces.set(str); };
	vSetters["pawnTypes"] = [&](const std::string& str) {
		pawnPieces.set(str);
		};
	vSetters["promotionPawnTypes"] = [&](const std::string& str) { promotionPawnPieces.set(str); };
	vSetters["enPassantTypes"] = [&](const std::string& str) { enPassantPieces.set(str); };
	vSetters["nMoveRuleTypes"] = [&](const std::string& str) { nMoveRulePieces.set(str); };
	vSetters["royalTypes"] = [&](const std::string& str) { royalPieces.set(str); };
	vSetters["promotionPieceTypesWhite"] = [&](const std::string& str) {
		whitePromotePieces.set(str);
		};
	vSetters["promotionPieceTypesBlack"] = [&](const std::string& str) {
		blackPromotePieces.set(str);
		};
	vSetters["promotionPieceTypes"] = [&](const std::string& str) {
		whitePromotePieces.set(str);
		blackPromotePieces.set(str);
		};
	vSetters["promotionLimit"] = [&](const std::string& str) {
		for (auto& s : split(str, ' ')) {
			promotionLimit.emplace(s.front(), std::stoi(s.substr(2)));
		}
		};
	vSetters["kingType"] = [&](const std::string& str) {
		if (str.front() == '-') {
			removePiece("kingType");
		}
		else {
			pieceTypes.emplace("kingType", str.front());
			royalPieces.add(str);
		}
		};
	vSetters["castlingRookPieces"] = [&](const std::string& str) {
		castlingRookPieces.set(str);
		};
	vSetters["castlingKingPiece"] = [&](const std::string& str) {
		castlingKingPiece.set(str);
		};
	vSetters["nFoldValue"] = [&](const std::string& str) { nFoldValue = Chess::getWinValue(str); };
	vSetters["checkmateValue"] = [&](const std::string& str) { checkmateValue = Chess::getWinValue(str); };
	vSetters["extinctionValue"] = [&](const std::string& str) { extinctionValue = Chess::getWinValue(str); };
	vSetters["stalemateValue"] = [&](const std::string& str) { stalemateValue = Chess::getWinValue(str); };

	vSetters["promotionRegionWhite"] = [&](const std::string& str) { whitePromotionRegion.load(str); };
	vSetters["promotionRegionBlack"] = [&](const std::string& str) { blackPromotionRegion.load(str); };

	vSetters["dropRegionWhite"] = [&](const std::string& str) { whiteDropRegion.load(str); };
	vSetters["dropRegionBlack"] = [&](const std::string& str) { blackDropRegion.load(str); };
	vSetters["flagRegion"] = [&](const std::string& str) {
		whiteFlagRegion.load(str);
		blackFlagRegion.load(str);
		};
	vSetters["flagRegionWhite"] = [&](const std::string& str) { whiteFlagRegion.load(str); };
	vSetters["flagRegionBlack"] = [&](const std::string& str) { blackFlagRegion.load(str); };

	vSetters["doubleStepRegionWhite"] = [&](const std::string& str) { whiteDoubleStepRegion.load(str); };
	vSetters["doubleStepRegionBlack"] = [&](const std::string& str) { blackDoubleStepRegion.load(str); };
	vSetters["tripleStepRegionWhite"] = [&](const std::string& str) { whiteTripleStepRegion.load(str); };
	vSetters["tripleStepRegionBlack"] = [&](const std::string& str) { blackTripleStepRegion.load(str); };

	vSetters["tripleStepRegionWhite"] = [&](const std::string& str) { whiteTripleStepRegion.load(str); };

	vSetters["promotedPieceType"] = [&](const std::string& str) {
		for (auto& str : split(str, ' ')) {
			promotedPieceTypes.emplace(str.front(), str.at(2));
		}
		};

	vSetters["pawn"] = [&](const std::string& str) {
		addPiece(str, "fmWfceF", "pawn");
		if (str.front() != '-') {
			pawnPiecesBlastImmunity.add(str.front());
			pawnPieces.add(str.front());
		}
		};
	vSetters["knight"] = [&](const std::string& str) { addPiece(str, "N", "knight"); };
	vSetters["bishop"] = [&](const std::string& str) { addPiece(str, "B", "bishop"); };
	vSetters["rook"] = [&](const std::string& str) { addPiece(str, "R", "rook"); };
	vSetters["queen"] = [&](const std::string& str) { addPiece(str, "Q", "queen"); };
	vSetters["fers"] = [&](const std::string& str) { addPiece(str, "F", "fers"); };
	vSetters["alfil"] = [&](const std::string& str) { addPiece(str, "A", "alfil"); };
	vSetters["fersAlfil"] = [&](const std::string& str) { addPiece(str, "FA", "fersAlfil"); };
	vSetters["silver"] = [&](const std::string& str) { addPiece(str, "FfW", "silver"); };
	vSetters["aiwok"] = [&](const std::string& str) { addPiece(str, "RNF", "aiwok"); };
	vSetters["bers"] = [&](const std::string& str) { addPiece(str, "RF", "bers"); };
	vSetters["archbishop"] = [&](const std::string& str) { addPiece(str, "BN", "archbishop"); };
	vSetters["chancellor"] = [&](const std::string& str) { addPiece(str, "RN", "chancellor"); };
	vSetters["amazon"] = [&](const std::string& str) { addPiece(str, "QN", "amazon"); };
	vSetters["knibis"] = [&](const std::string& str) { addPiece(str, "mNcB", "knibis"); };
	vSetters["biskni"] = [&](const std::string& str) { addPiece(str, "mBcN", "biskni"); };
	vSetters["kniroo"] = [&](const std::string& str) { addPiece(str, "mNcR", "kniroo"); };
	vSetters["rookni"] = [&](const std::string& str) { addPiece(str, "mRcN", "rookni"); };
	vSetters["shogiPawn"] = [&](const std::string& str) { addPiece(str, "fW", "shogiPawn"); };
	vSetters["lance"] = [&](const std::string& str) { addPiece(str, "fR", "lance"); };
	vSetters["shogiKnight"] = [&](const std::string& str) { addPiece(str, "fN", "shogiKnight"); };
	vSetters["gold"] = [&](const std::string& str) { addPiece(str, "WfF", "gold"); };
	vSetters["dragonHorse"] = [&](const std::string& str) { addPiece(str, "BW", "dragonHorse"); };
	vSetters["clobber"] = [&](const std::string& str) { addPiece(str, "cW", "clobber"); };
	vSetters["breakthrough"] = [&](const std::string& str) { addPiece(str, "fmWfF", "breakthrough"); };
	vSetters["immobile"] = [&](const std::string& str) { addPiece(str, "", "immobile"); };
	vSetters["cannon"] = [&](const std::string& str) { addPiece(str, "mRcpR", "cannon"); };
	vSetters["janggiCannon"] = [&](const std::string& str) { addPiece(str, "pR", "janggiCannon"); };
	vSetters["soldier"] = [&](const std::string& str) { addPiece(str, "fsW", "soldier"); };
	vSetters["horse"] = [&](const std::string& str) { addPiece(str, "nN", "horse"); };
	vSetters["elephant"] = [&](const std::string& str) { addPiece(str, "nA", "elephant"); };
	vSetters["janggiElephant"] = [&](const std::string& str) { addPiece(str, "nZ", "janggiElephant"); };
	vSetters["banner"] = [&](const std::string& str) { addPiece(str, "RcpRnN", "banner"); };
	vSetters["wazir"] = [&](const std::string& str) { addPiece(str, "W", "wazir"); };
	vSetters["commoner"] = [&](const std::string& str) { addPiece(str, "K", "commoner"); };
	vSetters["centaur"] = [&](const std::string& str) { addPiece(str, "KN", "centaur"); };
	vSetters["king"] = [&](const std::string& str) {
		addPiece(str, "K", "king");
		if (str.front() != '-') { royalPieces.add(str.front()); }
		};
	return vSetters;
}

inline void Chess::Variant::setVar(VariantSetters& vSetters, const std::string& key, const std::string& value)
{
	const auto it = vSetters.find(key);
	if (it != vSetters.end()) {
		it->second(value);
	}
	else if (std::regex_match(key, customPiecePattern)) {
		std::string pieceName = "customPiece" + key.substr(11);
		addPiece(value, "", pieceName);
	}
}

Chess::VariantList Chess::loadVariants(const std::string& variantPath)
{
	VariantList variantList;
	std::ifstream stream(variantPath);
	std::string line, currentVariant;
	while (std::getline(stream, line)) {
		if (line.empty() || line.front() == '#') { continue; }

		if (line.front() == '[') {
			if (!currentVariant.empty()) { variantList.emplace_back(std::make_shared<Chess::Variant>(currentVariant, variantList)); }
			currentVariant = line;
		}
		else {
			if (!currentVariant.empty()) { currentVariant += '\n'; }
			currentVariant += line;
		}
	}
	return variantList;
}

void Chess::addVariants(VariantList& variantList, const std::string& variantPath)
{
	std::ifstream stream(variantPath);
	std::string line, currentVariant;
	while (std::getline(stream, line)) {
		if (line.empty() || line.front() == '#') { continue; }

		if (line.front() == '[') {
			if (!currentVariant.empty()) { variantList.push_back(std::make_shared<Chess::Variant>(currentVariant, variantList)); }
			currentVariant = line;
		}
		else {
			if (!currentVariant.empty()) { currentVariant += '\n'; }
			currentVariant += line;
		}
	}
}