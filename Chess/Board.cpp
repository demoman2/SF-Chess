#include "Board.h"
#include "Main.h"

using namespace Chess;
Board::Board(Variant variant, std::optional<std::string> FEN_ID, float xOffset, float yOffset, int boardSize, bool animated, sf::Vector2u windowSize, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
	bool AI, bool AI_Only, std::optional<bool> playerWhite, bool chess960, bool endGamePosition, textureVector& pTextures, textureVector& boardTextures) : variant(variant), whiteToPlay(true), playerSideWhite(true), unpromoting(false), kingPromotionEnabled(false), dropPieceLock(false), AI_Only(AI_Only), endgamePosition(endGamePosition), Chess960Enabled(chess960), StockfishEnabled(AI), calculatingPos(false), hourGlass(false), eighthRankWhite(false), movePlayed(false), mouseSelecting(false), mouseClicked(false), pieceMoving(false), check(false), promoting(false), animationFinished(false), pieceSelectionLock(false), standardPosition(false), checksEnabled(true), castlingEnabled(true), dropsEnabled(false),
	selectedPiece(nullptr), capturePiece(nullptr), castleKing(nullptr), castleRook(nullptr), enPassantPiece(nullptr), promotePiece(nullptr),
	boardTexture(boardT), boardSprite(boardT), selectedPieceBackground(boardTextures.at(5)), checkSprite(boardTextures.at(2)), lastMoveStart(boardTextures.at(3)), lastMoveDest(boardTextures.at(3)), kothBackground(boardTextures.at(8)), kothShadow(boardTextures.at(9)), rankBackground(boardTextures.at(10)), rankShadowTop(boardTextures.at(11)),
	dropPieceBackgroundW(boardTextures.at(13)), dropPieceBackgroundB(boardTextures.at(13)), whiteCheckCount(boardTextures.at(12)), blackCheckCount(boardTextures.at(12)), whiteCheckText(textFont), blackCheckText(textFont), moveSound(moveBuffer), captureSound(captureBuffer), gameEndSound(gameEndBuffer), threshold(1.0f), pieceSize(320.0f), boardSize(boardSize), halfMoves(0), fullMoves(0), sizeMultiplier(scale), c("assets/other/fairy-stockfish-largeboard_x86-64.exe", bp::std_in < os, bp::std_out > is),
	whiteChecks(0), blackChecks(0), wKRook(-1), bKRook(-1), wQRook(-1), bQRook(-1), animated(animated), pieceTextures(pTextures), boardTextures(std::make_unique<textureVector>(boardTextures)), textFont(std::make_unique<sf::Font>(textFont)), dropPiece(pieceTextures.at(0), ' ')
{
	// Setup
	Main::loadBoard(boardSheet, boardSprite, boardTexture, boardSet, boardSize);
	Main::loadPieceSet(pieceSheets.at(pieceSet), pieceTextures, pieceSize);
	// Vars
	float windowSizeX = windowSize.x;
	float windowSizeY = windowSize.y;
	float ScaleX = windowSizeX / (float)boardTexture.getSize().x;
	float ScaleY = windowSizeY / (float)boardTexture.getSize().y;
	boardScale = std::min(ScaleX, ScaleY) * sizeMultiplier;
	pieceScale = boardScale * 128.0f / pieceSize;
	boardOffset.x = (windowSizeX / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + xOffset;
	boardOffset.y = (windowSizeY / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + yOffset;
	boardMultiplier = ((boardTexture.getSize().x * boardScale) / 8);

	// Sprite Setup
	boardSprite.setScale({ boardScale, boardScale });
	boardSprite.setOrigin(boardSprite.getLocalBounds().getCenter());
	boardSprite.setPosition({ windowSizeX / 2.0f + xOffset, windowSizeY / 2.0f + yOffset });
	promotionOverlay.setSize({ (float)boardSize, (float)boardSize });
	promotionOverlay.setOrigin(promotionOverlay.getGlobalBounds().getCenter());
	promotionOverlay.setScale(boardSprite.getScale());
	promotionOverlay.setPosition(boardSprite.getPosition());
	promotionOverlay.setFillColor(sf::Color(35, 35, 35, 150));
	selectedPieceBackground.setOrigin(selectedPieceBackground.getGlobalBounds().getCenter());
	selectedPieceBackground.setScale(boardSprite.getScale());
	checkSprite.setOrigin(checkSprite.getGlobalBounds().getCenter());
	checkSprite.setScale(boardSprite.getScale());
	lastMoveDest.setOrigin(lastMoveDest.getGlobalBounds().getCenter());
	lastMoveDest.setScale(boardSprite.getScale());
	lastMoveDest.setPosition({ -1000, -1000 });
	lastMoveStart.setOrigin(lastMoveStart.getGlobalBounds().getCenter());
	lastMoveStart.setScale(boardSprite.getScale());
	lastMoveStart.setPosition({ -1000, -1000 });
	kothBackground.setOrigin(kothBackground.getGlobalBounds().getCenter());
	kothBackground.setPosition(sf::Vector2f{ ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(4.5f) - 1.0f) * boardMultiplier } + boardOffset);
	kothBackground.setScale(boardSprite.getScale());
	kothShadow.setOrigin(kothShadow.getGlobalBounds().getCenter());
	kothShadow.setPosition(sf::Vector2f{ ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(4.5f) - 1.0f) * boardMultiplier } + boardOffset);
	kothShadow.setScale(boardSprite.getScale());
	rankBackground.setOrigin(rankBackground.getGlobalBounds().getCenter());
	rankBackground.setPosition(sf::Vector2f{ ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(8.0f) - 0.5f) * boardMultiplier } + boardOffset);
	rankBackground.setScale(boardSprite.getScale());
	rankShadowTop.setOrigin(rankShadowTop.getGlobalBounds().getCenter());
	rankShadowTop.setPosition(sf::Vector2f{ ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(8.0f) - 0.5f) * boardMultiplier } + boardOffset);
	rankShadowTop.setScale(boardSprite.getScale());
	whiteCheckCount.setOrigin(whiteCheckCount.getGlobalBounds().getCenter());
	whiteCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	blackCheckCount.setOrigin(blackCheckCount.getGlobalBounds().getCenter());
	blackCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	whiteCheckText.setOrigin(whiteCheckText.getLocalBounds().getCenter());
	whiteCheckText.setCharacterSize(35);
	whiteCheckText.setScale({ sizeMultiplier, sizeMultiplier });
	blackCheckText.setOrigin(blackCheckText.getLocalBounds().getCenter());
	blackCheckText.setCharacterSize(35);
	blackCheckText.setScale({ sizeMultiplier, sizeMultiplier });

	// Audio
	moveBuffer.loadFromFile("assets/sound/Move.mp3");
	captureBuffer.loadFromFile("assets/sound/Capture.mp3");
	checkBuffer.loadFromFile("assets/sound/Check.mp3");
	gameEndBuffer.loadFromFile("assets/sound/End.mp3");
	explosionBuffer.loadFromFile("assets/sound/Explosion.mp3");
	moveSound.setVolume(10);
	captureSound.setVolume(15);
	gameEndSound.setVolume(25);

	// Variant Setup
	if (variant == Antichess) {
		castlingEnabled = false;
		checksEnabled = false;
		kingPromotionEnabled = true;
	}
	else if (variant == Chess960) {
		Chess960Enabled = true;
	}
	else if (variant == Atomic) {
		captureSound.setBuffer(explosionBuffer);
		threshold = 30.0f;
	}
	else if (variant == RacingKings) {
		castlingEnabled = false;
	}
	else if (variant == Crazyhouse) {
		dropsEnabled = true;
	}

	// FEN Setup
	setupFEN(FEN_ID);

	// Stockfish
	if (StockfishEnabled) {
		playerSideWhite = std::rand() % 2;
		if (playerWhite.has_value()) { playerSideWhite = playerWhite.value(); }
	}
}

