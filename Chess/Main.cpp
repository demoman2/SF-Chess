#include "Main.h"

int main()
{
	std::cout << std::boolalpha;
	srand(time(NULL)); 
	sf::Clock deltaClock;
	sf::Time deltaTime = sf::Time::Zero;
	sf::Font textFont("assets/font/Roboto-Medium.ttf");
	sf::Image hourglassImage("assets/other/hourglass.png");
	std::vector<sf::Cursor> mouseCursors;
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::SizeAll).value());
	mouseCursors.emplace_back(sf::Cursor::createFromPixels(hourglassImage.getPixelsPtr(), hourglassImage.getSize(), sf::Vector2u{}).value());

	auto mode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(mode, "SF Chess", sf::Style::None);
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(500U);
	sf::View view(window.getDefaultView());
	view.setCenter({window.getSize().x / 2.0f, window.getSize().x / 2.0f});
	view.setRotation(sf::degrees(180.0f));

	// Icons
	sf::Texture* selectedIcon;
	sf::Texture checkersWhite("assets/icon/checkers_white.png"), checkersBlack("assets/icon/checkers_black.png");
	std::vector<sf::Texture> iconTextures;
	addTexture(iconTextures, "assets/icon/1.png", true);
	addTexture(iconTextures, "assets/icon/2.png", true);
	addTexture(iconTextures, "assets/icon/3.png", true);
	addTexture(iconTextures, "assets/icon/4.png", true);
	addTexture(iconTextures, "assets/icon/5.png", true);
	addTexture(iconTextures, "assets/icon/6.png", true);

	if (std::rand() % 50 == 0) {
		if (std::rand() % 2 == 0) { selectedIcon = &checkersWhite; }
		else { selectedIcon = &checkersBlack; }
	}
	else { selectedIcon = &iconTextures.at(std::rand() % iconTextures.size()); }

	// Board  
	bool hourGlass = false, makingBoard = false, isFocused = true, controlClicked = false;
	sf::Image boardSpriteSheet("assets/board/board_matrix.jpg");
	sf::Texture boardTexture(boardSpriteSheet, false, sf::IntRect({ 0, 0 }, { 1024, 1024 }));

	// Pieces
	std::vector<sf::Image> pieceSpriteSheets;
	addImage(pieceSpriteSheets, "assets/piece/pieces_matrix_alpha.png");
	addImage(pieceSpriteSheets, "assets/piece/pieces_matrix_cburnett.png");
	addImage(pieceSpriteSheets, "assets/piece/pieces_matrix_merida.png");
	addImage(pieceSpriteSheets, "assets/piece/pieces_matrix_disguised.png");
	std::vector<sf::Texture> pieceTextures = Chess::makePieceSet(pieceSpriteSheets.front(), 12, 320);

	// Extras
	std::vector<sf::Texture> boardTextures;
	addTexture(boardTextures, "assets/piece/move_gradient.png", true);            // 0
	addTexture(boardTextures, "assets/piece/capture_gradient.png", true);         // 1
	addTexture(boardTextures, "assets/piece/check.png", true);                    // 2
	addTexture(boardTextures, "assets/piece/last_move.png", true);                // 3
	addTexture(boardTextures, "assets/piece/selection_hover.png", true);          // 4
	addTexture(boardTextures, "assets/piece/piece_selection_hover.png", true);    // 5
	addTexture(boardTextures, "assets/piece/promotion_square.png", true);         // 6
	addTexture(boardTextures, "assets/piece/promotion_square_select.png", true);  // 7
	addTexture(boardTextures, "assets/board/koth_background.png", true);          // 8
	addTexture(boardTextures, "assets/board/koth_shadow.png", true);              // 9
	addTexture(boardTextures, "assets/board/rank_background.png", true);          // 10
	addTexture(boardTextures, "assets/board/rank_shadow_top.png", true);          // 11
	addTexture(boardTextures, "assets/board/check_color.png", true);              // 12
	addTexture(boardTextures, "assets/board/rectangle_gradient.png", true);       // 13
	addTexture(boardTextures, "assets/piece/drop_piece_count.png", true);         // 14
	addTexture(boardTextures, "assets/board/rounded_square.png", true);           // 15
	addTexture(boardTextures, "assets/board/rounded_rectangle.png", true);        // 16

	// Sounds
	std::vector<sf::SoundBuffer> soundBuffers;
	addSoundBuffer(soundBuffers, "assets/sound/Move.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/Capture.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/End.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/Explosion.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/LowTime.mp3");

	// Board Customization
	Chess::Variant VARIANT = Chess::Atomic;
	int PIECE_SET = 2;  // 0 - 3
	int BOARD_SET = 19; // 0 - 23
	bool AI = false;
	bool AI_ONLY = false;
	bool CHESS960 = false;
	bool ENDGAME_POSITION = false;
	bool ANIMATED = true;
	bool REPEAT_FEN = false;
	bool TIME_ENABLED = false;
	float X_OFFSET = 0;
	float Y_OFFSET = 0;
	float BOARD_SCALE = 1.0f;
	std::optional<std::string> FEN = "rnb1k1nr/ppp1p1bp/P3q1p1/3pPp2/8/8/1PPP1PPP/RNBQKBNR w KQkq - 0 7";
	std::optional<bool> WHITE = true;	
	sf::Time WHITE_TIME = sf::seconds(15);
	sf::Time BLACK_TIME = sf::seconds(15);
	sf::Time TIME_INCREMENT = sf::seconds(0);

	// Boards
	std::shared_ptr<Board> board = std::make_shared<Board>(VARIANT, FEN, X_OFFSET, Y_OFFSET, 1024, ANIMATED, window.getSize(), BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, boardSpriteSheet, textFont, boardTexture, AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME_POSITION, pieceTextures, boardTextures, soundBuffers, TIME_ENABLED, true, WHITE_TIME, BLACK_TIME, TIME_INCREMENT);
	std::shared_ptr<Board> selectedBoard = board;
	std::vector<std::shared_ptr<Board>> boardList{ board };

	// Vars
	sf::Vector2f mousePos{};
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f{ 200, 200 });
	rect.setPosition({ 0.0f, 0.0f });
	sf::Vector2f randomPos = { float(rand() % 1600), float(rand() % 900) };

	// Icon Setup
	sf::Sprite iconTexture{ *selectedIcon };
	sf::RenderTexture renderTexture(selectedIcon->getSize());
	sf::Image bIcon = makeIconFromBoard(selectedBoard, renderTexture, iconTexture);
	window.setIcon(bIcon.getSize(), bIcon.getPixelsPtr());
	deltaClock.restart();

	while (window.isOpen()) {
		deltaTime = deltaClock.restart();
		float dT = deltaTime.asSeconds();
		mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
		if (isFocused) {
			for (auto& b : boardList) {
				if (b && b->intersects((sf::Vector2f)mousePos)) {
					if (b != selectedBoard) {
						sf::Image newIcon = makeIconFromBoard(b, renderTexture, iconTexture);
						window.setIcon(newIcon.getSize(), newIcon.getPixelsPtr());
					}
					selectedBoard = b;
				}
			}
			if (selectedBoard) {
				selectedBoard->updateS(mousePos, isFocused);
			}
		}
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else {
				if (selectedBoard) { selectedBoard->handleEvent(event, mousePos, boardSpriteSheet, pieceSpriteSheets, isFocused, controlClicked); }
				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (isFocused) {
						if (keyPressed->code == sf::Keyboard::Key::B) {
							// Set Icon
							if (selectedBoard) {
								sf::Image newIcon = makeIconFromBoard(selectedBoard, renderTexture, iconTexture);
								window.setIcon(newIcon.getSize(), newIcon.getPixelsPtr());
							}
						}
						else if (keyPressed->code == sf::Keyboard::Key::Y) {
							if (!makingBoard) {
								for (auto& board : boardList) { board->mouseModeOff(); }
								makingBoard = true;
								std::thread makeBoardThread(makeBoard, std::ref(selectedBoard), VARIANT, FEN, X_OFFSET, Y_OFFSET, 1024, ANIMATED, window.getSize(), BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, std::ref(boardSpriteSheet), std::ref(textFont), std::ref(boardTexture), AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME_POSITION, std::ref(pieceTextures), std::ref(boardTextures), std::ref(soundBuffers), TIME_ENABLED, WHITE_TIME, BLACK_TIME, TIME_INCREMENT, std::ref(boardList), std::ref(makingBoard));
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
					if (keyPressed->code == sf::Keyboard::Key::LControl) {
						controlClicked = true;
					}
				} 
				else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
					if (keyReleased->code == sf::Keyboard::Key::LControl) {
						controlClicked = false;
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
			b->update(mousePos, isFocused, dT);
			b->draw(window);
		}
		if (makingBoard) { window.setMouseCursor(mouseCursors.at(3)); }
		else if (selectedBoard) { window.setMouseCursor(mouseCursors.at(selectedBoard->getMouseCursor(hourGlass))); }

		window.display();
	}
}

void block_until_gained_focus(sf::Window& window)
{
	while (const std::optional event = window.waitEvent()) {
		if (event->is<sf::Event::FocusGained>()) {
			return;
		}
	}
}

void addTexture(std::vector<sf::Texture>& textures, const std::string& path, bool smooth)
{
	sf::Texture texture;
	if (texture.loadFromFile(path)) {
		texture.setSmooth(smooth);
		textures.push_back(std::move(texture));
	}
	else {
		std::cout << "Failed to load texture from: " << path << std::endl;
	}
}

void addImage(std::vector<sf::Image>& images, const std::string& path)
{
	sf::Image image;
	if (image.loadFromFile(path)) {
		images.push_back(std::move(image));
	}
	else {
		std::cout << "Failed to load image from: " << path << std::endl;
	}
}

void addSoundBuffer(std::vector<sf::SoundBuffer>& soundBuffers, const std::string& path)
{
	sf::SoundBuffer soundBuffer;
	if (soundBuffer.loadFromFile(path)) {
		soundBuffers.push_back(std::move(soundBuffer));
	}
	else {
		std::cout << "Failed to load sound buffer from: " << path << std::endl;
	}
}

sf::Image makeIconFromBoard(std::shared_ptr<Board>& board, sf::RenderTexture& renderTexture, sf::Sprite& iconTexture)
{
	sf::Texture boardT = board->getBoardT();
	sf::Sprite currentBoardC{ boardT, sf::IntRect{ {0, 0}, {128 * 3, 128 * 3} } };
	currentBoardC.setOrigin(currentBoardC.getLocalBounds().getCenter());
	currentBoardC.setPosition({ renderTexture.getSize().x / 2.0f, renderTexture.getSize().y / 2.0f });
	renderTexture.clear();
	renderTexture.draw(currentBoardC);
	renderTexture.draw(iconTexture);
	renderTexture.display();
	return renderTexture.getTexture().copyToImage();
}

void makeBoard(std::shared_ptr<Board>& selectedBoard, Chess::Variant VARIANT, std::optional<std::string> FEN, float X_OFFSET, float Y_OFFSET, int boardSize, bool animated, sf::Vector2u windowSize, float BOARD_SCALE, int BOARD_SET, int PIECE_SET, std::vector<sf::Image> pieceSpriteSheets, sf::Image& boardSpriteSheet, sf::Font& textFont, sf::Texture& boardTexture, bool AI, bool AI_ONLY, std::optional<bool> WHITE, bool REPEAT_FEN, bool CHESS960, bool ENDGAME, std::vector<sf::Texture>& pieceTextures, std::vector<sf::Texture>& boardTextures, std::vector<sf::SoundBuffer>& soundBuffers, bool TIME_ENABLED, sf::Time WHITE_TIME, sf::Time BLACK_TIME, sf::Time TIME_INCREMENT, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard)
{
	std::shared_ptr<Board> board = std::make_shared<Board>(VARIANT, FEN, X_OFFSET, Y_OFFSET, boardSize, animated, windowSize, BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, boardSpriteSheet, textFont, boardTexture, AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME, pieceTextures, boardTextures, soundBuffers, TIME_ENABLED, false, WHITE_TIME, BLACK_TIME, TIME_INCREMENT);
	boardList.push_back(board);
	selectedBoard = board;
	makingBoard = false;
}