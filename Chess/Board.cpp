#include "Board.h"

Board::Board(Chess::Variant variant, std::optional<std::string> FEN_ID, float xOffset, float yOffset, int boardSize, bool animated, sf::Vector2u windowSize, float scale, int boardSet, int pieceSet, std::vector<sf::Image> pieceSheets, sf::Image& boardSheet, sf::Font& textFont, sf::Texture& boardT,
	bool AI, bool AI_Only, std::optional<bool> playerWhite, bool repeatFEN, bool chess960, bool endGamePosition, std::vector<sf::Texture>& pTextures, std::vector<sf::Texture>& boardTextures, std::vector<sf::SoundBuffer>& soundBuffers, bool timeEnabled, bool init, sf::Time whiteTime, sf::Time blackTime, sf::Time timeIncr) : variant(variant), whiteToPlay(true), playerSideWhite(true), holdingDropPiece(false), AI_Only(AI_Only), endgamePosition(endGamePosition), chess960Enabled(chess960), stockfishEnabled(AI),
	calculatingPos(false), eighthRankWhite(false), mouseSelecting(false), mouseClicked(false), pieceMoving(false), hasCheck(false), isPromoting(false), scaleMode(false), mouseMode(false), pieceSet(pieceSet), boardSet(boardSet), atomicKings(false),
	animationFinished(false), holdingPiece(false), checksEnabled(true), castlingEnabled(true), dropsEnabled(false), isPaused(false), gameEnded(false), repeatFEN(repeatFEN), selectedPiece(nullptr), capturePiece(nullptr), castleKing(nullptr), castleRook(nullptr), promotePiece(nullptr),
	boardTexture(boardT), boardSprite(boardT), selectedPieceBackground(boardTextures.at(5)), checkSprite(boardTextures.at(2)), lastMoveStart(boardTextures.at(3)), lastMoveDest(boardTextures.at(3)), kothBackground(boardTextures.at(8)), kothShadow(boardTextures.at(9)), rankBackground(boardTextures.at(10)), rankShadowTop(boardTextures.at(11)),
	dropPieceBackgroundW(boardTextures.at(13)), dropPieceBackgroundB(boardTextures.at(13)), whiteCheckCount(boardTextures.at(12)), blackCheckCount(boardTextures.at(12)), whiteCheckText(textFont, "3", 40U), blackCheckText(textFont, "3", 40U), soundBuffers(&soundBuffers), moveSound(soundBuffers.at(0)), captureSound(soundBuffers.at(1)), gameEndSound(soundBuffers.at(2)), lowTimeSound(soundBuffers.at(4)), captureThreshold(1.0f), pieceSize(320), boardTextureSize(boardSize), halfMoves(0), fullMoves(0), sizeMultiplier(scale), stockfish("assets/other/fairy-stockfish-largeboard_x86-64.exe", 1),
	whiteChecks(0), blackChecks(0), isAnimated(animated), pieceTextures(pTextures), boardTextures(&boardTextures), textFont(&textFont), selectedDropPiece(' ', pieceTextures.at(0)), windowSize(windowSize), selectedPos(), lastMoveStartLocal(100, 100), lastMoveDestLocal(100, 100), shouldPostMove(true), promotionEnabled(true), gameShouldEnd(false), ghostSprite(pieceTextures.back()),
	playingMove(false), isFlipped(false), autoFlip(false), optionMode(true), optionChangeText(textFont, "Paused: True", 70), boardSize(sf::Vector2f{ (float)boardSize, (float)boardSize }), generatingMoves(false), startedStockfish(AI), movesPlayed(0), playerWhite(playerWhite), ai(false), changingPosition(false), forwardMove(false), cPosition(false), initialMousePos(), initialBoardScale(1.0f),
	timeClock(), timeEnabled(timeEnabled), whiteTime(whiteTime), blackTime(blackTime), startingWhiteTime(whiteTime), startingBlackTime(blackTime), whiteTimeText(textFont, "00:00", 85), blackTimeText(textFont, "00:00", 85), whiteTimeBG(boardTextures.at(16)), blackTimeBG(boardTextures.at(16)), whiteTimeOutline(boardTextures.at(16)), blackTimeOutline(boardTextures.at(16)), timeIncrement(timeIncr), boardHSV({ 0, 0, 0 }), gameType(Chess::Classical), instantMove(false), moveSpeed(1.0f), moveSpeedI(1.0f), instantMoveI(false)
{
	// Setup
	Chess::loadBoard(boardSheet, boardSprite, boardTexture, boardSet, boardSize);
	Chess::loadPieceSet(pieceSheets.at(pieceSet), pieceTextures, pieceSize);

	// Vars
	float ScaleX = windowSize.x / (float)boardTexture.getSize().x; // + Crazyhouse
	float ScaleY = windowSize.y / (float)boardTexture.getSize().y;
	if (timeEnabled) {
		ScaleY = windowSize.y / ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f);
	}
	boardScale = std::roundf(std::min(ScaleX, ScaleY) * sizeMultiplier * 1000.0f) / 1000.0f; // Round to 0.00
	if (timeEnabled) {
		yOffset += whiteTimeOutline.getTexture().getSize().y * boardScale * 1.129f / 2.0f;
	}
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
	whiteCheckText.setOrigin({ whiteCheckText.getLocalBounds().position.x + whiteCheckText.getLocalBounds().size.x / 2.0f, whiteCheckText.getLocalBounds().position.y + whiteCheckText.getLocalBounds().size.y / 2.0f });
	whiteCheckText.setScale({ boardScale, boardScale });
	blackCheckText.setOrigin({ blackCheckText.getLocalBounds().position.x + blackCheckText.getLocalBounds().size.x / 2.0f, blackCheckText.getLocalBounds().position.y + blackCheckText.getLocalBounds().size.y / 2.0f });
	blackCheckText.setScale({ boardScale, boardScale });
	ghostSprite.setOrigin(ghostSprite.getLocalBounds().getCenter());
	ghostSprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	ghostSprite.setColor(sf::Color{ 255, 255, 255, 75 });
	kothBackground.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	kothShadow.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	unsigned int square = (unsigned int)(boardSize / 8.0f) + 1U;
	boardHSV = RGBtoHSV(boardSprite.getTexture().copyToImage().getPixel({ square, 0 }));

	blackTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	blackTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{6.5f, 8.5f}) });
	blackTimeBG.move({ 0, -(blackTimeBG.getTexture().getSize().y * blackTimeBG.getScale().y / 2.0f) - 1});
	blackTimeBG.setOrigin({ blackTimeBG.getLocalBounds().size.x / 2.0f, blackTimeBG.getLocalBounds().size.y / 2.0f });
	blackTimeBG.setColor(setHueSat(sf::Color(16, 9, 23), boardHSV));
	blackTimeBG.setRotation(sf::degrees(180));
	blackTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	blackTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{6.5f, 8.5f}) });
	blackTimeOutline.move({ 0, -(blackTimeOutline.getTexture().getSize().y * blackTimeOutline.getScale().y / 2.0f) - 1 });
	blackTimeOutline.setOrigin({ blackTimeOutline.getLocalBounds().size.x / 2.0f, blackTimeOutline.getLocalBounds().size.y / 2.0f });
	blackTimeOutline.setColor(setHueSat(sf::Color(74, 67, 78), boardHSV));
	blackTimeOutline.setRotation(sf::degrees(180));
	blackTimeText.setLetterSpacing(2);
	blackTimeText.setPosition({ blackTimeBG.getPosition() });
	blackTimeText.setOrigin({ blackTimeText.getLocalBounds().position.x + (blackTimeText.getLocalBounds().size.x / 2.0f), blackTimeText.getLocalBounds().position.y + (blackTimeText.getLocalBounds().size.y / 2.0f)});
	blackTimeText.setScale({ boardScale, boardScale });
	blackTimeText.setFillColor(sf::Color(255, 255, 255));

	whiteTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	whiteTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{2.5f, 8.5f}) });
	whiteTimeBG.move({ 0, -(whiteTimeBG.getTexture().getSize().y * whiteTimeBG.getScale().y / 2.0f) - 1 });
	whiteTimeBG.setOrigin({ whiteTimeBG.getLocalBounds().size.x / 2.0f, whiteTimeBG.getLocalBounds().size.y / 2.0f });
	whiteTimeBG.setColor(setHueSat(sf::Color(235, 230, 239), boardHSV));
	whiteTimeBG.setRotation(sf::degrees(180));
	whiteTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	whiteTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{2.5f, 8.5f}) });
	whiteTimeOutline.move({ 0, -(whiteTimeOutline.getTexture().getSize().y * whiteTimeOutline.getScale().y / 2.0f) - 1 });
	whiteTimeOutline.setOrigin({ whiteTimeOutline.getLocalBounds().size.x / 2.0f, whiteTimeOutline.getLocalBounds().size.y / 2.0f });
	whiteTimeOutline.setColor(setHueSat(sf::Color(176, 158, 191), boardHSV));
	whiteTimeOutline.setRotation(sf::degrees(180));
	whiteTimeText.setLetterSpacing(2);
	whiteTimeText.setPosition({ whiteTimeBG.getPosition() });
	whiteTimeText.setOrigin({ whiteTimeText.getLocalBounds().position.x + (whiteTimeText.getLocalBounds().size.x / 2.0f), whiteTimeText.getLocalBounds().position.y + (whiteTimeText.getLocalBounds().size.y / 2.0f) });
	whiteTimeText.setScale({ boardScale, boardScale });
	whiteTimeText.setFillColor(setHueSat(sf::Color(22, 8, 33), boardHSV));

	std::string minutes = std::to_string((int)std::floor(whiteTime.asSeconds() / 60.0f));
	std::string seconds = std::to_string((int)std::fmodf(whiteTime.asSeconds(), 60.0f));
	if (minutes.size() == 1) { minutes.insert(0, "0"); }
	if (seconds.size() == 1) { seconds.insert(0, "0"); }
	whiteTimeText.setString(minutes + ":" + seconds);

	minutes = std::to_string((int)std::floor(blackTime.asSeconds() / 60.0f));
	seconds = std::to_string((int)std::fmodf(blackTime.asSeconds(), 60.0f));
	if (minutes.size() == 1) { minutes.insert(0, "0"); }
	if (seconds.size() == 1) { seconds.insert(0, "0"); }
	blackTimeText.setString(minutes + ":" + seconds);
	setSpritePositions();

	// Audio
	moveSound.setVolume(10);
	captureSound.setVolume(15);
	gameEndSound.setVolume(25);
	lowTimeSound.setVolume(10);

	// Stockfish
	delayClock.start();
	if (playerWhite.has_value()) { playerSideWhite = playerWhite.value(); }
	if (stockfishEnabled) { 
		stockfish.startStockfish();
	}

	// FEN Setup
	loadFromFEN(FEN_ID, init, true);

	if (!init) {
		mouseMode = true;
	}
	animationClock.restart();
}

Board::~Board()
{
}