void Board::setupFEN(std::optional<std::string> FEN_ID)
{
	currentMove = "";
	moves = "";
	wKRook = -1; wQRook = -1; bKRook = -1; bQRook = -1, whiteChecks = 0, blackChecks = 0;
	check = false;
	eighthRankWhite = false;
	lastMoveStart.setPosition({ -1000, -1000 });
	lastMoveDest.setPosition({ -1000, -1000 });
	whiteDropPieces.clear();
	blackDropPieces.clear();
	selectedPiece.reset();
	capturePiece.reset();
	enPassantPiece.reset();
	pieceList.clear();
	allPositionsPlayed.clear();
	FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1";
	if (FEN_ID.has_value()) {
		FEN = FEN_ID.value();
	}
	else {
		if (endgamePosition) {
			std::string fen;
			if (variant == Antichess) {
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
			}
			else {
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
			}
			if (variant == ThreeCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "3+3 0 1";
			}
			else if (variant == FiveCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "5+5 0 1";
			}
			else if (variant == Crazyhouse) {
				FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
			}
			else {
				if (!fen.empty()) {
					FEN = fen;
				}
			}
		}
		else if (Chess960Enabled) {
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

			if (variant == ThreeCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "3+3 0 1";
			}
			else if (variant == FiveCheck) {
				FEN = fen.substr(0, fen.size() - 3);
				FEN += "5+5 0 1";
			}
			else if (variant == Horde) {
				FEN = fen.substr(0, 8) + "/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
			}
			else if (variant == RacingKings) {
				FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
			}
			else if (variant == Crazyhouse) {
				FEN = fen;
				FEN.insert(43, "[]");
			}
			else {
				if (!fen.empty()) {
					FEN = fen;
				}
			}

		}
		else if (variant == Horde) {
			FEN = "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
		}
		else if (variant == ThreeCheck) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1";
		}
		else if (variant == FiveCheck) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 5+5 0 1";
		}
		else if (variant == RacingKings) {
			FEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
		}
		else if (variant == Crazyhouse) {
			FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
		}
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
	pieceList = Main::generatePositionFromFENID(FEN, *this);
	if (StockfishEnabled) {
		playerSideWhite = std::rand() % 2;
		Main::startStockfish(FEN, c, os, is, variant, Chess960Enabled);
	}
	calculatePositions(true);
}

