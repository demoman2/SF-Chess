#include "Board.h"

// FEN
void Board::setupFEN(std::optional<std::string> FEN_ID, bool init)
{
	moves = "";
	whiteChecks = 0;
	blackChecks = 0;
	gameEnded = false;
	hasCheck = false;
	eighthRankWhite = false;
	dropsEnabled = false;
	castlingEnabled = true;
	promotionEnabled = true;
	checksEnabled = true;
	promotionPieceCount = 4;
	lastMoveStart.setPosition({ -1000, -1000 });
	lastMoveDest.setPosition({ -1000, -1000 });
	lastMoveStartLocal = { 100, 100 };
	lastMoveDestLocal = { 100, 100 };
	whiteDropPieces.clear();
	blackDropPieces.clear();
	selectedPiece.reset();
	capturePiece.reset();
	pieceList.clear();
	allPositionsPlayed.clear();
	captureSound.setBuffer(captureBuffer);
	captureThreshold = 1.0f;

	// Variant Setup
	if (variant == Chess::Antichess) {
		castlingEnabled = false;
		checksEnabled = false;
		promotionPieceCount = 5;
	}
	else if (variant == Chess::Atomic) {
		captureSound.setBuffer(explosionBuffer);
		captureThreshold = 30.0f;
	}
	else if (variant == Chess::RacingKings) {
		castlingEnabled = false;
	}
	else if (variant == Chess::Crazyhouse) {
		dropsEnabled = true;
	}

	FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1";
	if (FEN_ID.has_value()) {
		FEN = FEN_ID.value();
		if (variant == Chess::ThreeCheck) {
			if (FEN.find('+') == std::string::npos && findNthOf(FEN, ' ', 4) != 0) {
				FEN.insert(findNthOf(FEN, ' ', 4), " 3+3");
			}
		}
		else if (variant == Chess::FiveCheck) {
			if (FEN.find('+') == std::string::npos && findNthOf(FEN, ' ', 4) != 0) {
				FEN.insert(findNthOf(FEN, ' ', 4), " 5+5");
			}
		}
		if (dropsEnabled) {
			if (FEN.find('+') == std::string::npos && findNthOf(FEN, ' ', 1) != 0) {
				FEN.insert(findNthOf(FEN, ' ', 1), "[]");
			}
		}
	}
	else {
		if (endgamePosition) {
			std::string fen;
			if (variant == Chess::Antichess) {
				std::ifstream AntichessEndgameReadFile("assets/fen/antichess_endgame.txt");
				std::string line;
				int lines = 0;
				while (std::getline(AntichessEndgameReadFile, line)) { lines++; }
				int id = std::rand() % lines;
				AntichessEndgameReadFile.clear();
				AntichessEndgameReadFile.seekg(0, AntichessEndgameReadFile.beg);
				for (int i = 0; i <= id; i++) {
					std::getline(AntichessEndgameReadFile, fen);
				}
				AntichessEndgameReadFile.close();
				if (variant == Chess::ThreeCheck) {
					FEN = fen.substr(0, fen.size() - 3);
					FEN += "3+3 0 1";
				}
				else if (variant == Chess::FiveCheck) {
					FEN = fen.substr(0, fen.size() - 3);
					FEN += "5+5 0 1";
				}
				else if (variant == Chess::Crazyhouse) {
					FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
				}
				else {
					if (!fen.empty()) {
						FEN = fen;
					}
				}
			}
			else if (variant == Chess::Standard) {
				std::ifstream EndgameReadFile("assets/fen/endgame.txt");
				std::string line;
				int lines = 0;
				while (std::getline(EndgameReadFile, line)) { lines++; }
				int id = std::rand() % lines;
				EndgameReadFile.clear();
				EndgameReadFile.seekg(0, EndgameReadFile.beg);
				for (int i = 0; i <= id; i++) {
					std::getline(EndgameReadFile, fen);
				}
				EndgameReadFile.close();
				if (variant == Chess::ThreeCheck) {
					FEN = fen.substr(0, fen.size() - 3);
					FEN += "3+3 0 1";
				}
				else if (variant == Chess::FiveCheck) {
					FEN = fen.substr(0, fen.size() - 3);
					FEN += "5+5 0 1";
				}
				else if (variant == Chess::Crazyhouse) {
					FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
				}
				else {
					if (!fen.empty()) {
						FEN = fen;
					}
				}
			}
			else {
				if (chess960Enabled) {
					std::ifstream FRCReadFile("assets/fen/chess960.txt");
					std::string line;
					int lines = 0;
					while (std::getline(FRCReadFile, line)) { lines++; }
					int id = std::rand() % lines;
					FRCReadFile.clear();
					FRCReadFile.seekg(0, FRCReadFile.beg);
					std::cout << "Loading Chess960 Position #" << id << std::endl;
					std::string fen;
					for (int i = 0; i <= id; i++) {
						std::getline(FRCReadFile, fen);
					}
					FRCReadFile.close();

					if (variant == Chess::ThreeCheck) {
						FEN = fen.substr(0, fen.size() - 3);
						FEN += "3+3 0 1";
						std::cout << FEN << std::endl;
					}
					else if (variant == Chess::FiveCheck) {
						FEN = fen.substr(0, fen.size() - 3);
						FEN += "5+5 0 1";
					}
					else if (variant == Chess::Horde) {
						FEN = fen.substr(0, 8) + "/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
					}
					else if (variant == Chess::RacingKings) {
						FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
					}
					else if (variant == Chess::Crazyhouse) {
						FEN = fen;
						FEN.insert(43, "[]");
					}
					else {
						if (!fen.empty()) {
							FEN = fen;
						}
					}

				}
				else if (variant == Chess::Horde) {
					FEN = "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
				}
				else if (variant == Chess::ThreeCheck) {
					FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1";
				}
				else if (variant == Chess::FiveCheck) {
					FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 5+5 0 1";
				}
				else if (variant == Chess::RacingKings) {
					FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
				}
				else if (variant == Chess::Crazyhouse) {
					FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
				}
			}
		}
		else if (chess960Enabled) {
			std::ifstream FRCReadFile("assets/fen/chess960.txt");
			std::string line;
			int lines = 0;
			while (std::getline(FRCReadFile, line)) { lines++; }
			int id = std::rand() % lines;
			FRCReadFile.clear();
			FRCReadFile.seekg(0, FRCReadFile.beg);
			std::cout << "Loading Chess960 Position #" << id << std::endl;
			std::string fen;
			for (int i = 0; i <= id; i++) {
				std::getline(FRCReadFile, fen);
			}
			FRCReadFile.close();

			if (variant == Chess::ThreeCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "3+3 0 1";
				std::cout << FEN << std::endl;
			}
			else if (variant == Chess::FiveCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "5+5 0 1";
			}
			else if (variant == Chess::Horde) {
				FEN = fen.substr(0, 8) + "/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
			}
			else if (variant == Chess::RacingKings) {
				FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
			}
			else if (variant == Chess::Crazyhouse) {
				FEN = fen;
				FEN.insert(43, "[]");
			}
			else {
				if (!fen.empty()) {
					FEN = fen;
				}
			}

		}
		else if (variant == Chess::Horde) {
			FEN = "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
		}
		else if (variant == Chess::ThreeCheck) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1";
		}
		else if (variant == Chess::FiveCheck) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 5+5 0 1";
		}
		else if (variant == Chess::RacingKings) {
			FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
		}
		else if (variant == Chess::Crazyhouse) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
		}
	}

	if (!FEN.empty() && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
		isFlipped = true;
	}
	else if (!FEN.empty() && FEN.at(FEN.find_first_of(' ') + 1) == 'w') {
		isFlipped = false;
	}

	if (dropsEnabled) {
		for (auto& t : whiteDropPieces) {
			t.count = 0;
		}
		for (auto& t : blackDropPieces) {
			t.count = 0;
		}
		setupDropSprites();
	}
	bool s = isAnimated;
	if (!init) { isAnimated = false; }
	pieceList = generatePositionFromFENID(FEN);
	if (!init) { isAnimated = s; }
	stockfish.setVariant(variant, chess960Enabled);
	if (stockfishEnabled) {
		if (FEN_ID.has_value() || endgamePosition) {
			playerSideWhite = whiteToPlay;
		}
		else if (!init) { playerSideWhite = std::rand() % 2; }
	}
	if (init) { generateLegalMoves(true); }
}

