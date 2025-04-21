#include "Main.h"

using namespace Chess;

int main()
{
	srand(time(0));
	std::cout << std::boolalpha;
	sf::Image hourglassImage;
	hourglassImage.loadFromFile("assets/other/hourglass.png");
	const auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
	const auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
	const auto hourGlassCursor = sf::Cursor::createFromPixels(hourglassImage.getPixelsPtr(), hourglassImage.getSize(), sf::Vector2u{}).value();
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktopMode, "SF Chess", sf::Style::None);
	float windowSizeX = window.getSize().x;
	float windowSizeY = window.getSize().y;
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);

	// Icons
	sf::Image selectedIcon;
	sf::Image unicornWhiteIcon, unicornBlackIcon, zebraWhiteIcon, zebraBlackIcon, centaurWhiteIcon, centaurBlackIcon, checkersWhite, checkersBlack;
	unicornBlackIcon.loadFromFile("assets/icon/1.png");
	unicornWhiteIcon.loadFromFile("assets/icon/2.png");
	zebraWhiteIcon.loadFromFile("assets/icon/3.png");
	zebraBlackIcon.loadFromFile("assets/icon/4.png");
	centaurWhiteIcon.loadFromFile("assets/icon/5.png");
	centaurBlackIcon.loadFromFile("assets/icon/6.png");
	checkersWhite.loadFromFile("assets/icon/checkers_white.png");
	checkersBlack.loadFromFile("assets/icon/checkers_black.png");
	std::array<sf::Image, 6> iconTextures{ unicornBlackIcon, unicornWhiteIcon, zebraWhiteIcon, zebraBlackIcon, centaurWhiteIcon, centaurBlackIcon };
	if (std::rand() % 50 == 0) {
		if (std::rand() % 2 == 0) {
			selectedIcon = checkersWhite;
		}
		else {
			selectedIcon = checkersBlack;
		}
	}
	else {
		selectedIcon = iconTextures.at(std::rand() % 6);
	}

	// Chess Boards  
	int boardSize = 1024;
	sf::Image boardSpriteSheet;
	boardSpriteSheet.loadFromFile("assets/board/board_matrix.jpg");
	sf::Texture boardTexture;
	boardTexture.loadFromImage(boardSpriteSheet, false, sf::IntRect({ 0, 0 }, { boardSize, boardSize }));
	sf::Sprite board{ boardTexture };
	Main::loadBoard(boardSpriteSheet, board, boardTexture, 0, boardSize);
	sf::RectangleShape promotionOverlay;
	float ScaleX = windowSizeX / boardTexture.getSize().x;
	float ScaleY = windowSizeY / boardTexture.getSize().y;
	board.setScale({ std::min(ScaleX, ScaleY), std::min(ScaleX, ScaleY) });
	board.setOrigin(board.getLocalBounds().getCenter());
	board.setPosition({ windowSizeX / 2.0f, windowSizeY / 2.0f });
	promotionOverlay.setSize({ (float)boardSize, (float)boardSize });
	promotionOverlay.setOrigin(promotionOverlay.getGlobalBounds().getCenter());
	promotionOverlay.setScale(board.getScale());
	promotionOverlay.setPosition(board.getPosition());
	promotionOverlay.setFillColor(sf::Color(35, 35, 35, 150));

	// Pieces
	int halfMoves = 0, fullMoves = 0;
	bool whiteToPlay = true, playerSideWhite = true, calculatingPos = false, unpromoting = false;
	float pieceSize = 320.0f, threshold = 1.0f;
	float pieceScale = (std::min(ScaleX, ScaleY) * 128.0f) / (float)pieceSize;
	float boardOffset = (windowSizeX / 2.0f) - ((boardTexture.getSize().x * board.getScale().x) / 2.0f);
	float boardMultiplier = ((boardTexture.getSize().x * board.getScale().x) / 8);
	sf::Image pieceSpriteSheetAlpha, pieceSpriteSheetCburnett, pieceSpriteSheetMerida, pieceSpriteSheetDisguised;
	pieceSpriteSheetAlpha.loadFromFile("assets/piece/pieces_matrix_alpha.png");
	pieceSpriteSheetCburnett.loadFromFile("assets/piece/pieces_matrix_cburnett.png");
	pieceSpriteSheetMerida.loadFromFile("assets/piece/pieces_matrix_merida.png");
	pieceSpriteSheetDisguised.loadFromFile("assets/piece/pieces_matrix_disguised.png");
	sf::Image pieceStyle = pieceSpriteSheetCburnett;
	sf::Texture blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT;
	sf::Texture whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT;
	textureVector pieceTextures{ blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT,
	whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT };

	// Extras
	sf::Color promotionSquareColor{ 255, 255, 255 }, promotionSquareSelectedColor{ 255, 30, 0 };
	sf::Texture selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture, selectedPieceTexture, promoteBackgroundTexture, promoteBackgroundSelectionTexture;
	selectionTexture.loadFromFile("assets/piece/move_gradient.png");
	captureTexture.loadFromFile("assets/piece/capture_gradient.png");
	checkTexture.loadFromFile("assets/piece/check.png");
	lastMoveTexture.loadFromFile("assets/piece/last_move.png");
	selectionHoverTexture.loadFromFile("assets/piece/selection_hover.png");
	selectedPieceTexture.loadFromFile("assets/piece/piece_selection_hover.png");
	promoteBackgroundTexture.loadFromFile("assets/piece/promotion_square.png");
	promoteBackgroundSelectionTexture.loadFromFile("assets/piece/promotion_square_select.png");
	selectionTexture.setSmooth(true);
	captureTexture.setSmooth(true);
	checkTexture.setSmooth(true);
	lastMoveTexture.setSmooth(true);
	selectionHoverTexture.setSmooth(true);
	selectedPieceTexture.setSmooth(true);
	promoteBackgroundTexture.setSmooth(true);
	std::vector <sf::Vector2f> initialPromoteScales{ {pieceScale * 0.82f, pieceScale * 0.82f}, {pieceScale * 0.775f, pieceScale * 0.775f}, {pieceScale * 0.8f, pieceScale * 0.8f}, {pieceScale * 0.785f, pieceScale * 0.785f}, {pieceScale * 0.785f, pieceScale * 0.785f} };
	// Selection, Capture, Check, Last Move, Selection Hover
	textureVector extraTextures{ selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture };

	// Vars
	Variant variant = Atomic;
	float xAccl = 1.0f, yAccl = 1.0f;
	bool mouseSelecting = false, mouseClicked = false, pieceMoving = false, check = false, promoting = false, animationFinished = false,
		pieceSelectionLock = false, standardPosition = false, checksEnabled = true, castlingEnabled = true, kingPromotionEnabled = false,
		endgamePosition = false, Chess960Enabled = false, StockfishEnabled = true;
	int wKRook = -1, wQRook = -1, bKRook = -1, bQRook = -1;
	std::vector<std::pair<std::array<std::array<int, 8>, 8>, bool>> allPositionsPlayed;
	std::shared_ptr<Piece> selectedPiece = nullptr, capturePiece = nullptr, castleKing = nullptr, castleRook = nullptr, promotePiece = nullptr;
	std::shared_ptr<Pawn> enPassantPiece = nullptr;
	std::vector<sf::Sprite> promoteBackgrounds, promoteSprites;
	pieceVector::iterator it;
	sf::Vector2f selectedPos{ 0.0f, 0.0f };
	sf::Vector2i mousePos{ sf::Mouse::getPosition() };
	sf::Sprite selectedPieceBackground{ selectedPieceTexture };
	selectedPieceBackground.setOrigin(selectedPieceBackground.getGlobalBounds().getCenter());
	selectedPieceBackground.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
	sf::Sprite checkSprite{ checkTexture };
	checkSprite.setOrigin(checkSprite.getGlobalBounds().getCenter());
	checkSprite.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
	sf::Sprite lastMoveStart{ lastMoveTexture }, lastMoveDest{ lastMoveTexture };
	lastMoveStart.setOrigin(lastMoveStart.getGlobalBounds().getCenter());
	lastMoveStart.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
	lastMoveDest.setOrigin(lastMoveDest.getGlobalBounds().getCenter());
	lastMoveDest.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
	lastMoveStart.setPosition({ -1000, -1000 });
	lastMoveDest.setPosition({ -1000, -1000 });

	// Audio
	sf::SoundBuffer moveBuffer, captureBuffer, checkBuffer, gameEndBuffer, explosionBuffer;
	moveBuffer.loadFromFile("assets/sound/Move.mp3");
	captureBuffer.loadFromFile("assets/sound/Capture.mp3");
	checkBuffer.loadFromFile("assets/sound/Check.mp3");
	gameEndBuffer.loadFromFile("assets/sound/End.mp3");
	explosionBuffer.loadFromFile("assets/sound/Explosion.mp3");
	sf::Sound moveSound(moveBuffer);
	sf::Sound captureSound(captureBuffer);
	sf::Sound gameEndSound(gameEndBuffer);
	moveSound.setVolume(10);
	captureSound.setVolume(15);
	gameEndSound.setVolume(25);

	// Setup
	// Add antichess endgames and more normal endgames
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
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	if (Chess960Enabled) {
		int id = std::rand() % 960;
		std::cout << "Loading Chess960 Position #" << id << std::endl;
		std::ifstream FRCReadFile("assets/fen/chess960.txt");
		std::string fen;
		for (int i = 0; i <= id; i++) {
			std::getline(FRCReadFile, fen);
		}
		FRCReadFile.close();
		if (!fen.empty()) {
			startingFEN = fen;
		}
	}
	else if (endgamePosition) {
		int id = std::rand() % 139;
		std::ifstream EndgameReadFile("assets/fen/endgame.txt");
		std::string fen;
		for (int i = 0; i <= id; i++) {
			std::getline(EndgameReadFile, fen);
		}
		EndgameReadFile.close();
		if (!fen.empty()) {
			startingFEN = fen;
		}
	}
	std::string moves = "", currentMove = "";
	Main::loadPieceSet(pieceStyle, pieceTextures, pieceSize);
	pieceVector pieceList = Main::generatePositionFromFENID(startingFEN,
		pieceTextures, pieceScale, boardOffset, boardMultiplier, whiteToPlay, halfMoves, fullMoves, enPassantPiece, checkSprite, check, extraTextures, true,
		standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, Chess960Enabled, variant);

	// Stockfish
	bp::ipstream is;
	bp::opstream os;

	bp::child c("assets/other/fairy-stockfish-largeboard_x86-64.exe", bp::std_in < os, bp::std_out > is);
	Main::startStockfish(startingFEN, c, os, is, variant, Chess960Enabled);

	window.setIcon(selectedIcon.getSize(), selectedIcon.getPixelsPtr());
	// ==== MAIN ====
	while (window.isOpen())
	{
		mouseSelecting = false;
		mouseClicked = false;
		mousePos = sf::Mouse::getPosition(window);
		selectedPos = { std::ceil(((float)mousePos.x - boardOffset) / boardMultiplier),  9 - std::ceil((float)mousePos.y / boardMultiplier) };
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButtonPressed->button == sf::Mouse::Button::Left)
				{
					mouseClicked = true;
					if (StockfishEnabled) {
						if (whiteToPlay != playerSideWhite) {
							break;
						}
					}
					std::shared_ptr<Piece> p = Main::getPieceFromPosition(sf::Vector2i(selectedPos), pieceList);
					if (p != nullptr && promotePiece == nullptr && !pieceMoving && animationFinished) {
						// Side to Play == Color
						if (whiteToPlay == (p->color == PColor::White)) {
							if (selectedPiece != nullptr && selectedPiece->name == "King") {
								std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
								if (!std::any_of(king->captureCastlePositions.cbegin(), king->captureCastlePositions.cend(), [selectedPos](sf::Vector2i pos) { return pos == (sf::Vector2i)selectedPos; })) {
									selectedPiece = p;
								}
							}
							else {
								selectedPiece = p;
							}
						}
					}
				}
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::N) {
					if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& p) { return p->animationTarget.has_value() || p->targetPos.has_value(); })) {
						currentMove = "";
						moves = "";
						wKRook = -1; wQRook = -1; bKRook = -1; bQRook = -1;
						check = false;
						lastMoveStart.setPosition({ -1000, -1000 });
						lastMoveDest.setPosition({ -1000, -1000 });
						selectedPiece.reset();
						capturePiece.reset();
						enPassantPiece.reset();
						pieceList.clear();
						if (Chess960Enabled) {
							int id = std::rand() % 960;
							std::ifstream FRCReadFile("assets/fen/chess960.txt");
							std::string fen;
							for (int i = 0; i <= id; i++) {
								std::getline(FRCReadFile, fen);
							}
							FRCReadFile.close();
							if (!fen.empty()) {
								startingFEN = fen;
							}
						}
						else if (endgamePosition) {
							int id = std::rand() % 139;
							std::ifstream EndgameReadFile("assets/fen/endgame.txt");
							std::string fen;
							for (int i = 0; i <= id; i++) {
								std::getline(EndgameReadFile, fen);
							}
							EndgameReadFile.close();
							if (!fen.empty()) {
								startingFEN = fen;
							}
						}
						else {
							startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1";
						}
						pieceList = Main::generatePositionFromFENID(startingFEN,
							pieceTextures, pieceScale, boardOffset, boardMultiplier, whiteToPlay, halfMoves, fullMoves, enPassantPiece, checkSprite, check, extraTextures, true,
							standardPosition, wKRook, wQRook, bKRook, bQRook, checksEnabled, castlingEnabled, Chess960Enabled, variant);
						Main::startStockfish(startingFEN, c, os, is, variant, Chess960Enabled);
					}
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {

				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Q) {
					window.close();
				}
			}
			else if (event->is<sf::Event::FocusLost>()) {
				Main::block_until_gained_focus(window);
			}
		}

		if (StockfishEnabled && whiteToPlay != playerSideWhite && animationFinished) {
			if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& p) { return p->animationTarget.has_value() || p->targetPos.has_value(); })) {
				std::string t = Main::getBestMove(startingFEN, moves, c, os, is);
				moves += " ";
				moves += t;
				Main::playMove(t, pieceList, capturePiece, castleKing, castleRook, promotePiece, enPassantPiece, lastMoveStart, lastMoveDest, moveSound, captureSound, boardOffset, boardMultiplier, check, wKRook, wQRook, bKRook, bQRook, pieceScale, pieceTextures);
			}
		}

		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && pieceSelectionLock) {
			pieceSelectionLock = false;
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->selectionSquares) {
					if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
						if (check) { check = false; }
						// En Passant
						if (enPassantPiece != nullptr) {
							enPassantPiece->enPassantTarget = false;
							enPassantPiece.reset();
						}
						if (selectedPiece->name == "Pawn") {
							if (selectedPiece->color == PColor::White) {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 8) {
									promotePiece = selectedPiece;
									promotePiece->setTarget(sprite.getPosition());
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y + 2) {
									std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
									pawn->enPassantTarget = true;
									enPassantPiece = pawn;
								}
							}
							else {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 1) {
									promotePiece = selectedPiece;
									promotePiece->setTarget(sprite.getPosition());
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y - 2) {
									std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
									pawn->enPassantTarget = true;
									enPassantPiece = pawn;
								}
							}
						}
						moveSound.play();
						if (promotePiece == nullptr) {
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
							currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
							moves += " " + currentMove;
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							selectedPiece->setGlobalPosition(sprite.getPosition());
							Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->captureSquares) {
					if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
						if (check) { check = false; }
						if (enPassantPiece != nullptr) {
							enPassantPiece->enPassantTarget = false;
							enPassantPiece.reset();
						}
						if (selectedPiece->name == "Pawn") {
							if (selectedPiece->color == PColor::White) {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 8) {
									promotePiece = selectedPiece;
									promotePiece->setTarget(sprite.getPosition());
								}
							}
							else {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 1) {
									promotePiece = selectedPiece;
									promotePiece->setTarget(sprite.getPosition());
								}
							}
						}

						for (auto& piece : pieceList) {
							if (piece->getGlobalPosition() == sprite.getPosition()) {
								capturePiece = piece;
								piece->setGhostSpriteVisible(true, false);
							}
						};
						captureSound.play();
						if (promotePiece == nullptr) {
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
							currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
							moves += " " + currentMove;
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							selectedPiece->setGlobalPosition(sprite.getPosition());
							Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
					for (auto& sprite : king->castleSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
							currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
							moves += " ";
							moves += currentMove;
							moveSound.play();
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							if (check) { check = false; }
							// En Passant
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 3) {
								std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, pieceList);
								if (rook != nullptr) {
									rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									king->setGlobalPosition(sprite.getPosition());
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
								}
							}
							else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 7) {
								std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, pieceList);
								if (rook != nullptr) {
									rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									king->setGlobalPosition(sprite.getPosition());
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
								}
							}
						}
					}

					for (auto& sprite : king->castleCaptureSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							moveSound.play();
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							if (check) { check = false; }
							// En Passant
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (king->isWhite()) {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == wKRook) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ wKRook, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										if (Chess960Enabled) {
											currentMove = "";
											currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
											currentMove += Main::convertPositiontoNotation({ wKRook, king->getLocalPosition().y });
										}
										else {
											if (std::abs(7 - king->getLocalPosition().x) == 1) {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ wKRook, king->getLocalPosition().y });
											}
											else {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
											}
										}
										moves += " ";
										moves += currentMove;
										std::cout << "Moves " << moves << std::endl;
										rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setGlobalPosition(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == wQRook) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ wQRook, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										if (Chess960Enabled) {
											currentMove = "";
											currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
											currentMove += Main::convertPositiontoNotation({ wQRook, king->getLocalPosition().y });
										}
										else {
											if (std::abs(3 - king->getLocalPosition().x) == 1) {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ wQRook, king->getLocalPosition().y });
											}
											else {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
											}
										}
										moves += " ";
										moves += currentMove;
										rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setGlobalPosition(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
									}
								}
							}
							else {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == bKRook) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ bKRook, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										if (Chess960Enabled) {
											currentMove = "";
											currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
											currentMove += Main::convertPositiontoNotation({ bKRook, king->getLocalPosition().y });
										}
										else {
											if (std::abs(7 - king->getLocalPosition().x) == 1) {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ bKRook, king->getLocalPosition().y });
											}
											else {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
											}
										}
										moves += " ";
										moves += currentMove;
										rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setGlobalPosition(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == bQRook) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ bQRook, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										if (Chess960Enabled) {
											currentMove = "";
											currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
											currentMove += Main::convertPositiontoNotation({ bQRook, king->getLocalPosition().y });
										}
										else {
											if (std::abs(3 - king->getLocalPosition().x) == 1) {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ bQRook, king->getLocalPosition().y });
											}
											else {
												currentMove = "";
												currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
												currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
											}
										}
										moves += " ";
										moves += currentMove;
										rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setGlobalPosition(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
									}
								}
							}
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
					for (auto& sprite : pawn->enPassantSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							captureSound.play();
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							if (check) { check = false; }
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							for (auto& piece : pieceList) {
								if (pawn->isWhite()) {
									if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier) - sf::Vector2i(0, 1))) {
										capturePiece = piece;
									}
								}
								else {
									if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier) + sf::Vector2i(0, 1))) {
										capturePiece = piece;
									}
								}
							};
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
							currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
							moves += " " + currentMove;
							selectedPiece->setGlobalPosition(sprite.getPosition());
							Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (!selectedPieceBackground.getGlobalBounds().contains(selectedPiece->getGlobalPosition())) {
					selectedPiece->setGlobalPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
					selectedPiece.reset();
				}
				else {
					selectedPiece->setGlobalPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
				}
			}
		}
		else if (pieceSelectionLock) {
			selectedPiece->setGlobalPosition(sf::Vector2f(mousePos));
		}

		// Board Animation
		animationFinished = true;
		for (auto& piece : pieceList) {
			if (piece->animationTarget.has_value()) {
				if (piece->getGlobalPosition() != piece->animationTarget.value()) {
					animationFinished = false;
					piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->animationTarget.value(), 0.22f));
				}
				else {
					piece->animationTarget = {};
				}
			}
		}

		// Movement
		if (promotePiece != nullptr) {
			if (unpromoting) {
				if (promotePiece->targetPos.has_value()) {
					if (promotePiece->getGlobalPosition() == promotePiece->targetPos.value()) {
						promotePiece->setLocalPosition(Main::getLocalPosition(promotePiece->getGlobalPosition(), boardOffset, boardMultiplier));
						promotePiece->setTarget({});
						promotePiece.reset();
						unpromoting = false;
					}
					else {
						promotePiece->setGlobalPosition(Main::Interpolate(promotePiece->getGlobalPosition(), promotePiece->targetPos.value(), 0.25f));
					}
				}
			}
			else {
				if (promotePiece->targetPos.has_value()) {
					if (promotePiece->getGlobalPosition() == promotePiece->targetPos.value()) {
						promoting = true;
						int off = 0;
						if (kingPromotionEnabled) { off = 1; }
						int x = Main::getLocalPosition(promotePiece->getGlobalPosition(), boardOffset, boardMultiplier).x;
						if (promotePiece->isWhite()) {
							for (int y = 8; y >= 5 - off; y--) {
								sf::Sprite bg{ promoteBackgroundTexture };
								bg.setOrigin(bg.getGlobalBounds().getCenter());
								bg.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
								bg.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
								bg.setColor(promotionSquareColor);
								promoteBackgrounds.push_back(bg);
								// Bishop, King, Knight, Pawn, Queen, Rook
								// Black --> White
								switch (y) {
								case 8:
								{
									sf::Sprite piece{ pieceTextures.at(10) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.82f, pieceScale * 0.82f));
									promoteSprites.push_back(piece);
									break;
								}
								case 7:
								{
									sf::Sprite piece{ pieceTextures.at(8) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.775f, pieceScale * 0.775f));
									promoteSprites.push_back(piece);
									break;
								}
								case 6:
								{
									sf::Sprite piece{ pieceTextures.at(11) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.775f, pieceScale * 0.775f));
									promoteSprites.push_back(piece);
									break;
								}
								case 5:
								{
									sf::Sprite piece{ pieceTextures.at(6) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.785f, pieceScale * 0.785f));
									promoteSprites.push_back(piece);
									break;
								}
								case 4:
								{
									sf::Sprite piece{ pieceTextures.at(7) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.785f, pieceScale * 0.785f));
									promoteSprites.push_back(piece);
									break;
								}
								}
							}
							promotePiece->setTarget({});
						}
						else {
							for (int y = 1; y <= 4 + off; y++) {
								sf::Sprite bg{ promoteBackgroundTexture };
								bg.setOrigin(bg.getGlobalBounds().getCenter());
								bg.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });
								bg.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
								bg.setColor(promotionSquareColor);
								promoteBackgrounds.push_back(bg);
								// Bishop, King, Knight, Pawn, Queen, Rook
								// Black --> White
								switch (y) {
								case 1:
								{
									sf::Sprite piece{ pieceTextures.at(4) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.82f, pieceScale * 0.82f));
									promoteSprites.push_back(piece);
									break;
								}
								case 2:
								{
									sf::Sprite piece{ pieceTextures.at(2) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.775f, pieceScale * 0.775f));
									promoteSprites.push_back(piece);
									break;
								}
								case 3:
								{
									sf::Sprite piece{ pieceTextures.at(5) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.775f, pieceScale * 0.775f));
									promoteSprites.push_back(piece);
									break;
								}
								case 4:
								{
									sf::Sprite piece{ pieceTextures.at(0) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.785f, pieceScale * 0.785f));
									promoteSprites.push_back(piece);
									break;
								}
								case 5:
								{
									sf::Sprite piece{ pieceTextures.at(1) };
									piece.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
									piece.setOrigin(piece.getLocalBounds().getCenter());
									piece.setScale(sf::Vector2f(pieceScale * 0.785f, pieceScale * 0.785f));
									promoteSprites.push_back(piece);
									break;
								}
								}
							}
							promotePiece->setTarget({});
						}
					}
					else {
						if (capturePiece != nullptr) {
							if (std::fmax(std::abs(promotePiece->getGlobalPosition().x - promotePiece->targetPos.value().x), std::abs(promotePiece->getGlobalPosition().y - promotePiece->targetPos.value().y)) < 5.0f) {
								capturePiece->setGhostSpriteVisible(false, false);
							}
						}
						promotePiece->setGlobalPosition(Main::Interpolate(promotePiece->getGlobalPosition(), promotePiece->targetPos.value(), 0.25f));
					}
				}
			}
		}
		else if (castleKing != nullptr && castleRook != nullptr) {
			if (castleKing->targetPos.value() != castleKing->getGlobalPosition() || castleRook->targetPos.value() != castleRook->getGlobalPosition()) {
				castleKing->setGlobalPosition(Main::Interpolate(castleKing->getGlobalPosition(), castleKing->targetPos.value(), 0.25f));
				castleRook->setGlobalPosition(Main::Interpolate(castleRook->getGlobalPosition(), castleRook->targetPos.value(), 0.25f));
			}
			else {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(castleKing);
				Main::postCastle(king, castleRook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
				castleKing.reset();
				castleRook.reset();
			}
		}
		else {
			for (auto it2 = pieceList.begin(); it2 != pieceList.end(); ) {
				std::shared_ptr<Piece> piece = *it2;
				if (piece != nullptr && piece->targetPos.has_value()) {
					if (piece->getGlobalPosition() == piece->targetPos.value()) {
						calculatingPos = true;
						if (Main::postMove(piece, pieceList, it2, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite, extraTextures,
							allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant) == 1) {
							break;
						}
						calculatingPos = false;
					}
					else {
						pieceMoving = true;
						if (piece->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							if (pawn->capturingEnPassant) {
								piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, 15.0f));
							}
							else {
								if (capturePiece != nullptr) {
									piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, threshold));
									if (std::fmax(std::abs(piece->getGlobalPosition().x - piece->targetPos.value().x), std::abs(piece->getGlobalPosition().y - piece->targetPos.value().y)) < 5.0f) {
										capturePiece->setGhostSpriteVisible(false, false);
									}
								}
								else {
									piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f));
								}
							}
						}
						else {
							if (capturePiece != nullptr) {
								piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, threshold));
								if (std::fmax(std::abs(piece->getGlobalPosition().x - piece->targetPos.value().x), std::abs(piece->getGlobalPosition().y - piece->targetPos.value().y)) < 5.0f) {
									capturePiece->setGhostSpriteVisible(false, false);
								}
							}
							else {
								piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f));
							}
						}
					}
				}
				++it2;
			}
		}


		// Draw
		window.clear();
		window.draw(board);
		window.draw(lastMoveStart);
		window.draw(lastMoveDest);

		// Selection Sprites
		if (!pieceMoving && selectedPiece != nullptr && !promoting) {
			for (auto& sprite : selectedPiece->selectionSquares) {
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					sprite.setTexture(extraTextures.at(4));
				}
				else { sprite.setTexture(extraTextures.at(0)); }
				window.draw(sprite);
			}
			for (auto& sprite : selectedPiece->captureSquares) {
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					sprite.setTexture(extraTextures.at(4));
				}
				else { sprite.setTexture(extraTextures.at(1)); }
				window.draw(sprite);
			}
			if (selectedPiece->name == "King") {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
				for (auto& sprite : king->castleSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						mouseSelecting = true;
						sprite.setTexture(extraTextures.at(4));
					}
					else { sprite.setTexture(extraTextures.at(0)); }
					window.draw(sprite);
				}

				for (auto& sprite : king->castleCaptureSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						mouseSelecting = true;
						sprite.setTexture(extraTextures.at(4));
					}
					else { sprite.setTexture(extraTextures.at(1)); }
					window.draw(sprite);
				}
			}
			if (selectedPiece->name == "Pawn") {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
				for (auto& sprite : pawn->enPassantSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						mouseSelecting = true;
						sprite.setTexture(extraTextures.at(4));
					}
					else { sprite.setTexture(extraTextures.at(0)); }
					window.draw(sprite);
				}
			}
		}
		// Click Detection
		if (!pieceMoving && mouseClicked && animationFinished) {
			if (selectedPiece != nullptr) {
				if (selectedPiece->contains(mousePos)) {
					pieceSelectionLock = true;
				}
			}
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->selectionSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
							if (check) { check = false; }
							// En Passant
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (selectedPiece->name == "Pawn") {
								if (selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 8) {
										promotePiece = selectedPiece;
										promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y + 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
										pawn->enPassantTarget = true;
										enPassantPiece = pawn;
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 1) {
										promotePiece = selectedPiece;
										promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y - 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
										pawn->enPassantTarget = true;
										enPassantPiece = pawn;
									}
								}
							}
							moveSound.play();
							if (promotePiece == nullptr) {
								currentMove = "";
								currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
								currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
								moves += " " + currentMove;
								lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
								lastMoveDest.setPosition(sprite.getPosition());
							}
							selectedPiece->setTarget(sprite.getPosition());
							selectedPiece.reset();
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->captureSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
							// En Passant
							if (check) { check = false; }
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (selectedPiece->name == "Pawn") {
								if (selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 8) {
										promotePiece = selectedPiece;
										promotePiece->setTarget(sprite.getPosition());
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == 1) {
										promotePiece = selectedPiece;
										promotePiece->setTarget(sprite.getPosition());
									}
								}
							}
							captureSound.play();
							if (promotePiece == nullptr) {
								currentMove = "";
								currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
								currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
								moves += " ";
								moves += currentMove;
								lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
								lastMoveDest.setPosition(sprite.getPosition());
							}
							for (auto& piece : pieceList) {
								if (piece->getGlobalPosition() == sprite.getPosition()) {
									capturePiece = piece;
									piece->setGhostSpriteVisible(true, false);
								}
							};
							selectedPiece->setTarget(sprite.getPosition());
							selectedPiece.reset();
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
					for (auto& sprite : king->castleSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
								currentMove = "";
								currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
								currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier));
								moves += " ";
								moves += currentMove;
								moveSound.play();
								lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
								lastMoveDest.setPosition(sprite.getPosition());
								if (check) { check = false; }
								// En Passant
								if (enPassantPiece != nullptr) {
									enPassantPiece->enPassantTarget = false;
									enPassantPiece.reset();
								}
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 3) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										castleKing = king;
										castleRook = rook;
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 7) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										castleKing = king;
										castleRook = rook;
									}
								}
								selectedPiece.reset();
							}
						}
					}

					for (auto& sprite : king->castleCaptureSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
									moveSound.play();
									lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
									lastMoveDest.setPosition(sprite.getPosition());
									if (check) { check = false; }
									// En Passant
									if (enPassantPiece != nullptr) {
										enPassantPiece->enPassantTarget = false;
										enPassantPiece.reset();
									}
									if (king->isWhite()) {
										if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == wKRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ wKRook, king->getLocalPosition().y }, pieceList);
											if (rook != nullptr) {
												if (Chess960Enabled) {
													currentMove = "";
													currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
													currentMove += Main::convertPositiontoNotation({ wKRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(7 - king->getLocalPosition().x) == 1) {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ wKRook, king->getLocalPosition().y });
													}
													else {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
													}
												}
												moves += " ";
												moves += currentMove;
												rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												castleKing = king;
												castleRook = rook;
											}
										}
										else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == wQRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ wQRook, king->getLocalPosition().y }, pieceList);
											if (rook != nullptr) {
												if (Chess960Enabled) {
													currentMove = "";
													currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
													currentMove += Main::convertPositiontoNotation({ wQRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(3 - king->getLocalPosition().x) == 1) {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ wQRook, king->getLocalPosition().y });
													}
													else {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
													}
												}
												moves += " ";
												moves += currentMove;
												rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												castleKing = king;
												castleRook = rook;
											}
										}
									}
									else {
										if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == bKRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ bKRook, king->getLocalPosition().y }, pieceList);
											if (rook != nullptr) {
												if (Chess960Enabled) {
													currentMove = "";
													currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
													currentMove += Main::convertPositiontoNotation({ bKRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(7 - king->getLocalPosition().x) == 1) {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ bKRook, king->getLocalPosition().y });
													}
													else {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
													}
												}
												moves += " ";
												moves += currentMove;
												rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												castleKing = king;
												castleRook = rook;
											}
										}
										else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == bQRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ bQRook, king->getLocalPosition().y }, pieceList);
											if (rook != nullptr) {
												if (Chess960Enabled) {
													currentMove = "";
													currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
													currentMove += Main::convertPositiontoNotation({ bQRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(3 - king->getLocalPosition().x) == 1) {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ bQRook, king->getLocalPosition().y });
													}
													else {
														currentMove = "";
														currentMove += Main::convertPositiontoNotation(selectedPiece->getLocalPosition());
														currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
													}
												}
												moves += " ";
												moves += currentMove;
												rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
												castleKing = king;
												castleRook = rook;
											}
										}
									}
									selectedPiece.reset();
								}
							}
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
					for (auto& sprite : pawn->enPassantSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
								captureSound.play();
								lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
								lastMoveDest.setPosition(sprite.getPosition());
								if (check) { check = false; }
								// En Passant
								if (enPassantPiece != nullptr) {
									enPassantPiece->enPassantTarget = false;
									enPassantPiece.reset();
								}
								for (auto& piece : pieceList) {
									if (pawn->isWhite()) {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier) - sf::Vector2i(0, 1))) {
											capturePiece = piece;
										}
									}
									else {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier) + sf::Vector2i(0, 1))) {
											capturePiece = piece;
										}
									}
								};
								pawn->capturingEnPassant = true;
								selectedPiece->setTarget(sprite.getPosition());
								selectedPiece.reset();
							}
						}
					}
				}
			}
			if (Main::getPieceFromPosition(sf::Vector2i(selectedPos), pieceList) == nullptr) {
				selectedPiece.reset();
			}
		}

		if (selectedPiece != nullptr) {
			selectedPieceBackground.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
			window.draw(selectedPieceBackground);
			if (pieceSelectionLock) {
				selectedPiece->setGhostSpriteVisible(true, true);
			}
			else {
				selectedPiece->setGhostSpriteVisible(false, true);
			}
		}
		if (check) { window.draw(checkSprite); }

		for (auto& piece : pieceList) {
			piece->drawGhostSprite(window);
		}
		for (auto& piece : pieceList) {
			if (piece->contains(mousePos)) {
				mouseSelecting = true;
			}
			if (selectedPiece != nullptr) {
				if (piece != selectedPiece) {
					piece->draw(window);
				}
			}
			else {
				piece->draw(window);
			}
		}
		if (selectedPiece != nullptr) {
			selectedPiece->draw(window);
		}
		if (promotePiece != nullptr && !unpromoting) {
			window.draw(promotionOverlay);
			int offset = 0;
			if (promotePiece->isBlack()) {
				offset = -6;
			}
			for (int i = 0; i < promoteBackgrounds.size(); i++) {
				sf::Sprite sprite = promoteBackgrounds.at(i);
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					sprite.setTexture(promoteBackgroundSelectionTexture);
					sprite.setColor(promotionSquareSelectedColor);
					window.draw(sprite);
					mouseSelecting = true;
					promoteSprites.at(i).setScale(Main::Interpolate(promoteSprites.at(i).getScale(), initialPromoteScales.at(i) + sf::Vector2f{ 0.035f, 0.035f }, 0.5f, 0.01f));
					window.draw(promoteSprites.at(i));
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						sf::Vector2f globalPos = Main::getGlobalPosition(promotePiece->getLocalPosition(), boardOffset, boardMultiplier);
						sf::Vector2i localPos = Main::getLocalPosition(promotePiece->getGlobalPosition(), boardOffset, boardMultiplier);
						sf::Vector2i localPos2 = promotePiece->getLocalPosition();
						lastMoveStart.setPosition(globalPos);
						lastMoveDest.setPosition(promotePiece->getGlobalPosition());
						for (int j = 0; j < pieceList.size(); j++) {
							if (pieceList.at(j) != nullptr && pieceList.at(j) == promotePiece) {
								pieceList.erase(pieceList.begin() + j);
								break;
							}
						}
						PColor color = promotePiece->color;
						promotePiece.reset();
						// Bishop, King, Knight, Pawn, Queen, Rook
						// Black --> White
						switch (i) {
						case 0:
						{
							std::shared_ptr<Queen> queen = std::make_shared<Queen>(localPos.x, localPos.y, pieceScale, boardOffset, boardMultiplier, color, pieceTextures.at(10 + offset), false);
							pieceList.push_back(queen);
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(localPos2);
							currentMove += Main::convertPositiontoNotation(localPos);
							currentMove += 'q';
							moves += " ";
							moves += currentMove;
							Main::postMove(queen, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
							break;
						}
						case 1:
						{
							std::shared_ptr<Knight> knight = std::make_shared<Knight>(localPos.x, localPos.y, pieceScale, boardOffset, boardMultiplier, color, pieceTextures.at(8 + offset), false);
							pieceList.push_back(knight);
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(localPos2);
							currentMove += Main::convertPositiontoNotation(localPos);
							currentMove += 'n';
							moves += " ";
							moves += currentMove;
							Main::postMove(knight, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
							break;
						}
						case 2:
						{
							std::shared_ptr<Rook> rook = std::make_shared<Rook>(localPos.x, localPos.y, pieceScale, boardOffset, boardMultiplier, color, pieceTextures.at(11 + offset), false);
							pieceList.push_back(rook);
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(localPos2);
							currentMove += Main::convertPositiontoNotation(localPos);
							currentMove += 'r';
							moves += " ";
							moves += currentMove;
							Main::postMove(rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
							break;
						}
						case 3:
						{
							std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(localPos.x, localPos.y, pieceScale, boardOffset, boardMultiplier, color, pieceTextures.at(6 + offset), false);
							pieceList.push_back(bishop);
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(localPos2);
							currentMove += Main::convertPositiontoNotation(localPos);
							currentMove += 'b';
							moves += " ";
							moves += currentMove;
							Main::postMove(bishop, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
							break;
						}
						case 4:
						{
							std::shared_ptr<King> king = std::make_shared<King>(localPos.x, localPos.y, pieceScale, boardOffset, boardMultiplier, color, pieceTextures.at(7 + offset), false);
							pieceList.push_back(king);
							currentMove = "";
							currentMove += Main::convertPositiontoNotation(localPos2);
							currentMove += Main::convertPositiontoNotation(localPos);
							currentMove += 'k';
							moves += " ";
							moves += currentMove;
							Main::postMove(king, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, fullMoves, halfMoves, checkSprite,
								extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest, wKRook, wQRook, bKRook, bQRook, standardPosition, checksEnabled, castlingEnabled, variant);
							break;
						}
						}
						promoteBackgrounds.clear();
						promoteSprites.clear();
						promoting = false;
						window.clear();
						window.draw(board);
						window.draw(lastMoveStart);
						window.draw(lastMoveDest);
						for (auto& p : pieceList) {
							p->draw(window);
						}
						window.display();
					}
				}
				else {
					sprite.setTexture(promoteBackgroundTexture);
					sprite.setColor(promotionSquareColor);
					window.draw(sprite);
					promoteSprites.at(i).setScale(Main::Interpolate(promoteSprites.at(i).getScale(), initialPromoteScales.at(i), 0.5f, 0.01f));
					window.draw(promoteSprites.at(i));
				}
			}
		}
		if (promoting && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			unpromoting = true;
			promoting = false;
			promoteBackgrounds.clear();
			promoteSprites.clear();
			if (capturePiece != nullptr) {
				capturePiece->setGhostSpriteVisible(false, true);
				capturePiece.reset();
			}
			promotePiece->setTarget(Main::getGlobalPosition(promotePiece->getLocalPosition(), boardOffset, boardMultiplier));
		}
		if (animationFinished) {
			if (mouseSelecting) {
				window.setMouseCursor(handCursor);
			}
			else { window.setMouseCursor(arrowCursor); }
		}
		else {
			window.setMouseCursor(hourGlassCursor);
		}
		window.display();
	}
}