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

#include "Board.h"

const sf::Texture& Board::getBoardTexture() const
{
	return boardTexture;
}

std::shared_ptr<Piece> Board::getPieceFromCurrentPosition(sf::Vector2i position) const
{
	return Chess::getPieceFromPosition(position, this->position.pieceList);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2i localPos) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, variant->boardSize, isFlipped);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2f localPos) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, variant->boardSize, isFlipped);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2i localPos, bool reversed) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, variant->boardSize, reversed);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2f localPos, bool reversed) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, variant->boardSize, reversed);
}

sf::Vector2i Board::getLocalPosition(sf::Vector2f globalPos) const
{
	return Chess::getLocalPosition(globalPos, boardOffset, boardMultiplier, variant->boardSize, isFlipped);
}

void Board::getCastlingRights(const std::string& code)
{
	std::vector<std::string> splitString = split(code, ' ');

	int whiteKingX = -1, blackKingX = -1;
	int wKRook = -1, wQRook = -1, bKRook = -1, bQRook = -1;
	bool whiteCanNeverCastleK = true, whiteCanNeverCastleQ = true;
	bool blackCanNeverCastleK = true, blackCanNeverCastleQ = true;
	std::optional<sf::Vector2i> enPassantTarget{};
	std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
	std::vector<std::string> ranks = split(splitString.front(), '/');
	std::vector<char> start, end;
	std::vector<char> whiteKingSideCharacters, whiteQueenSideCharacters, blackKingSideCharacters, blackQueenSideCharacters;
	while (ranks.size() < variant->boardSize.y) {
		ranks.push_back(std::to_string(variant->boardSize.x));
	}
	int castleRank = variant->castleRank - 1;
	int castleRankReverse = Chess::reverseY(variant->castleRank, variant->boardSize) - 1;
	for (int j = 0; j < ranks.at(castleRank).size(); j++) {
		char c = ranks.at(castleRank).at(j);
		if (std::isdigit(c)) {
			int k = j;
			for (; k < ranks.at(castleRank).size(); k++) {
				if (!std::isdigit(ranks.at(castleRank).at(k))) {
					k--;
					break;
				}
			}
			int num = std::stoi(ranks.at(castleRank).substr(j, k - j + 1));
			j = k;
			for (int k = 0; k < num; k++) {
				start.push_back('0');
			}
		}
		else if (std::isalpha(c)) {
			start.push_back(c);
		}
	}
	while (start.size() < variant->boardSize.x) {
		start.push_back('0');
	}
	for (int j = 0; j < ranks.at(castleRankReverse).size(); j++) {
		char c = ranks.at(castleRankReverse).at(j);
		if (c == '[') { break; }
		if (std::isdigit(c)) {
			int k = j;
			for (; k < ranks.at(castleRankReverse).size(); k++) {
				if (!std::isdigit(ranks.at(castleRankReverse).at(k))) {
					k--;
					break;
				}
			}
			int num = std::stoi(ranks.at(castleRankReverse).substr(j, k - j + 1));
			j = k;
			for (int k = 0; k < num; k++) {
				end.push_back('0');
			}
		}
		else if (std::isalpha(c)) {
			end.push_back(c);
		}
	}
	while (end.size() < variant->boardSize.x) {
		end.push_back('0');
	}
	if (modifiers.size() > 1 && modifiers.at(1) != "-") {
		std::string modifier = modifiers.at(1);
		for (int j = 0; j < end.size(); j++) {
			if (variant->castlingKingPiece.has(std::tolower(end.at(j))) && std::isupper(end.at(j))) {
				whiteKingX = j + 1;
			}
		}
		for (int j = 0; j < start.size(); j++) {
			if (variant->castlingKingPiece.has(start.at(j)) && std::islower(start.at(j))) {
				blackKingX = j + 1;
			}
		}
		for (auto& l : modifier) {
			if (whiteKingX != -1) {
				if (l == 'Q') {
					whiteCanNeverCastleQ = false;
					for (int k = 0; k < whiteKingX; k++) {
						if (variant->castlingRookPieces.has(std::tolower(end.at(k)))) {
							wQRook = k + 1;
						}
					}
				}
				else if (l == 'K') {
					whiteCanNeverCastleK = false;
					for (int k = whiteKingX; k < variant->boardSize.x; k++) {
						if (variant->castlingRookPieces.has(std::tolower(end.at(k)))) {
							wKRook = k + 1;
						}
					}
				}
			}
			if (blackKingX != -1) {
				if (l == 'q') {
					blackCanNeverCastleQ = false;
					for (int k = 0; k < blackKingX; k++) {
						if (variant->castlingRookPieces.has(start.at(k))) {
							bQRook = k + 1;
						}
					}
				}
				else if (l == 'k') {
					blackCanNeverCastleK = false;
					for (int k = blackKingX; k < variant->boardSize.x; k++) {
						if (variant->castlingRookPieces.has(start.at(k))) {
							bKRook = k + 1;
						}
					}
				}
			}
		}
		// SHREDDER FEN
		if (whiteKingX != -1) {
			for (int k = 0; k < whiteKingX - 1; k++) {
				whiteQueenSideCharacters.push_back(Chess::convertXtoChar(k));
			}
			for (int k = whiteKingX; k < variant->boardSize.x; k++) {
				whiteKingSideCharacters.push_back(Chess::convertXtoChar(k));
			}
		}
		if (blackKingX != -1) {
			for (int k = 0; k < blackKingX - 1; k++) {
				blackQueenSideCharacters.push_back(Chess::convertXtoChar(k));
			}
			for (int k = blackKingX; k < variant->boardSize.x; k++) {
				blackKingSideCharacters.push_back(Chess::convertXtoChar(k));
			}
		}
		for (int j = 0; j < modifier.size(); j++) {
			char letter = modifier.at(j);
			if (isupper(letter) && whiteKingX != -1) {
				if (std::any_of(whiteKingSideCharacters.begin(), whiteKingSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
					int x = Chess::convertChartoX(std::tolower(letter));
					if (x < end.size() && variant->castlingRookPieces.has(std::tolower(end.at(x)))) {
						wKRook = x + 1;
						whiteCanNeverCastleK = false;
					}
				}
				else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
					int x = Chess::convertChartoX(std::tolower(letter));
					if (x < end.size() && variant->castlingRookPieces.has(std::tolower(end.at(x)))) {
						wQRook = x + 1;
						whiteCanNeverCastleQ = false;
					}
				}
			}
			else if (blackKingX != -1) {
				if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
					int x = Chess::convertChartoX(std::tolower(letter));
					if (x < start.size() && variant->castlingRookPieces.has(start.at(x))) {
						bKRook = x + 1;
						blackCanNeverCastleK = false;
					}
				}
				else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
					int x = Chess::convertChartoX(std::tolower(letter));
					if (x < start.size() && variant->castlingRookPieces.has(start.at(x))) {
						bQRook = x + 1;
						blackCanNeverCastleQ = false;
					}
				}
			}
		}

	}

	position.castlePieces.at(0) = wKRook;
	position.castlePieces.at(1) = wQRook;
	position.castlePieces.at(2) = bKRook;
	position.castlePieces.at(3) = bQRook;

	position.castlingRights.at(0) = !whiteCanNeverCastleK;
	position.castlingRights.at(1) = !whiteCanNeverCastleQ;
	position.castlingRights.at(2) = !blackCanNeverCastleK;
	position.castlingRights.at(3) = !blackCanNeverCastleQ;
}