// Sprite Setup
void Board::setupDropSprites() {
	selectedDropPiece.sprite.setOrigin(selectedDropPiece.sprite.getLocalBounds().getCenter());
	selectedDropPiece.sprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	float boardX = boardTexture.getSize().x * boardScale;
	float boardY = boardTexture.getSize().y * boardScale;
	dropPieceCount.x = 5;
	if (dropPieceCount.x >= 8) {
		dropPieceSquareSize.x = (boardY - 15.0f) / (float)dropPieceCount.x;
	}
	else if (dropPieceCount.x < 4) {
		dropPieceSquareSize.x = ((boardY - 15.0f) / 8.0f) + (60.0f / (float)dropPieceCount.x);
	}
	else {
		dropPieceSquareSize.x = ((boardY - 15.0f) / 8.0f) + (85.0f / (float)dropPieceCount.x);
	}
	dropPieceWidth.x = dropPieceSquareSize.x;
	dropPieceHeight.x = dropPieceSquareSize.x * dropPieceCount.x;

	dropPieceBackgroundW.setColor(sf::Color(210, 210, 210));
	dropPieceBackgroundW.setScale({ dropPieceWidth.x / dropPieceBackgroundW.getTexture().getSize().x, dropPieceHeight.x / dropPieceBackgroundW.getTexture().getSize().y });
	dropPieceBackgroundW.setOrigin(dropPieceBackgroundW.getLocalBounds().getCenter());
	dropPieceBackgroundW.setPosition({ boardOffset.x - (dropPieceSquareSize.x / 1.25f), (boardY / 2.0f) + boardOffset.y });

	dropPieceCount.y = 5;
	if (dropPieceCount.y >= 8) {
		dropPieceSquareSize.y = (boardY - 15.0f) / (float)dropPieceCount.y;
	}
	else if (dropPieceCount.y < 4) {
		dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (60.0f * 1.0f / (float)dropPieceCount.y);
	}
	else {
		dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (85.0f * 1.0f / (float)dropPieceCount.y);
	}
	dropPieceWidth.y = dropPieceSquareSize.y;
	dropPieceHeight.y = dropPieceSquareSize.y * dropPieceCount.y;

	dropPieceBackgroundB.setColor(sf::Color(210, 210, 210));
	dropPieceBackgroundB.setScale({ dropPieceWidth.y / dropPieceBackgroundB.getTexture().getSize().x, dropPieceHeight.y / dropPieceBackgroundB.getTexture().getSize().y });
	dropPieceBackgroundB.setOrigin(dropPieceBackgroundB.getLocalBounds().getCenter());
	dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardX), (boardY / 2.0f) + boardOffset.y });

	sf::Vector2f bgStart = { dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f), dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f) };

	std::vector<sf::Texture>& bT = *boardTextures;
	if (variant == Chess::Crazyhouse) {
		whiteDropPieces.emplace_back('p', getTextureFromID('p', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
		whiteDropPieces.emplace_back('n', getTextureFromID('n', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
		whiteDropPieces.emplace_back('b', getTextureFromID('b', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
		whiteDropPieces.emplace_back('r', getTextureFromID('r', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
		whiteDropPieces.emplace_back('q', getTextureFromID('q', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());

		blackDropPieces.emplace_back('p', getTextureFromID('p', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
		blackDropPieces.emplace_back('n', getTextureFromID('n', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
		blackDropPieces.emplace_back('b', getTextureFromID('b', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
		blackDropPieces.emplace_back('r', getTextureFromID('r', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
		blackDropPieces.emplace_back('q', getTextureFromID('q', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
	}
}

void Board::setupPromoteSprites()
{
	promotePieces.clear();
	if (promotePiece != nullptr) {
		std::array<char, 5> chars{ 'q', 'n', 'b', 'r', 'k' };
		std::vector<sf::Vector2f> promoteScales{ {pieceScale * 0.82f, pieceScale * 0.82f}, {pieceScale * 0.775f, pieceScale * 0.775f}, {pieceScale * 0.8f, pieceScale * 0.8f}, {pieceScale * 0.785f, pieceScale * 0.785f}, {pieceScale * 0.785f, pieceScale * 0.785f} };
		int x = getLocalPosition(promotePiece->getGlobalPos()).x;
		if (promotePiece->isWhite()) {
			for (int y = 8; y >= (8 - promotionPieceCount) + 1; y--) {
				int index = 8 - y;
				char id = chars.at(index);
				promotePieces.emplace_back(getTextureFromID(id, Chess::White), boardTextures->at(6), sf::Vector2i{ x, y }, id, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(), promotionSquareColor, promoteScales.at(index));
			}
			promotePiece->setTarget({});
		}
		else {
			for (int y = 1; y <= promotionPieceCount; y++) {
				int index = y - 1;
				char id = chars.at(index);
				promotePieces.emplace_back(getTextureFromID(id, Chess::Black), boardTextures->at(6), sf::Vector2i{ x, y }, id, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(), promotionSquareColor, promoteScales.at(index));
			}
		}
	}
}

void Board::setExtraSprites() {
	for (auto& piece : pieceList) { piece->setSprites(boardTextures->at(0), boardOffset, boardSize, boardMultiplier, pieceScale, isFlipped); }
}

// Board Movement, Textures
void Board::moveBy(float x, float y)
{
	offset.x += x;
	offset.y += y;
	boardOffset.x += x;
	boardOffset.y += y;
	for (auto& piece : pieceList) {
		piece->move(x, y);
	}
	boardSprite.move({ x, y });
	checkSprite.move({ x, y });
	selectedPieceBackground.move({ x, y });
	lastMoveStart.move({ x, y });
	lastMoveDest.move({ x, y });
	kothBackground.move({ x, y });
	kothShadow.move({ x, y });
	rankBackground.move({ x, y });
	rankShadowTop.move({ x, y });
	whiteCheckCount.move({ x, y });
	blackCheckCount.move({ x, y });
	dropPieceBackgroundW.move({ x, y });
	dropPieceBackgroundB.move({ x, y });
	promotionOverlay.move({ x, y });
	optionChangeOverlay.move({ x, y });
	optionChangeText.move({ x, y });
	ghostSprite.move({ x, y });
	if (variant == Chess::Crazyhouse) {
		for (auto& piece : whiteDropPieces) {
			piece.move(x, y);
		}
		for (auto& piece : blackDropPieces) {
			piece.move(x, y);
		}
	}
	setExtraSprites();
}

void Board::scale(float scale)
{
	boardScale *= scale;
	boardMultiplier *= scale;
	pieceScale *= scale;
	dropPieceSquareSize.x *= scale;
	dropPieceSquareSize.y *= scale;
	sizeMultiplier *= scale;
	boardSprite.setScale({ boardScale, boardScale });
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + offset.x;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + offset.y;
	boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	for (auto& piece : pieceList) {
		piece->updateScale(pieceScale, boardOffset, boardSize, boardMultiplier, isFlipped);
	}
	checkSprite.setScale({ boardScale, boardScale });
	selectedPieceBackground.setScale({ boardScale, boardScale });
	lastMoveStart.setScale({ boardScale, boardScale });
	lastMoveDest.setScale({ boardScale, boardScale });
	kothBackground.setScale({ boardScale, boardScale });
	kothShadow.setScale({ boardScale, boardScale });
	rankBackground.setScale({ boardScale, boardScale });
	rankShadowTop.setScale({ boardScale, boardScale });
	whiteCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	blackCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	whiteCheckText.setScale({ sizeMultiplier, sizeMultiplier });
	blackCheckText.setScale({ sizeMultiplier, sizeMultiplier });
	optionChangeText.setScale(boardSprite.getScale());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	dropPieceBackgroundW.setScale({ dropPieceBackgroundW.getScale().x * scale, dropPieceBackgroundW.getScale().y * scale });
	dropPieceBackgroundB.setScale({ dropPieceBackgroundB.getScale().x * scale, dropPieceBackgroundB.getScale().y * scale });
	dropPieceBackgroundW.setPosition({ boardOffset.x - (dropPieceSquareSize.x / 1.25f), dropPieceBackgroundW.getPosition().y });
	dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardTexture.getSize().x * boardScale), dropPieceBackgroundW.getPosition().y });
	ghostSprite.setScale({ pieceScale, pieceScale });
	if (hasCheck) {
		for (auto& piece : pieceList) {
			if (piece->IsA("King")) {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				if (king->inCheck) { checkSprite.setPosition(getGlobalPosition(king->getLocalPos())); }
			}
		}
	}

	if (variant == Chess::Crazyhouse) {
		sf::Vector2f bgStart = { dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f), dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f) };
		int i = 0;
		for (auto& piece : whiteDropPieces) {
			piece.scale(dropPieceSquareSize.x, dropPieceBackgroundW, bgStart.x, i);
			i++;
		}
		i = 0;
		for (auto& piece : blackDropPieces) {
			piece.scale(dropPieceSquareSize.y, dropPieceBackgroundB, bgStart.y, i);
			i++;
		}
	}
	setSpritePositions();
	setExtraSprites();
}

void Board::setLocation(sf::Vector2f pos)
{
	moveBy(pos.x - boardSprite.getPosition().x, pos.y - boardSprite.getPosition().y);
}

void Board::resetTransform() {
	setLocation(startingOffset + sf::Vector2f{ windowSize.x / 2.0f, windowSize.y / 2.0f });
	scale(startingScale / boardScale);
}

void Board::flipBoard()
{
	isFlipped = !isFlipped;
	for (auto& piece : pieceList) {
		piece->reversePosition(boardOffset, boardSize);
	}
	if (hasCheck) { checkSprite.setPosition(Chess::reversePosition(checkSprite.getPosition(), boardSize) + boardOffset + boardOffset); }
	setExtraSprites();
	setSpritePositions();
}

void Board::setOptionChange(std::string string, bool v)
{
	std::ostringstream out;
	out << string << ": " << std::boolalpha << v;
	std::string n = out.str();
	size_t at = n.find(':') + 2;
	if (n.find(':') != std::string::npos) { n.at(at) = (char)std::toupper(n.at(at)); }
	optionChangeText.setString(n);
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.reset();
	optionClock.start();
}

void Board::setOptionChange(std::string string, int v)
{
	optionChangeText.setString(string + ": " + std::to_string(v));
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.reset();
	optionClock.start();
}

void Board::setOptionChange(std::string string, Chess::Variant v)
{
	optionChangeText.setString(string + ": " + Chess::toString(v));
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.reset();
	optionClock.start();
}

void Board::setSpritePositions()
{
	ghostSprite.setPosition({ -1000, -1000 });
	lastMoveStart.setPosition(getGlobalPosition(lastMoveStartLocal));
	lastMoveDest.setPosition(getGlobalPosition(lastMoveDestLocal));
	rankBackground.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 8.0f }));
	rankShadowTop.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 8.0f }));
	promotionOverlay.setPosition(boardSprite.getPosition());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	if (isFlipped) {
		rankShadowTop.setRotation(sf::degrees(180));
	}
	else {
		rankShadowTop.setRotation(sf::degrees(0));
	}
}

void Board::setPieceSheet(std::vector<sf::Image> sheets, int set)
{
	Chess::loadPieceSet(sheets.at(set), pieceTextures, pieceSize);
}

void Board::setBoardTexture(sf::Image& boardSheet, int set)
{
	Chess::loadBoard(boardSheet, boardSprite, boardTexture, set, boardTextureSize);
}