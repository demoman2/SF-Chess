#include "Main.h"

int main()
{
	std::cout << std::boolalpha;
	srand(time(NULL));
	sf::Clock testClock;
	sf::Font textFont("assets/font/Roboto-Medium.ttf");
	sf::Image hourglassImage("assets/other/hourglass.png");
	std::vector<sf::Cursor> mouseCursors;
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::SizeAll).value());
	mouseCursors.emplace_back(sf::Cursor::createFromPixels(hourglassImage.getPixelsPtr(), hourglassImage.getSize(), sf::Vector2u{}).value());

	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "SF Chess", sf::Style::None);
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);
	sf::View view(window.getDefaultView());
	view.setCenter({800, 450});
	sf::Angle angle = sf::degrees(180.0f);
	view.setRotation(angle);

	// Icons
	sf::Texture selectedIcon, checkersWhite("assets/icon/checkers_white.png"), checkersBlack("assets/icon/checkers_black.png");
	std::vector<sf::Texture> iconTextures;
	iconTextures.emplace_back("assets/icon/1.png");
	iconTextures.emplace_back("assets/icon/2.png");
	iconTextures.emplace_back("assets/icon/3.png");
	iconTextures.emplace_back("assets/icon/4.png");
	iconTextures.emplace_back("assets/icon/5.png");
	iconTextures.emplace_back("assets/icon/6.png");

	if (std::rand() % 50 == 0) {
		if (std::rand() % 2 == 0) { selectedIcon = checkersWhite; }
		else { selectedIcon = checkersBlack; }
	}
	else { selectedIcon = iconTextures.at(std::rand() % iconTextures.size()); }

	// Board  
	bool hourGlass = false, makingBoard = false, isFocused = true;
	sf::Image boardSpriteSheet("assets/board/board_matrix.jpg");
	sf::Texture boardTexture(boardSpriteSheet, false, sf::IntRect({ 0, 0 }, { 1024, 1024 }));

	// Pieces
	std::vector<sf::Image> pieceSpriteSheets;
	pieceSpriteSheets.emplace_back("assets/piece/pieces_matrix_alpha.png");
	pieceSpriteSheets.emplace_back("assets/piece/pieces_matrix_cburnett.png");
	pieceSpriteSheets.emplace_back("assets/piece/pieces_matrix_merida.png");
	pieceSpriteSheets.emplace_back("assets/piece/pieces_matrix_disguised.png");
	std::vector<sf::Texture> pieceTextures = Chess::makePieceSet(pieceSpriteSheets.front(), 12, 320);

	// Extras
	std::vector<sf::Texture> boardTextures;
	boardTextures.emplace_back("assets/piece/move_gradient.png");			// 0
	boardTextures.emplace_back("assets/piece/capture_gradient.png");		// 1
	boardTextures.emplace_back("assets/piece/check.png");					// 2
	boardTextures.emplace_back("assets/piece/last_move.png");				// 3
	boardTextures.emplace_back("assets/piece/selection_hover.png");			// 4
	boardTextures.emplace_back("assets/piece/piece_selection_hover.png");	// 5
	boardTextures.emplace_back("assets/piece/promotion_square.png");		// 6
	boardTextures.emplace_back("assets/piece/promotion_square_select.png"); // 7
	boardTextures.emplace_back("assets/board/koth_background.png");         // 8
	boardTextures.emplace_back("assets/board/koth_shadow.png");             // 9
	boardTextures.emplace_back("assets/board/rank_background.png");         // 10
	boardTextures.emplace_back("assets/board/rank_shadow_top.png");         // 11
	boardTextures.emplace_back("assets/board/check_color.png");             // 12
	boardTextures.emplace_back("assets/board/rectangle_gradient.png");      // 13
	boardTextures.emplace_back("assets/piece/drop_piece_count.png");        // 14
	boardTextures.emplace_back("assets/board/rounded_square.png");         // 15
	for (auto& t : boardTextures) { t.setSmooth(true); }

	// Board Customization
	Chess::Variant VARIANT = Chess::Crazyhouse;
	int PIECE_SET = 2; // 0 - 3
	int BOARD_SET = 19; // 0 - 23
	bool AI = false;
	bool AI_ONLY = false;
	bool CHESS960 = true;
	bool ENDGAME_POSITION = false;
	bool ANIMATED = true;
	bool REPEAT_FEN = false;
	float X_OFFSET = 0;
	float Y_OFFSET = 0;
	float BOARD_SCALE = 1.0f;
	std::optional<std::string> FEN = std::nullopt;
	std::optional<bool> WHITE = std::nullopt;	

	// Boards
	std::shared_ptr<Board> board = std::make_shared<Board>(VARIANT, FEN, X_OFFSET, Y_OFFSET, 1024, ANIMATED, window.getSize(), BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, boardSpriteSheet, textFont, boardTexture, AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME_POSITION, pieceTextures, boardTextures, true);
	std::shared_ptr<Board> selectedBoard = board;
	std::vector<std::shared_ptr<Board>> boardList{ board };

	// Vars
	sf::Vector2f mousePos{};
	sf::RectangleShape rect;

	// Icon Setup
	sf::Sprite iconTexture{ selectedIcon };
	sf::RenderTexture renderTexture(selectedIcon.getSize());
	sf::Image bIcon = makeIconFromBoard(selectedBoard, renderTexture, iconTexture);
	window.setIcon(bIcon.getSize(), bIcon.getPixelsPtr());


	while (window.isOpen()) {
		mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
		for (auto& b : boardList) {
			if (b != nullptr && b->intersects((sf::Vector2f)mousePos)) {
				if (b != selectedBoard) {
					sf::Image newIcon = makeIconFromBoard(b, renderTexture, iconTexture);
					window.setIcon(newIcon.getSize(), newIcon.getPixelsPtr());
				}
				selectedBoard = b;
			}
		}
		if (selectedBoard != nullptr) {
			selectedBoard->updateS(mousePos, isFocused);
		}
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else {
				if (selectedBoard != nullptr) { selectedBoard->handleEvent(event, boardSpriteSheet, pieceSpriteSheets, isFocused); }
				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->code == sf::Keyboard::Key::B) {
						// Set Icon
						if (selectedBoard != nullptr) {
							sf::Image newIcon = makeIconFromBoard(selectedBoard, renderTexture, iconTexture);
							window.setIcon(newIcon.getSize(), newIcon.getPixelsPtr());
						}
					}

					if (keyPressed->code == sf::Keyboard::Key::Y) {
						if (!makingBoard) {
							for (auto& board : boardList) { board->mouseModeOff(); }
							testClock.start();
							std::thread makeBoardThread(makeBoard, std::ref(selectedBoard), VARIANT, FEN, X_OFFSET, Y_OFFSET, 1024, ANIMATED, window.getSize(), BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, std::ref(boardSpriteSheet), std::ref(textFont), std::ref(boardTexture), AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME_POSITION, std::ref(pieceTextures), std::ref(boardTextures), std::ref(boardList), std::ref(makingBoard));
							makeBoardThread.detach();
						}
					}
					else if (keyPressed->code == sf::Keyboard::Key::Delete) {
						for (auto it = boardList.begin(); it != boardList.end(); it++) {
							if (*it == selectedBoard) {
								boardList.erase(it);
								break;
							}
						}
					}
				}
				else if (const auto* focusGained = event->getIf<sf::Event::FocusGained>()) {
					isFocused = true;
				}
				else if (const auto* focusGained = event->getIf<sf::Event::FocusLost>()) {
					isFocused = false;
				}
			}
		}

		window.clear();
		for (auto& b : boardList) {
			b->update(mousePos, isFocused);
			b->draw(window);
		}
		if (makingBoard) { window.setMouseCursor(mouseCursors.at(3)); }
		else if (selectedBoard != nullptr) { window.setMouseCursor(mouseCursors.at(selectedBoard->getMouseCursor(hourGlass))); }

		window.display();
	}
}