void Board::calculatePositions(bool init) {
	if (dropsEnabled) {
		for (auto& piece : whiteDropPieces) {
			piece.update();
		}
		for (auto& piece : blackDropPieces) {
			piece.update();
		}
	}
	if (checksEnabled) {
		for (auto& p : pieceList) {
			if (whiteToPlay != p->isWhite()) {
				if (p->name != "King") {
					Main::calculatePositions(p, *this);
				}
			}
			if (p->name == "King") {
				std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
				k->inCheck = false;
			}
		}
		check = false;
		for (auto& p : pieceList) {
			if (p->name == "King" && whiteToPlay == (p->color == PColor::White)) {
				for (auto& p2 : pieceList) {
					if (p2->color != p->color) {
						std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
						for (auto& pos : p2->getAvailableCapturePositions()) {
							if (pos == k->getLocalPosition()) {
								k->inCheck = true;
								checkSprite.setPosition(k->getGlobalPosition());
								check = true;
								if (!init) {
									if (whiteToPlay) {
										whiteChecks++;
									}
									else {
										blackChecks++;
									}
								}
								goto next;
							}
						}
					}
				}
				break;
			}
		}
	next:
		for (auto& p : pieceList) {
			if (whiteToPlay == p->isWhite()) {
				if (p->name != "King") {
					Main::calculatePositions(p, *this);
				}
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name == "King") {
				Main::calculatePositions(pieceList.at(i), *this);
			}
		}
	}
	else {
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name != "King") {
				Main::calculatePositions(pieceList.at(i), *this);
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name == "King") {
				Main::calculatePositions(pieceList.at(i), *this);
			}
		}
	}
	if (variant == Antichess) {
		bool hasCapture = false;
		for (auto& p : pieceList) {
			if (p != nullptr && whiteToPlay == p->isWhite()) {
				if (!p->availableCapturePositions.empty()) {
					hasCapture = true;
					break;
				}
				else if (p->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(p);
					if (!pawn->enPassantPositions.empty()) {
						hasCapture = true;
						break;
					}
				}
			}
		}
		if (hasCapture) {
			for (auto& p : pieceList) {
				if (whiteToPlay == p->isWhite()) {
					p->availablePositions.clear();
				}
			}
		}
	}
	if (dropsEnabled) {
		calculateDropPositions();
	}
	Main::setExtraSprites(pieceList, pieceScale, boardOffset, boardMultiplier, *boardTextures);
}