// Bishop, King, Knight, Pawn, Queen, Rook
// Black --> White
pieceVector Board::generatePositionFromFENID(const std::string& code) {

	pieceVector pieces;
	std::vector<std::string> splitString = split(code, ' ');
	// ========= MODIFIERS =========
	std::optional<sf::Vector2i> enPassantTarget{};
	std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
	std::vector<std::string> ranks = split(splitString.front(), '/');
	std::vector<char> whiteKingSideCharacters, whiteQueenSideCharacters, blackKingSideCharacters, blackQueenSideCharacters;
	while (ranks.size() < variant->boardSize.y) {
		ranks.push_back(std::to_string(variant->boardSize.x));
	}
	if (variant->castlingEnabled) {
		getCastlingRights(code);
	}
	for (int i = 0; i < modifiers.size(); i++) {
		std::string modifier = modifiers.at(i);
		if (variant->nCheckCounting) {
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
				// En Passant Target
			case 2:
				if (modifier.at(0) != '-') {
					enPassantTarget = Chess::convertChessNotationtoPosition(modifier);
				}
				else {
					enPassantTarget = {};
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
			if (modifier.size() >= 3) {
				size_t plus = modifier.find('+');
				if (plus != std::string::npos) {
					blackChecksLeft = std::stoi(modifier.substr(0, plus));
					whiteChecksLeft = std::stoi(modifier.substr(plus));
				}
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
				// En Passant Target
			case 2:
				if (modifier.front() != '-') {
					enPassantTarget = Chess::convertChessNotationtoPosition(modifier);
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
	}
	// ========= RANKS =============
	std::string back = ranks.back();
	size_t dropBegin = back.find('['), dropEnd = back.find(']');
	if (dropBegin != std::string::npos && dropEnd != std::string::npos) {
		std::string drop = ranks.back().substr(dropBegin + 1, dropEnd);
		drop.pop_back();
		for (auto& l : drop) {
			std::vector<Chess::DropPiece>& currentDropPieces = std::isupper(l) ? whiteDropPieces : blackDropPieces;
			for (auto& piece : currentDropPieces) {
				if (piece.id == std::tolower(l)) {
					piece.count += 1;
					break;
				}
			}
		}
		ranks.back() = ranks.back().substr(0, dropBegin);
	}
	if (ranks.size() == variant->boardSize.y + 1) {
		for (auto& l : ranks.back()) {
			std::vector<Chess::DropPiece>& currentDropPieces = std::isupper(l) ? whiteDropPieces : blackDropPieces;
			for (auto& piece : currentDropPieces) {
				if (piece.id == std::tolower(l)) {
					piece.count += 1;
					break;
				}
			}
		}
	}

	for (int i = 0; i < ranks.size() && i < variant->boardSize.y; i++) {
		int x = 1;
		int y = Chess::reverseY(i + 1, variant->boardSize);
		std::string rank = ranks.at(i);
		for (int j = 0; j < rank.size(); j++) {
			bool promoted = false, promotedPieceType = false;
			char letter = rank.at(j);
			if (letter != '~' && letter != '+') {
				if (j + 1 < rank.size()) {
					char next = rank.at(j + 1);
					if (next == '~') { promoted = true; }
				}
				if (j > 0) {
					char previous = rank.at(j - 1);
					if (previous == '+') { promotedPieceType = true; }
				}
				if (std::isdigit(letter)) {
					int k = j;
					for (; k < rank.size(); k++) {
						if (!std::isdigit(rank.at(k))) {
							k--;
							break;
						}
					}
					x += std::stoi(rank.substr(j, k - j + 1));
					j = k;
				}
				else {
					if (variant->hasPiece(letter)) {
						Chess::PColor color = std::isupper(letter) ? Chess::PColor::White : Chess::PColor::Black;
						if (promotedPieceType) {
							auto find = variant->promotedPieceTypes.find((char)std::tolower(letter));
							if (find != variant->promotedPieceTypes.end()) {
								std::shared_ptr<Piece> piece = std::make_shared<Piece>(find->second, x, y,
									pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, color, getTextureFromID(find->second, color), isAnimated, promoted, promotedPieceType, isFlipped);
								for (auto& t : variant->promotedPieceTypes) {
									if (t.second == piece->id) {
										piece->demotedID = t.first;
										break;
									}
								}
								pieces.push_back(piece);
							}
						}
						else {
							std::shared_ptr<Piece> piece = std::make_shared<Piece>(static_cast<char>(std::tolower(letter)), x, y,
								pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, color, getTextureFromID(letter, color), isAnimated, promoted, promotedPieceType, isFlipped);
							pieces.push_back(piece);
						}
					}
					x++;
				}
			}
		}
	}

	if (variant->gating && variant->seirawanGating) {
		for (auto& piece : pieces) {
			if ((piece->getLocalPos().y == (piece->isWhite() ? 1 : variant->boardSize.y)) || position.hasChess960) {
				if (variant->castlingKingPiece.has(piece->id)) {
					int offset = piece->isWhite() ? 0 : 2;
					if (!position.castlingRights[offset] && !position.castlingRights[offset + 1]) {
						piece->hasMoved = true;
					}
				}
				else if (variant->castlingRookPieces.has(piece->id)) {
					int offset = piece->isWhite() ? 0 : 2;
					bool kingside = piece->getLocalPos().x == position.castlePieces[offset];
					bool queenside = piece->getLocalPos().x == position.castlePieces[offset + 1];
					if ((kingside && !position.castlingRights[offset]) ||
						(!kingside && !queenside) || (queenside && !position.castlingRights[offset + 1])) {
						piece->hasMoved = true;
					}
				}
				else {
					piece->hasMoved = true;
				}
			}
		}
		if (modifiers.size() > 1 && modifiers.at(1) != "-") {
			std::string modifier = modifiers.at(1);
			for (auto& l : modifier) {
				if (std::isalpha(l) && l != 'k' && l != 'q' && l != 'K' && l != 'Q') {
					Chess::PColor color = std::isupper(l) ? Chess::PColor::White : Chess::PColor::Black;
					int x = Chess::convertChartoX(std::tolower(l)) + 1;
					int y = (color == Chess::PColor::White) ? 1 : variant->boardSize.y;
					sf::Vector2i pos = { x, y };
					auto piece = Chess::getPieceFromPosition(pos, pieces);
					if (piece) { piece->hasMoved = false; }
				}
			}
		}
	}

	return pieces;
}

size_t Board::getMoveCount()
{
	size_t count = 0;
	for (auto& piece : position.pieceList) {
		if (whiteToPlay == piece->isWhite()) {
			count += piece->getEnPassantSquares().size();
			count += piece->getCaptureCastleSquares().size();
			for (auto& sq : piece->getCaptureCastleSquares()) {
				if (variant->gating && variant->seirawanGating && !piece->hasMoved &&
					piece->getLocalPos().y == (piece->isWhite() ? 1 : variant->boardSize.y)) {
					auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
					for (auto& piece : currentDropPieces) {
						if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
							count += 2;
						}
					}
				}
			}
			for (auto& sq : piece->getMoveSquares()) {
				if (variant->gating && variant->seirawanGating && !piece->hasMoved &&
					piece->getLocalPos().y == (piece->isWhite() ? 1 : variant->boardSize.y)) {
					auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
					for (auto& piece : currentDropPieces) {
						if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
							count++;
						}
					}
				}
				if (sq.promoteSquare) {
					count += sq.validPromotionTypes.count();
				}
				else {
					count++;
				}
			}
			for (auto& sq : piece->getCaptureSquares()) {
				if (variant->gating && variant->seirawanGating && !piece->hasMoved &&
					piece->getLocalPos().y == (piece->isWhite() ? 1 : variant->boardSize.y)) {
					auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
					for (auto& piece : currentDropPieces) {
						if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
							count++;
						}
					}
				}
				if (sq.promoteSquare) {
					count += sq.validPromotionTypes.count();
				}
				else {
					count++;
				}
			}
		}
	}
	if (variant->dropsEnabled) {
		std::vector<Chess::DropPiece>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		for (auto& piece : currentDropPieces) {
			if (piece.count != 0) {
				count += piece.dropSquares.size();
			}
		}
	}
	return count;
}

Board::BasicPosition Board::savePosition() {
	std::vector<std::vector<char>> array{ variant->boardSize.y, std::vector<char>(variant->boardSize.x, 0) };
	for (auto& piece : position.pieceList) {
		if (piece && Chess::isValidSquare(piece->getLocalPos(), variant->boardSize)) {
			int x = piece->getLocalPos().x - 1;
			int y = piece->getLocalPos().y - 1;
			if (piece->isWhite()) {
				array.at(y).at(x) = std::toupper(piece->id);
			}
			else {
				array.at(y).at(x) = piece->id;
			}
		}
	}
	lastPositionAdded = std::make_pair(array, whiteToPlay);
	return std::make_pair(array, whiteToPlay);
}

bool Board::isRoyal(const std::shared_ptr<Piece>& piece) const
{
	return (variant->royalPieces.has(piece->id)
		|| (variant->extinctionPseudoRoyal && Chess::isExtinctionRoyal(piece, position.extinctionRoyalPieces)));
}

std::string Board::getNewFEN() const
{
	return Chess::getNewFEN(*variant, position.hasChess960);
}

std::string Board::getCurrentFEN() const {
	std::string sideToPlayString = whiteToPlay ? "w" : "b";
	std::string dropString = "[", checkString = "", rank = "", newFEN = "";
	if (variant->dropsEnabled || whiteDropPieces.size() > 0 || blackDropPieces.size() > 0) {
		for (auto& piece : whiteDropPieces) {
			if (piece.count > 0) {
				for (int i = 0; i < piece.count; i++) {
					dropString += (char)std::toupper(piece.id);
				}
			}
		}
		for (auto& piece : blackDropPieces) {
			if (piece.count > 0) {
				for (int i = 0; i < piece.count; i++) {
					dropString += piece.id;
				}
			}
		}
		dropString += "]";
	}
	if (variant->nCheckCounting) {
		checkString = " " + std::to_string(blackChecksLeft) + "+" + std::to_string(whiteChecksLeft);
	}
	for (int y = variant->boardSize.y; y >= 1; y--) {
		rank = "";
		for (int x = 1; x <= variant->boardSize.x; x++) {
			sf::Vector2i pos{ x, y };
			std::shared_ptr<Piece> piece = getPieceFromCurrentPosition(pos);
			if (piece) {
				char id = piece->isWhite() ? std::toupper(piece->id) : piece->id;
				if (piece->promotedPieceType) {
					rank += '+';
					for (auto& t : variant->promotedPieceTypes) {
						if (t.second == piece->id) {
							id = piece->isWhite() ? std::toupper(t.first) : t.first;
							rank += id;
							break;
						}
					}
				}
				else {
					rank += id;
				}
				if (variant->dropsEnabled && piece->promoted) { rank += '~'; }
			}
			else {
				if (rank.empty() || !std::isdigit(rank.back())) {
					rank += '1';
				}
				else if (std::isdigit(rank.back())) {
					size_t i = rank.size() - 1;
					for (i; i >= 0; i--) {
						if (!std::isdigit(rank.at(i))) {
							i++;
							break;
						}
						if (i == 0) { break; }
					}
					int current = std::stoi(rank.substr(i));
					rank.erase(i, std::string::npos);
					rank += std::to_string(current + 1);
				}
			}
		}
		newFEN += rank + '/';
	}
	newFEN.pop_back();
	if (variant->dropsEnabled || whiteDropPieces.size() > 0 || blackDropPieces.size() > 0) { newFEN += dropString; }
	newFEN += ' ' + sideToPlayString;
	newFEN += ' ';
	if (variant->gating && variant->seirawanGating) {
		std::string castleField = "";
		if (position.castlingRights[0]) { castleField += 'K'; }
		if (position.castlingRights[1]) { castleField += 'Q'; }
		for (auto& piece : position.pieceList) {
			if (piece->isWhite()) {
				if (piece->getLocalPos().y == 1 && !piece->hasMoved) {
					int x = piece->getLocalPos().x - 1;
					if (variant->castlingKingPiece.has(piece->id)) {
						if (!position.castlingRights[0] && !position.castlingRights[1]) {
							castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
						}
					}
					else if (variant->castlingRookPieces.has(piece->id)) {
						bool kingside = piece->getLocalPos().x == position.castlePieces[0];
						bool queenside = piece->getLocalPos().x == position.castlePieces[1];
						if ((kingside && !position.castlingRights[0]) ||
							(!kingside && !queenside) || (queenside && !position.castlingRights[1])) {
							castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
						}
					}
					else {
						castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
					}
				}
			}
		}
		if (position.castlingRights[2]) { castleField += 'k'; }
		if (position.castlingRights[3]) { castleField += 'q'; }
		for (auto& piece : position.pieceList) {
			if (piece->isBlack()) {
				if (piece->getLocalPos().y == variant->boardSize.y && !piece->hasMoved) {
					int x = piece->getLocalPos().x - 1;
					if (variant->castlingKingPiece.has(piece->id)) {
						if (!position.castlingRights[2] && !position.castlingRights[3]) {
							castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
						}
					}
					else if (variant->castlingRookPieces.has(piece->id)) {
						bool kingside = piece->getLocalPos().x == position.castlePieces[2];
						bool queenside = piece->getLocalPos().x == position.castlePieces[3];
						if ((kingside && !position.castlingRights[2]) ||
							(!kingside && !queenside) || (queenside && !position.castlingRights[2])) {
							castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
						}
					}
					else {
						castleField += piece->isWhite() ? std::toupper(Chess::convertXtoChar(x)) : Chess::convertXtoChar(x);
					}
				}
			}
		}

		if (castleField.empty()) { castleField = '-'; }
		newFEN += castleField;
	}
	else {
		if (!position.castlingRights[0] && !position.castlingRights[1] && !position.castlingRights[2] && !position.castlingRights[3]) {
			newFEN += '-';
		}
		else {
			if (position.castlingRights[0]) { newFEN += 'K'; }
			if (position.castlingRights[1]) { newFEN += 'Q'; }
			if (position.castlingRights[2]) { newFEN += 'k'; }
			if (position.castlingRights[3]) { newFEN += 'q'; }
		}
	}
	newFEN += ' ';
	if (position.enPassantTarget.has_value()) {
		newFEN += Chess::convertPositiontoNotation(position.enPassantTarget.value());
	}
	else {
		newFEN += "-";
	}
	newFEN += checkString;
	newFEN += ' ' + std::to_string(halfMoves) + ' ' + std::to_string(fullMoves);
	return newFEN;
}

const sf::Texture& Board::getTextureFromID(char id, Chess::PColor color) const
{
	if (color == Chess::PColor::White) { id = std::toupper(id); }
	else { id = std::tolower(id); }
	auto find = currentPieceTextures->find(id);
	if (find != currentPieceTextures->end()) {
		return find->second;
	}
	return currentPieceTextures->begin()->second;
}

std::shared_ptr<Piece> Board::getPieceFromID(char id, sf::Vector2i pos, Chess::PColor color, bool promoted, bool promotedPieceType) const
{
	return std::make_shared<Piece>(std::tolower(id), pos.x, pos.y, pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, promotedPieceType, isFlipped);
}