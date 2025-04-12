#include "Main.h"

using namespace Chess;
int main()
{
	srand(time(0));
	const auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
	const auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktopMode, "SF Chess", sf::Style::None);
	float windowSizeX = window.getSize().x;
	float windowSizeY = window.getSize().y;

	// Chess Boards
	int boardSize = 1024;
	sf::Image boardSpriteSheet;
	boardSpriteSheet.loadFromFile("assets/board/board_matrix.jpg");
	sf::Texture boardTexture;
	boardTexture.loadFromImage(boardSpriteSheet, false, sf::IntRect({ 0, 0 }, { boardSize, boardSize }));
	sf::Sprite board{ boardTexture };
	Main::loadBoard(boardSpriteSheet, board, boardTexture, 0, boardSize);
	float ScaleX = windowSizeX / boardTexture.getSize().x;
	float ScaleY = windowSizeY / boardTexture.getSize().y;
	board.setScale({ std::min(ScaleX, ScaleY), std::min(ScaleX, ScaleY) });
	board.setOrigin(board.getLocalBounds().getCenter());
	board.setPosition({ windowSizeX / 2.0f, windowSizeY / 2.0f });

	// Pieces
	int halfMoves = 0, fullMoves = 0;
	bool whiteToPlay = true, calculatingPos = false;
	float pieceSize = 320;
	float pieceScale = (std::min(ScaleX, ScaleY) * 128.0f) / (float)pieceSize;
	float boardOffset = (windowSizeX / 2.0f) - ((boardTexture.getSize().x * board.getScale().x) / 2);
	float boardMultiplier = ((boardTexture.getSize().x * board.getScale().x) / 8);
	sf::Image pieceSpriteSheetAlpha, pieceSpriteSheetCburnett, pieceSpriteSheetMerida, pieceSpriteSheetDisguised;
	pieceSpriteSheetAlpha.loadFromFile("assets/piece/pieces_matrix_alpha.png");
	pieceSpriteSheetCburnett.loadFromFile("assets/piece/pieces_matrix_cburnett.png");
	pieceSpriteSheetMerida.loadFromFile("assets/piece/pieces_matrix_merida.png");
	pieceSpriteSheetDisguised.loadFromFile("assets/piece/pieces_matrix_disguised.png");
	sf::Image pieceStyle = pieceSpriteSheetCburnett;
	sf::Texture blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT;
	sf::Texture whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT;
	std::vector<std::reference_wrapper<sf::Texture>> pieceTextures{ blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT,
	whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT };

	// Extras
	sf::Texture selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture, selectedPieceTexture;
	selectionTexture.loadFromFile("assets/piece/move_gradient.png");
	captureTexture.loadFromFile("assets/piece/capture_gradient.png");
	checkTexture.loadFromFile("assets/piece/check.png");
	lastMoveTexture.loadFromFile("assets/piece/last_move.png");
	selectionHoverTexture.loadFromFile("assets/piece/selection_hover.png");
	selectedPieceTexture.loadFromFile("assets/piece/piece_selection_hover.png");
	selectionTexture.setSmooth(true);
	captureTexture.setSmooth(true);
	checkTexture.setSmooth(true);
	lastMoveTexture.setSmooth(true);
	selectionHoverTexture.setSmooth(true);
	selectedPieceTexture.setSmooth(true);
	// Selection, Capture, Check, Last Move, Selection Hover
	std::vector<std::reference_wrapper<sf::Texture>> extraTextures{ selectionTexture, captureTexture, checkTexture, lastMoveTexture, selectionHoverTexture };

	// Vars
	float xAccl = 1.0f, yAccl = 1.0f;
	bool mouseSelecting = false, mouseClicked = false, pieceMoving = false, check = false, animationFinished = true, pieceSelectionLock = false;
	std::vector<std::pair<std::array<std::array<int, 8>, 8>, bool>> allPositionsPlayed;
	std::shared_ptr<Piece> selectedPiece = nullptr, capturePiece = nullptr, castleKing = nullptr, castleRook = nullptr;
	std::shared_ptr<Pawn> enPassantPiece = nullptr;
	std::vector<std::shared_ptr<Piece>>::iterator it;
	sf::Vector2f selectedPos{ 0.0f, 0.0f };
	sf::Vector2i mousePos;
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
	moveBuffer.loadFromFile("assets/audio/Move.mp3");
	captureBuffer.loadFromFile("assets/audio/Capture.mp3");
	checkBuffer.loadFromFile("assets/audio/Check.mp3");
	gameEndBuffer.loadFromFile("assets/audio/End.mp3");
	explosionBuffer.loadFromFile("assets/audio/Explosion.mp3");
	sf::Sound moveSound(moveBuffer);
	sf::Sound captureSound(captureBuffer);
	sf::Sound checkSound(checkBuffer);
	sf::Sound gameEndSound(gameEndBuffer);
	sf::Sound explosionSound(explosionBuffer);
	moveSound.setVolume(100);
	captureSound.setVolume(100);
	checkSound.setVolume(100);
	gameEndSound.setVolume(100);
	explosionSound.setVolume(100);

	// Setup
	Main::loadPieceSet(pieceStyle, pieceTextures, pieceSize);
	std::vector<std::shared_ptr<Piece>> pieceList = Main::generatePositionFromFENID("7k/8/8/8/8/5Q2/8/4K3 w - - 0 1",
		pieceTextures, pieceScale, boardOffset, boardMultiplier, whiteToPlay, halfMoves, fullMoves, enPassantPiece, true);
	for (auto& p : pieceList) {
		if (p->name != "King") {
			Main::calculatePositions(p, pieceList);
		}
	}
	for (auto& p : pieceList) {
		if (p->name == "King") {
			Main::calculatePositions(p, pieceList);
		}
	}
	Main::setExtraSprites(pieceList, extraTextures);

	// ==== MAIN ====
	std::cout << std::boolalpha;
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
					std::shared_ptr<Piece> p = Main::getPieceFromPosition(sf::Vector2i(selectedPos), pieceList);
					if (p != nullptr && !pieceMoving) {
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
					check = false;
					lastMoveStart.setPosition({ -1000, -1000 });
					lastMoveDest.setPosition({ -1000, -1000 });
					selectedPiece.reset();
					capturePiece.reset();
					enPassantPiece.reset();
					pieceList.clear();
					pieceList = Main::generatePositionFromFENID("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
						pieceTextures, pieceScale, boardOffset, boardMultiplier, whiteToPlay, halfMoves, fullMoves, enPassantPiece, true);
					for (auto& p : pieceList) {
						if (p->name != "King") {
							Main::calculatePositions(p, pieceList);
						}
					}
					for (auto& p : pieceList) {
						if (p->name == "King") {
							Main::calculatePositions(p, pieceList);
						}
					}
					Main::setExtraSprites(pieceList, extraTextures);
				}
			}
		}

		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && pieceSelectionLock) {
			pieceSelectionLock = false;
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->selectionSquares) {
					if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
						lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
						lastMoveDest.setPosition(sprite.getPosition());
						if (check) { check = false; }
						// En Passant
						if (enPassantPiece != nullptr) {
							enPassantPiece->enPassantTarget = false;
							enPassantPiece.reset();
						}
						if (selectedPiece->name == "Pawn") {
							if (selectedPiece->color == PColor::White) {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y + 2) {
									std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
									pawn->enPassantTarget = true;
									enPassantPiece = pawn;
								}
							}
							else {
								if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y - 2) {
									std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
									pawn->enPassantTarget = true;
									enPassantPiece = pawn;
								}
							}
						}
						selectedPiece->setGlobalPosition(sprite.getPosition());
						Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite,
							extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest);
					}
				}
			}
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->captureSquares) {
					if (sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {
						lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
						lastMoveDest.setPosition(sprite.getPosition());
						if (check) { check = false; }
						if (enPassantPiece != nullptr) {
							enPassantPiece->enPassantTarget = false;
							enPassantPiece.reset();
						}
						for (auto& piece : pieceList) {
							if (piece->getGlobalPosition() == sprite.getPosition()) {
								capturePiece = piece;
								piece->setGhostSpriteVisible(true, false);
							}
						};
						selectedPiece->setGlobalPosition(sprite.getPosition());
						Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite,
							extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest);
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "King") {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
					for (auto& sprite : king->castleSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
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
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest);
								}
							}
							else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 7) {
								std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, pieceList);
								if (rook != nullptr) {
									rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									king->setGlobalPosition(sprite.getPosition());
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest);
								}
							}
						}
					}

					for (auto& sprite : king->castleCaptureSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							lastMoveStart.setPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
							lastMoveDest.setPosition(sprite.getPosition());
							if (check) { check = false; }
							// En Passant
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 1) {
								std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, pieceList);
								if (rook != nullptr) {
									rook->setGlobalPosition(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									king->setGlobalPosition(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest);
								}
							}
							else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 8) {
								std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, pieceList);
								if (rook != nullptr) {
									rook->setGlobalPosition(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									king->setGlobalPosition(Main::getGlobalPosition({ 7, king->getLocalPosition().y }, boardOffset, boardMultiplier));
									Main::postCastle(king, rook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest);
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
							selectedPiece->setGlobalPosition(sprite.getPosition());
							Main::postMove(selectedPiece, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest);
						}
					}
				}
			}
			if (selectedPiece != nullptr) {
				selectedPiece->setGlobalPosition(Main::getGlobalPosition(selectedPiece->getLocalPosition(), boardOffset, boardMultiplier));
			}
		}
		else if (pieceSelectionLock) {
			selectedPiece->setGlobalPosition(sf::Vector2f(mousePos));
		}

		// Board Animation
		for (auto& piece : pieceList) {
			if (piece->animationTarget.has_value()) {
				if (piece->getGlobalPosition() != piece->animationTarget.value()) {
					animationFinished = false;
					piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->animationTarget.value(), 0.005f));
				}
				else {
					piece->animationTarget = {};
				}
			}
		}

		// Movement
		if (castleKing != nullptr && castleRook != nullptr) {
			if (castleKing->targetPos.value() != castleKing->getGlobalPosition() || castleRook->targetPos.value() != castleRook->getGlobalPosition()) {
				castleKing->setGlobalPosition(Main::Interpolate(castleKing->getGlobalPosition(), castleKing->targetPos.value(), 0.005f));
				castleRook->setGlobalPosition(Main::Interpolate(castleRook->getGlobalPosition(), castleRook->targetPos.value(), 0.005f));
			}
			else {
				Main::postCastle(castleKing, castleRook, pieceList, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures, allPositionsPlayed, selectedPiece, window, board, lastMoveStart, lastMoveDest);
			}
		}
		else {
			for (auto it2 = pieceList.begin(); it2 != pieceList.end(); ) {
				std::shared_ptr<Piece> piece = *it2;
				if (piece != nullptr && piece->targetPos.has_value()) {
					if (piece->getGlobalPosition() == piece->targetPos.value()) {
						if (Main::postMove(piece, pieceList, it2, boardOffset, boardMultiplier, whiteToPlay, check, pieceMoving, checkSprite, extraTextures,
							allPositionsPlayed, selectedPiece, capturePiece, window, board, lastMoveStart, lastMoveDest) == 1) {
							break;
						}
					}
					else {
						pieceMoving = true;
						if (piece->name == "Pawn") {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							if (pawn->capturingEnPassant) {
								piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.005f, 15.0f));
							}
							else {
								piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.005f));
							}
						}
						else {
							if (capturePiece != nullptr) {
								if (std::fmax(std::abs(piece->getGlobalPosition().x - piece->targetPos.value().x), std::abs(piece->getGlobalPosition().y - piece->targetPos.value().y)) < 5.0f) {
									capturePiece->setGhostSpriteVisible(false, false);
								}
							}
							piece->setGlobalPosition(Main::Interpolate(piece->getGlobalPosition(), piece->targetPos.value(), 0.005f));
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
		if (!pieceMoving && selectedPiece != nullptr) {
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
		if (!pieceMoving && mouseClicked) {
			if (selectedPiece != nullptr) {
				if (selectedPiece->contains(mousePos)) {
					pieceSelectionLock = true;
				}
			}
			if (selectedPiece != nullptr) {
				for (auto& sprite : selectedPiece->selectionSquares) {
					if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
							lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
							lastMoveDest.setPosition(sprite.getPosition());
							if (check) { check = false; }
							// En Passant
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
							}
							if (selectedPiece->name == "Pawn") {
								if (selectedPiece->color == PColor::White) {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y + 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
										pawn->enPassantTarget = true;
										enPassantPiece = pawn;
									}
								}
								else {
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).y == selectedPiece->getLocalPosition().y - 2) {
										std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
										pawn->enPassantTarget = true;
										enPassantPiece = pawn;
									}
								}
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
							lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
							lastMoveDest.setPosition(sprite.getPosition());
							// En Passant
							if (check) { check = false; }
							if (enPassantPiece != nullptr) {
								enPassantPiece->enPassantTarget = false;
								enPassantPiece.reset();
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
										king->setTarget(sprite.getPosition());
										whiteToPlay = !whiteToPlay;
									}
								}
								else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 7) {
									std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 8, king->getLocalPosition().y }, pieceList);
									if (rook != nullptr) {
										rook->setTarget(Main::getGlobalPosition({ 6, king->getLocalPosition().y }, boardOffset, boardMultiplier));
										king->setTarget(sprite.getPosition());
										whiteToPlay = !whiteToPlay;
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
									lastMoveStart.setPosition(selectedPiece->getGlobalPosition());
									lastMoveDest.setPosition(sprite.getPosition());
									if (check) { check = false; }
									// En Passant
									if (enPassantPiece != nullptr) {
										enPassantPiece->enPassantTarget = false;
										enPassantPiece.reset();
									}
									if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 1) {
										std::shared_ptr<Piece> rook = Main::getPieceFromPosition({ 1, king->getLocalPosition().y }, pieceList);
										if (rook != nullptr) {
											rook->setTarget(Main::getGlobalPosition({ 4, king->getLocalPosition().y }, boardOffset, boardMultiplier));
											king->setTarget(Main::getGlobalPosition({ 3, king->getLocalPosition().y }, boardOffset, boardMultiplier));
											castleKing = king;
											castleRook = rook;
										}
									}
									else if (Main::getLocalPosition(sprite.getPosition(), boardOffset, boardMultiplier).x == 8) {
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
					}
				}
			}
			if (selectedPiece != nullptr) {
				if (selectedPiece->name == "Pawn") {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(selectedPiece);
					for (auto& sprite : pawn->enPassantSquares) {
						if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
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
		if (mouseSelecting) {
			window.setMouseCursor(handCursor);
		}
		else { window.setMouseCursor(arrowCursor); }
		window.display();
	}
}