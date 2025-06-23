#include "Board.h"

Board::Board(Chess::Variant variant, std::optional<std::string> FEN_ID, float xOffset, float yOffset, int boardSize, bool animated, sf::Vector2u windowSize, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
	bool AI, bool AI_Only, std::optional<bool> playerWhite, bool repeatFEN, bool chess960, bool endGamePosition, std::vector<sf::Texture>& pTextures, std::vector<sf::Texture>& boardTextures, bool init) : variant(variant), whiteToPlay(true), playerSideWhite(true), holdingDropPiece(false), AI_Only(AI_Only), endgamePosition(endGamePosition), chess960Enabled(chess960), stockfishEnabled(AI),
	calculatingPos(false), eighthRankWhite(false), mouseSelecting(false), mouseClicked(false), pieceMoving(false), hasCheck(false), isPromoting(false), scaleMode(false), mouseMode(false), pieceSet(pieceSet), boardSet(boardSet), atomicKings(false),
	animationFinished(false), holdingPiece(false), checksEnabled(true), castlingEnabled(true), dropsEnabled(false), hasDoubleCheck(false), isPaused(false), gameEnded(false), repeatFEN(repeatFEN), selectedPiece(nullptr), capturePiece(nullptr), castleKing(nullptr), castleRook(nullptr), promotePiece(nullptr),
	boardTexture(boardT), boardSprite(boardT), selectedPieceBackground(boardTextures.at(5)), checkSprite(boardTextures.at(2)), lastMoveStart(boardTextures.at(3)), lastMoveDest(boardTextures.at(3)), kothBackground(boardTextures.at(8)), kothShadow(boardTextures.at(9)), rankBackground(boardTextures.at(10)), rankShadowTop(boardTextures.at(11)),
	dropPieceBackgroundW(boardTextures.at(13)), dropPieceBackgroundB(boardTextures.at(13)), whiteCheckCount(boardTextures.at(12)), blackCheckCount(boardTextures.at(12)), whiteCheckText(textFont), blackCheckText(textFont), moveSound(moveBuffer), captureSound(captureBuffer), gameEndSound(gameEndBuffer), captureThreshold(1.0f), pieceSize(320), boardTextureSize(boardSize), halfMoves(0), fullMoves(0), sizeMultiplier(scale), stockfish("assets/other/fairy-stockfish-largeboard_x86-64.exe", 20),
	whiteChecks(0), blackChecks(0), isAnimated(animated), pieceTextures(pTextures), boardTextures(std::make_shared<std::vector<sf::Texture>>(boardTextures)), textFont(std::make_shared<sf::Font>(textFont)), selectedDropPiece(' ', pieceTextures.at(0)), windowSize(windowSize), selectedPos(), lastMoveStartLocal(100, 100), lastMoveDestLocal(100, 100), promotionPieceCount(4), shouldPostMove(true), promotionEnabled(true), gameShouldEnd(false), ghostSprite(pieceTextures.back()),
	playingMove(false), isFlipped(false), autoFlip(false), optionMode(true), optionChangeText(textFont, "Paused: True", 70), boardSize(sf::Vector2f{(float)boardSize, (float)boardSize}), generatingMoves(false), startedStockfish(AI)
{
	// Setup
	Chess::loadBoard(boardSheet, boardSprite, boardTexture, boardSet, boardSize);
	Chess::loadPieceSet(pieceSheets.at(pieceSet), pieceTextures, pieceSize);

	// Vars
	float ScaleX = windowSize.x / (float)boardTexture.getSize().x;
	float ScaleY = windowSize.y / (float)boardTexture.getSize().y;
	boardScale = std::roundf(std::min(ScaleX, ScaleY) * sizeMultiplier * 1000.0f) / 1000.0f; // Round to 0.00
	startingScale = boardScale;
	pieceScale = boardScale * (128.0f / pieceSize);
	boardMultiplier = boardScale * 128.0f;
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + xOffset;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + yOffset;
	this->boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	startingOffset = { xOffset, yOffset };
	offset += {xOffset, yOffset};
	if (!stockfishEnabled) { AI_Only = false; }

	// Sprite Setup
	boardSprite.setScale({ boardScale, boardScale });
	boardSprite.setOrigin(boardSprite.getLocalBounds().getCenter());
	boardSprite.setPosition({ boardOffset.x + ((boardTexture.getSize().x * boardScale) / 2.0f),  boardOffset.y + ((boardTexture.getSize().y * boardScale) / 2.0f) });
	promotionOverlay.setSize({ (float)boardSize, (float)boardSize });
	promotionOverlay.setOrigin(promotionOverlay.getGlobalBounds().getCenter());
	promotionOverlay.setScale(boardSprite.getScale());
	promotionOverlay.setFillColor(sf::Color(35, 35, 35, 150));
	optionChangeOverlay.setSize({ (float)boardSize, (float)boardSize });
	optionChangeOverlay.setOrigin(optionChangeOverlay.getGlobalBounds().getCenter());
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(boardSprite.getPosition());
	optionChangeText.setFillColor(sf::Color(0, 0, 0, 0));

	selectedPieceBackground.setOrigin(selectedPieceBackground.getLocalBounds().getCenter());
	selectedPieceBackground.setScale(boardSprite.getScale());
	checkSprite.setOrigin(checkSprite.getLocalBounds().getCenter());
	checkSprite.setScale(boardSprite.getScale());
	lastMoveDest.setOrigin(lastMoveDest.getLocalBounds().getCenter());
	lastMoveDest.setScale(boardSprite.getScale());
	lastMoveStart.setOrigin(lastMoveStart.getLocalBounds().getCenter());
	lastMoveStart.setScale(boardSprite.getScale());
	kothBackground.setOrigin(kothBackground.getLocalBounds().getCenter());
	kothBackground.setScale(boardSprite.getScale());
	kothShadow.setOrigin(kothShadow.getLocalBounds().getCenter());
	kothShadow.setScale(boardSprite.getScale());
	rankBackground.setOrigin(rankBackground.getLocalBounds().getCenter());
	rankBackground.setScale(boardSprite.getScale());
	rankShadowTop.setOrigin(rankShadowTop.getLocalBounds().getCenter());
	rankShadowTop.setScale(boardSprite.getScale());
	whiteCheckCount.setOrigin(whiteCheckCount.getLocalBounds().getCenter());
	whiteCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	blackCheckCount.setOrigin(blackCheckCount.getLocalBounds().getCenter());
	blackCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	whiteCheckText.setOrigin(whiteCheckText.getLocalBounds().getCenter());
	whiteCheckText.setCharacterSize(35);
	whiteCheckText.setScale({ sizeMultiplier, sizeMultiplier });
	blackCheckText.setOrigin(blackCheckText.getLocalBounds().getCenter());
	blackCheckText.setCharacterSize(35);
	blackCheckText.setScale({ sizeMultiplier, sizeMultiplier });
	ghostSprite.setOrigin(ghostSprite.getLocalBounds().getCenter());
	ghostSprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	ghostSprite.setColor(sf::Color{ 255, 255, 255, 75 });
	kothBackground.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	kothShadow.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	setSpritePositions();

	// Audio
	moveBuffer.loadFromFile("assets/sound/Move.mp3");
	captureBuffer.loadFromFile("assets/sound/Capture.mp3");
	checkBuffer.loadFromFile("assets/sound/Check.mp3");
	gameEndBuffer.loadFromFile("assets/sound/End.mp3");
	explosionBuffer.loadFromFile("assets/sound/Explosion.mp3");
	moveSound.setVolume(10);
	captureSound.setVolume(15);
	gameEndSound.setVolume(25);

	// Stockfish
	if (stockfishEnabled) { 
		if (playerWhite.has_value()) { playerSideWhite = playerWhite.value(); }
		stockfish.startStockfish();
	}

	// FEN Setup
	setupFEN(FEN_ID, init);

	if (!init) {
		isPaused = true;
		mouseMode = true;
	}
}

Board::~Board()
{
}