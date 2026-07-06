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
#include <iostream>
#include <string>
#include "ChessUtil.h"
#include "Region.h"
#include <sstream>
#include <functional>
#include <map>
#include <SFML/System/Vector2.hpp>
#include <regex>
#include <map>
#include <memory>
#include <unordered_set>
#include "MoveSet.h"
#include "PieceType.h"

namespace Chess {

	const std::string chessStr = "[chess]\npocketSize = 0\nmaxRank = 8\nmaxFile = 8\nchess960 = true\npawn = p\nknight = n\nbishop = b\nrook = r\nqueen = q\nking = k\nstartFen = rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\npromotionRegionWhite = *8\npromotionRegionBlack = *1\npawnTypes = p\npromotionPawnTypes = p\npromotionPieceTypes = nbrq\npromotedPieceType =\npiecePromotionOnCapture = false\nmandatoryPawnPromotion = true\nmandatoryPiecePromotion = false\ndoubleStep = true\ndoubleStepRegionWhite = *2\ndoubleStepRegionBlack = *7\ntripleStepRegionWhite = -\ntripleStepRegionBlack = -\nenPassantRegion = *\nenPassantTypes = p\ncastling = true\ncastlingDroppedPiece = false\ncastlingKingsideFile = g\ncastlingQueensideFile = c\ncastlingRank = 1\ncastlingKingFile = e\ncastlingKingPiece = k\ncastlingRookKingsideFile = l\ncastlingRookQueensideFile = a\ncastlingRookPieces = r\noppositeCastling = false\ncastlingWins = -\nchecking = true\ndropChecks = true\nmustCapture = false\nmustDrop = false\nmustDropType = *\npieceDrops = false\ndropLoop = false\ncapturesToHand = false\nfirstRankPawnDrops = false\npromotionZonePawnDrops = false\ndropRegionWhite = *\ndropRegionBlack = *\ndropPromoted = false\nnMoveRuleTypes = p\nnMoveRuleTypesWhite = p\nnMoveRuleTypesBlack = p\nnMoveRule = 50\nnFoldRule = 3\nnFoldValue = draw\nnFoldValueAbsolute = false\nstalemateValue = draw\nstalematePieceCount = false\ncheckmateValue = loss\nextinctionValue = none\nextinctionPseudoRoyal = false\nextinctionPieceTypes = -\nextinctionPieceCount = 0\nflagPieceCount = 1\nflagMove = false\ncheckCounting = false\ndropRegionWhite = *\ndropRegionBlack = *";

	const std::set<std::string> pieceStrings = { "knight", "bishop", "rook", "queen", "fers", "alfil", "fersAlfil", "silver", "aiwok", "bers", "archbishop", "chancellor", "amazon", "knibis", "biskni", "kniroo", "rookni", "shogiPawn", "lance", "shogiKnight", "gold", "dragonHorse", "clobber", "breakthrough", "immobile", "cannon", "janggiCannon", "soldier", "horse", "elephant", "janggiElephan", "banner", "wazir", "commoner", "centaur", "king" };

	class Variant {

	public:
		static const std::regex customPiecePattern;

		std::string name, inherit, displayName, description;

		int nChecksWhite, nChecksBlack, nMoveRule, nFoldRule, flagPieceCount, extinctionPieceCount;
		std::string startingFEN;
		sf::Vector2u boardSize;

		bool hasPromotion, allowsChess960, firstRankPawnDrops, promotionZonePawnDrops, extinctionPseudoRoyal, piecePromotionOnCapture,
			checksEnabled, blockChecks, castlingEnabled, dropsEnabled, atomicExplosions, dropPromotedState, flyingGeneral, immobilityIllegal;
		bool capturesToHand, forceCapture, forceDrop, forcePawnPromotion, forcePiecePromotion, pawnDoubleStep, extraFlagMove, nCheckCounting, dropLoop,
			dropOppositeColoredBishop, castlingDroppedPiece, pieceDemotion, gating, seirawanGating;
		int castleRank, castleKDestination, castleQDestination, castlingKingFile, dropNoDoubledCount;
		int startKRookW, startQRookW, startKRookB, startQRookB;

		Chess::PieceType mutuallyImmunePieces, atomicImmunePieces, whiteExtinctionPieces, blackExtinctionPieces, whiteFlagPieces, blackFlagPieces, castlingKingPiece, castlingRookPieces, dropNoDoubledPieces;
		Chess::PieceType royalPieces, promotionPawnPieces, enPassantPieces, nMoveRulePieces, whiteDropPieces, blackDropPieces, whitePromotePieces, blackPromotePieces, pawnPieces, pawnPiecesBlastImmunity, janggiCannons;
		std::map<std::string, char> pieceTypes;
		std::map<char, int> promotionLimit;

		Chess::Region whitePromotionRegion, blackPromotionRegion, whiteDropRegion, blackDropRegion, whiteFlagRegion, blackFlagRegion;
		Chess::Region whiteDoubleStepRegion, blackDoubleStepRegion, whiteTripleStepRegion, blackTripleStepRegion;

		Chess::WinValue checkmateValue, nFoldValue, stalemateValue, extinctionValue;

		std::map<char, char> promotedPieceTypes;
		std::map<char, Chess::MoveSetVector> pieceMoves;
		std::vector<std::string> sRanks;

		bool operator==(const Chess::Variant& other) { return name == other.name; }
		bool operator!=(const Chess::Variant& other) { return name != other.name; }

		Variant(const std::string& variantStr = chessStr, const std::vector<std::shared_ptr<Chess::Variant>>& inheritList = {});
		~Variant() {};

		void loadFromStr(const std::string& variantStr = chessStr, const std::vector<std::shared_ptr<Chess::Variant>>& inheritList = {});
		void removePiece(const std::string& pieceName);
		void removePiece(char id);
		void addPiece(const std::string& pieceStr, const std::string& defaultMoveStr, const std::string& pieceName);

		bool hasPiece(char id) const {
			for (auto& type : pieceTypes) {
				if (type.second == static_cast<char>(std::tolower(id))) {
					return true;
				}
			}
			return false;
		}

	private:
		using VariantSetters = std::map<std::string, std::function<void(const std::string&)>>;

		VariantSetters getSetters();
		inline void setVar(VariantSetters& vSetters, const std::string& key, const std::string& value);
	};

	using VariantList = std::vector<std::shared_ptr<Chess::Variant>>;

	VariantList loadVariants(const std::string& variantPath);
	void addVariants(VariantList& variantList, const std::string& variantPath);

	struct VariantType {

	public:
		std::string name, description;
		std::vector<std::shared_ptr<Chess::Variant>> variants;

		VariantType(const std::string& name, const std::string& description) : name(name), description(description) {};
		
		void add(const std::shared_ptr<Chess::Variant>& v) {
			variants.push_back(v);
		}
	};

	/*
	class PositionT {
		pieceVector pV;
		Chess::Variant v;
		bool hasChess960, atomicKings;
	};
	*/

}