#include "Board.h"
#include "Main.h"

using namespace Chess;
Board::Board(Variant variant, std::optional<std::string> FEN_ID, int boardSize, bool animated, sf::RenderWindow& window, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Texture& boardT, sf::Texture& selectBackgroundTexture, sf::Texture& checkTexture, sf::Texture& lastMoveTexture, sf::Texture& kothBackgroundTexture, sf::Texture& kothShadowTexture, sf::Texture& rankBackgroundTexture, sf::Texture& rankShadowTopTexture, sf::Texture& checkCountTexture, sf::Font& textFont,
	bool AI, std::optional<bool> playerWhite, bool chess960, bool endGamePosition, textureVector& pTextures, textureVector& extraTextures) : whiteToPlay(true), playerSideWhite(true), unpromoting(false), kingPromotionEnabled(false), endgamePosition(endGamePosition), Chess960Enabled(chess960), StockfishEnabled(AI), calculatingPos(false), hourGlass(false), eighthRankWhite(false), movePlayed(false), mouseSelecting(false), mouseClicked(false), pieceMoving(false), check(false), promoting(false), animationFinished(false), pieceSelectionLock(false), standardPosition(false), checksEnabled(true), castlingEnabled(true), dropsEnabled(false),
	selectedPiece(nullptr), capturePiece(nullptr), castleKing(nullptr), castleRook(nullptr), enPassantPiece(nullptr), promotePiece(nullptr),
	boardTexture(boardT), boardSprite(boardT), selectedPieceBackground(selectBackgroundTexture), checkSprite(checkTexture), lastMoveStart(lastMoveTexture), lastMoveDest(lastMoveTexture), kothBackground(kothBackgroundTexture), kothShadow(kothShadowTexture), rankBackground(rankBackgroundTexture), rankShadowTop(rankShadowTopTexture),
	whiteCheckCount(checkCountTexture), blackCheckCount(checkCountTexture), whiteCheckText(textFont), blackCheckText(textFont), moveSound(moveBuffer), captureSound(captureBuffer), gameEndSound(gameEndBuffer), threshold(1.0f), pieceSize(320.0f), boardSize(1024), halfMoves(0), fullMoves(0), dropPieceCount(5), sizeMultiplier(scale), c("assets/other/fairy-stockfish-largeboard_x86-64.exe", bp::std_in < os, bp::std_out > is),
	pieceSpriteSheets(pieceSheets), boardSpriteSheet(boardSheet), whiteChecks(0), blackChecks(0), wKRook(-1), bKRook(-1), wQRook(-1), bQRook(-1), animated(animated), pieceTextures(pTextures)
{
	// Setup
	this->boardSize = boardSize;
	Main::loadBoard(boardSpriteSheet, boardSprite, boardTexture, boardSet, boardSize);
	Main::loadPieceSet(pieceSpriteSheets.at(pieceSet), pieceTextures, pieceSize);
	// Vars
	float windowSizeX = window.getSize().x;
	float windowSizeY = window.getSize().y;
	float ScaleX = windowSizeX / (float)boardTexture.getSize().x;
	float ScaleY = windowSizeY / (float)boardTexture.getSize().y;
	boardScale = std::min(ScaleX, ScaleY) * sizeMultiplier;
	pieceScale = boardScale * 128.0f / pieceSize;
	boardXOffset = (windowSizeX / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f);
	boardYOffset = (windowSizeY / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f);
	boardMultiplier = ((boardTexture.getSize().x * boardScale) / 8);

	// Sprite Setup
	boardSprite.setScale({ boardScale, boardScale });
	boardSprite.setOrigin(boardSprite.getLocalBounds().getCenter());
	boardSprite.setPosition({ windowSizeX / 2.0f, windowSizeY / 2.0f });
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
	kothBackground.setPosition(sf::Vector2f{ boardXOffset + ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(4.5f) - 1.0f) * boardMultiplier });
	kothBackground.setScale(boardSprite.getScale());
	kothShadow.setOrigin(kothShadow.getGlobalBounds().getCenter());
	kothShadow.setPosition(sf::Vector2f{ boardXOffset + ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(4.5f) - 1.0f) * boardMultiplier });
	kothShadow.setScale(boardSprite.getScale());
	rankBackground.setOrigin(rankBackground.getGlobalBounds().getCenter());
	rankBackground.setPosition(sf::Vector2f{ boardXOffset + ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(8.0f) - 0.5f) * boardMultiplier });
	rankBackground.setScale(boardSprite.getScale());
	rankShadowTop.setOrigin(rankShadowTop.getGlobalBounds().getCenter());
	rankShadowTop.setPosition(sf::Vector2f{ boardXOffset + ((4.5f - 0.5f) * boardMultiplier), (Main::reverseY(8.0f) - 0.5f) * boardMultiplier });
	rankShadowTop.setScale(boardSprite.getScale());
	whiteCheckCount.setOrigin(whiteCheckCount.getGlobalBounds().getCenter());
	whiteCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	blackCheckCount.setOrigin(blackCheckCount.getGlobalBounds().getCenter());
	blackCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	whiteCheckText.setOrigin(whiteCheckText.getLocalBounds().getCenter());
	whiteCheckText.setCharacterSize(32 * sizeMultiplier);
	blackCheckText.setOrigin(blackCheckText.getLocalBounds().getCenter());
	blackCheckText.setCharacterSize(32 * sizeMultiplier);
	dropPieceBackground.setSize({ dropPieceWidth, dropPieceHeight });
	dropPieceBackground.setOrigin(dropPieceBackground.getLocalBounds().getCenter());
	dropPieceBackground.setPosition({ boardXOffset / 2.0f, 500.0f });

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

	// FEN Setup
	setupFEN(FEN_ID, extraTextures);

	// Stockfish
	if (StockfishEnabled) {
		playerSideWhite = std::rand() % 2;
		if (playerWhite.has_value()) { playerSideWhite = playerWhite.value(); }
	}

	if (dropPieceCount >= 5) {
		dropPieceSquareSize = (boardXOffset - 25.0f) / (float)dropPieceCount;
	}
	else {
		dropPieceSquareSize = 60.0f + (60.0f * (4.0f / (float)dropPieceCount) / 7.5f);
	}
	dropPieceWidth = dropPieceSquareSize * dropPieceCount;
	dropPieceHeight = dropPieceSquareSize;
}

