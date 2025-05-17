#include "Main.h"

using namespace Chess;
int main()
{
	srand(time(NULL));
	std::cout << std::boolalpha;
	sf::Image hourglassImage;
	hourglassImage.loadFromFile("assets/other/hourglass.png");
	const auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
	const auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
	const auto hourGlassCursor = sf::Cursor::createFromPixels(hourglassImage.getPixelsPtr(), hourglassImage.getSize(), sf::Vector2u{}).value();
	sf::Font textFont;
	textFont.openFromFile("assets/font/tahomabd.ttf");
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktopMode, "SF Chess", sf::Style::None);
	float windowSizeX = window.getSize().x;
	float windowSizeY = window.getSize().y;
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);

	// Icons
	sf::Image selectedIcon, unicornWhiteIcon, unicornBlackIcon, zebraWhiteIcon, zebraBlackIcon, centaurWhiteIcon, centaurBlackIcon, checkersWhite, checkersBlack;
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

	// Pieces
	sf::Image pieceSpriteSheetAlpha, pieceSpriteSheetCburnett, pieceSpriteSheetMerida, pieceSpriteSheetDisguised;
	pieceSpriteSheetAlpha.loadFromFile("assets/piece/pieces_matrix_alpha.png");
	pieceSpriteSheetCburnett.loadFromFile("assets/piece/pieces_matrix_cburnett.png");
	pieceSpriteSheetMerida.loadFromFile("assets/piece/pieces_matrix_merida.png");
	pieceSpriteSheetDisguised.loadFromFile("assets/piece/pieces_matrix_disguised.png");
	sf::Image pieceStyle = pieceSpriteSheetAlpha;
	std::vector<sf::Image> pieceSpriteSheets{ pieceSpriteSheetAlpha, pieceSpriteSheetCburnett, pieceSpriteSheetMerida, pieceSpriteSheetDisguised };
	sf::Texture blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT;
	sf::Texture whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT;
	textureVector pieceTextures{ blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT,
	whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT };
	Main::loadPieceSet(pieceSpriteSheets.at(0), pieceTextures, 320);

	// Extras
	sf::Color promotionSquareColor{ 255, 255, 255 }, promotionSquareSelectedColor{ 255, 30, 0 };
	sf::Texture selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture, selectedPieceTexture, promoteBackgroundTexture, promoteBackgroundSelectionTexture, 
		kothBackgroundTexture, kothShadowTexture, rankBackgroundTexture, rankShadowTopTexture, checkCount, dropBackgroundTexture, dropTextBackground;
	selectionTexture.loadFromFile("assets/piece/move_gradient.png");
	captureTexture.loadFromFile("assets/piece/capture_gradient.png");
	checkTexture.loadFromFile("assets/piece/check.png");
	lastMoveTexture.loadFromFile("assets/piece/last_move.png");
	selectionHoverTexture.loadFromFile("assets/piece/selection_hover.png");
	selectedPieceTexture.loadFromFile("assets/piece/piece_selection_hover.png");
	promoteBackgroundTexture.loadFromFile("assets/piece/promotion_square.png");
	promoteBackgroundSelectionTexture.loadFromFile("assets/piece/promotion_square_select.png");
	kothBackgroundTexture.loadFromFile("assets/board/koth_background.png");
	kothShadowTexture.loadFromFile("assets/board/koth_shadow.png");
	rankBackgroundTexture.loadFromFile("assets/board/rank_background.png");
	rankShadowTopTexture.loadFromFile("assets/board/rank_shadow_top.png");
	checkCount.loadFromFile("assets/board/check_color.png");
	dropBackgroundTexture.loadFromFile("assets/board/rectangle_gradient.png");
	dropTextBackground.loadFromFile("assets/piece/drop_piece_count.png");
	selectionTexture.setSmooth(true);
	captureTexture.setSmooth(true);
	checkTexture.setSmooth(true);
	lastMoveTexture.setSmooth(true);
	selectionHoverTexture.setSmooth(true);
	selectedPieceTexture.setSmooth(true);
	promoteBackgroundTexture.setSmooth(true);
	kothBackgroundTexture.setSmooth(true);
	kothShadowTexture.setSmooth(true);
	checkCount.setSmooth(true);
	dropBackgroundTexture.setSmooth(true);
	dropTextBackground.setSmooth(true);
	textureVector boardTextures{ selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture, selectedPieceTexture, promoteBackgroundTexture, promoteBackgroundSelectionTexture,
		kothBackgroundTexture, kothShadowTexture, rankBackgroundTexture, rankShadowTopTexture, checkCount, dropBackgroundTexture, dropTextBackground };

	// Board Customization
	Variant VARIANT = Crazyhouse;
	int PIECE_SET = 2; // 0 - 3
	int BOARD_SET = 19; // 0 - 23
	bool AI = true; 
	bool AI_ONLY = false;
	bool CHESS960 = true;
	bool ENDGAME = false;
	bool ANIMATED = true;
	float X_OFFSET = 0;
	float Y_OFFSET = 0;
	float BOARD_SCALE = 1.0f;
	std::optional<std::string> FEN = std::nullopt;
	std::optional<bool> WHITE = std::nullopt;
		
	Board board{ VARIANT, FEN, X_OFFSET, Y_OFFSET, boardSize, ANIMATED, window.getSize(), BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, boardSpriteSheet, textFont, boardTexture, AI, AI_ONLY, WHITE, CHESS960, ENDGAME, pieceTextures, boardTextures};

	// Vars
	sf::Vector2f selectedPos{ 0.0f, 0.0f };
	sf::Vector2i mousePos{ sf::Mouse::getPosition() };
	std::vector <sf::Vector2f> initialPromoteScales{ {board.pieceScale * 0.82f, board.pieceScale * 0.82f}, {board.pieceScale * 0.775f, board.pieceScale * 0.775f}, {board.pieceScale * 0.8f, board.pieceScale * 0.8f}, {board.pieceScale * 0.785f, board.pieceScale * 0.785f}, {board.pieceScale * 0.785f, board.pieceScale * 0.785f} };

	window.setIcon(selectedIcon.getSize(), selectedIcon.getPixelsPtr());
	// ==== MAIN ====
	while (window.isOpen())
	{
		board.mouseSelecting = false;
		board.mouseClicked = false;
		mousePos = sf::Mouse::getPosition(window);
		selectedPos = { std::ceil(((float)mousePos.x - board.boardOffset.x) / board.boardMultiplier),  9 - std::ceil(((float)mousePos.y - board.boardOffset.y) / board.boardMultiplier) };
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
					board.mouseClicked = true;
					if (board.StockfishEnabled) {
						if (board.whiteToPlay != board.playerSideWhite) {
							break;
						}
					}
					std::shared_ptr<Piece> p = Main::getPieceFromPosition(sf::Vector2i(selectedPos), board.pieceList);
					if (p != nullptr && board.promotePiece == nullptr && !board.pieceMoving && board.animationFinished && !board.calculatingPos && !board.mouseMode) {
						// Side to Play == Color
						if (board.whiteToPlay == (p->color == PColor::White)) {
							if (board.selectedPiece != nullptr && board.selectedPiece->name == "King") {
								std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(board.selectedPiece);
								if (!std::any_of(king->captureCastlePositions.cbegin(), king->captureCastlePositions.cend(), [selectedPos](sf::Vector2i pos) { return pos == (sf::Vector2i)selectedPos; })) {
									board.selectedPiece = p;
								}
							}
							else {
								board.selectedPiece = p;
							}
						}
					}
				}
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::N) {
					if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& p) { return p->animationTarget.has_value() || p->targetPos.has_value(); }) && !board.calculatingPos) {
						board.setupFEN({});
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::D) {
					if (!board.pieceMoving && board.animationFinished && !board.calculatingPos) {
						board.scale(1.2f);
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::I) {
					if (!board.pieceMoving && board.animationFinished && !board.calculatingPos) {
						board.scale(0.8f);
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::Q) {
					window.close();
				}
				else if (keyPressed->code == sf::Keyboard::Key::R) {
					board.scaleMode = !board.scaleMode;
				}
				else if (keyPressed->code == sf::Keyboard::Key::T) {
					board.resetTransform();
				}
				else if (keyPressed->code == sf::Keyboard::Key::M) {
					board.mouseMode = !board.mouseMode;
				}
				else if (keyPressed->code == sf::Keyboard::Key::P) {
					++board.pieceSet %= pieceSpriteSheets.size();
					board.setPieceSheet(pieceSpriteSheets, board.pieceSet);
				}
				else if (keyPressed->code == sf::Keyboard::Key::B) {
					++board.boardSet %= 23;
					board.setBoardTexture(boardSpriteSheet, board.boardSet);
				}
			}
			else if (event->is<sf::Event::FocusLost>()) {
				if (!board.calculatingPos) {
					Main::block_until_gained_focus(window);
				}
			}
		}

		if (board.variant == ThreeCheck) {
			if (!board.pieceSelectionLock) {
				for (auto& piece : board.pieceList) {
					if (piece->name == "King" && piece->isWhite()) {
						board.whiteCheckCount.setPosition({ piece->getGlobalPosition().x - (board.boardMultiplier / 2.91f), piece->getGlobalPosition().y - (board.boardMultiplier / 2.91f) });
						board.whiteCheckText.setPosition({ board.whiteCheckCount.getPosition().x - (11.0f * board.sizeMultiplier), board.whiteCheckCount.getPosition().y - (21.5f * board.sizeMultiplier) });
						break;
					}
				}
				for (auto& piece : board.pieceList) {
					if (piece->name == "King" && !piece->isWhite()) {
						board.blackCheckCount.setPosition({ piece->getGlobalPosition().x - (board.boardMultiplier / 2.91f), piece->getGlobalPosition().y - (board.boardMultiplier / 2.91f) });
						board.blackCheckText.setPosition({ board.blackCheckCount.getPosition().x - (11.0f * board.sizeMultiplier), board.blackCheckCount.getPosition().y - (21.5f * board.sizeMultiplier) });
						break;
					}
				}
			}
			switch (board.whiteChecks) {
			case 0:
				board.whiteCheckCount.setColor(sf::Color(100, 173, 105));
				board.whiteCheckText.setString("3");
				break;
			case 1:
				board.whiteCheckCount.setColor(sf::Color(242, 119, 58));
				board.whiteCheckText.setString("2");
				break;
			case 2:
				board.whiteCheckCount.setColor(sf::Color(210, 0, 0));
				board.whiteCheckText.setString("1");
				break;
			case 3:
				board.whiteCheckCount.setColor(sf::Color(45, 45, 45));
				board.whiteCheckText.setString("0");
				break;
			}
			switch (board.blackChecks) {
			case 0:
				board.blackCheckCount.setColor(sf::Color(100, 173, 105));
				board.blackCheckText.setString("3");
				break;
			case 1:	 
				board.blackCheckCount.setColor(sf::Color(242, 119, 58));
				board.blackCheckText.setString("2");
				break;
			case 2:
				board.blackCheckCount.setColor(sf::Color(210, 0, 0));
				board.blackCheckText.setString("1");
				break;
			case 3:
				board.blackCheckCount.setColor(sf::Color(45, 45, 45));
				board.blackCheckText.setString("0");
				break;
			}
		}
		else if (board.variant == FiveCheck) {
			if (!board.pieceSelectionLock) {
				for (auto& piece : board.pieceList) {
					if (piece->name == "King" && piece->isWhite()) {
						board.whiteCheckCount.setPosition({ piece->getGlobalPosition().x - (board.boardMultiplier / 2.91f), piece->getGlobalPosition().y - (board.boardMultiplier / 2.91f) });
						board.whiteCheckText.setPosition({ board.whiteCheckCount.getPosition().x - (11.0f * board.sizeMultiplier), board.whiteCheckCount.getPosition().y - (21.5f * board.sizeMultiplier) });
						break;
					}
				}
				for (auto& piece : board.pieceList) {
					if (piece->name == "King" && !piece->isWhite()) {
						board.blackCheckCount.setPosition({ piece->getGlobalPosition().x - (board.boardMultiplier / 2.91f), piece->getGlobalPosition().y - (board.boardMultiplier / 2.91f) });
						board.blackCheckText.setPosition({ board.blackCheckCount.getPosition().x - (11.0f * board.sizeMultiplier), board.blackCheckCount.getPosition().y - (21.5f * board.sizeMultiplier) });
						break;
					}
				}
			}
			switch (board.whiteChecks) {
			case 0:
				board.whiteCheckCount.setColor(sf::Color(65, 168, 224));
				board.whiteCheckText.setString("5");
				break;
			case 1:
				board.whiteCheckCount.setColor(sf::Color(66, 175, 137));
				board.whiteCheckText.setString("4");
				break;
			case 2:
				board.whiteCheckCount.setColor(sf::Color(100, 173, 105));
				board.whiteCheckText.setString("3");
				break;
			case 3:
				board.whiteCheckCount.setColor(sf::Color(242, 119, 58));
				board.whiteCheckText.setString("2");
				break;
			case 4:
				board.whiteCheckCount.setColor(sf::Color(210, 0, 0));
				board.whiteCheckText.setString("1");
				break;
			case 5:
				board.whiteCheckCount.setColor(sf::Color(45, 45, 45));
				board.whiteCheckText.setString("0");
				break;
			}
			switch (board.blackChecks) {
			case 0:
				board.blackCheckCount.setColor(sf::Color(65, 168, 224));
				board.blackCheckText.setString("5");
				break;
			case 1:
				board.blackCheckCount.setColor(sf::Color(66, 175, 137));
				board.blackCheckText.setString("4");
				break;
			case 2:
				board.blackCheckCount.setColor(sf::Color(100, 173, 105));
				board.blackCheckText.setString("3");
				break;
			case 3:
				board.blackCheckCount.setColor(sf::Color(242, 119, 58));
				board.blackCheckText.setString("2");
				break;
			case 4:
				board.blackCheckCount.setColor(sf::Color(210, 0, 0));
				board.blackCheckText.setString("1");
				break;
			case 5:
				board.blackCheckCount.setColor(sf::Color(45, 45, 45));
				board.blackCheckText.setString("0");
				break;
			}
		}

		if (board.movePlayed) {
			board.calculatingPos = false;
			if (board.move != "") {
				board.moves += " " + board.move;
				std::thread playMoveT(Main::playMove, board.move, std::ref(board));
				playMoveT.join();
			}
			board.move = "";
			board.movePlayed = false;
		}

		if (!board.calculatingPos) {
			if (board.StockfishEnabled && (board.AI_Only || (!board.AI_Only && board.whiteToPlay != board.playerSideWhite)) && board.animationFinished) {
				if (std::none_of(board.pieceList.begin(), board.pieceList.end(), [](std::shared_ptr<Piece>& p) { return p->animationTarget.has_value() || p->targetPos.has_value(); })) {
					board.calculatingPos = true;
					std::thread moveT(Main::getBestMove, board.FEN, board.moves, std::ref(board.os), std::ref(board.is), std::ref(board.movePlayed), std::ref(board.move));
					moveT.detach();
				}
			}

			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && board.pieceSelectionLock) {
				board.pieceSelectionLock = false;
				if (board.selectedPiece != nullptr) {
					for (auto& sprite : board.selectedPiece->selectionSquares) {
						if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
							if (board.check) { board.check = false; }
							// En Passant
							if (board.enPassantPiece != nullptr) {
								board.enPassantPiece->enPassantTarget = false;
								board.enPassantPiece.reset();
							}
							if (board.selectedPiece->name == "Pawn") {
								if (board.selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 8) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == board.selectedPiece->getLocalPosition().y + 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
										pawn->enPassantTarget = true;
										board.enPassantPiece = pawn;
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 1) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == board.selectedPiece->getLocalPosition().y - 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
										pawn->enPassantTarget = true;
										board.enPassantPiece = pawn;
									}
								}
							}
							board.moveSound.play();
							if (board.promotePiece == nullptr) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " " + board.currentMove;
								board.lastMoveStart.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
								board.lastMoveDest.setPosition(sprite.getPosition());
								board.selectedPiece->setGlobalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier);
								board.selectedPiece->setGhostSpriteVisible(false, true);
								board.calculatingPos = true;
								std::thread postMoveF(Main::postMove, board.selectedPiece, std::ref(board));
								board.selectedPiece.reset();
								postMoveF.detach();
							}
						}
					}
				}
				if (board.selectedPiece != nullptr) {
					for (auto& sprite : board.selectedPiece->captureSquares) {
						if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
							if (board.check) { board.check = false; }
							if (board.enPassantPiece != nullptr) {
								board.enPassantPiece->enPassantTarget = false;
								board.enPassantPiece.reset();
							}
							if (board.selectedPiece->name == "Pawn") {
								if (board.selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 8) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 1) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
								}
							}

							for (auto& piece : board.pieceList) {
								if (piece->getGlobalPosition() == sprite.getPosition()) {
									board.capturePiece = piece;
									piece->setGhostSpriteVisible(true, false);
								}
							};
							board.captureSound.play();
							if (board.promotePiece == nullptr) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " " + board.currentMove;
								board.lastMoveStart.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
								board.lastMoveDest.setPosition(sprite.getPosition());
								board.selectedPiece->setGlobalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier);
								board.selectedPiece->setGhostSpriteVisible(false, true);
								board.capturePiece->setGhostSpriteVisible(false, false);
								board.calculatingPos = true;
								std::thread postMoveF(Main::postMove, board.selectedPiece, std::ref(board));
								board.selectedPiece.reset();
								postMoveF.detach();
							}
						}
					}
				}
				if (board.selectedPiece != nullptr) {
					if (board.selectedPiece->name == "King") {
						std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(board.selectedPiece);
						for (auto& sprite : king->castleSquares) {
							if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " ";
								board.moves += board.currentMove;
								board.moveSound.play();
								board.lastMoveStart.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
								board.lastMoveDest.setPosition(sprite.getPosition());
								if (board.check) { board.check = false; }
								// En Passant
								if (board.enPassantPiece != nullptr) {
									board.enPassantPiece->enPassantTarget = false;
									board.enPassantPiece.reset();
								}
								if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == 3) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, board.pieceList);
									if (rook != nullptr) {
										rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
										king->setGlobalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier);
										king->setGhostSpriteVisible(false, true);
										board.calculatingPos = true;
										std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
										board.selectedPiece.reset();
										postCastleF.detach();
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == 7) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, board.pieceList);
									if (rook != nullptr) {
										rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
										king->setGlobalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier);
										king->setGhostSpriteVisible(false, true);
										board.calculatingPos = true;
										std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
										board.selectedPiece.reset();
										postCastleF.detach();
									}
								}
							}
						}

						for (auto& sprite : king->castleCaptureSquares) {
							if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								board.moveSound.play();
								board.lastMoveStart.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
								board.lastMoveDest.setPosition(sprite.getPosition());
								if (board.check) { board.check = false; }
								// En Passant
								if (board.enPassantPiece != nullptr) {
									board.enPassantPiece->enPassantTarget = false;
									board.enPassantPiece.reset();
								}
								if (king->isWhite()) {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.wKRook) {
										std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.wKRook, king->getLocalPosition().y }, board.pieceList);
										if (rook != nullptr) {
											if (board.Chess960Enabled) {
												board.currentMove = "";
												board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
												board.currentMove += Main::convertPositiontoNotation({ board.wKRook, king->getLocalPosition().y });
											}
											else {
												if (std::abs(7 - king->getLocalPosition().x) == 1) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.wKRook, king->getLocalPosition().y });
												}
												else {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
												}
											}
											board.moves += " ";
											board.moves += board.currentMove;
											rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGlobalPosition(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGhostSpriteVisible(false, true);
											board.calculatingPos = true;
											std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
											board.selectedPiece.reset();
											postCastleF.detach();
										}
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.wQRook) {
										std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.wQRook, king->getLocalPosition().y }, board.pieceList);
										if (rook != nullptr) {
											if (board.Chess960Enabled) {
												board.currentMove = "";
												board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
												board.currentMove += Main::convertPositiontoNotation({ board.wQRook, king->getLocalPosition().y });
											}
											else {
												if (std::abs(3 - king->getLocalPosition().x) == 1) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.wQRook, king->getLocalPosition().y });
												}
												else {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
												}
											}
											board.moves += " ";
											board.moves += board.currentMove;
											rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGlobalPosition(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGhostSpriteVisible(false, true);
											board.calculatingPos = true;
											std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
											board.selectedPiece.reset();
											postCastleF.detach();
										}
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.bKRook) {
										std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.bKRook, king->getLocalPosition().y }, board.pieceList);
										if (rook != nullptr) {
											if (board.Chess960Enabled) {
												board.currentMove = "";
												board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
												board.currentMove += Main::convertPositiontoNotation({ board.bKRook, king->getLocalPosition().y });
											}
											else {
												if (std::abs(7 - king->getLocalPosition().x) == 1) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.bKRook, king->getLocalPosition().y });
												}
												else {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
												}
											}
											board.moves += " ";
											board.moves += board.currentMove;
											rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGlobalPosition(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGhostSpriteVisible(false, true);
											board.calculatingPos = true;
											std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
											board.selectedPiece.reset();
											postCastleF.detach();
										}
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.bQRook) {
										std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.bQRook, king->getLocalPosition().y }, board.pieceList);
										if (rook != nullptr) {
											if (board.Chess960Enabled) {
												board.currentMove = "";
												board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
												board.currentMove += Main::convertPositiontoNotation({ board.bQRook, king->getLocalPosition().y });
											}
											else {
												if (std::abs(3 - king->getLocalPosition().x) == 1) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.bQRook, king->getLocalPosition().y });
												}
												else {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
												}
											}
											board.moves += " ";
											board.moves += board.currentMove;
											rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGlobalPosition(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
											king->setGhostSpriteVisible(false, true);
											board.calculatingPos = true;
											std::thread postCastleF(Main::postCastle, king, rook, std::ref(board));
											board.selectedPiece.reset();
											postCastleF.detach();
										}
									}
								}
							}
						}
					}
				}
				if (board.selectedPiece != nullptr) {
					if (board.selectedPiece->name == "Pawn") {
						std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
						for (auto& sprite : pawn->enPassantSquares) {
							if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								board.captureSound.play();
								board.lastMoveStart.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
								board.lastMoveDest.setPosition(sprite.getPosition());
								if (board.check) { board.check = false; }
								if (board.enPassantPiece != nullptr) {
									board.enPassantPiece->enPassantTarget = false;
									board.enPassantPiece.reset();
								}
								for (auto& piece : board.pieceList) {
									if (pawn->isWhite()) {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier) - sf::Vector2i(0, 1))) {
											board.capturePiece = piece;
											break;
										}
									}
									else {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier) + sf::Vector2i(0, 1))) {
											board.capturePiece = piece;
											break;
										}
									}
								};
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " " + board.currentMove;
								board.selectedPiece->setGlobalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier);
								board.selectedPiece->setGhostSpriteVisible(false, true);
								board.capturePiece->setGhostSpriteVisible(false, false);
								board.calculatingPos = true;
								std::thread postMoveF(Main::postMove, board.selectedPiece, std::ref(board));
								board.selectedPiece.reset();
								postMoveF.detach();
							}
						}
					}
				}
				if (board.selectedPiece != nullptr) {
					if (!board.selectedPieceBackground.getGlobalBounds().contains(board.selectedPiece->getGlobalPosition())) {
						board.selectedPiece->setGlobalPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
						board.selectedPiece.reset();
					}
					else {
						board.selectedPiece->setGlobalPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier), board.boardOffset, board.boardMultiplier);
					}
				}
			}
			else if (board.pieceSelectionLock) {
				board.selectedPiece->setGlobalPosition(sf::Vector2f(mousePos), board.boardOffset, board.boardMultiplier);
			}


			// Board Animation
			board.animationFinished = true;
			for (auto& piece : board.pieceList) {
				if (piece->animationTarget.has_value()) {
					if (piece->getGlobalPosition() != piece->animationTarget.value()) {
						board.animationFinished = false;
						piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->animationTarget.value(), 0.22f), board.boardOffset, board.boardMultiplier);
					}
					else {
						piece->animationTarget = {};
					}
				}
			}

			// Movement
			if (board.promotePiece != nullptr) {
				if (board.unpromoting) {
					if (board.promotePiece->targetPos.has_value()) {
						if (board.promotePiece->getGlobalPosition() == board.promotePiece->targetPos.value()) {
							board.promotePiece->setLocalPosition(Main::getLocalPosition(board.promotePiece->getGlobalPosition(), board.boardOffset, board.boardMultiplier));
							board.promotePiece->setTarget({});
							board.promotePiece.reset();
							board.unpromoting = false;
						}
						else {
							board.promotePiece->setGlobalPosition(Main::Interpolate(board.promotePiece->getGlobalPosition(), board.promotePiece->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
						}
					}
				}
				else {
					if (board.promotePiece->targetPos.has_value()) {
						if (board.promotePiece->getGlobalPosition() == board.promotePiece->targetPos.value()) {
							board.promoting = true;
							int off = 0;
							if (board.kingPromotionEnabled) { off = 1; }
							int x = Main::getLocalPosition(board.promotePiece->getGlobalPosition(), board.boardOffset, board.boardMultiplier).x;
							if (board.promotePiece->isWhite()) {
								for (int y = 8; y >= 5 - off; y--) {
									sf::Sprite bg{ promoteBackgroundTexture };
									bg.setOrigin(bg.getGlobalBounds().getCenter());
									bg.setScale(board.boardSprite.getScale());
									bg.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
									bg.setColor(promotionSquareColor);
									board.promoteBackgrounds.push_back(bg);
									// Bishop, King, Knight, Pawn, Queen, Rook
									// Black --> White
									switch (y) {
									case 8:
									{
										sf::Sprite piece{ board.pieceTextures.at(10) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.82f, board.pieceScale * 0.82f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 7:
									{
										sf::Sprite piece{ board.pieceTextures.at(8) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.775f, board.pieceScale * 0.775f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 6:
									{
										sf::Sprite piece{ board.pieceTextures.at(11) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.775f, board.pieceScale * 0.775f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 5:
									{
										sf::Sprite piece{ board.pieceTextures.at(6) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.785f, board.pieceScale * 0.785f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 4:
									{
										sf::Sprite piece{ board.pieceTextures.at(7) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.785f, board.pieceScale * 0.785f));
										board.promoteSprites.push_back(piece);
										break;
									}
									}
								}
								board.promotePiece->setTarget({});
							}
							else {
								for (int y = 1; y <= 4 + off; y++) {
									sf::Sprite bg{ promoteBackgroundTexture };
									bg.setOrigin(bg.getGlobalBounds().getCenter());
									bg.setScale(board.boardSprite.getScale());
									bg.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
									bg.setColor(promotionSquareColor);
									board.promoteBackgrounds.push_back(bg);
									// Bishop, King, Knight, Pawn, Queen, Rook
									// Black --> White
									switch (y) {
									case 1:
									{
										sf::Sprite piece{ board.pieceTextures.at(4) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.82f, board.pieceScale * 0.82f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 2:
									{
										sf::Sprite piece{ board.pieceTextures.at(2) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.775f, board.pieceScale * 0.775f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 3:
									{
										sf::Sprite piece{ board.pieceTextures.at(5) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.775f, board.pieceScale * 0.775f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 4:
									{
										sf::Sprite piece{ board.pieceTextures.at(0) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.785f, board.pieceScale * 0.785f));
										board.promoteSprites.push_back(piece);
										break;
									}
									case 5:
									{
										sf::Sprite piece{ board.pieceTextures.at(1) };
										piece.setPosition(Main::getGlobalPosition({ x, y }, board.boardOffset, board.boardMultiplier));
										piece.setOrigin(piece.getLocalBounds().getCenter());
										piece.setScale(sf::Vector2f(board.pieceScale * 0.785f, board.pieceScale * 0.785f));
										board.promoteSprites.push_back(piece);
										break;
									}
									}
								}
								board.promotePiece->setTarget({});
							}
						}
						else {
							if (board.capturePiece != nullptr) {
								if (std::fmax(std::abs(board.promotePiece->getGlobalPosition().x - board.promotePiece->targetPos.value().x), std::abs(board.promotePiece->getGlobalPosition().y - board.promotePiece->targetPos.value().y)) < 5.0f) {
									board.capturePiece->setGhostSpriteVisible(false, false);
								}
							}
							board.promotePiece->setGlobalPosition(Main::Interpolate(board.promotePiece->getGlobalPosition(), board.promotePiece->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
						}
					}
				}
			}
			else if (board.castleKing != nullptr && board.castleRook != nullptr) {
				if (board.castleKing->targetPos.value() != board.castleKing->getGlobalPosition() || board.castleRook->targetPos.value() != board.castleRook->getGlobalPosition()) {
					board.castleKing->setGlobalPosition(Main::Interpolate(board.castleKing->getGlobalPosition(), board.castleKing->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
					board.castleRook->setGlobalPosition(Main::Interpolate(board.castleRook->getGlobalPosition(), board.castleRook->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
				}
				else {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(board.castleKing);
					board.calculatingPos = true;
					std::thread postCastleF(Main::postCastle, king, board.castleRook, std::ref(board));
					board.castleKing.reset();
					board.castleRook.reset();
					postCastleF.detach();
				}
			}
			else {
				for (auto it2 = board.pieceList.begin(); it2 != board.pieceList.end(); ) {
					std::shared_ptr<Piece> piece = *it2;
					if (piece != nullptr && piece->targetPos.has_value()) {
						if (piece->getGlobalPosition() == piece->targetPos.value()) {
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, piece, std::ref(board));
							postMoveF.detach();
							break;
						}
						else {
							if (piece->name == "Pawn") {
								std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
								if (pawn->capturingEnPassant) {
									piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, 15.0f), board.boardOffset, board.boardMultiplier);
								}
								else {
									if (board.capturePiece != nullptr) {
										piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, board.threshold), board.boardOffset, board.boardMultiplier);
										if (std::fmax(std::abs(piece->getGlobalPosition().x - piece->targetPos.value().x), std::abs(piece->getGlobalPosition().y - piece->targetPos.value().y)) < 5.0f) {
											board.capturePiece->setGhostSpriteVisible(false, false);
										}
									}
									else {
										piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
									}
								}
							}
							else {
								if (board.capturePiece != nullptr) {
									piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f, board.threshold), board.boardOffset, board.boardMultiplier);
									if (std::fmax(std::abs(piece->getGlobalPosition().x - piece->targetPos.value().x), std::abs(piece->getGlobalPosition().y - piece->targetPos.value().y)) < 5.0f) {
										board.capturePiece->setGhostSpriteVisible(false, false);
									}
								}
								else {
									piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.25f), board.boardOffset, board.boardMultiplier);
								}
							}
						}
					}
					++it2;
				}
			}
		}
		
		// Draw
		window.clear();
		window.draw(board.boardSprite);
		window.draw(board.lastMoveStart);
		window.draw(board.lastMoveDest);
		if (board.variant == KOTH) {
			window.draw(board.kothShadow);
			window.draw(board.kothBackground);
		}
		else if (board.variant == RacingKings) {
			window.draw(board.rankBackground);
			window.draw(board.rankShadowTop);
		}

		// Selection Sprites
		if (!board.pieceMoving && board.selectedPiece != nullptr && !board.promoting && !board.calculatingPos && !board.mouseMode) {
			for (auto& sprite : board.selectedPiece->selectionSquares) {
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					board.mouseSelecting = true;
					sprite.setTexture(boardTextures.at(4));
				}
				else { sprite.setTexture(boardTextures.at(0)); }
				window.draw(sprite);
			}
			for (auto& sprite : board.selectedPiece->captureSquares) {
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					board.mouseSelecting = true;
					sprite.setTexture(boardTextures.at(4));
				}
				else { sprite.setTexture(boardTextures.at(1)); }
				window.draw(sprite);
			}
			if (board.selectedPiece->name == "King") {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(board.selectedPiece);
				for (auto& sprite : king->castleSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						board.mouseSelecting = true;
						sprite.setTexture(boardTextures.at(4));
					}
					else { sprite.setTexture(boardTextures.at(0)); }
					window.draw(sprite);
				}

				for (auto& sprite : king->castleCaptureSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						board.mouseSelecting = true;
						sprite.setTexture(boardTextures.at(4));
					}
					else { sprite.setTexture(boardTextures.at(1)); }
					window.draw(sprite);
				}
			}
			if (board.selectedPiece->name == "Pawn") {
				std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
				for (auto& sprite : pawn->enPassantSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						board.mouseSelecting = true;
						sprite.setTexture(boardTextures.at(4));
					}
					else { sprite.setTexture(boardTextures.at(0)); }
					window.draw(sprite);
				}
			}
		}
		// Click Detection
		if (!board.pieceMoving && board.mouseClicked && board.animationFinished && !board.calculatingPos && !board.mouseMode) {
			if (board.selectedPiece != nullptr) {
				if (board.selectedPiece->contains(mousePos)) {
					board.pieceSelectionLock = true;
				}
			}
			if (board.selectedPiece != nullptr) {
				for (auto& sprite : board.selectedPiece->selectionSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
							if (board.check) { board.check = false; }
							// En Passant
							if (board.enPassantPiece != nullptr) {
								board.enPassantPiece->enPassantTarget = false;
								board.enPassantPiece.reset();
							}
							if (board.selectedPiece->name == "Pawn") {
								if (board.selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 8) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == board.selectedPiece->getLocalPosition().y + 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
										pawn->enPassantTarget = true;
										board.enPassantPiece = pawn;
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 1) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
									else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == board.selectedPiece->getLocalPosition().y - 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
										pawn->enPassantTarget = true;
										board.enPassantPiece = pawn;
									}
								}
							}
							board.moveSound.play();
							if (board.promotePiece == nullptr) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " " + board.currentMove;
								board.lastMoveStart.setPosition(board.selectedPiece->getGlobalPosition());
								board.lastMoveDest.setPosition(sprite.getPosition());
							}
							board.pieceMoving = true;
							board.selectedPiece->setTarget(sprite.getPosition());
							board.selectedPiece.reset();
						}
					}
				}
			}
			if (board.selectedPiece != nullptr) {
				for (auto& sprite : board.selectedPiece->captureSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
							// En Passant
							if (board.check) { board.check = false; }
							if (board.enPassantPiece != nullptr) {
								board.enPassantPiece->enPassantTarget = false;
								board.enPassantPiece.reset();
							}
							if (board.selectedPiece->name == "Pawn") {
								if (board.selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 8) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).y == 1) {
										board.promotePiece = board.selectedPiece;
										board.promotePiece->setTarget(sprite.getPosition());
									}
								}
							}
							board.captureSound.play();
							if (board.promotePiece == nullptr) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " ";
								board.moves += board.currentMove;
								board.lastMoveStart.setPosition(board.selectedPiece->getGlobalPosition());
								board.lastMoveDest.setPosition(sprite.getPosition());
							}
							for (auto& piece : board.pieceList) {
								if (piece->getGlobalPosition() == sprite.getPosition()) {
									board.capturePiece = piece;
									piece->setGhostSpriteVisible(true, false);
								}
							};
							board.pieceMoving = true;
							board.selectedPiece->setTarget(sprite.getPosition());
							board.selectedPiece.reset();
						}
					}
				}
			}
			if (board.selectedPiece != nullptr) {
				if (board.selectedPiece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(board.selectedPiece);
					for (auto& sprite : king->castleSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
								board.currentMove = "";
								board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
								board.currentMove += Main::convertPositiontoNotation(Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier));
								board.moves += " ";
								board.moves += board.currentMove;
								board.moveSound.play();
								board.lastMoveStart.setPosition(board.selectedPiece->getGlobalPosition());
								board.lastMoveDest.setPosition(sprite.getPosition());
								if (board.check) { board.check = false; }
								// En Passant
								if (board.enPassantPiece != nullptr) {
									board.enPassantPiece->enPassantTarget = false;
									board.enPassantPiece.reset();
								}
								if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == 3) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, board.pieceList);
									if (rook != nullptr) {
										rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
										king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
										board.castleKing = king;
										board.castleRook = rook;
										board.pieceMoving = true;
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == 7) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, board.pieceList);
									if (rook != nullptr) {
										rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
										king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
										board.castleKing = king;
										board.castleRook = rook;
										board.pieceMoving = true;
									}
								}
								board.selectedPiece.reset();
							}
						}
					}

					for (auto& sprite : king->castleCaptureSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
									board.moveSound.play();
									board.lastMoveStart.setPosition(board.selectedPiece->getGlobalPosition());
									board.lastMoveDest.setPosition(sprite.getPosition());
									if (board.check) { board.check = false; }
									// En Passant
									if (board.enPassantPiece != nullptr) {
										board.enPassantPiece->enPassantTarget = false;
										board.enPassantPiece.reset();
									}
									if (king->isWhite()) {
										if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.wKRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.wKRook, king->getLocalPosition().y }, board.pieceList);
											if (rook != nullptr) {
												if (board.Chess960Enabled) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.wKRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(7 - king->getLocalPosition().x) == 1) {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ board.wKRook, king->getLocalPosition().y });
													}
													else {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
													}
												}
												board.moves += " ";
												board.moves += board.currentMove;
												rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												board.castleKing = king;
												board.castleRook = rook;
											}
										}
										else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.wQRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.wQRook, king->getLocalPosition().y }, board.pieceList);
											if (rook != nullptr) {
												if (board.Chess960Enabled) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.wQRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(3 - king->getLocalPosition().x) == 1) {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ board.wQRook, king->getLocalPosition().y });
													}
													else {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
													}
												}
												board.moves += " ";
												board.moves += board.currentMove;
												rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												board.castleKing = king;
												board.castleRook = rook;
												board.pieceMoving = true;
											}
										}
									}
									else {
										if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.bKRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.bKRook, king->getLocalPosition().y }, board.pieceList);
											if (rook != nullptr) {
												if (board.Chess960Enabled) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.bKRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(7 - king->getLocalPosition().x) == 1) {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ board.bKRook, king->getLocalPosition().y });
													}
													else {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ 7, king->getLocalPosition().y });
													}
												}
												board.moves += " ";
												board.moves += board.currentMove;
												rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												board.castleKing = king;
												board.castleRook = rook;
												board.pieceMoving = true;
											}
										}
										else if (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier).x == board.bQRook) {
											std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ board.bQRook, king->getLocalPosition().y }, board.pieceList);
											if (rook != nullptr) {
												if (board.Chess960Enabled) {
													board.currentMove = "";
													board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
													board.currentMove += Main::convertPositiontoNotation({ board.bQRook, king->getLocalPosition().y });
												}
												else {
													if (std::abs(3 - king->getLocalPosition().x) == 1) {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ board.bQRook, king->getLocalPosition().y });
													}
													else {
														board.currentMove = "";
														board.currentMove += Main::convertPositiontoNotation(board.selectedPiece->getLocalPosition());
														board.currentMove += Main::convertPositiontoNotation({ 3, king->getLocalPosition().y });
													}
												}
												board.moves += " ";
												board.moves += board.currentMove;
												rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, board.boardOffset, board.boardMultiplier));
												board.castleKing = king;
												board.castleRook = rook;
												board.pieceMoving = true;
											}
										}
									}
									board.selectedPiece.reset();
								}
							}
						}
					}
				}
			}
			if (board.selectedPiece != nullptr) {
				if (board.selectedPiece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(board.selectedPiece);
					for (auto& sprite : pawn->enPassantSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
								board.captureSound.play();
								board.lastMoveStart.setPosition(board.selectedPiece->getGlobalPosition());
								board.lastMoveDest.setPosition(sprite.getPosition());
								if (board.check) { board.check = false; }
								// En Passant
								if (board.enPassantPiece != nullptr) {
									board.enPassantPiece->enPassantTarget = false;
									board.enPassantPiece.reset();
								}
								for (auto& piece : board.pieceList) {
									if (pawn->isWhite()) {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier) - sf::Vector2i(0, 1))) {
											board.capturePiece = piece;
										}
									}
									else {
										if (piece->getLocalPosition() == (Main::getLocalPosition(sprite.getPosition(), board.boardOffset, board.boardMultiplier) + sf::Vector2i(0, 1))) {
											board.capturePiece = piece;
										}
									}
								};
								pawn->capturingEnPassant = true;
								board.selectedPiece->setTarget(sprite.getPosition());
								board.selectedPiece.reset();
								board.pieceMoving = true;
							}
						}
					}
				}
			}
			if (Main::getPieceFromPosition(sf::Vector2i(selectedPos), board.pieceList) == nullptr) {
				board.selectedPiece.reset();
			}
		}

		if (!board.pieceMoving && board.animationFinished && !board.calculatingPos) {
			if (!board.scaleMode) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
					board.moveBy(3, 0);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
					board.moveBy(-3, 0);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
					board.moveBy(0, -3);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
					board.moveBy(0, 3);
				}
			}
			else {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
					board.scale(1.01f);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
					board.scale(0.99f);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
					board.scale(1.01f);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
					board.scale(0.99f);
				}
			}
			if (board.mouseMode) {
				board.mouseSelecting = true;
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					board.setPosition((sf::Vector2f)mousePos);
				}
			}
		}

		if (board.selectedPiece != nullptr) {
			board.selectedPieceBackground.setPosition(Main::getGlobalPosition(board.selectedPiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
			window.draw(board.selectedPieceBackground);
			if (board.pieceSelectionLock) {
				board.selectedPiece->setGhostSpriteVisible(true, true);
			}
			else {
				board.selectedPiece->setGhostSpriteVisible(false, true);
			}
		}
		if (board.check) { window.draw(board.checkSprite); }

		for (auto& piece : board.pieceList) {
			piece->drawGhostSprite(window);
		}
		for (auto& piece : board.pieceList) {
			if (piece->contains(mousePos)) {
				board.mouseSelecting = true;
			}
			if (board.selectedPiece != nullptr) {
				if (piece != board.selectedPiece) {
					piece->draw(window);
				}
			}
			else {
				piece->draw(window);
			}
		}
		if (board.promotePiece != nullptr && !board.unpromoting && !board.calculatingPos) {
			window.draw(board.promotionOverlay);
			int offset = 0;
			if (board.promotePiece->isBlack()) {
				offset = -6;
			}
			for (int i = 0; i < board.promoteBackgrounds.size(); i++) {
				sf::Sprite sprite = board.promoteBackgrounds.at(i);
				if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					sprite.setTexture(promoteBackgroundSelectionTexture);
					sprite.setColor(promotionSquareSelectedColor);
					window.draw(sprite);
					board.mouseSelecting = true;
					board.promoteSprites.at(i).setScale(Main::Interpolate(board.promoteSprites.at(i).getScale(), initialPromoteScales.at(i) + sf::Vector2f{ 0.035f, 0.035f }, 0.5f, 0.01f));
					window.draw(board.promoteSprites.at(i));
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						sf::Vector2f globalPos = Main::getGlobalPosition(board.promotePiece->getLocalPosition(), board.boardOffset, board.boardMultiplier);
						sf::Vector2i localPos = Main::getLocalPosition(board.promotePiece->getGlobalPosition(), board.boardOffset, board.boardMultiplier);
						sf::Vector2i localPos2 = board.promotePiece->getLocalPosition();
						board.lastMoveStart.setPosition(globalPos);
						board.lastMoveDest.setPosition(board.promotePiece->getGlobalPosition());
						for (int j = 0; j < board.pieceList.size(); j++) {
							if (board.pieceList.at(j) != nullptr && board.pieceList.at(j) == board.promotePiece) {
								board.pieceList.erase(board.pieceList.begin() + j);
								break;
							}
						}
						PColor color = board.promotePiece->color;
						board.promotePiece.reset();
						// Bishop, King, Knight, Pawn, Queen, Rook
						// Black --> White
						switch (i) {
						case 0:
						{
							std::shared_ptr<Queen> queen = std::make_shared<Queen>(localPos.x, localPos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, color, board.pieceTextures.at(10 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
							queen->promoted = true;
							board.pieceList.push_back(queen);
							board.currentMove = "";
							board.currentMove += Main::convertPositiontoNotation(localPos2);
							board.currentMove += Main::convertPositiontoNotation(localPos);
							board.currentMove += 'q';
							board.moves += " ";
							board.moves += board.currentMove;
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, queen, std::ref(board));
							postMoveF.detach();
							break;
						}
						case 1:
						{
							std::shared_ptr<Knight> knight = std::make_shared<Knight>(localPos.x, localPos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, color, board.pieceTextures.at(8 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
							knight->promoted = true;
							board.pieceList.push_back(knight);
							board.currentMove = "";
							board.currentMove += Main::convertPositiontoNotation(localPos2);
							board.currentMove += Main::convertPositiontoNotation(localPos);
							board.currentMove += 'n';
							board.moves += " ";
							board.moves += board.currentMove;
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, knight, std::ref(board));
							postMoveF.detach();
							break;
						}
						case 2:
						{
							std::shared_ptr<Rook> rook = std::make_shared<Rook>(localPos.x, localPos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, color, board.pieceTextures.at(11 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
							rook->promoted = true;
							board.pieceList.push_back(rook);
							board.currentMove = "";
							board.currentMove += Main::convertPositiontoNotation(localPos2);
							board.currentMove += Main::convertPositiontoNotation(localPos);
							board.currentMove += 'r';
							board.moves += " ";
							board.moves += board.currentMove;
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, rook, std::ref(board));
							postMoveF.detach();
							break;
						}
						case 3:
						{
							std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(localPos.x, localPos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, color, board.pieceTextures.at(6 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
							bishop->promoted = true;
							board.pieceList.push_back(bishop);
							board.currentMove = "";
							board.currentMove += Main::convertPositiontoNotation(localPos2);
							board.currentMove += Main::convertPositiontoNotation(localPos);
							board.currentMove += 'b';
							board.moves += " ";
							board.moves += board.currentMove;
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, bishop, std::ref(board));
							postMoveF.detach();
							break;
						}
						case 4:
						{
							std::shared_ptr<King> king = std::make_shared<King>(localPos.x, localPos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, color, board.pieceTextures.at(7 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
							king->promoted = true;
							board.pieceList.push_back(king);
							board.currentMove = "";
							board.currentMove += Main::convertPositiontoNotation(localPos2);
							board.currentMove += Main::convertPositiontoNotation(localPos);
							board.currentMove += 'k';
							board.moves += " ";
							board.moves += board.currentMove;
							board.calculatingPos = true;
							std::thread postMoveF(Main::postMove, king, std::ref(board));
							postMoveF.detach();
							break;
						}
						}
						board.promoteBackgrounds.clear();
						board.promoteSprites.clear();
						board.promoting = false;
					}
				}
				else {
					sprite.setTexture(promoteBackgroundTexture);
					sprite.setColor(promotionSquareColor);
					window.draw(sprite);
					board.promoteSprites.at(i).setScale(Main::Interpolate(board.promoteSprites.at(i).getScale(), initialPromoteScales.at(i), 0.5f, 0.01f));
					window.draw(board.promoteSprites.at(i));
				}
			}
		}
		if (board.promoting && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			board.unpromoting = true;
			board.promoting = false;
			board.promoteBackgrounds.clear();
			board.promoteSprites.clear();
			if (board.capturePiece != nullptr) {
				board.capturePiece->setGhostSpriteVisible(false, true);
				board.capturePiece.reset();
			}
			board.promotePiece->setTarget(Main::getGlobalPosition(board.promotePiece->getLocalPosition(), board.boardOffset, board.boardMultiplier));
		}
		if (board.selectedPiece != nullptr) {
			board.selectedPiece->draw(window);
		}
		for (auto& piece : board.pieceList) {
			if (piece->targetPos.has_value()) {
				piece->draw(window);
			}
		}
		if (board.variant == ThreeCheck || board.variant == FiveCheck) {
			window.draw(board.whiteCheckCount);
			window.draw(board.blackCheckCount);
			window.draw(board.whiteCheckText);
			window.draw(board.blackCheckText);
		}
		else if (board.dropsEnabled) {
			window.draw(board.dropPieceBackgroundW);
			window.draw(board.dropPieceBackgroundB);
			for (auto& piece : board.whiteDropPieces) {
				if (!board.calculatingPos && board.whiteToPlay) {
					if (piece.mouseSelecting(mousePos) && !board.dropPieceLock) {
						board.mouseSelecting = true;
						if (board.mouseClicked) {
							board.dropPiece.first.setTexture(board.pieceTextures.at(piece.tex));
							board.dropPiece.second = piece.id;
							board.dropPieceLock = true;
						}
					}
				}
				piece.draw(window);
			}
			for (auto& piece : board.blackDropPieces) {
				if (!board.calculatingPos && !board.whiteToPlay) {
					if (piece.mouseSelecting(mousePos) && !board.dropPieceLock) {
						board.mouseSelecting = true;
						if (board.mouseClicked) {
							board.dropPiece.first.setTexture(board.pieceTextures.at(piece.tex));
							board.dropPiece.second = piece.id;
							board.dropPieceLock = true;
						}
					}
				}
				piece.draw(window);
			}
			if (board.dropPieceLock && !board.calculatingPos) {
				if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					board.dropPieceLock = false;
					for (auto& square : board.dropSquares) {
						if (board.dropPiece.second != 'p' || (board.dropPiece.second == 'p' && Main::getLocalPosition(square.getPosition(), board.boardOffset, board.boardMultiplier).y != 1 && Main::getLocalPosition(square.getPosition(), board.boardOffset, board.boardMultiplier).y != 8)) {
							if (board.dropPiece.first.getGlobalBounds().contains(square.getPosition())) {
								board.moveSound.play();
								board.lastMoveStart.setPosition({ -1000, -1000 });
								board.lastMoveDest.setPosition(square.getPosition());
								if (board.whiteToPlay) {
									for (auto& piece : board.whiteDropPieces) {
										if (piece.id == board.dropPiece.second) {
											piece.count--;
											break;
										}
									}
								}
								else {
									for (auto& piece : board.blackDropPieces) {
										if (piece.id == board.dropPiece.second) {
											piece.count--;
											break;
										}
									}
								}
								sf::Vector2i pos = Main::getLocalPosition(square.getPosition(), board.boardOffset, board.boardMultiplier);
								PColor col = board.whiteToPlay ? White : Black;
								int offset = board.whiteToPlay ? 0 : -6;
								// Bishop, King, Knight, Pawn, Queen, Rook
								switch (board.dropPiece.second) {
								case 'p':
								{
									board.currentMove = "";
									board.currentMove += "P@";
									board.currentMove += Main::convertPositiontoNotation(pos);
									board.moves += " " + board.currentMove;
									std::shared_ptr<Pawn> pawn = std::make_shared<Pawn>(pos.x, pos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(9 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
									board.pieceList.push_back(pawn);
									board.calculatingPos = true;
									std::thread postMoveF(Main::postMove, pawn, std::ref(board));
									postMoveF.detach();
									break;
								}
								case 'q':
								{
									board.currentMove = "";
									board.currentMove += "Q@";
									board.currentMove += Main::convertPositiontoNotation(pos);
									board.moves += " " + board.currentMove;
									std::shared_ptr<Queen> queen = std::make_shared<Queen>(pos.x, pos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(10 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
									board.pieceList.push_back(queen);
									board.calculatingPos = true;
									std::thread postMoveF(Main::postMove, queen, std::ref(board));
									postMoveF.detach();
									break;
								}
								case 'b':
								{
									board.currentMove = "";
									board.currentMove += "B@";
									board.currentMove += Main::convertPositiontoNotation(pos);
									board.moves += " " + board.currentMove;
									std::shared_ptr<Bishop> bishop = std::make_shared<Bishop>(pos.x, pos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(6 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
									board.pieceList.push_back(bishop);
									board.calculatingPos = true;
									std::thread postMoveF(Main::postMove, bishop, std::ref(board));
									postMoveF.detach();
									break;
								}
								case 'r':
								{
									board.currentMove = "";
									board.currentMove += "R@";
									board.currentMove += Main::convertPositiontoNotation(pos);
									board.moves += " " + board.currentMove;
									std::shared_ptr<Rook> rook = std::make_shared<Rook>(pos.x, pos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(11 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
									board.pieceList.push_back(rook);
									board.calculatingPos = true;
									std::thread postMoveF(Main::postMove, rook, std::ref(board));
									postMoveF.detach();
									break;
								}
								case 'n':
								{
									board.currentMove = "";
									board.currentMove += "N@";
									board.currentMove += Main::convertPositiontoNotation(pos);
									board.moves += " " + board.currentMove;
									std::shared_ptr<Knight> knight = std::make_shared<Knight>(pos.x, pos.y, board.pieceScale, board.boardOffset, board.boardMultiplier, col, board.pieceTextures.at(8 + static_cast<std::vector<std::reference_wrapper<sf::Texture>, std::allocator<std::reference_wrapper<sf::Texture>>>::size_type>(offset)), false);
									board.pieceList.push_back(knight);
									board.calculatingPos = true;
									std::thread postMoveF(Main::postMove, knight, std::ref(board));
									postMoveF.detach();
									break;
								}
								}
							}
						}
					}
				}
				else {
					board.mouseSelecting = true;
					board.dropPiece.first.setPosition((sf::Vector2f)mousePos);
					for (auto& square : board.dropSquares) {
						if (board.dropPiece.second != 'p' || (board.dropPiece.second == 'p' && Main::getLocalPosition(square.getPosition(), board.boardOffset, board.boardMultiplier).y != 1 && Main::getLocalPosition(square.getPosition(), board.boardOffset, board.boardMultiplier).y != 8)) {
							if (board.dropPiece.first.getGlobalBounds().contains(square.getPosition())) {
								square.setTexture(boardTextures.at(4));
							}
							else {
								square.setTexture(boardTextures.at(0));
							}
							window.draw(square);
						}
					}
					window.draw(board.dropPiece.first);
				}
			}
		}


		if (board.calculatingPos) {
			if (board.mouseClicked && Main::getPieceFromPosition(sf::Vector2i(selectedPos), board.pieceList) != nullptr && Main::getPieceFromPosition(sf::Vector2i(selectedPos), board.pieceList)->isWhite() == board.whiteToPlay) {
				if (board.StockfishEnabled) {
					if (board.whiteToPlay == board.playerSideWhite) {
						board.hourGlass = true;
					}
				}
				else {
					board.hourGlass = true;
				}
			}
		}
		else {
			board.hourGlass = false;
		}
		if (board.animationFinished) {
			if (board.mouseSelecting) {
				if (board.hourGlass) {
					window.setMouseCursor(hourGlassCursor);
				}
				else {
					window.setMouseCursor(handCursor);
				}
			}
			else { window.setMouseCursor(arrowCursor); }
		}
		else {
			window.setMouseCursor(hourGlassCursor);
		}
		window.display();
	}
}