void Board::setupDropSprites() {
	dropPiece.first.setOrigin(dropPiece.first.getLocalBounds().getCenter());
	dropPiece.first.setScale(sf::Vector2f(pieceScale, pieceScale));
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

	textureVector& bT = *boardTextures;
	sf::Font& font = *textFont;
	whiteDropPieces.push_back(DropPiece('p', 9, pieceTextures, bT, font, dropPieceBackgroundW, dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
	whiteDropPieces.push_back(DropPiece('n', 8, pieceTextures, bT, font, dropPieceBackgroundW, dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
	whiteDropPieces.push_back(DropPiece('b', 6, pieceTextures, bT, font, dropPieceBackgroundW, dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
	whiteDropPieces.push_back(DropPiece('r', 11, pieceTextures, bT, font, dropPieceBackgroundW, dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
	whiteDropPieces.push_back(DropPiece('q', 10, pieceTextures, bT, font, dropPieceBackgroundW, dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
	
	blackDropPieces.push_back(DropPiece('p', 3, pieceTextures, bT, font, dropPieceBackgroundB, dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
	blackDropPieces.push_back(DropPiece('n', 2, pieceTextures, bT, font, dropPieceBackgroundB, dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
	blackDropPieces.push_back(DropPiece('b', 0, pieceTextures, bT, font, dropPieceBackgroundB, dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
	blackDropPieces.push_back(DropPiece('r', 5, pieceTextures, bT, font, dropPieceBackgroundB, dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
	blackDropPieces.push_back(DropPiece('q', 4, pieceTextures, bT, font, dropPieceBackgroundB, dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
}

void Board::calculateDropPositions() {
	dropSquares.clear();
	textureVector& bT = *boardTextures;
	bool hasCheck = false;
	for (auto& piece : pieceList) {
		if (piece->name == "King") {
			std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
			if (king->inCheck) {
				hasCheck = true; 
				break;
			}
		}
	}
	for (int x = 1; x <= 8; x++) {
		for (int y = 1; y <= 8; y++) {
			if (Main::getPieceFromPosition({ x, y }, pieceList) == nullptr) {
				if (checksEnabled && hasCheck) {
					PColor col = whiteToPlay ? White : Black;
					pieceVector newV;
					for (size_t j = 0; j < pieceList.size(); j++)
					{
						newV.push_back(pieceList[j]->clone());
					}
					newV.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardMultiplier, col, pieceTextures.at(0), false));
					if (Main::isValidPosition(newV, col, variant)) {
						sf::Sprite square{ bT.at(0) };
						square.setOrigin(square.getLocalBounds().getCenter());
						square.setScale(boardSprite.getScale());
						square.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
						dropSquares.push_back(square);
					}
				}
				else {
					sf::Sprite square{ bT.at(0) };
					square.setOrigin(square.getLocalBounds().getCenter());
					square.setScale(boardSprite.getScale());
					square.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
					dropSquares.push_back(square);
				}
			}
		}
	}
}

void Board::draw(sf::RenderWindow& window) {
	window.draw(boardSprite);
}

Board::~Board()
{
}