void Board::setupFEN(std::optional<std::string> FEN_ID, textureVector extraTextures)
{
	currentMove = "";
	moves = "";
	wKRook = -1; wQRook = -1; bKRook = -1; bQRook = -1, whiteChecks = 0, blackChecks = 0;
	check = false;
	eighthRankWhite = false;
	lastMoveStart.setPosition({ -1000, -1000 });
	lastMoveDest.setPosition({ -1000, -1000 });
	selectedPiece.reset();
	capturePiece.reset();
	enPassantPiece.reset();
	pieceList.clear();
	allPositionsPlayed.clear();
	whiteDropPieces.clear();
	blackDropPieces.clear();
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
	}
	pieceList = Main::generatePositionFromFENID(FEN, *this, extraTextures);
	if (StockfishEnabled) {
		playerSideWhite = std::rand() % 2;
		Main::startStockfish(FEN, c, os, is, variant, Chess960Enabled);
	}
	calculatePositions(extraTextures, true);
}

void Board::calculatePositions(textureVector extraTextures, bool initial) {
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
								if (!initial) {
									if (whiteToPlay) {
										blackChecks++;
									}
									else {
										whiteChecks++;
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
	Main::setExtraSprites(pieceList, extraTextures);
}

void Board::draw(sf::RenderWindow& window) {
	window.draw(boardSprite);
}

Board::~Board()
{
}
