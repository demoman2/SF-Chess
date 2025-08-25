#include "Board.h"

const sf::Texture& Board::getBoardT()
{
	return boardTexture;
}

std::shared_ptr<Piece> Board::getPieceFromCurrentPosition(sf::Vector2i position) const
{
	return Chess::getPieceFromPosition(position, pieceList);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2i localPos) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, isFlipped);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2f localPos) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, isFlipped);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2i localPos, bool reversed) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, reversed);
}

sf::Vector2f Board::getGlobalPosition(sf::Vector2f localPos, bool reversed) const
{
	return Chess::getGlobalPosition(localPos, boardOffset, boardSize, boardMultiplier, reversed);
}

sf::Vector2i Board::getLocalPosition(sf::Vector2f globalPos) const
{
	return Chess::getLocalPosition(globalPos, boardOffset, boardMultiplier, isFlipped);
}

// Bishop, King, Knight, Pawn, Queen, Rook
// Black --> White
pieceVector Board::generatePositionFromFENID(std::string code) {

	pieceVector pieces;
	std::vector<std::string> splitString = split(code, ' ');
	// ========= MODIFIERS =========

	int whiteKingX = 0, blackKingX = 0;
	int wKRook = -1, wQRook = -1, bKRook = -1, bQRook = -1;
	bool whiteCanNeverCastleK = true, whiteCanNeverCastleQ = true;
	bool blackCanNeverCastleK = true, blackCanNeverCastleQ = true;
	std::optional<sf::Vector2i> enPassantTarget{};
	std::vector<std::string> modifiers{ splitString.begin() + 1, splitString.end() };
	std::vector<std::string> ranks = split(splitString.front(), '/');
	std::vector<char> start, end;
	std::vector<char> whiteKingSideCharacters, whiteQueenSideCharacters, blackKingSideCharacters, blackQueenSideCharacters;
	for (int j = 0; j < ranks.front().size(); j++) {
		if (std::isdigit(ranks.front().at(j))) {
			int num = ranks.front().at(j) - '0';
			for (int k = 0; k < num; k++) {
				start.push_back('0');
			}
		}
		else if (ranks.front().at(j) != '~') {
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
		else if (ranks.back().at(j) != '~') {
			end.push_back(ranks.back().at(j));
		}
	}
	for (int i = 0; i < modifiers.size(); i++) {
		std::string modifier = modifiers.at(i);
		if (variant == Chess::ThreeCheck || variant == Chess::FiveCheck) {
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
					whiteQueenSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = whiteKingX; k < 8; k++) {
					whiteKingSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = 0; k < blackKingX - 1; k++) {
					blackQueenSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = blackKingX; k < 8; k++) {
					blackKingSideCharacters.push_back(Chess::convertXtoChar(k));
				}

				for (int j = 0; j < modifier.size(); j++) {
					char letter = modifier.at(j);
					if (isupper(letter)) {
						if (std::any_of(whiteKingSideCharacters.begin(), whiteKingSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wKRook = x + 1;
								whiteCanNeverCastleK = false;
							}
						}
						else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wQRook = x + 1;
								whiteCanNeverCastleQ = false;
							}
						}
					}
					else {
						if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (start.at(x) == 'r') {
								bKRook = x + 1;
								blackCanNeverCastleK = false;
							}
						}
						else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = Chess::convertChartoX(std::tolower(letter));
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
					enPassantTarget = Chess::convertChessNotationtoPosition(modifier);
				}
				else {
					enPassantTarget = {};
				}
				break;
			case 3:
				if (modifier.size() >= 3) {
					size_t plus = modifier.find('+');
					if (variant == Chess::ThreeCheck) {
						blackChecks = 3 - std::stoi(modifier.substr(0, plus));
						whiteChecks = 3 - std::stoi(modifier.substr(plus));
					}
					else if (variant == Chess::FiveCheck) {
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
								break;
							}
						}
					}
					else if (l == 'K') {
						whiteCanNeverCastleK = false;
						for (int k = whiteKingX; k < 8; k++) {
							if (end.at(k) == 'R') {
								wKRook = k + 1;
								break;
							}
						}
					}
					else if (l == 'q') {
						blackCanNeverCastleQ = false;
						for (int k = 0; k < blackKingX; k++) {
							if (start.at(k) == 'r') {
								bQRook = k + 1;
								break;
							}
						}
					}
					else if (l == 'k') {
						blackCanNeverCastleK = false;
						for (int k = blackKingX; k < 8; k++) {
							if (start.at(k) == 'r') {
								bKRook = k + 1;
								break;
							}
						}
					}
				}
				// SHREDDER FEN
				for (int k = 0; k < whiteKingX - 1; k++) {
					whiteQueenSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = whiteKingX; k < 8; k++) {
					whiteKingSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = 0; k < blackKingX - 1; k++) {
					blackQueenSideCharacters.push_back(Chess::convertXtoChar(k));
				}
				for (int k = blackKingX; k < 8; k++) {
					blackKingSideCharacters.push_back(Chess::convertXtoChar(k));
				}

				for (int j = 0; j < modifier.size(); j++) {
					char letter = modifier.at(j);
					if (isupper(letter)) {
						if (std::any_of(whiteKingSideCharacters.begin(), whiteKingSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wKRook = x + 1;
								whiteCanNeverCastleK = false;
							}
						}
						else if (std::any_of(whiteQueenSideCharacters.begin(), whiteQueenSideCharacters.end(), [letter](char c) { return c == std::tolower(letter); })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (end.at(x) == 'R') {
								wQRook = x + 1;
								whiteCanNeverCastleQ = false;
							}
						}
					}
					else {
						if (std::any_of(blackKingSideCharacters.begin(), blackKingSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = Chess::convertChartoX(std::tolower(letter));
							if (start.at(x) == 'r') {
								bKRook = x + 1;
								blackCanNeverCastleK = false;
							}
						}
						else if (std::any_of(blackQueenSideCharacters.begin(), blackQueenSideCharacters.end(), [letter](char c) { return c == letter; })) {
							int x = Chess::convertChartoX(std::tolower(letter));
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
	if (dropsEnabled) {
		std::string back = ranks.back();
		size_t dropBegin = back.find('['), dropEnd = back.find(']');
		if (dropBegin != std::string::npos && dropEnd != std::string::npos) {
			std::string drop = ranks.back().substr(dropBegin + 1, dropEnd);
			drop.pop_back();
			for (auto& l : drop) {
				if (std::isupper(l)) {
					for (auto& piece : whiteDropPieces) {
						if (piece->id == std::tolower(l)) {
							piece->count += 1;
							break;
						}
					}
				}
				else {
					for (auto& piece : blackDropPieces) {
						if (piece->id == l) {
							piece->count += 1;
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
		int y = Chess::reverseY(i + 1);
		std::string rank = ranks.at(i);
		for (int j = 0; j < rank.size(); j++) {
			bool promoted = false;
			char letter = rank.at(j);
			if (j + 1 < rank.size()) {
				char next = rank.at(j + 1);
				if (next == '~') { promoted = true; }
			}
			if (std::isdigit(letter)) {
				x += letter - '0';
			}
			else {
				Chess::PColor color = std::isupper(letter) ? Chess::PColor::White : Chess::PColor::Black;
				int offset = std::isupper(letter) ? 6 : 0;
				switch (std::tolower(letter)) {
				case 'b':
				{
					pieces.emplace_back(std::make_shared<Bishop>(x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(0 + offset), isAnimated, promoted, isFlipped));
					break;
				}
				case 'k':
				{
					if (color == Chess::White) {
						std::shared_ptr<King> king = std::make_shared<King>(whiteCanNeverCastleK, whiteCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(1 + offset), isAnimated, promoted, isFlipped);
						king->Krook = wKRook;
						king->Qrook = wQRook;
						pieces.push_back(king);
					}
					else {
						std::shared_ptr<King> king = std::make_shared<King>(blackCanNeverCastleK, blackCanNeverCastleQ, x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(1 + offset), isAnimated, promoted, isFlipped);
						king->Krook = bKRook;
						king->Qrook = bQRook;
						pieces.push_back(king);
					}
					break;
				}
				case 'n':
				{
					pieces.emplace_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(2 + offset), isAnimated, promoted, isFlipped));
					break;
				}
				case 'p':
				{
					int off = color == Chess::White ? -1 : 1;
					if (enPassantTarget.has_value() && enPassantTarget.value() == sf::Vector2i{ x, y + off }) {
						std::shared_ptr<Pawn> p = std::make_shared<Pawn>(true, x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(3 + offset), isAnimated, promoted, isFlipped);
						pieces.push_back(p);
					}
					else {
						pieces.emplace_back(std::make_shared<Pawn>(false, x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(3 + offset), isAnimated, promoted, isFlipped));
					}
					break;
				}
				case 'q':
				{
					pieces.emplace_back(std::make_shared<Queen>(x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(4 + offset), isAnimated, promoted, isFlipped));
					break;
				}
				case 'r':
				{
					pieces.emplace_back(std::make_shared<Rook>(x, y, pieceScale, boardOffset, boardSize, boardMultiplier, color, pieceTextures.at(5 + offset), isAnimated, promoted, isFlipped));
					break;
				}
				}
				x++;
			}
			if (promoted) { x--; }
		}
	}
	return pieces;
}

int Board::getMoveCount()
{
	size_t count = 0;
	for (auto& piece : pieceList) {
		if (whiteToPlay == piece->isWhite()) {
			count += piece->getMoveSquares().size();
			count += piece->getCaptureSquares().size();
			if (piece->hasID('k')) {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				count += king->getCaptureCastleSquares().size();
			}
			else if (piece->hasID('p')) {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
				count += pawn->getEnPassantSquares().size();
				for (auto& sq : pawn->getMoveSquares()) {
					if (pawn->isWhite() && sq.pos.y == 8) {
						count += whitePromotePieces.size() - 1;
					}
					else if (!pawn->isWhite() && sq.pos.y == 1) {
						count += blackPromotePieces.size() - 1;
					}
				}
				for (auto& sq : pawn->getCaptureSquares()) {
					if (pawn->isWhite() && sq.pos.y == 8) {
						count += whitePromotePieces.size() - 1;
					}
					else if (!pawn->isWhite() && sq.pos.y == 1) {
						count += blackPromotePieces.size() - 1;
					}
				}
			}
		}
	}
	if (dropsEnabled) {
		if (whiteToPlay) {
			for (auto& piece : whiteDropPieces) {
				if (piece->count != 0) {
					count += piece->dropSquares.size();
				}
			}
		}
		else {
			for (auto& piece : blackDropPieces) {
				if (piece->count != 0) {
					count += piece->dropSquares.size();
				}
			}
		}
	}
	return static_cast<int>(count);
}

Board::BasicPosition Board::savePosition() {
	std::array<std::array<int, 8>, 8> array{ 0 };
	for (auto& piece : pieceList) {
		if (piece && Chess::isValidSquare(piece->getLocalPos())) {
			int x = piece->getLocalPos().x - 1;
			int y = piece->getLocalPos().y - 1;
			if (piece->isWhite()) {
				array.at(y).at(x) = std::toupper(piece->id);
			}
			else {
				array.at(y).at(x) = piece->id - '0';
			}
		}
	}
	return std::make_pair(array, whiteToPlay);
}

std::string Board::getNewFEN() const
{
	return Chess::getNewFEN(variant, chess960Enabled);
}

std::string Board::getCurrentFEN() {
	bool whiteCanNeverCastleK = true, whiteCanNeverCastleQ = true, blackCanNeverCastleK = true, blackCanNeverCastleQ = true;
	std::optional<sf::Vector2i> enPassantTarget{};
	std::string sideToPlayString = whiteToPlay ? "w" : "b";
	std::string dropString = "[", checkString = "", rank = "", newFEN = "";
	for (auto& piece : pieceList) {
		if (piece->hasID('k')) {
			std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
			if (king->isWhite()) {
				std::shared_ptr<Piece> kRook = getPieceFromCurrentPosition({ king->Krook, king->getLocalPos().y });
				std::shared_ptr<Piece> qRook = getPieceFromCurrentPosition({ king->Qrook, king->getLocalPos().y });
				if (!king->canNeverCastleK && !king->hasMoved && !king->inCheck && kRook && kRook->hasID('r') && !kRook->hasMoved) { whiteCanNeverCastleK = false; }
				if (!king->canNeverCastleQ && !king->hasMoved && !king->inCheck && qRook && qRook->hasID('r') && !qRook->hasMoved) { whiteCanNeverCastleQ = false; }
			}
			else {
				std::shared_ptr<Piece> kRook = getPieceFromCurrentPosition({ king->Krook, king->getLocalPos().y });
				std::shared_ptr<Piece> qRook = getPieceFromCurrentPosition({ king->Qrook, king->getLocalPos().y });
				if (!king->canNeverCastleK && !king->hasMoved && !king->inCheck && kRook && kRook->hasID('r') && !kRook->hasMoved) { blackCanNeverCastleK = false; }
				if (!king->canNeverCastleQ && !king->hasMoved && !king->inCheck && qRook && qRook->hasID('r') && !qRook->hasMoved) { blackCanNeverCastleQ = false; }
			}
		}
		else if (piece->hasID('p')) {
			std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
			if (pawn->enPassantTarget) {
				if (pawn->isWhite()) {
					enPassantTarget = sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y - 1 };
				}
				else {
					enPassantTarget = sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y + 1 };
				}
			}
		}
	}
	if (dropsEnabled) {
		for (auto& piece : whiteDropPieces) {
			if (piece->count > 0) {
				for (int i = 0; i < piece->count; i++) {
					dropString += (char)std::toupper(piece->id);
				}
			} 
		}
		for (auto& piece : blackDropPieces) {
			if (piece->count > 0) {
				for (int i = 0; i < piece->count; i++) {
					dropString += piece->id;
				}
			}
		}
		dropString += "]";
	}
	if (variant == Chess::ThreeCheck) {
		checkString = " " + std::to_string(3 - blackChecks) + "+" + std::to_string(3 - whiteChecks);
	}
	else if (variant == Chess::FiveCheck) {
		checkString = " " + std::to_string(5 - blackChecks) + "+" + std::to_string(5 - whiteChecks);
	}
	for (int y = 8; y >= 1; y--) {
		rank = "";
		for (int x = 1; x <= 8; x++) {
			sf::Vector2i pos{ x, y };
			std::shared_ptr<Piece> piece = getPieceFromCurrentPosition(pos);
			if (piece) {
				if (piece->isWhite()) {
					rank += (char)std::toupper(piece->id);
				}
				else {
					rank += piece->id;
				}
				if (dropsEnabled && piece->promoted) { rank += '~'; }
			}
			else {
				if (!std::isdigit(rank.back()) || rank.empty()) {
					rank += '1';
				}
				else {
					rank.back() = int((rank.back() - '0') + 1) + '0';
				}
			}
		}
		newFEN += rank + '/';
	}
	newFEN.pop_back();
	if (dropsEnabled) { newFEN += dropString; }
	newFEN += ' ' + sideToPlayString;
	newFEN += ' ';
	if (whiteCanNeverCastleK && whiteCanNeverCastleQ && blackCanNeverCastleK && blackCanNeverCastleQ) {
		newFEN += '-';
	}
	else {
		if (!whiteCanNeverCastleK) { newFEN += 'K'; }
		if (!whiteCanNeverCastleQ) { newFEN += 'Q'; }
		if (!blackCanNeverCastleK) { newFEN += 'k'; }
		if (!blackCanNeverCastleQ) { newFEN += 'q'; }
	}
	newFEN += ' ';
	if (enPassantTarget.has_value()) {
		newFEN += Chess::convertPositiontoNotation(enPassantTarget.value());
	}
	else {
		newFEN += "-";
	}
	newFEN += checkString;
	newFEN += ' ' + std::to_string(halfMoves) + ' ' + std::to_string(fullMoves);
	return newFEN;
}

sf::Texture& Board::getTextureFromID(char id, Chess::PColor color)
{
	// Bishop, King, Knight, Pawn, Queen, Rook
	std::array<char, 6> chars{ 'b', 'k', 'n', 'p', 'q', 'r' };
	unsigned int textureID = 0;
	for (int i = 0; i < chars.size(); i++) {
		if ((char)std::tolower(id) == chars.at(i)) {
			textureID = i;
			break;
		}
	}
	if (color == Chess::PColor::White) { textureID += 6; }
	return pieceTextures.at(textureID);
}

std::shared_ptr<Piece> Board::getPieceFromID(char id, sf::Vector2i pos, Chess::PColor color, bool promoted)
{
	switch ((char)std::tolower(id)) {
	case 'p':
	{
		return std::make_shared<Pawn>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}
	case 'q':
	{
		return std::make_shared<Queen>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}
	case 'b':
	{
		return std::make_shared<Bishop>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}
	case 'r':
	{
		return std::make_shared<Rook>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}
	case 'n':
	{
		return std::make_shared<Knight>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}
	case 'k':
	{
		return std::make_shared<King>(pos.x, pos.y, pieceScale, boardOffset, boardSize, boardMultiplier, color, getTextureFromID(id, color), false, promoted, isFlipped);
	}

	}
	std::cout << "Got nullptr!" << std::endl;
	return nullptr;
}