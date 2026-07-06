/*
	SF Chess, a Chess GUI which supports many chess variants
	Copyright (C) 2026  demoman2 (https://github.com/demoman2)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Main.h"
#include "Variant.h"

int main()
{
	std::cout << std::boolalpha;
	std::srand(time(0));
	sf::Clock deltaClock;
	sf::Time deltaTime = sf::Time::Zero;
	sf::Font textFont("assets/font/Roboto-Medium.ttf");
	std::vector<sf::Cursor> mouseCursors;
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::SizeAll).value());
	mouseCursors.emplace_back(sf::Cursor::createFromSystem(sf::Cursor::Type::NotAllowed).value());

	auto mode = sf::VideoMode::getDesktopMode();
	const unsigned int hardwareConcurrency = std::thread::hardware_concurrency();
	sf::RenderWindow window(mode, "SF Chess", sf::Style::None, sf::State::Windowed);
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);

	// Icons
	sf::Texture* selectedIcon;
	std::vector<sf::Texture> iconTextures;
	addTexture(iconTextures, "assets/icon/1.png", true);
	addTexture(iconTextures, "assets/icon/2.png", true);
	addTexture(iconTextures, "assets/icon/3.png", true);
	addTexture(iconTextures, "assets/icon/4.png", true);
	addTexture(iconTextures, "assets/icon/5.png", true);
	addTexture(iconTextures, "assets/icon/6.png", true);

	selectedIcon = &iconTextures.at(std::rand() % iconTextures.size()); 

	// Board  
	bool blockCursor = false, makingBoard = false, isFocused = true, controlClicked = false, shiftClicked = false;
	sf::Image boardSpriteSheet("assets/board/board_matrix.jpg");
	sf::Texture bGUIPieceBackground("assets/ui/textures/white.jpg");
	sf::Texture bGUIGradient("assets/ui/textures/flipped_gradient.png");
	sf::Texture aGUIGradient("assets/ui/textures/gradient_2.png");
	sf::Image bAIGradient = makeAIStrengthTexture(bGUIGradient, sf::Color(240, 218, 181));
	sf::Image sAIGradient = makeAIStrengthTexture(bGUIGradient, sf::Color(240, 218, 181));

	// Pieces
	const std::string piecePath = "assets\\piece\\normal", piecePathSVG = "assets\\ui\\svg\\normal";
	const std::string piecePathVariant = "assets\\piece\\variants", piecePathVariantSVG = "assets\\ui\\svg\\variants";
	std::vector<std::map<char, sf::Texture>> pieceTextures;

	for (const auto& entry : std::filesystem::directory_iterator(piecePath)) {
		pieceTextures.push_back(Chess::loadPieceSet(entry.path().string()));
	}
	if (pieceTextures.size() > 2) {
		std::swap(pieceTextures.at(1), pieceTextures.at(2));
	}

	std::map<std::string, std::map<char, sf::Texture>> variantPieceTextures;
	for (const auto& entry : std::filesystem::directory_iterator(piecePathVariant)) {
		auto path = entry.path().string();
		std::string vName = path.substr(path.find_last_of('\\') + 1);
		variantPieceTextures.emplace(vName, Chess::loadPieceSet(path, piecePath + "\\merida"));
	}

	std::vector<std::map<char, tgui::Texture>> pieceTexturesSVG;

	for (const auto& entry : std::filesystem::directory_iterator(piecePathSVG)) {
		pieceTexturesSVG.push_back(Chess::loadPieceSetSVG(entry.path().string()));
	}
	if (pieceTexturesSVG.size() > 2) {
		std::swap(pieceTexturesSVG.at(1), pieceTexturesSVG.at(2));
	}

	std::map<std::string, std::map<char, tgui::Texture>> variantPieceTexturesSVG;
	for (const auto& entry : std::filesystem::directory_iterator(piecePathVariantSVG)) {
		auto path = entry.path().string();
		std::string vName = path.substr(path.find_last_of('\\') + 1);
		variantPieceTexturesSVG.emplace(vName, Chess::loadPieceSetSVG(path, piecePathSVG + "\\merida"));
	}

	sf::Clock testClock;

	// Extras
	std::vector<sf::Texture> boardTextures;
	addTexture(boardTextures, "assets/board/move_gradient.png", true);            // 0
	addTexture(boardTextures, "assets/board/capture_gradient.png", true);         // 1
	addTexture(boardTextures, "assets/board/check.png", true);                    // 2
	addTexture(boardTextures, "assets/board/last_move.png", true);                // 3
	addTexture(boardTextures, "assets/board/selection_hover.png", true);          // 4
	addTexture(boardTextures, "assets/board/piece_selection_hover.png", true);    // 5
	addTexture(boardTextures, "assets/board/promotion_square.png", true);         // 6
	addTexture(boardTextures, "assets/board/promotion_square_select.png", true);  // 7
	addTexture(boardTextures, "assets/board/koth_background.png", true);           // 8
	addTexture(boardTextures, "assets/board/koth_shadow.png", true);               // 9
	addTexture(boardTextures, "assets/board/rank_background.png", true);           // 10
	addTexture(boardTextures, "assets/board/rank_shadow_top.png", true);           // 11
	addTexture(boardTextures, "assets/misc/check_color.png", true);               // 12
	addTexture(boardTextures, "assets/misc/rectangle_gradient.png", true);        // 13
	addTexture(boardTextures, "assets/misc/drop_piece_count.png", true);         // 14
	addTexture(boardTextures, "assets/misc/rounded_square.png", true);            // 15
	addTexture(boardTextures, "assets/misc/rounded_rectangle.png", true);         // 16
	addTexture(boardTextures, "assets/misc/invisible.png", true);				  // 17

	std::vector<sf::Texture> backgroundTextures;
	size_t backgroundIndex = 0;
	addTexture(backgroundTextures, "assets/background/landscape.jpg", true);            // 0
	addTexture(backgroundTextures, "assets/background/black.jpg", true);            // 1
	addTexture(backgroundTextures, "assets/background/gradient.jpg", true);            // 2
	addTexture(backgroundTextures, "assets/background/white.jpg", true);            // 3

	const sf::Texture* backgroundTexture = &backgroundTextures.at(backgroundIndex);
	sf::Sprite backgroundSprite(*backgroundTexture);
	backgroundSprite.setScale(sf::Vector2f{ window.getSize().x / static_cast<float>(backgroundTexture->getSize().x),
											window.getSize().y / static_cast<float>(backgroundTexture->getSize().y)});
	backgroundSprite.setOrigin(backgroundSprite.getLocalBounds().getCenter());
	backgroundSprite.setPosition({ window.getSize().x / 2.0f, window.getSize().y / 2.0f });

	// Sounds
	std::vector<sf::SoundBuffer> soundBuffers;
	addSoundBuffer(soundBuffers, "assets/sound/Move.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/Capture.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/End.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/Explosion.mp3");
	addSoundBuffer(soundBuffers, "assets/sound/LowTime.mp3");

	// Variants
	Chess::VariantList variantList;
	variantList.emplace_back(std::make_shared<Chess::Variant>(Chess::chessStr, variantList));
	addVariants(variantList, "assets/other/variants.ini");
	std::vector<Chess::VariantType> variantTypes = loadVariantTypes("assets/other/variantsettings.txt", variantList);
	std::set<std::string> endgameVariants;
	std::string line;
	std::ifstream vStream("assets/fen/endgame/list.txt");
	while (std::getline(vStream, line, '\n')) {
		endgameVariants.emplace(line);
	}

	// Board Customization
	BoardSettings bP(variantTypes), boardSettings(variantTypes);
	StockfishSettings selectedSFSettings, sfSettings;

	// Boards
	std::shared_ptr<Board> selectedBoard;
	std::vector<std::shared_ptr<Board>> boardList{ };

	auto newBoard = [&]() {
		if (!makingBoard) {
			for (auto& board : boardList) { board->mouseModeOff(); }
			makingBoard = true;
			std::thread makeBoardThread(makeBoard, std::ref(selectedBoard), std::cref(boardSettings), std::cref(sfSettings), std::cref(pieceTextures), std::cref(variantPieceTextures),
				window.getSize(), std::cref(boardSpriteSheet), std::cref(textFont), std::cref(boardTextures), std::cref(soundBuffers),
				std::cref(endgameVariants), std::ref(boardList), std::ref(makingBoard));
			makeBoardThread.detach();
		}
		};

	auto newBoardWith = [&](const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings) {
		if (!makingBoard) {
			for (auto& board : boardList) { board->mouseModeOff(); }
			makingBoard = true;
			std::thread makeBoardThread(makeBoard, std::ref(selectedBoard), std::cref(boardSettings), std::cref(stockfishSettings), std::cref(pieceTextures), std::cref(variantPieceTextures),
				window.getSize(), std::cref(boardSpriteSheet), std::cref(textFont), std::cref(boardTextures), std::cref(soundBuffers),
				std::cref(endgameVariants), std::ref(boardList), std::ref(makingBoard));
			makeBoardThread.detach();
		}
		};

	// GUI
	sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window), previousMousePos = mousePos;
	bool bGUIvisible = false, sGUIvisible = false, fGUIvisible = false, aBGUIVisible = false, asGUIvisible = false, headerVisible = true;
	tgui::Gui boardGUI{ window }, stockfishGUI{ window }, fenGUI{ window }, fenPositionGUI{ window }, positionGUI{ window }, largePositionGUI{ window }, headerGUI{ window }, headerGUI2{ window }, headerGUI3{ window };

	// Advanced
	tgui::Gui advancedBoardPosition{ window }, advancedBoardTime{ window }, advancedBoardAppearance{ window }, advancedBoardAI{ window };
	tgui::Gui advancedStockfish{ window }, advancedStockfish2{ window };

#pragma region Board GUI

	std::function<void(const BoardSettings&, const StockfishSettings&, bool, bool)> loadAdvancedBoard;

	int bMode = 0;
	bool bSharedTime = false, bSide = false, bSideHover = false;

	try {
		boardGUI.loadWidgetsFromFile("assets/ui/board_editor.txt");
	}
	catch (tgui::Exception e) {
		std::cout << e.what();
	}
	boardGUI.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	boardGUI.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	boardGUI.get<tgui::Panel>("Outline")->setPosition("50%", "9.7%");
	boardGUI.get<tgui::Panel>("Outline")->setOrigin(0.5f, 0.0f);
	boardGUI.get<tgui::Panel>("Variant Panel")->setOrigin(0.5f, 0.0f);
	boardGUI.get<tgui::Panel>("Variant Panel")->setPosition("50%", "10%");
	boardGUI.get<tgui::Panel>("Side Panel")->setOrigin(0.5f, 0.0f);
	boardGUI.get<tgui::Panel>("Side Panel")->setPosition("83.5%", "0.99%");
	tgui::Label::Ptr bTitle = boardGUI.get<tgui::Label>("Title");
	tgui::Label::Ptr bVariantTypeDescription = boardGUI.get<tgui::Label>("Variant Type Description");
	tgui::Label::Ptr bVariantDescription = boardGUI.get<tgui::Label>("Variant Description");
	tgui::BitmapButton::Ptr bRandomVariant = boardGUI.get<tgui::BitmapButton>("Randomize Variant");
	tgui::Picture::Ptr bVariantTypeImage = boardGUI.get<tgui::Picture>("Variant Type Image");
	tgui::BitmapButton::Ptr bCloseButton = boardGUI.get<tgui::BitmapButton>("Close Button");
	tgui::TextArea::Ptr bFENArea = boardGUI.get<tgui::TextArea>("FEN Area");
	tgui::Picture::Ptr bFENImage = boardGUI.get<tgui::Picture>("FEN Image");
	tgui::Picture::Ptr bLargeFENImage = boardGUI.get<tgui::Picture>("Large FEN Image");
	tgui::ComboBox::Ptr bVariantComboBox = boardGUI.get<tgui::ComboBox>("VariantBox");
	tgui::CheckBox::Ptr bChess960 = boardGUI.get<tgui::CheckBox>("Chess960 CheckBox");
	tgui::ComboBox::Ptr bTimeBox = boardGUI.get<tgui::ComboBox>("Time Box");
	tgui::Label::Ptr bTimeWALabel = boardGUI.get<tgui::Label>("White Time Label");
	tgui::Label::Ptr bTimeBPLabel = boardGUI.get<tgui::Label>("Black Time Label");
	tgui::Label::Ptr bTimeIncrLabel = boardGUI.get<tgui::Label>("Increment Label");
	tgui::EditBox::Ptr bTimeWA = boardGUI.get<tgui::EditBox>("White Time Box");
	tgui::EditBox::Ptr bTimeBP = boardGUI.get<tgui::EditBox>("Black Time Box");
	tgui::EditBox::Ptr bTimeIncr = boardGUI.get<tgui::EditBox>("Increment Box");
	tgui::BitmapButton::Ptr bTimeWAUnit = boardGUI.get<tgui::BitmapButton>("White Time Unit");
	tgui::BitmapButton::Ptr bTimeBPUnit = boardGUI.get<tgui::BitmapButton>("Black Time Unit");
	tgui::BitmapButton::Ptr bTimeIncrUnit = boardGUI.get<tgui::BitmapButton>("Increment Unit");
	tgui::BitmapButton::Ptr bCloseVariant = boardGUI.get<tgui::BitmapButton>("Close Variant");
	tgui::ToggleButton::Ptr bStockfishL1 = boardGUI.get<tgui::ToggleButton>("1");
	tgui::ToggleButton::Ptr bStockfishL2 = boardGUI.get<tgui::ToggleButton>("2");
	tgui::ToggleButton::Ptr bStockfishL3 = boardGUI.get<tgui::ToggleButton>("3");
	tgui::ToggleButton::Ptr bStockfishL4 = boardGUI.get<tgui::ToggleButton>("4");
	tgui::ToggleButton::Ptr bStockfishL5 = boardGUI.get<tgui::ToggleButton>("5");
	tgui::ToggleButton::Ptr bStockfishL6 = boardGUI.get<tgui::ToggleButton>("6");
	tgui::ToggleButton::Ptr bStockfishL7 = boardGUI.get<tgui::ToggleButton>("7");
	tgui::ToggleButton::Ptr bStockfishL8 = boardGUI.get<tgui::ToggleButton>("8");
	tgui::ToggleButton::Ptr bStockfishL9 = boardGUI.get<tgui::ToggleButton>("9");
	std::vector<tgui::ToggleButton::Ptr> bStockfishLevels = { bStockfishL1, bStockfishL2, bStockfishL3, bStockfishL4, bStockfishL5, bStockfishL6, bStockfishL7, bStockfishL8, bStockfishL9 };
	tgui::ComboBox::Ptr bWhiteBox = boardGUI.get<tgui::ComboBox>("White Box");
	tgui::ComboBox::Ptr bBlackBox = boardGUI.get<tgui::ComboBox>("Black Box");
	tgui::EditBox::Ptr bBoardSet = boardGUI.get<tgui::EditBox>("Board Set");
	tgui::EditBox::Ptr bPieceSet = boardGUI.get<tgui::EditBox>("Piece Set");
	tgui::BitmapButton::Ptr bBoardSetMinus = boardGUI.get<tgui::BitmapButton>("Board Set Minus");
	tgui::BitmapButton::Ptr bBoardSetPlus = boardGUI.get<tgui::BitmapButton>("Board Set Plus");
	tgui::BitmapButton::Ptr bVariantButton = boardGUI.get<tgui::BitmapButton>("Variant Button");
	tgui::Label::Ptr bBoardSetMinusL = boardGUI.get<tgui::Label>("Minus Label 1");
	tgui::Label::Ptr bBoardSetPlusL = boardGUI.get<tgui::Label>("Plus Label 1");
	tgui::BitmapButton::Ptr bPieceSetMinus = boardGUI.get<tgui::BitmapButton>("Piece Set Minus");
	tgui::BitmapButton::Ptr bPieceSetPlus = boardGUI.get<tgui::BitmapButton>("Piece Set Plus");
	tgui::Label::Ptr bPieceSetMinusL = boardGUI.get<tgui::Label>("Minus Label 2");
	tgui::Label::Ptr bPieceSetPlusL = boardGUI.get<tgui::Label>("Plus Label 2");
	tgui::Picture::Ptr bBoardSetImage = boardGUI.get<tgui::Picture>("Board Set Image");
	tgui::Picture::Ptr bPieceSetImage = boardGUI.get<tgui::Picture>("Piece Set Image");
	tgui::BitmapButton::Ptr bCancelButton = boardGUI.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr bSaveButton = boardGUI.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr bVariantTypeButton = boardGUI.get<tgui::BitmapButton>("Variant Type Button");
	tgui::Panel::Ptr bMainPanel = boardGUI.get<tgui::Panel>("Panel");
	tgui::Panel::Ptr bVariantPanel = boardGUI.get<tgui::Panel>("Variant Panel");
	tgui::Panel::Ptr bSidePanel = boardGUI.get<tgui::Panel>("Side Panel");
	tgui::Panel::Ptr bTopPanel = boardGUI.get<tgui::Panel>("Round Top");
	tgui::Panel::Ptr bPieceSetBackground = boardGUI.get<tgui::Panel>("Piece Set Background");
	tgui::Panel::Ptr bVariantShadow = boardGUI.get<tgui::Panel>("Shadow 2");
	tgui::Panel::Ptr bVariantOutline = boardGUI.get<tgui::Panel>("Outline");
	tgui::CheckBox::Ptr bRepeatPosition = boardGUI.get<tgui::CheckBox>("Repeat Position CheckBox");
	tgui::RadioButton::Ptr bWhiteRadioButton = boardGUI.get<tgui::RadioButton>("White Radio Button");
	tgui::RadioButton::Ptr bBlackRadioButton = boardGUI.get<tgui::RadioButton>("Black Radio Button");
	tgui::RadioButton::Ptr bRandomRadioButton = boardGUI.get<tgui::RadioButton>("Random Radio Button");
	tgui::RadioButton::Ptr bCycleRadioButton = boardGUI.get<tgui::RadioButton>("Cycle Radio Button");
	tgui::TextArea::Ptr bRepeatFENArea = boardGUI.get<tgui::TextArea>("Repeat FEN Area");
	tgui::Label::Ptr bPlayerSideL = boardGUI.get<tgui::Label>("Player Side");
	tgui::Label::Ptr bResetL = boardGUI.get<tgui::Label>("On Restart");
	tgui::Label::Ptr bResetWhiteL = boardGUI.get<tgui::Label>("Reset White");
	tgui::Label::Ptr bResetBlackL = boardGUI.get<tgui::Label>("Reset Black");
	tgui::Label::Ptr bResetRandomL = boardGUI.get<tgui::Label>("Reset Random");
	tgui::Label::Ptr bCycleL = boardGUI.get<tgui::Label>("Cycle Label");
	tgui::BitmapButton::Ptr bAdvanced = boardGUI.get<tgui::BitmapButton>("Advanced");
	tgui::BitmapButton::Ptr bStartpos = boardGUI.get<tgui::BitmapButton>("startpos");
	bSaveButton->setOrigin(1, 0);
	bSaveButton->setPosition("97.56%", "95.09%");
	bFENArea->setTabString("/");
	bRepeatFENArea->setTabString("/");

	GUIBoard FENBoard{ bFENImage, positionGUI }, largeFENBoard{ bLargeFENImage, largePositionGUI };
	std::ostringstream stream, timeStream;
	timeStream << std::fixed << std::setprecision(2);
	stream << std::fixed << std::setprecision(3);
	bSidePanel->setVisible(false);
	bTimeWAUnit->setUserData(1);
	bTimeBPUnit->setUserData(1);
	bTimeIncrUnit->setUserData(0);

	bVariantComboBox->setItemsToDisplay(12);

	auto setResetButtons = [&]() {
		bWhiteRadioButton->setEnabled(bP.AI && !bP.AI_Only);
		bBlackRadioButton->setEnabled(bP.AI && !bP.AI_Only);
		bRandomRadioButton->setEnabled(bP.AI && !bP.AI_Only);
		bCycleRadioButton->setEnabled(bP.AI && !bP.AI_Only);
		std::string textCol = (bP.AI && !bP.AI_Only) ? "#3C3C3C" : "#9E9E9E";
		bPlayerSideL->getRenderer()->setTextColor(tgui::String(textCol));
		bResetL->getRenderer()->setTextColor(tgui::String(textCol));
		bResetWhiteL->getRenderer()->setTextColor(tgui::String(textCol));
		bResetBlackL->getRenderer()->setTextColor(tgui::String(textCol));
		bResetRandomL->getRenderer()->setTextColor(tgui::String(textCol));
		bCycleL->getRenderer()->setTextColor(tgui::String(textCol));
		};

	bFENArea->onTextChange([&](const tgui::String& text) {
		bool reversed = false;
		if (text.empty()) { bP.FEN = {}; }
		else {
			bP.FEN = text.toStdString();
			std::string FEN = bP.FEN.value();
			size_t find = FEN.find(' ');
			if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
				reversed = true;
			}
		}
		auto& variant = bP.variantList->at(bP.variantIndex);
		auto& pTexturesSVG = variantPieceTextures.find(variant->name) != variantPieceTextures.end() ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(bP.pieceSet);
		FENBoard.setPieces(bP.FEN.value_or(""), *bP.variantList->at(bP.variantIndex), pTexturesSVG, reversed);
		largeFENBoard.setPieces(bP.FEN.value_or(""), *bP.variantList->at(bP.variantIndex), pTexturesSVG, reversed);
		});
	bFENArea->onUnfocus([&]() {
		if (bSide) { bSideHover = true; }
		});
	bSidePanel->onMouseEnter([&]() { bSide = true; });
	bSidePanel->onMouseLeave([&]() { bSideHover = false; bSide = false; });
	bChess960->onCheck([&]() { bP.chess960 = true; });
	bChess960->onUncheck([&]() { bP.chess960 = false; });
	bTimeBox->onItemSelect([&](int index) {
		switch (index) {
		case 0:
			bP.sharedTime = false;
			bP.timeEnabled = false;
			bP.AI_Time = false;
			bTimeWALabel->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			bTimeBPLabel->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			bTimeIncrLabel->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			bTimeWALabel->setText("White Time:");
			bTimeBPLabel->setText("Black Time:");
			bTimeBP->setEnabled(false);
			bTimeWA->setEnabled(false);
			bTimeIncr->setEnabled(false);
			bTimeWAUnit->setEnabled(false);
			bTimeBPUnit->setEnabled(false);
			bTimeIncrUnit->setEnabled(false);
			break;
		case 1:
			bP.sharedTime = false;
			bP.sharedTime = true;
			bP.timeEnabled = true;
			bP.AI_Time = false;
			bTimeWALabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeBPLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeIncrLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeWALabel->setText("White Time:");
			bTimeBPLabel->setText("Black Time:");
			bTimeBP->setEnabled(true);
			bTimeWA->setEnabled(true);
			bTimeIncr->setEnabled(true);
			bTimeWAUnit->setEnabled(true);
			bTimeBPUnit->setEnabled(true);
			bTimeIncrUnit->setEnabled(true);
			bTimeBP->setText(bTimeWA->getText());
			bTimeBPUnit->setText(bTimeWAUnit->getText());
			bTimeBPUnit->setUserData(bTimeWAUnit->getUserData<int>());
			break;
		case 2:
			bP.sharedTime = false;
			bP.timeEnabled = true;
			bP.AI_Time = false;
			bTimeWALabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeBPLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeIncrLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeWALabel->setText("White Time:");
			bTimeBPLabel->setText("Black Time:");
			bTimeBP->setEnabled(true);
			bTimeWA->setEnabled(true);
			bTimeIncr->setEnabled(true);
			bTimeWAUnit->setEnabled(true);
			bTimeBPUnit->setEnabled(true);
			bTimeIncrUnit->setEnabled(true);
			break;
		case 3:
			bP.timeEnabled = true;
			bP.AI_Time = true;
			bTimeWALabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeBPLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeIncrLabel->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
			bTimeWALabel->setText("AI Time:");
			bTimeBPLabel->setText("Player Time:");
			bTimeBP->setEnabled(true);
			bTimeWA->setEnabled(true);
			bTimeIncr->setEnabled(true);
			bTimeWAUnit->setEnabled(true);
			bTimeBPUnit->setEnabled(true);
			bTimeIncrUnit->setEnabled(true);
			break;
		}
		});
	bTimeWAUnit->onPress([&]() {
		bTimeWAUnit->setUserData((bTimeWAUnit->getUserData<int>() + 1) % 3);
		bTimeWA->setText(bTimeWA->getText());
		switch (bTimeWAUnit->getUserData<int>()) {
		case 0:
			bTimeWAUnit->setText("s");
			break;
		case 1:
			bTimeWAUnit->setText("min");
			break;
		case 2:
			bTimeWAUnit->setText("hrs");
			break;
		}
		if (bTimeBox->getSelectedItemIndex() == 1) {
			bTimeBPUnit->setUserData(bTimeWAUnit->getUserData<int>());
			bTimeBPUnit->setText(bTimeWAUnit->getText());
			bTimeBP->setText(bTimeWA->getText());
		}
		bP.whiteUnit = bTimeWAUnit->getUserData<int>();
		});
	bTimeBPUnit->onPress([&]() {
		bTimeBPUnit->setUserData((bTimeBPUnit->getUserData<int>() + 1) % 3);
		bTimeBP->setText(bTimeBP->getText());
		switch (bTimeBPUnit->getUserData<int>()) {
		case 0:
			bTimeBPUnit->setText("s");
			break;
		case 1:
			bTimeBPUnit->setText("min");
			break;
		case 2:
			bTimeBPUnit->setText("hrs");
			break;
		}
		if (bTimeBox->getSelectedItemIndex() == 1) {
			bTimeWAUnit->setUserData(bTimeBPUnit->getUserData<int>());
			bTimeWAUnit->setText(bTimeBPUnit->getText());
			bTimeWA->setText(bTimeBP->getText());
		}
		bP.blackUnit = bTimeBPUnit->getUserData<int>();
		});
	bTimeIncrUnit->onPress([&]() {
		bTimeIncrUnit->setUserData((bTimeIncrUnit->getUserData<int>() + 1) % 2);
		bTimeIncr->setText(bTimeIncr->getText());
		switch (bTimeIncrUnit->getUserData<int>()) {
		case 0: bTimeIncrUnit->setText("s"); break;
		case 1: bTimeIncrUnit->setText("min"); break;
		}
		bP.incrUnit = bTimeIncrUnit->getUserData<int>();
		});
	bTimeWA->onTextChange([&](const tgui::String& text) {
		sf::Time timeBefore = bP.whiteTime;
		float time = text.toFloat();
		if (bTimeBox->getSelectedItemIndex() == 1 && !bSharedTime) { bSharedTime = true; bTimeBP->setText(text); }
		switch (bTimeWAUnit->getUserData<int>()) {
		case 0:
			bP.whiteTime = sf::seconds(time); break;
		case 1:
			bP.whiteTime = sf::seconds(time * 60); break;
		case 2:
			bP.whiteTime = sf::seconds(time * 3600); break;
		}
		if (timeBefore == bP.startingWhiteTime && bP.blackTime == bP.startingBlackTime) {
			bP.startingWhiteTime = bP.whiteTime;
		}
		bSharedTime = false;
		});
	bTimeBP->onTextChange([&](const tgui::String& text) {
		sf::Time timeBefore = bP.blackTime;
		float time = text.toFloat();
		if (bTimeBox->getSelectedItemIndex() == 1 && !bSharedTime) { bSharedTime = true; bTimeWA->setText(text); }
		switch (bTimeBPUnit->getUserData<int>()) {
		case 0: bP.blackTime = sf::seconds(time); break;
		case 1: bP.blackTime = sf::seconds(time * 60); break;
		case 2: bP.blackTime = sf::seconds(time * 3600); break;
		}
		if (bP.whiteTime == bP.startingWhiteTime && timeBefore == bP.startingBlackTime) {
			bP.startingBlackTime = bP.blackTime;
		}
		bSharedTime = false;
		});
	bTimeIncr->onTextChange([&](const tgui::String& text) {
		float time = text.toFloat();
		switch (bTimeIncrUnit->getUserData<int>()) {
		case 0: bP.timeIncrement = sf::seconds(time); break;
		case 1: bP.timeIncrement = sf::seconds(time * 60); break;
		}
		});
	bWhiteBox->onItemSelect([&](int index) {
		if (index == 2) {
			if (bMode == 2) {
				bWhiteBox->onItemSelect.setEnabled(false);
				bWhiteBox->setSelectedItemByIndex(0);
				if (bP.AI && !bP.AI_Only) {
					if (*bP.white) {
						bWhiteBox->setSelectedItemByIndex(0);
					}
					else {
						bWhiteBox->setSelectedItemByIndex(1);
					}
				}
				else if (bP.AI_Only) {
					bWhiteBox->setSelectedItemByIndex(1);
				}
				bWhiteBox->onItemSelect.setEnabled(true);
			}
			else {
				bP.AI = true;
				bP.AI_Only = false;
				bP.white = {};
				bBlackBox->setSelectedItemByIndex(2);
			}
		}
		else {
			if (bBlackBox->getSelectedItemIndex() == 0) {
				if (bWhiteBox->getSelectedItemIndex() == 0) {
					bP.AI = false;
					bP.AI_Only = false;
					bP.white = {};
				}
				else if (bWhiteBox->getSelectedItemIndex() == 1) {
					bP.AI = true;
					bP.AI_Only = false;
					bP.white = false;
				}
			}
			else if (bBlackBox->getSelectedItemIndex() == 1) {
				if (bWhiteBox->getSelectedItemIndex() == 0) {
					bP.AI = true;
					bP.AI_Only = false;
					bP.white = true;
				}
				else if (bWhiteBox->getSelectedItemIndex() == 1) {
					bP.AI_Only = true;
					bP.white = {};
				}
			}
			else if (bBlackBox->getSelectedItemIndex() == 2) {
				bBlackBox->setSelectedItemByIndex(0);
			}
		}
		setResetButtons();
		});
	bBlackBox->onItemSelect([&](int index) {
		if (index == 2) {
			if (bMode == 2) {
				bBlackBox->onItemSelect.setEnabled(false);
				bBlackBox->setSelectedItemByIndex(0);
				if (bP.AI && !bP.AI_Only) {
					if (*bP.white) {
						bBlackBox->setSelectedItemByIndex(1);
					}
					else {
						bBlackBox->setSelectedItemByIndex(0);
					}
				}
				else if (bP.AI_Only) {
					bBlackBox->setSelectedItemByIndex(1);
				}
				bBlackBox->onItemSelect.setEnabled(true);
			}
			else {
				bP.AI = true;
				bP.AI_Only = false;
				bP.white = {};
				bWhiteBox->setSelectedItemByIndex(2);
			}
		}
		else {
			if (bWhiteBox->getSelectedItemIndex() == 0) {
				if (bBlackBox->getSelectedItemIndex() == 0) {
					bP.AI = false;
					bP.AI_Only = false;
					bP.white = {};
				}
				else if (bBlackBox->getSelectedItemIndex() == 1) {
					bP.AI = true;
					bP.AI_Only = false;
					bP.white = true;
				}
			}
			else if (bWhiteBox->getSelectedItemIndex() == 1) {
				if (bBlackBox->getSelectedItemIndex() == 0) {
					bP.AI = true;
					bP.AI_Only = false;
					bP.white = false;
				}
				else if (bBlackBox->getSelectedItemIndex() == 1) {
					bP.AI_Only = true;
					bP.white = {};
				}
			}
			else if (bWhiteBox->getSelectedItemIndex() == 2) {
				bWhiteBox->setSelectedItemByIndex(0);
			}
		}
		setResetButtons();
		});

	auto setStockfishLevelB = [&](const tgui::String& name) {
		selectedSFSettings.skillLevel = (name.toInt() * 5) - 25;
		for (auto& stockfishLevel : bStockfishLevels) {
			if (stockfishLevel->getText() != name) {
				stockfishLevel->setDown(false);
			}
			else {
				stockfishLevel->setDown(true);
			}
		}
		};

	for (auto& stockfishLevel : bStockfishLevels) {
		stockfishLevel->onMouseRelease(setStockfishLevelB, stockfishLevel->getText());
	}

	auto setStockfishLevelTexturesB = [&](const sf::Texture& boardT) {
		bAIGradient = makeAIStrengthTexture(bGUIGradient, boardT.copyToImage().getPixel({ 129, 129 }));
		tgui::Texture texture;
		texture.loadFromPixelData(bAIGradient.getSize(), bAIGradient.getPixelsPtr());
		for (auto& stockfishLevel : bStockfishLevels) {
			stockfishLevel->getRenderer()->setTextureDown(texture);
		}
		};

	auto onBoardSetChange = [&](int add = 0) {
		int newInt = std::clamp(bBoardSet->getText().toInt() + add, 1, 24);
		bBoardSet->setText(std::to_string(newInt));
		bBoardSetMinus->setEnabled(true);
		bBoardSetPlus->setEnabled(true);
		bBoardSetMinusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		bBoardSetPlusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		if (newInt <= 1) {
			bBoardSetMinus->setEnabled(false);
			bBoardSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		else if (newInt >= 24) {
			bBoardSetPlus->setEnabled(false);
			bBoardSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		sf::Texture nBoardTexture = Chess::loadBoard(boardSpriteSheet, newInt - 1, { 2, 2 });
		nBoardTexture.setSmooth(true);
		tgui::Texture boardTextureT;
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		bBoardSetImage->getRenderer()->setTexture(boardTextureT);
		nBoardTexture = Chess::loadBoard(boardSpriteSheet, newInt - 1, bP.variantList->at(bP.variantIndex)->boardSize);
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		bFENImage->getRenderer()->setTexture(boardTextureT);
		bLargeFENImage->getRenderer()->setTexture(boardTextureT);
		FENBoard.setBoard(bFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		largeFENBoard.setBoard(bLargeFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		setStockfishLevelTexturesB(nBoardTexture);
		bPieceSetBackground->getRenderer()->setBackgroundColor(nBoardTexture.copyToImage().getPixel({ 1U, 3U }));
		bP.boardSet = newInt - 1;
		};

	auto onPieceSetChange = [&](int add = 0) {
		auto& variant = bP.variantList->at(bP.variantIndex);
		bool variantTexture = variantPieceTextures.find(variant->name) != variantPieceTextures.end();
		int newInt = variantTexture ? 1 : std::clamp(bPieceSet->getText().toInt() + add, 1, static_cast<int>(pieceTextures.size()));
		auto& pTextures = variantTexture ? variantPieceTextures.find(variant->name)->second : pieceTextures.at(newInt - 1);
		auto& pTexturesSVG = variantTexture ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(newInt - 1);
		bPieceSet->setText(std::to_string(newInt));
		bPieceSetMinus->setEnabled(true);
		bPieceSetPlus->setEnabled(true);
		bPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		bPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		if (newInt <= 1 || variantTexture) {
			bPieceSetMinus->setEnabled(false);
			bPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		if (newInt >= pieceTextures.size() || variantTexture) {
			bPieceSetPlus->setEnabled(false);
			bPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		const sf::Texture& pieceTexture = pTextures.begin()->second;
		tgui::Texture pieceTextureT;
		pieceTextureT.loadFromPixelData(pieceTexture.getSize(), pieceTexture.copyToImage().getPixelsPtr());
		bPieceSetImage->getRenderer()->setTexture(pieceTextureT);
		bool reversed = false;
		std::string FEN = bP.FEN.value_or("");
		size_t find = FEN.find(' ');
		if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
			reversed = true;
		}
		FENBoard.setPieces(FEN, *variant, pTexturesSVG, reversed);
		largeFENBoard.setPieces(FEN, *variant, pTexturesSVG, reversed);
		bP.pieceSet = newInt - 1;
		if (variantPieceTextures.find(variant->name) == variantPieceTextures.end()) { bP.pieceSetSave = bP.pieceSet; }
		};

	bBoardSet->onUnfocus(onBoardSetChange);

	bPieceSet->onUnfocus(onPieceSetChange);

	bBoardSetMinus->onClick(onBoardSetChange, -1);
	bBoardSetPlus->onClick(onBoardSetChange, 1);

	bPieceSetMinus->onClick(onPieceSetChange, -1);
	bPieceSetPlus->onClick(onPieceSetChange, 1);

	bRandomVariant->onClick([&]() {
		bVariantComboBox->setSelectedItemByIndex(rand() % bP.variantList->size());
		});

	bVariantComboBox->onItemSelect([&](size_t index) {
		bP.variantIndex = index;
		auto& variant = bP.variantList->at(bP.variantIndex);
		if (variant->description != "") {
			bVariantDescription->setText(variant->description);
		}
		if (bP.saveFEN != std::nullopt && bP.variantIndex == bP.currentVariantIndex && bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			bFENArea->setText(bP.saveFEN.value());
		}
		else { bFENArea->setText(""); }
		bVariantButton->setText(variant->displayName != "" ? variant->displayName : variant->name);

		sf::Texture nBoardTexture = Chess::loadBoard(boardSpriteSheet, bP.boardSet, variant->boardSize);
		tgui::Texture boardTextureT;
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		bFENImage->getRenderer()->setTexture(boardTextureT);
		bLargeFENImage->getRenderer()->setTexture(boardTextureT);
		if (variantPieceTextures.find(variant->name) != variantPieceTextures.end()) {
			bPieceSet->setText("1");
		}
		else {
			bPieceSet->setText(std::to_string(bP.pieceSetSave + 1));
		}
		onPieceSetChange();
		FENBoard.setBoard(bFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		largeFENBoard.setBoard(bLargeFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		bFENArea->setText(bFENArea->getText());
		});

	bVariantTypeButton->onClick([&]() {
		bP.variantTypeIndex = (bP.variantTypeIndex + 1) % variantTypes.size();
		bP.variantList = &variantTypes.at(bP.variantTypeIndex).variants;
		const Chess::VariantType& vType = variantTypes.at(bP.variantTypeIndex);
		bVariantTypeButton->setText(vType.name);
		bVariantTypeDescription->setText(vType.description);

		bVariantComboBox->removeAllItems();
		for (auto& v : vType.variants) {
			bVariantComboBox->addItem(v->displayName != "" ? v->displayName : v->name);
		}

		if (bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			bVariantComboBox->setSelectedItemByIndex(bP.currentVariantIndex);
		}
		else { bVariantComboBox->setSelectedItemByIndex(0); }
		auto& v = bP.variantList->at(bP.variantIndex);
		bVariantButton->setText(v->displayName != "" ? v->displayName : v->name);

		if (bP.saveFEN != std::nullopt && bP.variantIndex == bP.currentVariantIndex && bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			bFENArea->setText(bP.saveFEN.value());
		}
		else { bFENArea->setText(""); }

		});

	bVariantButton->onClick([&]() {
		if (!bVariantPanel->isVisible()) {
			bMainPanel->setIgnoreMouseEvents(true);
			bVariantPanel->setIgnoreMouseEvents(false);
			bVariantPanel->setVisible(true);
			bVariantOutline->setVisible(true);
			bVariantShadow->setVisible(true);
		}
		});

	bCloseVariant->onClick([&]() {
		bMainPanel->setIgnoreMouseEvents(false);
		bVariantPanel->setIgnoreMouseEvents(true);
		bVariantPanel->setVisible(false);
		bVariantOutline->setVisible(false);
		bVariantShadow->setVisible(false);
		});

	bWhiteRadioButton->onCheck([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = true;
		});

	bBlackRadioButton->onCheck([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = false;
		});

	bRandomRadioButton->onCheck([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = std::nullopt;
		});

	bCycleRadioButton->onCheck([&]() {
		bP.cycleSides = true;
		});

	bRepeatPosition->onChange([&](bool c) { bP.repeatFEN = c; bRepeatFENArea->setEnabled(c); });

	bRepeatFENArea->onTextChange([&](const tgui::String& text) {
		bP.newPositionFEN = text.empty() ? std::nullopt : std::make_optional(text.toStdString());
		});

	bCloseButton->onPress([&]() { bGUIvisible = false; });
	bCancelButton->onPress([&]() { bGUIvisible = false; });
	bSaveButton->onPress([&]() {
		if (bMode == 0) {
			boardSettings = bP;
			sfSettings = selectedSFSettings;
		}
		else if (bMode == 1) {
			newBoardWith(bP, selectedSFSettings);
		}
		else if (bMode == 2 && selectedBoard) {
			selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
			selectedBoard->setStockfishSettings(sfSettings);
		}
		bGUIvisible = false;
		});
	bAdvanced->onPress([&]() {
		bGUIvisible = false;
		if (bMode == 2 && !selectedBoard) { return; }
		loadAdvancedBoard(bP, selectedSFSettings, (bMode == 2), (bMode == 1));
		});
	bStartpos->onPress([&]() {
		bFENArea->setText(Chess::getNewFEN(*bP.variantList->at(bP.variantIndex), bP.chess960));
		});

	auto loadBoardGUI = [&](int mode, const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings) {
		bP = boardSettings;
		selectedSFSettings = stockfishSettings;
		bMode = mode;
		if (bMode == 0) {
			bTitle->setText("New Board Settings");
			bTitle->setPosition("20.47%", "2.08%");
			bSaveButton->setText("Save");
		}
		else if (bMode == 1) {
			bTitle->setText("Create Board");
			bTitle->setPosition("28.4%", "2.08%");
			bSaveButton->setText("Create");
		}
		else if (bMode == 2) {
			bTitle->setText("Board Settings");
			bTitle->setPosition("27.76%", "2.08%");
			bSaveButton->setText("Save");
		}
		bP.variantList = &variantTypes.at(bP.variantTypeIndex).variants;
		bFENArea->setText(boardSettings.FEN.value_or(""));
		bChess960->setChecked(boardSettings.chess960);
		if (!boardSettings.timeEnabled) { bTimeBox->setSelectedItemByIndex(0); }
		else {
			if (boardSettings.AI_Time) {
				bTimeBox->setSelectedItemByIndex(3);
			}
			else {
				if (boardSettings.sharedTime && boardSettings.whiteTime == boardSettings.blackTime) {
					bTimeBox->setSelectedItemByIndex(1);

				}
				else {
					bTimeBox->setSelectedItemByIndex(2);
					bP.sharedTime = false;
				}
			}
		}
		const Chess::VariantType& vType = variantTypes.at(bP.variantTypeIndex);
		bVariantTypeButton->setText(vType.name);
		bVariantTypeDescription->setText(vType.description);

		bVariantComboBox->removeAllItems();
		for (auto& v : vType.variants) {
			bVariantComboBox->addItem(v->displayName != "" ? v->displayName : v->name);
		}
		bVariantComboBox->setSelectedItemByIndex(bP.variantIndex);
		auto& v = bP.variantList->at(bP.variantIndex);
		bVariantButton->setText(v->displayName != "" ? v->displayName : v->name);

		bTimeWAUnit->setUserData(boardSettings.whiteUnit);
		switch (bTimeWAUnit->getUserData<int>()) {
		case 0:
			bTimeWA->setText(timeToStr(boardSettings.whiteTime, timeStream));
			bTimeWAUnit->setText("s");
			break;
		case 1:
			bTimeWA->setText(timeToStr(boardSettings.whiteTime, timeStream, 1 / 60.0f));
			bTimeWAUnit->setText("min");
			break;
		case 2:
			bTimeWA->setText(timeToStr(boardSettings.whiteTime, timeStream, 1 / 3600.0f));
			bTimeWAUnit->setText("hrs");
			break;
		}

		bTimeBPUnit->setUserData(boardSettings.blackUnit);
		switch (bTimeBPUnit->getUserData<int>()) {
		case 0:
			bTimeBP->setText(timeToStr(boardSettings.blackTime, timeStream));
			bTimeBPUnit->setText("s");
			break;
		case 1:
			bTimeBP->setText(timeToStr(boardSettings.blackTime, timeStream, 1 / 60.0f));
			bTimeBPUnit->setText("min");
			break;
		case 2:
			bTimeBP->setText(timeToStr(boardSettings.blackTime, timeStream, 1 / 3600.0f));
			bTimeBPUnit->setText("hrs");
			break;
		}

		bTimeIncrUnit->setUserData(boardSettings.incrUnit);
		switch (bTimeIncrUnit->getUserData<int>()) {
		case 0:
			bTimeIncr->setText(timeToStr(boardSettings.timeIncrement, timeStream));
			bTimeIncrUnit->setText("s");
			break;
		case 1:
			bTimeIncr->setText(timeToStr(boardSettings.timeIncrement, timeStream, 1 / 60.0f));
			bTimeIncrUnit->setText("min");
			break;
		}

		if (!boardSettings.white.has_value() && !boardSettings.AI_Only) {
			bWhiteBox->setSelectedItemByIndex(2);
			bBlackBox->setSelectedItemByIndex(2);
		}
		else if (boardSettings.AI && !boardSettings.AI_Only) {
			if (*boardSettings.white) {
				bWhiteBox->setSelectedItemByIndex(0);
				bBlackBox->setSelectedItemByIndex(1);
			}
			else {
				bWhiteBox->setSelectedItemByIndex(1);
				bBlackBox->setSelectedItemByIndex(0);
			}
		}
		else if (boardSettings.AI_Only) {
			bWhiteBox->setSelectedItemByIndex(1);
			bBlackBox->setSelectedItemByIndex(1);
		}

		setStockfishLevelB(std::to_string((stockfishSettings.skillLevel + 25) / 5));

		bBoardSet->setText(std::to_string(boardSettings.boardSet + 1));
		onBoardSetChange();

	 	if (variantPieceTextures.find(v->name) != variantPieceTextures.end()) {
			bPieceSetMinus->setEnabled(false);
			bPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			bPieceSetPlus->setEnabled(false);
			bPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			bPieceSet->setText("1");
			onPieceSetChange();
		}
		else {
			bPieceSet->setText(std::to_string(boardSettings.pieceSet + 1));
			onPieceSetChange();
		}

		bRepeatPosition->setChecked(boardSettings.repeatFEN);
		if (boardSettings.cycleSides) {
			bCycleRadioButton->setChecked(true);
		}
		else {
			if (boardSettings.newPositionWhite.has_value()) {
				if (*boardSettings.newPositionWhite) {
					bWhiteRadioButton->setChecked(true);
				}
				else {
					bBlackRadioButton->setChecked(true);
				}
			}
			else {
				bRandomRadioButton->setChecked(true);
			}
		}
		bRepeatPosition->setChecked(boardSettings.repeatFEN);
		bRepeatFENArea->setText(bP.newPositionFEN.value_or(""));
		largePositionGUI.setOpacity(0.0f);
		setResetButtons();
		bGUIvisible = true;
		};

#pragma endregion 

#pragma region Stockfish GUI

	bool sFromBoard = false;

	stockfishGUI.loadWidgetsFromFile("assets/ui/stockfish_settings.txt");
	stockfishGUI.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	stockfishGUI.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	tgui::CheckBox::Ptr sEnableStockfish = stockfishGUI.get<tgui::CheckBox>("Enable Stockfish");
	tgui::CheckBox::Ptr sAIOnly = stockfishGUI.get<tgui::CheckBox>("AI Only");
	tgui::Label::Ptr sAIOnlyLabel = stockfishGUI.get<tgui::Label>("AI Only Label");
	tgui::EditBox::Ptr sThreads = stockfishGUI.get<tgui::EditBox>("Threads");
	tgui::EditBox::Ptr sHash = stockfishGUI.get<tgui::EditBox>("Hash");
	tgui::SpinButton::Ptr sThreadsSpin = stockfishGUI.get<tgui::SpinButton>("Threads Spin");
	tgui::SpinButton::Ptr sHashSpin = stockfishGUI.get<tgui::SpinButton>("Hash Spin");
	tgui::BitmapButton::Ptr sClearHash = stockfishGUI.get<tgui::BitmapButton>("Clear Hash");
	tgui::ComboBox::Ptr sTimeBox = stockfishGUI.get<tgui::ComboBox>("Time Box");
	tgui::Label::Ptr sFixedTimeLabel = stockfishGUI.get<tgui::Label>("Fixed Time");
	tgui::EditBox::Ptr sFixedTime = stockfishGUI.get<tgui::EditBox>("Fixed Time Box");
	tgui::ToggleButton::Ptr sStockfishL1 = stockfishGUI.get<tgui::ToggleButton>("1");
	tgui::ToggleButton::Ptr sStockfishL2 = stockfishGUI.get<tgui::ToggleButton>("2");
	tgui::ToggleButton::Ptr sStockfishL3 = stockfishGUI.get<tgui::ToggleButton>("3");
	tgui::ToggleButton::Ptr sStockfishL4 = stockfishGUI.get<tgui::ToggleButton>("4");
	tgui::ToggleButton::Ptr sStockfishL5 = stockfishGUI.get<tgui::ToggleButton>("5");
	tgui::ToggleButton::Ptr sStockfishL6 = stockfishGUI.get<tgui::ToggleButton>("6");
	tgui::ToggleButton::Ptr sStockfishL7 = stockfishGUI.get<tgui::ToggleButton>("7");
	tgui::ToggleButton::Ptr sStockfishL8 = stockfishGUI.get<tgui::ToggleButton>("8");
	tgui::ToggleButton::Ptr sStockfishL9 = stockfishGUI.get<tgui::ToggleButton>("9");
	std::vector<tgui::ToggleButton::Ptr> sStockfishLevels = { sStockfishL1, sStockfishL2, sStockfishL3, sStockfishL4, sStockfishL5, sStockfishL6, sStockfishL7, sStockfishL8, sStockfishL9 };
	tgui::BitmapButton::Ptr sClose = stockfishGUI.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr sCancel = stockfishGUI.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr sSave = stockfishGUI.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr sAdvanced = stockfishGUI.get<tgui::BitmapButton>("Advanced");

	auto setStockfishLevelS = [&](const tgui::String& name) {
		selectedSFSettings.skillLevel = (name.toInt() * 5) - 25;
		for (auto& stockfishLevel : sStockfishLevels) {
			if (stockfishLevel->getText() != name) {
				stockfishLevel->setDown(false);
			}
			else {
				stockfishLevel->setDown(true);
			}
		}
		};

	for (auto& stockfishLevel : sStockfishLevels) {
		stockfishLevel->onMouseRelease(setStockfishLevelS, stockfishLevel->getText());
	}

	sEnableStockfish->onChange([&](bool c) {
		bP.AI = c;
		sAIOnly->setEnabled(c);
		if (c) {
			sAIOnlyLabel->getRenderer()->setTextColor("#3C3C3C");
		}
		else {
			sAIOnlyLabel->getRenderer()->setTextColor("#9E9E9E");
			sAIOnly->setChecked(false);
		}
		});
	sAIOnly->onChange([&](bool c) {
		bP.AI_Only = c;
		});

	std::function<void(const BoardSettings&, const StockfishSettings&, bool)> loadAdvancedStockfishGUI;

	sThreads->onTextChange([&]() {
		sThreads->onTextChange.setEnabled(false);
		sThreads->setText(std::to_string(std::clamp(sThreads->getText().toUInt(), 1U, hardwareConcurrency)));
		sThreads->onTextChange.setEnabled(true);
		selectedSFSettings.sfThreads = sThreads->getText().toInt();
		});
	sThreadsSpin->onValueChange([&](float value) {
		sThreadsSpin->onValueChange.setEnabled(false);
		sThreadsSpin->setValue(1);
		sThreadsSpin->onValueChange.setEnabled(true);
		sThreads->setText(std::to_string(int((value - 1) + selectedSFSettings.sfThreads)));
		});
	sHash->onTextChange([&]() {
		sHash->onTextChange.setEnabled(false);
		sHash->setText(std::to_string(std::clamp(sHash->getText().toInt(), 1, 4096)));
		sHash->onTextChange.setEnabled(true);
		selectedSFSettings.sfMemory = sHash->getText().toInt();
		});
	sHashSpin->onValueChange([&](float value) {
		sHashSpin->onValueChange.setEnabled(false);
		sHashSpin->setValue(1);
		sHashSpin->onValueChange.setEnabled(true);
		sHash->setText(std::to_string(int((value - 1) + selectedSFSettings.sfMemory)));
		});
	sClearHash->onClick([&]() {
		if (selectedBoard) {
			selectedBoard->getEngine().clearHash();
		}
		});
	sAdvanced->onPress([&]() {
		if (sFromBoard && !selectedBoard) { return; }
		loadAdvancedStockfishGUI(bP, selectedSFSettings, sFromBoard);
		sGUIvisible = false;
		});

	sTimeBox->onItemSelect([&](int index) {
		selectedSFSettings.usesFixedTime = (bool)index;
		});

	sFixedTime->onTextChange([&](const tgui::String& text) {
		selectedSFSettings.fixedTime = sf::seconds(text.toFloat());
		});

	auto setStockfishLevelTexturesS = [&](const sf::Texture& boardT) {
		sAIGradient = makeAIStrengthTexture(bGUIGradient, boardT.copyToImage().getPixel({ 129, 129 }));
		tgui::Texture texture;
		texture.loadFromPixelData(sAIGradient.getSize(), sAIGradient.getPixelsPtr());
		for (auto& stockfishLevel : sStockfishLevels) {
			stockfishLevel->getRenderer()->setTextureDown(texture);
		}
		};

	sSave->onClick([&]() {
		if (sFromBoard && selectedBoard) {
			selectedBoard->setStockfishSettings(selectedSFSettings);
			selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
		}
		else if (!sFromBoard) {
			sfSettings = selectedSFSettings;
			boardSettings = bP;
		}
		sGUIvisible = false;
		});

	sClose->onClick([&]() { sGUIvisible = false; });
	sCancel->onClick([&]() { sGUIvisible = false; });

	auto loadStockfishGUI = [&](const BoardSettings& boardSettings, const StockfishSettings& settings, bool fromBoard) {
		selectedSFSettings = settings;
		bP = boardSettings;
		sFromBoard = fromBoard;
		sEnableStockfish->setChecked(boardSettings.AI);
		sAIOnly->setChecked(boardSettings.AI_Only);
		sThreads->setText(std::to_string(settings.sfThreads));
		sHash->setText(std::to_string(settings.sfMemory));
		sTimeBox->setSelectedItemByIndex((int)settings.usesFixedTime);
		sClearHash->setEnabled(fromBoard);
		sFixedTime->setText(timeToStr(settings.fixedTime, timeStream));

		if (fromBoard && selectedBoard) {
			setStockfishLevelTexturesS(selectedBoard->getBoardTexture());
		}
		else if (!fromBoard) {
			setStockfishLevelTexturesS(Chess::loadBoard(boardSpriteSheet, boardSettings.boardSet, { 8, 8 }));
		}
		setStockfishLevelS(std::to_string((settings.skillLevel + 25) / 5));
		sGUIvisible = true;
		};


#pragma endregion 

#pragma region FEN GUI

	fenGUI.loadWidgetsFromFile("assets/ui/load_fen.txt");
	fenGUI.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	fenGUI.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	tgui::TextArea::Ptr fFENArea = fenGUI.get<tgui::TextArea>("FEN Area");
	tgui::Picture::Ptr fBoard = fenGUI.get<tgui::Picture>("Board");
	tgui::BitmapButton::Ptr fClose = fenGUI.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr fCancel = fenGUI.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr fLoadPosition = fenGUI.get<tgui::BitmapButton>("Load Position");
	GUIBoard fBoardGUI(fBoard, fenPositionGUI);

	fFENArea->onTextChange([&](const tgui::String& text) {
		if (selectedBoard) {
			bool reversed = false;
			if (text.empty()) { bP.FEN = {}; }
			else {
				bP.FEN = text.toStdString();
				std::string FEN = bP.FEN.value();
				size_t find = FEN.find(' ');
				if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
					reversed = true;
				}
			}
			auto& variant = bP.variantList->at(bP.variantIndex);
			auto& pTexturesSVG = variantPieceTextures.find(variant->name) != variantPieceTextures.end() ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(bP.pieceSet);
			fBoardGUI.setPieces(text.toStdString(), *variant, pTexturesSVG);
		}
		});

	fClose->onClick([&]() { fGUIvisible = false; });

	fCancel->onClick([&]() { fGUIvisible = false; });

	fLoadPosition->onClick([&]() { if (selectedBoard) { selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet); fGUIvisible = false; } });

	auto loadFENGUI = [&](const BoardSettings& boardSettings) {
		bP = boardSettings;
		bP.variantList = &variantTypes.at(bP.variantTypeIndex).variants;
		fFENArea->setText("");
		if (selectedBoard) {
			tgui::Texture fBoardTexture;
			fBoardTexture.loadFromPixelData(selectedBoard->getBoardTexture().getSize(), selectedBoard->getBoardTexture().copyToImage().getPixelsPtr());
			fBoard->getRenderer()->setTexture(fBoardTexture);
			fBoardGUI.setBoard(fBoard, bP.variantList->at(bP.variantIndex)->boardSize);
		}
		fGUIvisible = true;
		};

#pragma endregion

#pragma region Advanced Board GUI

	bool aFromBoard = false, aCreateBoard = false;
	int loadedGUI = 0;

	auto saveAdvancedBoard = [&]() {
		if (aCreateBoard) {
			newBoardWith(bP, selectedSFSettings);
		}
		else {
			if (aFromBoard && selectedBoard) {
				selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
				selectedBoard->setStockfishSettings(selectedSFSettings);
			}
			else if (!aFromBoard) {
				boardSettings = bP;
				sfSettings = selectedSFSettings;
			}
		}
		aBGUIVisible = false;
		};

	std::function<void(const sf::Texture&)> setAdvancedBoardTextures;

#pragma region Time

	bool fromShared = false;
	int timeMode = 0;

	advancedBoardTime.loadWidgetsFromFile("assets/ui/advanced_board_time.txt");
	advancedBoardTime.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedBoardTime.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedBoardTime.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedBoardTime.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::ToggleButton::Ptr aTimeOff = advancedBoardTime.get<tgui::ToggleButton>("Off");
	tgui::ToggleButton::Ptr aTimeSingle = advancedBoardTime.get<tgui::ToggleButton>("Single");
	tgui::ToggleButton::Ptr aTimeShared = advancedBoardTime.get<tgui::ToggleButton>("Shared");
	tgui::ToggleButton::Ptr aTimeAI = advancedBoardTime.get<tgui::ToggleButton>("AI");
	std::vector<tgui::ToggleButton::Ptr> aTimeModes = { aTimeOff, aTimeSingle, aTimeShared, aTimeAI };
	tgui::EditBox::Ptr aWhiteTime = advancedBoardTime.get<tgui::EditBox>("White Time");
	tgui::EditBox::Ptr aBlackTime = advancedBoardTime.get<tgui::EditBox>("Black Time");
	tgui::EditBox::Ptr aIncrement = advancedBoardTime.get<tgui::EditBox>("Increment");
	tgui::BitmapButton::Ptr aWhiteTimeUnit = advancedBoardTime.get<tgui::BitmapButton>("White Time Unit");
	tgui::BitmapButton::Ptr aBlackTimeUnit = advancedBoardTime.get<tgui::BitmapButton>("Black Time Unit");
	tgui::BitmapButton::Ptr aIncrementUnit = advancedBoardTime.get<tgui::BitmapButton>("Increment Unit");
	tgui::CheckBox::Ptr aPauseTime = advancedBoardTime.get<tgui::CheckBox>("Pause Time");
	tgui::EditBox::Ptr aInitialWhiteTime = advancedBoardTime.get<tgui::EditBox>("White Time Initial");
	tgui::EditBox::Ptr aInitialBlackTime = advancedBoardTime.get<tgui::EditBox>("Black Time Initial");
	tgui::Label::Ptr aWhiteLabel = advancedBoardTime.get<tgui::Label>("White Label");
	tgui::Label::Ptr aBlackLabel = advancedBoardTime.get<tgui::Label>("Black Label");
	tgui::Label::Ptr aInitialWhiteLabel = advancedBoardTime.get<tgui::Label>("Initial White Label");
	tgui::Label::Ptr aInitialBlackLabel = advancedBoardTime.get<tgui::Label>("Initial Black Label");
	tgui::BitmapButton::Ptr aInitialWhiteTimeUnit = advancedBoardTime.get<tgui::BitmapButton>("White Time Initial Unit");
	tgui::BitmapButton::Ptr aInitialBlackTimeUnit = advancedBoardTime.get<tgui::BitmapButton>("Black Time Initial Unit");
	tgui::CheckBox::Ptr aShowMilliseconds = advancedBoardTime.get<tgui::CheckBox>("Show Milliseconds");
	tgui::EditBox::Ptr aMillisecondsTime = advancedBoardTime.get<tgui::EditBox>("Condition Seconds");
	tgui::BitmapButton::Ptr aMillisecondsOperator = advancedBoardTime.get<tgui::BitmapButton>("Condition Operator");
	tgui::CheckBox::Ptr aAnimatePieces = advancedBoardTime.get<tgui::CheckBox>("Animate Pieces");
	tgui::EditBox::Ptr aPieceSpeed = advancedBoardTime.get<tgui::EditBox>("Piece Speed");
	tgui::BitmapButton::Ptr aTimeCancel = advancedBoardTime.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr aTimeSave = advancedBoardTime.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr aTimeLeft = advancedBoardTime.get<tgui::BitmapButton>("Left");
	tgui::BitmapButton::Ptr aTimeClose = advancedBoardTime.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr aTimeRight = advancedBoardTime.get<tgui::BitmapButton>("Right");
	tgui::BitmapButton::Ptr aTimeMain = advancedBoardTime.get<tgui::BitmapButton>("Main");
	tgui::Label::Ptr aWhiteInitial = advancedBoardTime.get<tgui::Label>("Initial White");
	tgui::Label::Ptr aBlackInitial = advancedBoardTime.get<tgui::Label>("Initial Black");

	aTimeOff->onClick([&]() {
		aTimeOff->setDown(true);
		timeMode = 0;
		bP.timeEnabled = false;
		bP.AI_Time = false;
		aWhiteLabel->setText("White Time:");
		aBlackLabel->setText("Black Time:");
		aInitialWhiteLabel->setText("White Time");
		aInitialBlackLabel->setText("Black Time");
		aWhiteInitial->setPosition("39.93%", "59.26%");
		aBlackInitial->setPosition("38.82%", "65.88%");
		for (auto& m : aTimeModes) { if (m != aTimeOff) { m->setDown(false); } }
		});
	aTimeSingle->onClick([&]() {
		aTimeSingle->setDown(true);
		timeMode = 1;
		bP.AI_Time = false;
		bP.timeEnabled = true;
		aWhiteLabel->setText("White Time:");
		aBlackLabel->setText("Black Time:");
		aInitialWhiteLabel->setText("White Time");
		aInitialBlackLabel->setText("Black Time");
		aWhiteInitial->setPosition("39.93%", "59.26%");
		aBlackInitial->setPosition("38.82%", "65.88%");
		for (auto& m : aTimeModes) { if (m != aTimeSingle) { m->setDown(false); } }
		});
	aTimeShared->onClick([&]() {
		aTimeShared->setDown(true);
		timeMode = 2;
		bP.AI_Time = false;
		bP.timeEnabled = true;
		aWhiteLabel->setText("White Time:");
		aBlackLabel->setText("Black Time:");
		aInitialWhiteLabel->setText("White Time");
		aInitialBlackLabel->setText("Black Time");
		aWhiteInitial->setPosition("39.93%", "59.26%");
		aBlackInitial->setPosition("38.82%", "65.88%");
		aBlackTimeUnit->setUserData(aWhiteTimeUnit->getUserData<int>());
		aBlackTimeUnit->setText(aWhiteTimeUnit->getText());
		aBlackTime->setText(aWhiteTime->getText());
		for (auto& m : aTimeModes) { if (m != aTimeShared) { m->setDown(false); } }
		});
	aTimeAI->onClick([&]() {
		aTimeAI->setDown(true);
		timeMode = 3;
		bP.AI_Time = true;
		bP.timeEnabled = true;
		aWhiteLabel->setText("AI Time:");
		aBlackLabel->setText("Player Time:");
		aInitialWhiteLabel->setText("AI Time");
		aInitialBlackLabel->setText("Player Time");
		aWhiteInitial->setPosition("29.07%", "59.26%");
		aBlackInitial->setPosition("40.38%", "65.88%");
		for (auto& m : aTimeModes) { if (m != aTimeAI) { m->setDown(false); } }
		});
	aPauseTime->onChange([&](bool c) { bP.isPaused = c; });
	aWhiteTimeUnit->onPress([&]() {
		aWhiteTimeUnit->setUserData((aWhiteTimeUnit->getUserData<int>() + 1) % 3);
		aWhiteTime->setText(aWhiteTime->getText());
		switch (aWhiteTimeUnit->getUserData<int>()) {
		case 0:
			aWhiteTimeUnit->setText("s");
			break;
		case 1:
			aWhiteTimeUnit->setText("min");
			break;
		case 2:
			aWhiteTimeUnit->setText("hrs");
			break;
		}
		if (timeMode == 2) {
			aBlackTimeUnit->setUserData(aWhiteTimeUnit->getUserData<int>());
			aBlackTimeUnit->setText(aWhiteTimeUnit->getText());
			aBlackTime->setText(aWhiteTime->getText());
			bP.blackUnit = aBlackTimeUnit->getUserData<int>();
		}
		bP.whiteUnit = aWhiteTimeUnit->getUserData<int>();
		});
	aBlackTimeUnit->onPress([&]() {
		aBlackTimeUnit->setUserData((aBlackTimeUnit->getUserData<int>() + 1) % 3);
		aBlackTime->setText(aBlackTime->getText());
		switch (aBlackTimeUnit->getUserData<int>()) {
		case 0:
			aBlackTimeUnit->setText("s");
			break;
		case 1:
			aBlackTimeUnit->setText("min");
			break;
		case 2:
			aBlackTimeUnit->setText("hrs");
			break;
		}
		if (timeMode == 2) {
			aWhiteTimeUnit->setUserData(aWhiteTimeUnit->getUserData<int>());
			aWhiteTimeUnit->setText(aWhiteTimeUnit->getText());
			aWhiteTime->setText(aWhiteTime->getText());
			bP.whiteUnit = aWhiteTimeUnit->getUserData<int>();
		}
		bP.blackUnit = aBlackTimeUnit->getUserData<int>();
		});
	aIncrementUnit->onPress([&]() {
		aIncrementUnit->setUserData((aIncrementUnit->getUserData<int>() + 1) % 2);
		aIncrement->setText(aIncrement->getText());
		switch (aIncrementUnit->getUserData<int>()) {
		case 0:
			aIncrementUnit->setText("s");
			break;
		case 1:
			aIncrementUnit->setText("min");
			break;
		}
		bP.incrUnit = aIncrementUnit->getUserData<int>();
		});
	aWhiteTime->onTextChange([&](tgui::String text) {
		switch (aWhiteTimeUnit->getUserData<int>()) {
		case 0:
			bP.whiteTime = sf::seconds(text.toFloat());
			break;
		case 1:
			bP.whiteTime = sf::seconds(text.toFloat() * 60);
			break;
		case 2:
			bP.whiteTime = sf::seconds(text.toFloat() * 3600);
			break;
		}
		if (!fromShared && timeMode == 2) {
			fromShared = true;
			aBlackTime->setText(text);
			fromShared = false;
		}
		});
	aBlackTime->onTextChange([&](tgui::String text) {
		switch (aBlackTimeUnit->getUserData<int>()) {
		case 0:
			bP.blackTime = sf::seconds(text.toFloat());
			break;
		case 1:
			bP.blackTime = sf::seconds(text.toFloat() * 60);
			break;
		case 2:
			bP.blackTime = sf::seconds(text.toFloat() * 3600);
			break;
		}
		if (!fromShared && timeMode == 2) {
			fromShared = true;
			aWhiteTime->setText(text);
			fromShared = false;
		}
		});
	aIncrement->onTextChange([&](tgui::String text) {
		switch (aIncrementUnit->getUserData<int>()) {
		case 0:
			bP.timeIncrement = sf::seconds(text.toFloat());
			break;
		case 1:
			bP.timeIncrement = sf::seconds(text.toFloat() * 60);
			break;
		}
		});
	aInitialWhiteTimeUnit->onPress([&]() {
		aInitialWhiteTimeUnit->setUserData((aInitialWhiteTimeUnit->getUserData<int>() + 1) % 3);
		aInitialWhiteTime->setText(aInitialWhiteTime->getText());
		switch (aInitialWhiteTimeUnit->getUserData<int>()) {
		case 0:
			aInitialWhiteTimeUnit->setText("s");
			break;
		case 1:
			aInitialWhiteTimeUnit->setText("min");
			break;
		case 2:
			aInitialWhiteTimeUnit->setText("hrs");
			break;
		}
		if (timeMode == 2) {
			aInitialBlackTimeUnit->setUserData(aInitialWhiteTimeUnit->getUserData<int>());
			aInitialBlackTimeUnit->setText(aInitialWhiteTimeUnit->getText());
			aInitialBlackTime->setText(aInitialWhiteTime->getText());
			bP.startingBlackUnit = aInitialBlackTimeUnit->getUserData<int>();
		}
		bP.startingWhiteUnit = aInitialWhiteTimeUnit->getUserData<int>();
		});
	aInitialBlackTimeUnit->onPress([&]() {
		aInitialBlackTimeUnit->setUserData((aInitialBlackTimeUnit->getUserData<int>() + 1) % 3);
		aInitialBlackTime->setText(aInitialBlackTime->getText());
		switch (aInitialBlackTimeUnit->getUserData<int>()) {
		case 0:
			aInitialBlackTimeUnit->setText("s");
			break;
		case 1:
			aInitialBlackTimeUnit->setText("min");
			break;
		case 2:
			aInitialBlackTimeUnit->setText("hrs");
			break;
		}
		if (timeMode == 2) {
			aInitialWhiteTimeUnit->setUserData(aInitialBlackTimeUnit->getUserData<int>());
			aInitialWhiteTimeUnit->setText(aInitialBlackTimeUnit->getText());
			aInitialWhiteTime->setText(aInitialBlackTime->getText());
			bP.startingWhiteUnit = aInitialWhiteTimeUnit->getUserData<int>();
		}
		bP.startingBlackUnit = aInitialBlackTimeUnit->getUserData<int>();
		});
	aInitialWhiteTime->onTextChange([&](tgui::String text) {
		switch (aInitialWhiteTimeUnit->getUserData<int>()) {
		case 0:
			bP.startingWhiteTime = sf::seconds(text.toFloat());
			break;
		case 1:
			bP.startingWhiteTime = sf::seconds(text.toFloat() * 60);
			break;
		case 2:
			bP.startingWhiteTime = sf::seconds(text.toFloat() * 3600);
			break;
		}
		if (!fromShared && timeMode == 2) {
			fromShared = true;
			aInitialBlackTime->setText(text);
			fromShared = false;
		}
		});
	aInitialBlackTime->onTextChange([&](tgui::String text) {
		switch (aInitialBlackTimeUnit->getUserData<int>()) {
		case 0:
			bP.startingBlackTime = sf::seconds(text.toFloat());
			break;
		case 1:
			bP.startingBlackTime = sf::seconds(text.toFloat() * 60);
			break;
		case 2:
			bP.startingBlackTime = sf::seconds(text.toFloat() * 3600);
			break;
		}
		if (!fromShared && timeMode == 2) {
			fromShared = true;
			aInitialWhiteTime->setText(text);
			fromShared = false;
		}
		});
	aShowMilliseconds->onChange([&](bool c) {
		aMillisecondsTime->setEnabled(c);
		aMillisecondsOperator->setEnabled(c);
		bP.showMilliseconds = c;
		});
	auto setMillisecondsCondition = [&]() {
		if (aMillisecondsOperator->getText() == "<") {
			bP.millisecondsConditionID = 0;
			bP.millisecondsCondition = [millisecondsTime = bP.millisecondsTime](const sf::Time& t) { return t < millisecondsTime; };
		}
		else if (aMillisecondsOperator->getText() == ">") {
			bP.millisecondsConditionID = 1;
			bP.millisecondsCondition = [millisecondsTime = bP.millisecondsTime](const sf::Time& t) { return t > millisecondsTime; };
		}
		else if (aMillisecondsOperator->getText() == "≤") {
			bP.millisecondsConditionID = 2;
			bP.millisecondsCondition = [millisecondsTime = bP.millisecondsTime](const sf::Time& t) { return t <= millisecondsTime; };
		}
		else if (aMillisecondsOperator->getText() == "≥") {
			bP.millisecondsConditionID = 3;
			bP.millisecondsCondition = [millisecondsTime = bP.millisecondsTime](const sf::Time& t) { return t >= millisecondsTime; };
		}
		};
	aMillisecondsTime->onTextChange([&](const tgui::String& text) {
		bP.millisecondsTime = sf::milliseconds(text.toInt());
		setMillisecondsCondition();
		});
	aMillisecondsOperator->onPress([&](const tgui::String& text) {
		if (text == "<") {
			aMillisecondsOperator->setText(">");
		}
		else if (text == ">") {
			aMillisecondsOperator->setText(L"≤");
		}
		else if (text == L"≤") {
			aMillisecondsOperator->setText(L"≥");
		}
		else if (text == L"≥") {
			aMillisecondsOperator->setText("<");
		}
		else {
			aMillisecondsOperator->setText("<");
		}
		setMillisecondsCondition();
		});
	aPieceSpeed->onTextChange([&](const tgui::String& text) {
		bP.pieceSpeed = text.toFloat();
		});
	aAnimatePieces->onChange([&](bool c) {
		bP.overridePieceSpeed = c;
		aPieceSpeed->setEnabled(c);
		});
	aTimeSave->onClick(saveAdvancedBoard);
	aTimeCancel->onClick([&]() { aBGUIVisible = false; });
	aTimeClose->onClick([&]() { aBGUIVisible = false; });
	aTimeLeft->onPress([&]() {loadedGUI--; });
	aTimeRight->onPress([&]() {loadedGUI++; });
	aTimeMain->onPress([&]() {
		if (aFromBoard && !selectedBoard) { return; }
		loadBoardGUI(aCreateBoard ? 1 : (aFromBoard ? 2 : 0), bP, selectedSFSettings);
		aBGUIVisible = false;
		});

	auto loadAdvancedTime = [&]() {
		aTimeAI->setDown(false);
		aTimeShared->setDown(false);
		aTimeSingle->setDown(false);
		aTimeOff->setDown(false);
		if (bP.timeEnabled) {
			if (bP.AI_Time) {
				aTimeAI->setDown(true);
			}
			else if (bP.sharedTime && bP.whiteTime == bP.blackTime) {
				aTimeShared->setDown(true);
			}
			else {
				aTimeSingle->setDown(true);
			}
		}
		else { aTimeOff->setDown(true); }

		if (aTimeAI->isDown()) {
			timeMode = 3;
			aWhiteLabel->setText("AI Time:");
			aBlackLabel->setText("Player Time:");
			aInitialWhiteLabel->setText("AI Time");
			aInitialBlackLabel->setText("Player Time");
			aWhiteInitial->setPosition("29.07%", "59.26%");
			aBlackInitial->setPosition("40.38%", "65.88%");
		}
		else {
			if (aTimeOff->isDown()) {
				timeMode = 0;
			}
			else if (aTimeSingle->isDown()) {
				timeMode = 1;
			}
			aWhiteLabel->setText("White Time:");
			aBlackLabel->setText("Black Time:");
			aInitialWhiteLabel->setText("White Time");
			aInitialBlackLabel->setText("Black Time");
			aWhiteInitial->setPosition("39.93%", "59.26%");
			aBlackInitial->setPosition("38.82%", "65.88%");
		}
		aWhiteTimeUnit->setUserData(bP.whiteUnit);
		switch (bP.whiteUnit) {
		case 0:
			aWhiteTime->setText(timeToStr(bP.whiteTime, timeStream));
			aWhiteTimeUnit->setText("s");
			break;
		case 1:
			aWhiteTime->setText(timeToStr(bP.whiteTime, timeStream, 1 / 60.0f));
			aWhiteTimeUnit->setText("min");
			break;
		case 2:
			aWhiteTime->setText(timeToStr(bP.whiteTime, timeStream, 1 / 3600.0f));
			aWhiteTimeUnit->setText("hrs");
			break;
		}
		aBlackTimeUnit->setUserData(bP.blackUnit);
		switch (bP.blackUnit) {
		case 0:
			aBlackTime->setText(timeToStr(bP.blackTime, timeStream));
			aBlackTimeUnit->setText("s");
			break;
		case 1:
			aBlackTime->setText(timeToStr(bP.blackTime, timeStream, 1 / 60.0f));
			aBlackTimeUnit->setText("min");
			break;
		case 2:
			aBlackTime->setText(timeToStr(bP.blackTime, timeStream, 1 / 3600.0f));
			aBlackTimeUnit->setText("hrs");
			break;
		}
		aIncrementUnit->setUserData(bP.incrUnit);
		switch (bP.incrUnit) {
		case 0:
			aIncrement->setText(timeToStr(bP.timeIncrement, timeStream));
			aIncrementUnit->setText("s");
			break;
		case 1:
			aIncrement->setText(timeToStr(bP.timeIncrement, timeStream, 1 / 60.0f));
			aIncrementUnit->setText("min");
			break;
		}
		aInitialWhiteTimeUnit->setUserData(bP.startingWhiteUnit);
		switch (bP.startingWhiteUnit) {
		case 0:
			aInitialWhiteTime->setText(timeToStr(bP.startingWhiteTime, timeStream));
			aInitialWhiteTimeUnit->setText("s");
			break;
		case 1:
			aInitialWhiteTime->setText(timeToStr(bP.startingWhiteTime, timeStream, 1 / 60.0f));
			aInitialWhiteTimeUnit->setText("min");
			break;
		case 2:
			aInitialWhiteTime->setText(timeToStr(bP.startingWhiteTime, timeStream, 1 / 3600.0f));
			aInitialWhiteTimeUnit->setText("hrs");
			break;
		}
		aInitialBlackTimeUnit->setUserData(bP.startingBlackUnit);
		switch (bP.startingBlackUnit) {
		case 0:
			aInitialBlackTime->setText(timeToStr(bP.startingBlackTime, timeStream));
			aInitialBlackTimeUnit->setText("s");
			break;
		case 1:
			aInitialBlackTime->setText(timeToStr(bP.startingBlackTime, timeStream, 1 / 60.0f));
			aInitialBlackTimeUnit->setText("min");
			break;
		case 2:
			aInitialBlackTime->setText(timeToStr(bP.startingBlackTime, timeStream, 1 / 3600.0f));
			aInitialBlackTimeUnit->setText("min");
			break;
		}
		aShowMilliseconds->setChecked(bP.showMilliseconds);
		aMillisecondsTime->setText(timeToStr(bP.millisecondsTime, timeStream));
		switch (bP.millisecondsConditionID) {
		case 0:
			aMillisecondsOperator->setText("<");
			break;
		case 1:
			aMillisecondsOperator->setText(">");
			break;
		case 2:
			aMillisecondsOperator->setText("<=");
			break;
		case 3:
			aMillisecondsOperator->setText(">=");
			break;
		}
		aAnimatePieces->setChecked(bP.overridePieceSpeed);
		aPieceSpeed->setText(floatToStr(bP.pieceSpeed, stream));
		};

#pragma endregion

#pragma region Position

	tgui::ToggleButton::Ptr aAIWhite, aAIBlack;

	advancedBoardPosition.loadWidgetsFromFile("assets/ui/advanced_board_position.txt");
	advancedBoardPosition.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedBoardPosition.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedBoardPosition.get<tgui::Panel>("Outline")->setPosition("50%", "9.7%");
	advancedBoardPosition.get<tgui::Panel>("Outline")->setOrigin(0.5f, 0.0f);
	advancedBoardPosition.get<tgui::Panel>("Variant Panel")->setOrigin(0.5f, 0.0f);
	advancedBoardPosition.get<tgui::Panel>("Variant Panel")->setPosition("50%", "10%");
	advancedBoardPosition.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedBoardPosition.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::Panel::Ptr aMainPanel = advancedBoardPosition.get<tgui::Panel>("Panel");
	tgui::Panel::Ptr aVariantPanel = advancedBoardPosition.get<tgui::Panel>("Variant Panel");
	tgui::Panel::Ptr aVariantShadow = advancedBoardPosition.get<tgui::Panel>("Shadow 2");
	tgui::Panel::Ptr aVariantOutline = advancedBoardPosition.get<tgui::Panel>("Outline");
	tgui::BitmapButton::Ptr aVariantTypeButton = advancedBoardPosition.get<tgui::BitmapButton>("Variant Type Button");
	tgui::Picture::Ptr aVariantTypeImage = advancedBoardPosition.get<tgui::Picture>("Variant Type Image");
	tgui::Label::Ptr aVariantTypeDescription = advancedBoardPosition.get<tgui::Label>("Variant Type Description");
	tgui::Label::Ptr aVariantDescription = advancedBoardPosition.get<tgui::Label>("Variant Description");
	tgui::ComboBox::Ptr aVariantComboBox = advancedBoardPosition.get<tgui::ComboBox>("VariantBox");
	tgui::BitmapButton::Ptr aRandomVariant = advancedBoardPosition.get<tgui::BitmapButton>("Randomize Variant");
	tgui::CheckBox::Ptr aChess960 = advancedBoardPosition.get<tgui::CheckBox>("Chess960");
	tgui::TextArea::Ptr aFENArea = advancedBoardPosition.get<tgui::TextArea>("FEN Area");
	tgui::Picture::Ptr aFENImage = advancedBoardPosition.get<tgui::Picture>("FEN Image");
	tgui::Picture::Ptr aFENBorder = advancedBoardPosition.get<tgui::Picture>("FEN Border");
	tgui::CheckBox::Ptr aRepeatPosition = advancedBoardPosition.get<tgui::CheckBox>("Repeat Position");
	tgui::TextArea::Ptr aRepeatFENArea = advancedBoardPosition.get<tgui::TextArea>("Repeat FEN Area");
	tgui::Picture::Ptr aRepeatFENImage = advancedBoardPosition.get<tgui::Picture>("Repeat FEN Image");
	tgui::Picture::Ptr aRepeatFENBorder = advancedBoardPosition.get<tgui::Picture>("Repeat FEN Border");
	tgui::ToggleButton::Ptr aPlayerWhite = advancedBoardPosition.get<tgui::ToggleButton>("White");
	tgui::ToggleButton::Ptr aPlayerBlack = advancedBoardPosition.get<tgui::ToggleButton>("Black");
	tgui::BitmapButton::Ptr aRandomize = advancedBoardPosition.get<tgui::BitmapButton>("Randomize");
	tgui::ToggleButton::Ptr aPlayerWhiteRestart = advancedBoardPosition.get<tgui::ToggleButton>("White Restart");
	tgui::ToggleButton::Ptr aPlayerBlackRestart = advancedBoardPosition.get<tgui::ToggleButton>("Black Restart");
	tgui::ToggleButton::Ptr aPlayerRandomRestart = advancedBoardPosition.get<tgui::ToggleButton>("Random Restart");
	tgui::ToggleButton::Ptr aCycle = advancedBoardPosition.get<tgui::ToggleButton>("Cycle");
	tgui::CheckBox::Ptr aAnimateBoard = advancedBoardPosition.get<tgui::CheckBox>("Animate");
	tgui::BitmapButton::Ptr aCloseVariant = advancedBoardPosition.get<tgui::BitmapButton>("Close Variant");
	tgui::BitmapButton::Ptr aVariantButton = advancedBoardPosition.get<tgui::BitmapButton>("Variant Button");
	tgui::BitmapButton::Ptr aPosCancel = advancedBoardPosition.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr aPosClose = advancedBoardPosition.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr aPosSave = advancedBoardPosition.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr aPosLeft = advancedBoardPosition.get<tgui::BitmapButton>("Left");
	tgui::BitmapButton::Ptr aPosRight = advancedBoardPosition.get<tgui::BitmapButton>("Right");
	tgui::BitmapButton::Ptr aPosMain = advancedBoardPosition.get<tgui::BitmapButton>("Main");
	tgui::BitmapButton::Ptr aStartpos = advancedBoardPosition.get<tgui::BitmapButton>("startpos");
	tgui::ComboBox::Ptr aPositionSet = advancedBoardPosition.get<tgui::ComboBox>("Position Set");

	aVariantComboBox->setItemsToDisplay(12);

	tgui::EditBox::Ptr aPieceSet;
	std::function<void(int)> onPieceSetChangeA;

	tgui::Gui aPositionGUI{ window }, aRepeatPositionGUI{ window };
	GUIBoard aFENBoard(aFENImage, aPositionGUI), aRepeatFENBoard(aRepeatFENImage, aRepeatPositionGUI);
	aFENArea->setTabString("/");
	aRepeatFENArea->setTabString("/");

	aRandomVariant->onClick([&]() {
		aVariantComboBox->setSelectedItemByIndex(rand() % bP.variantList->size());
		});

	aVariantComboBox->onItemSelect([&](size_t index) {
		bP.variantIndex = index;
		auto& variant = bP.variantList->at(bP.variantIndex);
		if (variant->description != "") {
			aVariantDescription->setText(variant->description);
		}
		if (bP.saveFEN != std::nullopt && bP.variantIndex == bP.currentVariantIndex && bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			aFENArea->setText(bP.saveFEN.value());
		}
		else { aFENArea->setText(""); }
		aVariantButton->setText(variant->displayName != "" ? variant->displayName : variant->name);
		for (int i = 1; i < aPositionSet->getItems().size(); i++) {
			aPositionSet->removeItemByIndex(i);
		}
		if (endgameVariants.count(variant->name) > 0) {
			aPositionSet->addItem("Endgame");
			if (bP.endgamePosition) { aPositionSet->setSelectedItemByIndex(1); }
			else { aPositionSet->setSelectedItemByIndex(0); }
		}
		else {
			aPositionSet->setSelectedItemByIndex(0);
		}

		sf::Texture nBoardTexture = Chess::loadBoard(boardSpriteSheet, bP.boardSet, bP.variantList->at(bP.variantIndex)->boardSize);
		tgui::Texture boardTextureT;
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		aFENImage->getRenderer()->setTexture(boardTextureT);
		aRepeatFENImage->getRenderer()->setTexture(boardTextureT);
		if (variantPieceTextures.find(variant->name) != variantPieceTextures.end()) {
			aPieceSet->setText("1");
		}
		else {
			aPieceSet->setText(std::to_string(bP.pieceSetSave + 1));
		}
		onPieceSetChangeA(0);
		aFENBoard.setBoard(aFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		aRepeatFENBoard.setBoard(aRepeatFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
	});

	aVariantTypeButton->onClick([&]() {
		bP.variantTypeIndex = (bP.variantTypeIndex + 1) % variantTypes.size();
		bP.variantList = &variantTypes.at(bP.variantTypeIndex).variants;
		const Chess::VariantType& vType = variantTypes.at(bP.variantTypeIndex);
		aVariantTypeButton->setText(vType.name);
		aVariantTypeDescription->setText(vType.description);

		aVariantComboBox->removeAllItems();
		for (auto& v : vType.variants) {
			aVariantComboBox->addItem(v->displayName != "" ? v->displayName : v->name);
		}

		if (bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			aVariantComboBox->setSelectedItemByIndex(bP.currentVariantIndex);
		}
		else { aVariantComboBox->setSelectedItemByIndex(0); }
		auto& v = bP.variantList->at(bP.variantIndex);
		aVariantButton->setText(v->displayName != "" ? v->displayName : v->name);

		if (bP.saveFEN != std::nullopt && bP.variantIndex == bP.currentVariantIndex && bP.variantTypeIndex == bP.currentVariantTypeIndex) {
			aFENArea->setText(bP.saveFEN.value());
		}
		else { aFENArea->setText(""); }

		});

	aVariantButton->onClick([&]() {
		if (!aVariantPanel->isVisible()) {
			aMainPanel->setIgnoreMouseEvents(true);
			aVariantPanel->setIgnoreMouseEvents(false);
			aVariantPanel->setVisible(true);
			aVariantOutline->setVisible(true);
			aVariantShadow->setVisible(true);
		}
		});

	aCloseVariant->onClick([&]() {
		aMainPanel->setIgnoreMouseEvents(false);
		aVariantPanel->setIgnoreMouseEvents(true);
		aVariantPanel->setVisible(false);
		aVariantOutline->setVisible(false);
		aVariantShadow->setVisible(false);
		});

	aChess960->onCheck([&]() { bP.chess960 = true; });
	aChess960->onUncheck([&]() { bP.chess960 = false; });
	aPositionSet->onItemSelect([&](int i) {
		if (i == 0) {
			bP.endgamePosition = false;
			aFENArea->setEnabled(true);
			aFENArea->setText(aFENArea->getText());
			aFENImage->getRenderer()->setOpacity(1.0f);
			aFENBorder->getRenderer()->setOpacity(1.0f);
		}
		else {
			bP.endgamePosition = true;
			aFENArea->setEnabled(false);
			aFENImage->getRenderer()->setOpacity(0.7f);
			aFENBorder->getRenderer()->setOpacity(0.7f);
		}
		});
	aFENArea->onTextChange([&](const tgui::String& text) {
		bool reversed = false;
		if (text.empty()) {
			bP.FEN = {};
		}
		else {
			bP.FEN = text.toStdString();
			std::string FEN = bP.FEN.value();
			size_t find = FEN.find(' ');
			if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
				reversed = true;
			}
		}
		auto& variant = bP.variantList->at(bP.variantIndex);
		auto& pTexturesSVG = variantPieceTextures.find(variant->name) != variantPieceTextures.end() ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(bP.pieceSet);
		aFENBoard.setPieces(bP.FEN.value_or(""), *bP.variantList->at(bP.variantIndex), pTexturesSVG, reversed);
		});
	aRepeatPosition->onChange([&](bool c) {
		bP.repeatFEN = c;
		aRepeatFENArea->setEnabled(c);
		if (c) {
			aRepeatFENArea->setText(aRepeatFENArea->getText());
			aRepeatFENImage->getRenderer()->setOpacity(1.0f);
			aRepeatFENBorder->getRenderer()->setOpacity(1.0f);
		}
		else {
			aRepeatFENImage->getRenderer()->setOpacity(0.7f);
			aRepeatFENBorder->getRenderer()->setOpacity(0.7f);
		}
		});

	aRepeatFENArea->onTextChange([&](const tgui::String& text) {
		bool reversed = false;
		if (text.empty()) {
			bP.newPositionFEN = {};
		}
		else {
			bP.newPositionFEN = text.toStdString();
			std::string FEN = bP.newPositionFEN.value();
			size_t find = FEN.find(' ');
			if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
				reversed = true;
			}
		}
		auto& variant = bP.variantList->at(bP.variantIndex);
		auto& pTexturesSVG = variantPieceTextures.find(variant->name) != variantPieceTextures.end() ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(bP.pieceSet);
		aRepeatFENBoard.setPieces(bP.newPositionFEN.value_or(""), *bP.variantList->at(bP.variantIndex), pTexturesSVG, reversed);
		});

	aPlayerWhite->onMouseRelease([&]() {
		if (aFromBoard) {
			bP.white = true;
			aPlayerWhite->setDown(true);
			aPlayerBlack->setDown(false);
			aAIWhite->setDown(false);
			aAIBlack->setDown(true);
		}
		else {
			if (aPlayerWhite->isDown()) {
				bP.white = true;
				aPlayerBlack->setDown(false);
				aAIWhite->setDown(false);
				aAIBlack->setDown(true);
			}
			else {
				bP.white = std::nullopt;
				aAIWhite->setDown(false);
				aAIBlack->setDown(false);
			}
		}
		});
	aPlayerBlack->onMouseRelease([&]() {
		if (aFromBoard) {
			bP.white = false;
			aPlayerWhite->setDown(false);
			aPlayerBlack->setDown(true);
			aAIWhite->setDown(true);
			aAIBlack->setDown(false);
		}
		else {
			if (aPlayerBlack->isDown()) {
				bP.white = false;
				aPlayerWhite->setDown(false);
				aAIWhite->setDown(true);
				aAIBlack->setDown(false);
			}
			else {
				bP.white = std::nullopt;
				aAIWhite->setDown(false);
				aAIBlack->setDown(false);
			}
		}
		});
	aRandomize->onPress([&]() {
		if (randomBool()) {
			aPlayerWhite->setDown(true);
			aPlayerBlack->setDown(false);
			aAIWhite->setDown(false);
			aAIBlack->setDown(true);
			bP.white = true;
		}
		else {
			aPlayerBlack->setDown(true);
			aPlayerWhite->setDown(false);
			aAIWhite->setDown(true);
			aAIBlack->setDown(false);
			bP.white = false;
		}
		});
	aPlayerWhiteRestart->onMouseRelease([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = true;
		aPlayerWhiteRestart->setDown(true);
		aPlayerBlackRestart->setDown(false);
		aPlayerRandomRestart->setDown(false);
		aCycle->setDown(false);
		});
	aPlayerBlackRestart->onMouseRelease([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = false;
		aPlayerWhiteRestart->setDown(false);
		aPlayerBlackRestart->setDown(true);
		aPlayerRandomRestart->setDown(false);
		aCycle->setDown(false);
		});
	aPlayerRandomRestart->onMouseRelease([&]() {
		bP.cycleSides = false;
		bP.newPositionWhite = std::nullopt;
		aPlayerWhiteRestart->setDown(false);
		aPlayerBlackRestart->setDown(false);
		aPlayerRandomRestart->setDown(true);
		aCycle->setDown(false);
		});
	aCycle->onMouseRelease([&]() {
		bP.cycleSides = true;
		aPlayerWhiteRestart->setDown(false);
		aPlayerBlackRestart->setDown(false);
		aPlayerRandomRestart->setDown(false);
		aCycle->setDown(true);
		});
	aAnimateBoard->onChange([&](bool c) { bP.boardAnimated = c; });
	aPosSave->onClick(saveAdvancedBoard);
	aPosCancel->onClick([&]() { aBGUIVisible = false; });
	aPosClose->onClick([&]() { aBGUIVisible = false; });
	aPosRight->onPress([&]() { loadedGUI++; });
	aPosMain->onPress([&]() {
		if (aFromBoard && !selectedBoard) { return; }
		loadBoardGUI(aCreateBoard ? 1 : (aFromBoard ? 2 : 0), bP, selectedSFSettings);
		aBGUIVisible = false;
		});

	aStartpos->onPress([&]() {
		aFENArea->setText(Chess::getNewFEN(*bP.variantList->at(bP.variantIndex), bP.chess960));
		});

	auto loadAdvancedPosition = [&]() {
		bP.variantList = &variantTypes.at(bP.variantTypeIndex).variants;
		const Chess::VariantType& vType = variantTypes.at(bP.variantTypeIndex);
		aVariantTypeButton->setText(vType.name);
		aVariantTypeDescription->setText(vType.description);

		aVariantComboBox->removeAllItems();
		for (auto& v : vType.variants) {
			aVariantComboBox->addItem(v->displayName != "" ? v->displayName : v->name);
		}
		aVariantComboBox->setSelectedItemByIndex(bP.variantIndex);
		auto& v = bP.variantList->at(bP.variantIndex);
		aVariantButton->setText(v->displayName != "" ? v->displayName : v->name);
		bP.endgamePosition = bP.endgamePosition;
		aChess960->setChecked(bP.chess960);
		aFENArea->setText(bP.FEN.value_or(""));
		aRepeatFENArea->setText(bP.newPositionFEN.value_or(""));
		aRepeatPosition->setChecked(bP.repeatFEN);
		aPlayerWhite->setDown(bP.white.has_value() && bP.white.value());
		aPlayerBlack->setDown(bP.white.has_value() && !bP.white.value());

		if (bP.cycleSides) {
			aCycle->setDown(true);
			aPlayerWhiteRestart->setDown(false);
			aPlayerBlackRestart->setDown(false);
			aPlayerRandomRestart->setDown(false);
		}
		else {
			if (bP.newPositionWhite.has_value()) {
				if (*bP.newPositionWhite) {
					aCycle->setDown(false);
					aPlayerWhiteRestart->setDown(true);
					aPlayerBlackRestart->setDown(false);
					aPlayerRandomRestart->setDown(false);
				}
				else {
					aCycle->setDown(false);
					aPlayerWhiteRestart->setDown(false);
					aPlayerBlackRestart->setDown(true);
					aPlayerRandomRestart->setDown(false);
				}
			}
			else {
				aCycle->setDown(false);
				aPlayerWhiteRestart->setDown(false);
				aPlayerBlackRestart->setDown(false);
				aPlayerRandomRestart->setDown(true);
			}
		}
		aAnimateBoard->setChecked(bP.boardAnimated);
		};
#pragma endregion

#pragma region Appearance

	auto setButtonDownAfter = [&](tgui::ToggleButton::Ptr& button, int milliseconds)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
			button->setDown(false);
			button->onToggle.setEnabled(true);
		};

	advancedBoardAppearance.loadWidgetsFromFile("assets/ui/advanced_board_appearance.txt");
	advancedBoardAppearance.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedBoardAppearance.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedBoardAppearance.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedBoardAppearance.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::CheckBox::Ptr aEnableKeybinds = advancedBoardAppearance.get<tgui::CheckBox>("Enable Keybinds");
	tgui::ToggleButton::Ptr aFlipBoard = advancedBoardAppearance.get<tgui::ToggleButton>("Flip Board");
	tgui::ToggleButton::Ptr aResetPosition = advancedBoardAppearance.get<tgui::ToggleButton>("Reset Position");
	tgui::ToggleButton::Ptr aResetScale = advancedBoardAppearance.get<tgui::ToggleButton>("Reset Scale");
	tgui::ToggleButton::Ptr aMouseOff = advancedBoardAppearance.get<tgui::ToggleButton>("Off");
	tgui::ToggleButton::Ptr aMouseFollow = advancedBoardAppearance.get<tgui::ToggleButton>("Follow");
	tgui::ToggleButton::Ptr aMouseScale = advancedBoardAppearance.get<tgui::ToggleButton>("Scale");
	tgui::CheckBox::Ptr aShowOptionChanges = advancedBoardAppearance.get<tgui::CheckBox>("Show Option Changes");
	tgui::CheckBox::Ptr aAutoFlip = advancedBoardAppearance.get<tgui::CheckBox>("Auto Flip");
	tgui::CheckBox::Ptr aAtomicExplosionEffect = advancedBoardAppearance.get<tgui::CheckBox>("Atomic Explosions");
	tgui::BitmapButton::Ptr aColorPreview = advancedBoardAppearance.get<tgui::BitmapButton>("Color Preview");
	tgui::EditBox::Ptr aOffsetX = advancedBoardAppearance.get<tgui::EditBox>("X Offset");
	tgui::EditBox::Ptr aOffsetY = advancedBoardAppearance.get<tgui::EditBox>("Y Offset");
	tgui::EditBox::Ptr aBoardScale = advancedBoardAppearance.get<tgui::EditBox>("Board Scale");
	tgui::EditBox::Ptr aBoardSet = advancedBoardAppearance.get<tgui::EditBox>("Board Set");
	aPieceSet = advancedBoardAppearance.get<tgui::EditBox>("Piece Set");
	tgui::BitmapButton::Ptr aBoardSetPlus = advancedBoardAppearance.get<tgui::BitmapButton>("Board Set Plus");
	tgui::BitmapButton::Ptr aBoardSetMinus = advancedBoardAppearance.get<tgui::BitmapButton>("Board Set Minus");
	tgui::Label::Ptr aBoardSetPlusL = advancedBoardAppearance.get<tgui::Label>("Board Set Plus Label");
	tgui::Label::Ptr aBoardSetMinusL = advancedBoardAppearance.get<tgui::Label>("Board Set Minus Label");
	tgui::BitmapButton::Ptr aPieceSetPlus = advancedBoardAppearance.get<tgui::BitmapButton>("Piece Set Plus");
	tgui::BitmapButton::Ptr aPieceSetMinus = advancedBoardAppearance.get<tgui::BitmapButton>("Piece Set Minus");
	tgui::Label::Ptr aPieceSetPlusL = advancedBoardAppearance.get<tgui::Label>("Piece Set Plus Label");
	tgui::Label::Ptr aPieceSetMinusL = advancedBoardAppearance.get<tgui::Label>("Piece Set Minus Label");
	tgui::Picture::Ptr aBoardSetImage = advancedBoardAppearance.get<tgui::Picture>("Board Set Image");
	tgui::Picture::Ptr aPieceSetImage = advancedBoardAppearance.get<tgui::Picture>("Piece Set Image");
	tgui::Panel::Ptr aPieceSetBackground = advancedBoardAppearance.get<tgui::Panel>("Piece Set Background");
	tgui::BitmapButton::Ptr aColorButton = advancedBoardAppearance.get<tgui::BitmapButton>("Color Button");
	tgui::ColorPicker::Ptr aColorPicker = tgui::ColorPicker::create("Promote Square Color");
	aColorPicker->setVisible(false);
	tgui::Panel::Ptr aPanel = advancedBoardAppearance.get<tgui::Panel>("Panel2");
	tgui::BitmapButton::Ptr aAppCancel = advancedBoardAppearance.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr aAppClose = advancedBoardAppearance.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr aAppSave = advancedBoardAppearance.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr aAppLeft = advancedBoardAppearance.get<tgui::BitmapButton>("Left");
	tgui::BitmapButton::Ptr aAppRight = advancedBoardAppearance.get<tgui::BitmapButton>("Right");
	tgui::BitmapButton::Ptr aAppMain = advancedBoardAppearance.get<tgui::BitmapButton>("Main");

	aEnableKeybinds->onChange([&](bool c) { bP.keybindsEnabled = c; });
	aFlipBoard->onToggle([&](bool c) {
		aFlipBoard->onToggle.setEnabled(false);
		aFlipBoard->setDown(true);
		if (c && aFromBoard && selectedBoard) {
			selectedBoard->flipBoard();
		}
		std::thread buttonThread(setButtonDownAfter, std::ref(aFlipBoard), 75);
		buttonThread.detach();
		});
	aResetPosition->onToggle([&](bool c) {
		aResetPosition->onToggle.setEnabled(false);
		aResetPosition->setDown(true);
		if (c && aFromBoard && selectedBoard) {
			selectedBoard->resetPosition();
		}
		std::thread buttonThread(setButtonDownAfter, std::ref(aResetPosition), 75);
		buttonThread.detach();
		});
	aResetScale->onToggle([&](bool c) {
		aResetScale->onToggle.setEnabled(false);
		aResetScale->setDown(true);
		if (c && aFromBoard && selectedBoard) {
			selectedBoard->resetScale();
		}
		std::thread buttonThread(setButtonDownAfter, std::ref(aResetScale), 75);
		buttonThread.detach();
		});
	aMouseOff->onClick([&]() {
		aMouseOff->setDown(true);
		bP.followMouse = false;
		bP.scaleMouse = false;
		aMouseFollow->setDown(false);
		aMouseScale->setDown(false);
		});
	aMouseFollow->onClick([&]() {
		aMouseFollow->setDown(true);
		bP.followMouse = true;
		bP.scaleMouse = false;
		aMouseOff->setDown(false);
		aMouseScale->setDown(false);
		});
	aMouseScale->onClick([&]() {
		aMouseScale->setDown(true);
		bP.followMouse = false;
		bP.scaleMouse = true;
		aMouseOff->setDown(false);
		aMouseFollow->setDown(false);
		});
	aShowOptionChanges->onChange([&](bool c) { bP.showOptionChanges = c; });
	aAutoFlip->onChange([&](bool c) { bP.autoFlip = c; });
	aAtomicExplosionEffect->onChange([&](bool c) { bP.atomicExplosionEffect = c; });
	aOffsetX->onTextChange([&](const tgui::String& text) {
		bP.xOffset = text.toFloat();
		});
	aOffsetY->onTextChange([&](const tgui::String& text) {
		bP.yOffset = text.toFloat();
		});
	aBoardScale->onTextChange([&](const tgui::String& text) {
		bP.boardScale = text.toFloat();
		});

	aColorPreview->onMouseEnter([&]() {
		aColorButton->getRenderer()->setTexture("assets/ui/textures/gradient_hover_2.png");
		});
	aColorPreview->onMouseLeave([&]() {
		aColorButton->getRenderer()->setTexture("assets/ui/textures/gradient.png");
		});
	aColorPreview->onMousePress([&]() {
		aColorButton->getRenderer()->setTexture("assets/ui/textures/gradient_down_2.png");
		});
	aColorPreview->onMouseRelease([&]() {
		aColorButton->getRenderer()->setTexture("assets/ui/textures/gradient_hover_2.png");
		aPanel->getRenderer()->setOpacity(0.4f);
		aAppLeft->setEnabled(false);
		aAppRight->setEnabled(false);
		aColorPicker->setVisible(true);
		aColorPicker->create("Promote Square Color", bP.promotionSquareColor);
		aColorPicker->setOrigin(0.5f, 0.5f);
		aColorPicker->setPosition("50%", "50%");
		advancedBoardAppearance.add(aColorPicker);
		});
	aColorButton->onPress([&]() {
		aPanel->getRenderer()->setOpacity(0.4f);
		aAppLeft->setEnabled(false);
		aAppRight->setEnabled(false);
		aColorPicker->setVisible(true);
		aColorPicker->create("Promote Square Color", bP.promotionSquareColor);
		aColorPicker->setOrigin(0.5f, 0.5f);
		aColorPicker->setPosition("50%", "50%");
		advancedBoardAppearance.add(aColorPicker);
		});

	aColorPicker->onColorChange([&](tgui::Color color) {
		bP.promotionSquareColor = color;
		aColorPreview->getRenderer()->setBackgroundColor(color);
		});

	aColorPicker->onClose([&]() {
		aPanel->getRenderer()->setOpacity(0.0f);
		aAppLeft->setEnabled(true);
		aAppRight->setEnabled(true);
		});

	auto onBoardSetChangeA = [&](int add = 0) {
		int newInt = std::clamp(aBoardSet->getText().toInt() + add, 1, 24);
		aBoardSet->setText(std::to_string(newInt));
		aBoardSetMinus->setEnabled(true);
		aBoardSetPlus->setEnabled(true);
		aBoardSetMinusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		aBoardSetPlusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		if (newInt <= 1) {
			aBoardSetMinus->setEnabled(false);
			aBoardSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		else if (newInt >= 24) {
			aBoardSetPlus->setEnabled(false);
			aBoardSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		sf::Texture nBoardTexture = Chess::loadBoard(boardSpriteSheet, newInt - 1, { 2, 2 });
		nBoardTexture.setSmooth(true);
		tgui::Texture boardTextureT;
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		aBoardSetImage->getRenderer()->setTexture(boardTextureT);
		nBoardTexture = Chess::loadBoard(boardSpriteSheet, newInt - 1, bP.variantList->at(bP.variantIndex)->boardSize);
		boardTextureT.loadFromPixelData(nBoardTexture.getSize(), nBoardTexture.copyToImage().getPixelsPtr());
		aFENImage->getRenderer()->setTexture(boardTextureT);
		aRepeatFENImage->getRenderer()->setTexture(boardTextureT);
		aPieceSetBackground->getRenderer()->setBackgroundColor(nBoardTexture.copyToImage().getPixel({ 1U, 3U }));
		aFENBoard.setBoard(aFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		aRepeatFENBoard.setBoard(aRepeatFENImage, bP.variantList->at(bP.variantIndex)->boardSize);
		setAdvancedBoardTextures(nBoardTexture);
		bP.boardSet = newInt - 1;
		};

	onPieceSetChangeA = [&](int add = 0) {
		auto& variant = bP.variantList->at(bP.variantIndex);
		bool variantTexture = variantPieceTextures.find(variant->name) != variantPieceTextures.end();
		int newInt = variantTexture ? 1 : std::clamp(aPieceSet->getText().toInt() + add, 1, static_cast<int>(pieceTextures.size()));
		auto& pTextures = variantTexture ? variantPieceTextures.find(variant->name)->second : pieceTextures.at(newInt - 1);
		auto& pTexturesSVG = variantTexture ? variantPieceTexturesSVG.find(variant->name)->second : pieceTexturesSVG.at(newInt - 1);
		aPieceSet->setText(std::to_string(newInt));
		aPieceSetMinus->setEnabled(true);
		aPieceSetPlus->setEnabled(true);
		aPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		aPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#3C3C3C"));
		if (newInt <= 1 || variantTexture) {
			aPieceSetMinus->setEnabled(false);
			aPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		if (newInt >= pieceTextures.size() || variantTexture) {
			aPieceSetPlus->setEnabled(false);
			aPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
		}
		const sf::Texture& pieceTexture = pTextures.begin()->second;
		tgui::Texture pieceTextureT;
		pieceTextureT.loadFromPixelData(pieceTexture.getSize(), pieceTexture.copyToImage().getPixelsPtr());
		aPieceSetImage->getRenderer()->setTexture(pieceTextureT);
		bool reversed = false;
		std::string FEN = bP.FEN.value_or("");
		size_t find = FEN.find(' ');
		if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
			reversed = true;
		}
		aFENBoard.setPieces(FEN, *bP.variantList->at(bP.variantIndex).get(), pTexturesSVG, reversed);

		reversed = false;
		FEN = bP.newPositionFEN.value_or("");
		find = FEN.find(' ');
		if (!FEN.empty() && find != std::string::npos && find != (FEN.size() - 1) && FEN.at(FEN.find_first_of(' ') + 1) == 'b') {
			reversed = true;
		}
		aRepeatFENBoard.setPieces(FEN, *bP.variantList->at(bP.variantIndex).get(), pTexturesSVG, reversed);
		bP.pieceSet = newInt - 1;
		if (variantPieceTextures.find(variant->name) == variantPieceTextures.end()) { bP.pieceSetSave = bP.pieceSet; }
		};

	aBoardSet->onUnfocus(onBoardSetChangeA);
	aPieceSet->onUnfocus(onPieceSetChangeA, 0);

	aBoardSetMinus->onPress(onBoardSetChangeA, -1);
	aBoardSetPlus->onPress(onBoardSetChangeA, 1);
	aPieceSetMinus->onPress(onPieceSetChangeA, -1);
	aPieceSetPlus->onPress(onPieceSetChangeA, 1);

	aAppSave->onPress(saveAdvancedBoard);
	aAppClose->onPress([&]() { aBGUIVisible = false; });
	aAppCancel->onPress([&]() { aBGUIVisible = false; });
	aAppLeft->onPress([&]() { loadedGUI--; });
	aAppRight->onPress([&]() { loadedGUI++; });
	aAppMain->onPress([&]() {
		if (aFromBoard && !selectedBoard) { return; }
		loadBoardGUI(aCreateBoard ? 1 : (aFromBoard ? 2 : 0), bP, selectedSFSettings);
		aBGUIVisible = false;
		});

	auto loadAdvancedAppearance = [&]() {
		aFlipBoard->setEnabled(aFromBoard);
		aResetPosition->setEnabled(aFromBoard);
		aResetScale->setEnabled(aFromBoard);
		aEnableKeybinds->setChecked(bP.keybindsEnabled);
		aMouseOff->setDown(!bP.followMouse && !bP.scaleMouse);
		aMouseFollow->setDown(bP.followMouse && !bP.scaleMouse);
		aMouseScale->setDown(!bP.followMouse && bP.scaleMouse);
		aShowOptionChanges->setChecked(bP.showOptionChanges);
		aAutoFlip->setChecked(bP.autoFlip);
		aAtomicExplosionEffect->setChecked(bP.atomicExplosionEffect);
		aOffsetX->setText(tgui::String(floatToStr(bP.xOffset, stream)));
		aOffsetY->setText(tgui::String(floatToStr(bP.yOffset, stream)));
		aBoardScale->setText(tgui::String(floatToStr(bP.boardScale, stream)));
		aBoardSet->setText(tgui::String(std::to_string(bP.boardSet + 1)));
		aPieceSet->setText(tgui::String(std::to_string(bP.pieceSet + 1)));
		onBoardSetChangeA();

		auto& v = bP.variantList->at(bP.variantIndex);
		if (variantPieceTextures.find(v->name) != variantPieceTextures.end()) {
			aPieceSetMinus->setEnabled(false);
			aPieceSetMinusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			aPieceSetPlus->setEnabled(false);
			aPieceSetPlusL->getRenderer()->setTextColor(tgui::Color("#9E9E9E"));
			aPieceSet->setText("1");
		}
		else {
			aPieceSet->setText(std::to_string(bP.pieceSet + 1));
		}
		onPieceSetChangeA(0);
		aColorButton->getRenderer()->setTexture("assets/ui/textures/gradient.png");
		aColorPicker->setColor(tgui::Color(bP.promotionSquareColor));
		};

#pragma endregion

#pragma region AI
	advancedBoardAI.loadWidgetsFromFile("assets/ui/advanced_board_ai.txt");
	advancedBoardAI.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedBoardAI.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedBoardAI.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedBoardAI.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::CheckBox::Ptr aEnableStockfish = advancedBoardAI.get<tgui::CheckBox>("Enable Stockfish");
	tgui::CheckBox::Ptr aAIOnly = advancedBoardAI.get<tgui::CheckBox>("AI Only");
	tgui::Label::Ptr aAIOnlyLabel = advancedBoardAI.get<tgui::Label>("AI Only Label");
	aAIWhite = advancedBoardAI.get<tgui::ToggleButton>("AI White");
	aAIBlack = advancedBoardAI.get<tgui::ToggleButton>("AI Black");
	tgui::ToggleButton::Ptr aTimeBased = advancedBoardAI.get<tgui::ToggleButton>("Time Based");
	tgui::ToggleButton::Ptr aFixed = advancedBoardAI.get<tgui::ToggleButton>("Fixed");
	tgui::EditBox::Ptr aFixedTime = advancedBoardAI.get<tgui::EditBox>("Fixed Time");
	tgui::BitmapButton::Ptr aFixedTimeUnit = advancedBoardAI.get<tgui::BitmapButton>("Fixed Time Unit");
	tgui::Label::Ptr aFixedTimeLabel = advancedBoardAI.get<tgui::Label>("Fixed Time Label");
	tgui::EditBox::Ptr aSkillLevel = advancedBoardAI.get<tgui::EditBox>("Skill Level");
	tgui::SpinButton::Ptr aSkillSpin = advancedBoardAI.get<tgui::SpinButton>("Skill Spin");
	tgui::BitmapButton::Ptr aAICancel = advancedBoardAI.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr aAIClose = advancedBoardAI.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr aAISave = advancedBoardAI.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr aAILeft = advancedBoardAI.get<tgui::BitmapButton>("Left");
	tgui::BitmapButton::Ptr aAIRight = advancedBoardAI.get<tgui::BitmapButton>("Right");
	tgui::BitmapButton::Ptr aAIMain = advancedBoardAI.get<tgui::BitmapButton>("Main");

	aEnableStockfish->onChange([&](bool c) {
		bP.AI = c;
		aAIOnly->setEnabled(c);
		if (c) {
			aAIOnlyLabel->getRenderer()->setTextColor("#3C3C3C");
		}
		else {
			aAIOnlyLabel->getRenderer()->setTextColor("#9E9E9E");
			aAIOnly->setChecked(false);
		}
		});
	aAIOnly->onChange([&](bool c) {
		bP.AI_Only = c;
		});
	aAIWhite->onClick([&]() {
		if (aFromBoard) {
			bP.white = false;
			aAIWhite->setDown(true);
			aAIBlack->setDown(false);
			aPlayerWhite->setDown(false);
			aPlayerBlack->setDown(true);
		}
		else {
			if (aAIWhite->isDown()) {
				bP.white = false;
				aAIBlack->setDown(false);
				aPlayerWhite->setDown(false);
				aPlayerBlack->setDown(true);
			}
			else {
				bP.white = std::nullopt;
				aPlayerWhite->setDown(false);
				aPlayerBlack->setDown(false);
			}
		}
		});
	aAIBlack->onClick([&]() {
		if (aFromBoard) {
			bP.white = true;
			aAIBlack->setDown(true);
			aAIWhite->setDown(false);
			aPlayerWhite->setDown(true);
			aPlayerBlack->setDown(false);
		}
		else {
			if (aAIBlack->isDown()) {
				bP.white = true;
				aAIWhite->setDown(false);
				aPlayerWhite->setDown(true);
				aPlayerBlack->setDown(false);
			}
			else {
				bP.white = std::nullopt;
				aPlayerWhite->setDown(false);
				aPlayerBlack->setDown(false);
			}
		}
		});
	aTimeBased->onClick([&]() {
		aTimeBased->setDown(true);
		aFixed->setDown(false);
		selectedSFSettings.usesFixedTime = false;
		});
	aFixed->onClick([&]() {
		aFixed->setDown(true);
		aTimeBased->setDown(false);
		selectedSFSettings.usesFixedTime = true;
		});
	aFixedTime->onTextChange([&]() {
		switch (aFixedTimeUnit->getUserData<int>()) {
		case 0:
			selectedSFSettings.fixedTime = sf::seconds(aFixedTime->getText().toFloat());
			break;
		case 1:
			selectedSFSettings.fixedTime = sf::seconds(aFixedTime->getText().toFloat() * 60);
			break;
		}
		});
	aFixedTimeUnit->onClick([&]() {
		int newInt = (aFixedTimeUnit->getUserData<int>() + 1) % 2;
		aFixedTimeUnit->setUserData(newInt);
		switch (newInt) {
		case 0:
			aFixedTimeUnit->setText("s");
			break;
		case 1:
			aFixedTimeUnit->setText("min");
			break;
		}
		if (controlClicked) {
			std::ostringstream str;
			str << std::fixed << std::setprecision(2);
			switch (newInt) {
			case 0:
				str << selectedSFSettings.fixedTime.asSeconds();
				aFixedTime->setText(str.str());
				break;
			case 1:
				str << selectedSFSettings.fixedTime.asSeconds() / 60;
				aFixedTime->setText(str.str());
				break;
			}
		}
		});
	aSkillLevel->onTextChange([&]() {
		aSkillLevel->onTextChange.setEnabled(false);
		aSkillLevel->setText(std::to_string(std::clamp(aSkillLevel->getText().toInt(), -20, 20)));
		aSkillLevel->onTextChange.setEnabled(true);
		selectedSFSettings.skillLevel = aSkillLevel->getText().toInt();
		});
	aSkillSpin->onValueChange([&](float value) {
		aSkillSpin->onValueChange.setEnabled(false);
		aSkillSpin->setValue(1);
		aSkillSpin->onValueChange.setEnabled(true);
		aSkillLevel->setText(std::to_string(int((value - 1) + selectedSFSettings.skillLevel)));
		});

	aAILeft->onPress([&]() { loadedGUI--; });
	aAISave->onPress(saveAdvancedBoard);
	aAIClose->onPress([&]() { aBGUIVisible = false; });
	aAICancel->onPress([&]() { aBGUIVisible = false; });
	aAIMain->onPress([&]() {
		if (aFromBoard && !selectedBoard) { return; }
		loadBoardGUI(aCreateBoard ? 1 : (aFromBoard ? 2 : 0), bP, selectedSFSettings);
		aBGUIVisible = false;
		});

	auto loadAdvancedAI = [&]() {
		aEnableStockfish->setChecked(bP.AI);
		aAIOnly->setChecked(bP.AI_Only);
		aAIWhite->setDown(bP.white.has_value() && !bP.white.value());
		aAIBlack->setDown(bP.white.has_value() && bP.white.value());
		if (selectedSFSettings.usesFixedTime) {
			aFixed->setDown(true);
			aTimeBased->setDown(false);
			aFixedTime->setEnabled(true);
			aFixedTimeUnit->setEnabled(true);
			aFixedTimeLabel->getRenderer()->setTextColor("#3C3C3C");
		}
		else {
			aTimeBased->setDown(true);
			aFixed->setDown(false);
			aFixedTime->setEnabled(false);
			aFixedTimeUnit->setEnabled(false);
			aFixedTimeLabel->getRenderer()->setTextColor("#9E9E9E");
		}
		aFixedTimeUnit->setText("s");
		aFixedTimeUnit->setUserData(0);
		aFixedTime->setText(timeToStr(selectedSFSettings.fixedTime, timeStream));
		aAIOnly->setEnabled(bP.AI);
		aAIOnlyLabel->getRenderer()->setTextColor(bP.AI ? "#3C3C3C" : "#9E9E9E");
		aSkillLevel->setText(std::to_string(selectedSFSettings.skillLevel));
		};

#pragma endregion

	setAdvancedBoardTextures = [&](const sf::Texture& boardT) {
		bAIGradient = makeAIStrengthTexture(aGUIGradient, boardT.copyToImage().getPixel({ 129, 129 }));
		tgui::Texture texture;
		texture.loadFromPixelData(bAIGradient.getSize(), bAIGradient.getPixelsPtr());
		aPlayerWhite->getRenderer()->setTextureDown(texture);
		aPlayerBlack->getRenderer()->setTextureDown(texture);
		aPlayerWhiteRestart->getRenderer()->setTextureDown(texture);
		aPlayerBlackRestart->getRenderer()->setTextureDown(texture);
		aPlayerRandomRestart->getRenderer()->setTextureDown(texture);
		aFlipBoard->getRenderer()->setTextureDown(texture);
		aResetPosition->getRenderer()->setTextureDown(texture);
		aResetScale->getRenderer()->setTextureDown(texture);
		aMouseOff->getRenderer()->setTextureDown(texture);
		aMouseFollow->getRenderer()->setTextureDown(texture);
		aMouseScale->getRenderer()->setTextureDown(texture);
		aTimeOff->getRenderer()->setTextureDown(texture);
		aTimeSingle->getRenderer()->setTextureDown(texture);
		aTimeShared->getRenderer()->setTextureDown(texture);
		aTimeAI->getRenderer()->setTextureDown(texture);
		aAIWhite->getRenderer()->setTextureDown(texture);
		aAIBlack->getRenderer()->setTextureDown(texture);
		aFixed->getRenderer()->setTextureDown(texture);
		aTimeBased->getRenderer()->setTextureDown(texture);
		aCycle->getRenderer()->setTextureDown(texture);
		};

	loadAdvancedBoard = [&](const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings, bool fromBoard, bool createBoard) {
		loadedGUI = 0;
		aFromBoard = fromBoard;
		aCreateBoard = createBoard;
		bP = boardSettings;
		selectedSFSettings = stockfishSettings;
		loadAdvancedTime();
		loadAdvancedPosition();
		loadAdvancedAppearance();
		loadAdvancedAI();
		aAISave->setText(createBoard ? "Create" : "Save");
		aTimeSave->setText(createBoard ? "Create" : "Save");
		aAppSave->setText(createBoard ? "Create" : "Save");
		aPosSave->setText(createBoard ? "Create" : "Save");
		aBGUIVisible = true;
		};

#pragma endregion

#pragma region Advanced Stockfish GUI

	int asLoadedGUI = 0;
	bool asFromBoard = false;

	auto saveAdvancedStockfish = [&]() {
		if (!asFromBoard) {
			sfSettings = selectedSFSettings;
			boardSettings = bP;
		}
		else if (selectedBoard) {
			selectedBoard->setStockfishSettings(selectedSFSettings);
			selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
		}
		asLoadedGUI = 0;
		asGUIvisible = false;
		};

#pragma region Stockfish 1

	advancedStockfish.loadWidgetsFromFile("assets/ui/advanced_stockfish.txt");
	advancedStockfish.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedStockfish.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedStockfish.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedStockfish.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::TextArea::Ptr asDebugLogFile = advancedStockfish.get<tgui::TextArea>("Debug Log File");
	tgui::EditBox::Ptr asThreads = advancedStockfish.get<tgui::EditBox>("Threads");
	tgui::SpinButton::Ptr asThreadsSpin = advancedStockfish.get<tgui::SpinButton>("Threads Spin");
	tgui::EditBox::Ptr asHash = advancedStockfish.get<tgui::EditBox>("Hash");
	tgui::BitmapButton::Ptr asClearHash = advancedStockfish.get<tgui::BitmapButton>("Clear Hash");
	tgui::CheckBox::Ptr asPonder = advancedStockfish.get<tgui::CheckBox>("Ponder");
	tgui::EditBox::Ptr asMultiPV = advancedStockfish.get<tgui::EditBox>("MultiPV");
	tgui::SpinButton::Ptr asMultiPVSpin = advancedStockfish.get<tgui::SpinButton>("MultiPV Spin");
	tgui::EditBox::Ptr asSkillLevel = advancedStockfish.get<tgui::EditBox>("Skill Level");
	tgui::SpinButton::Ptr asSkillSpin = advancedStockfish.get<tgui::SpinButton>("Skill Level Spin");
	tgui::EditBox::Ptr asMoveOverhead = advancedStockfish.get<tgui::EditBox>("Move Overhead");
	tgui::SpinButton::Ptr asMoveOverheadSpin = advancedStockfish.get<tgui::SpinButton>("Move Overhead Spin");
	tgui::EditBox::Ptr asSlowMover = advancedStockfish.get<tgui::EditBox>("Slow Mover");
	tgui::SpinButton::Ptr asSlowMoverSpin = advancedStockfish.get<tgui::SpinButton>("Slow Mover Spin");
	tgui::EditBox::Ptr asNodestime = advancedStockfish.get<tgui::EditBox>("nodestime");
	tgui::CheckBox::Ptr asUCI_Chess960 = advancedStockfish.get<tgui::CheckBox>("UCI_Chess960");
	tgui::TextArea::Ptr asUCI_Variant = advancedStockfish.get<tgui::TextArea>("UCI_Variant");
	tgui::CheckBox::Ptr asUCI_AnalyseMode = advancedStockfish.get<tgui::CheckBox>("UCI_AnalyseMode");
	tgui::BitmapButton::Ptr asRight = advancedStockfish.get<tgui::BitmapButton>("Right");
	tgui::BitmapButton::Ptr asSave = advancedStockfish.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr asCancel = advancedStockfish.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr asClose = advancedStockfish.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr asMain = advancedStockfish.get<tgui::BitmapButton>("Main");

	asDebugLogFile->onTextChange([&](const tgui::String& text) {
		selectedSFSettings.debugLogFile = text.toStdString();
		});
	asThreads->onTextChange([&]() {
		asThreads->onTextChange.setEnabled(false);
		asThreads->setText(std::to_string(std::clamp(asThreads->getText().toInt(), 1, 512)));
		asThreads->onTextChange.setEnabled(true);
		selectedSFSettings.sfThreads = asThreads->getText().toInt();
		});
	asThreadsSpin->onValueChange([&](float value) {
		asThreadsSpin->onValueChange.setEnabled(false);
		asThreadsSpin->setValue(1);
		asThreadsSpin->onValueChange.setEnabled(true);
		asThreads->setText(std::to_string(int((value - 1) + selectedSFSettings.sfThreads)));
		});
	asHash->onTextChange([&]() {
		asHash->onTextChange.setEnabled(false);
		asHash->setText(std::to_string(std::clamp(asHash->getText().toInt(), 1, 33554432)));
		asHash->onTextChange.setEnabled(true);
		selectedSFSettings.sfMemory = asHash->getText().toInt();
		});
	asClearHash->onClick([&]() {
		if (selectedBoard) {
			selectedBoard->getEngine().clearHash();
		}
		});
	asPonder->onChange([&](bool c) {
		selectedSFSettings.ponder = c;
		});
	asMultiPV->onTextChange([&]() {
		asMultiPV->onTextChange.setEnabled(false);
		asMultiPV->setText(std::to_string(std::clamp(asMultiPV->getText().toInt(), 1, 500)));
		asMultiPV->onTextChange.setEnabled(true);
		selectedSFSettings.multiPV = asMultiPV->getText().toInt();
		});
	asMultiPVSpin->onValueChange([&](float value) {
		asMultiPVSpin->onValueChange.setEnabled(false);
		asMultiPVSpin->setValue(1);
		asMultiPVSpin->onValueChange.setEnabled(true);
		asMultiPV->setText(std::to_string(int((value - 1) + selectedSFSettings.multiPV)));
		});
	asSkillLevel->onTextChange([&]() {
		asSkillLevel->onTextChange.setEnabled(false);
		asSkillLevel->setText(std::to_string(std::clamp(asSkillLevel->getText().toInt(), -20, 20)));
		asSkillLevel->onTextChange.setEnabled(true);
		selectedSFSettings.skillLevel = asSkillLevel->getText().toInt();
		});
	asSkillSpin->onValueChange([&](float value) {
		asSkillSpin->onValueChange.setEnabled(false);
		asSkillSpin->setValue(1);
		asSkillSpin->onValueChange.setEnabled(true);
		asSkillLevel->setText(std::to_string(int((value - 1) + selectedSFSettings.skillLevel)));
		});
	asMoveOverhead->onTextChange([&]() {
		asMoveOverhead->onTextChange.setEnabled(false);
		asMoveOverhead->setText(std::to_string(std::clamp(asMoveOverhead->getText().toInt(), 0, 5000)));
		asMoveOverhead->onTextChange.setEnabled(true);
		selectedSFSettings.moveOverhead = asMoveOverhead->getText().toInt();
		});
	asMoveOverheadSpin->onValueChange([&](float value) {
		asMoveOverheadSpin->onValueChange.setEnabled(false);
		asMoveOverheadSpin->setValue(1);
		asMoveOverheadSpin->onValueChange.setEnabled(true);
		asMoveOverhead->setText(std::to_string(int((value - 1) + selectedSFSettings.moveOverhead)));
		});
	asSlowMover->onTextChange([&]() {
		asSlowMover->onTextChange.setEnabled(false);
		asSlowMover->setText(std::to_string(std::clamp(asSlowMover->getText().toInt(), 10, 1000)));
		asSlowMover->onTextChange.setEnabled(true);
		selectedSFSettings.slowMover = asSlowMover->getText().toInt();
		});
	asSlowMoverSpin->onValueChange([&](float value) {
		asSlowMoverSpin->onValueChange.setEnabled(false);
		asSlowMoverSpin->setValue(1);
		asSlowMoverSpin->onValueChange.setEnabled(true);
		asSlowMover->setText(std::to_string(int((value - 1) + selectedSFSettings.slowMover)));
		});
	asNodestime->onTextChange([&]() {
		asNodestime->onTextChange.setEnabled(false);
		asNodestime->setText(std::to_string(std::clamp(asNodestime->getText().toInt(), 0, 10000)));
		asNodestime->onTextChange.setEnabled(true);
		selectedSFSettings.nodestime = asNodestime->getText().toInt();
		});
	asUCI_Chess960->onChange([&](bool c) {
		selectedSFSettings.UCI_Chess960 = c;
		});
	asUCI_Variant->onTextChange([&]() {
		selectedSFSettings.UCI_Variant = asUCI_Variant->getText().toStdString();
		});
	asUCI_AnalyseMode->onChange([&](bool c) {
		selectedSFSettings.UCI_AnalyseMode = c;
		});
	asRight->onPress([&]() { asLoadedGUI++; });

	asSave->onClick(saveAdvancedStockfish);
	asCancel->onClick([&]() {
		asLoadedGUI = 0;
		asGUIvisible = false;
		});
	asClose->onClick([&]() {
		asLoadedGUI = 0;
		asGUIvisible = false;
		});
	asMain->onPress([&]() {
		if (asFromBoard && !selectedBoard) { return; }
		loadStockfishGUI(bP, selectedSFSettings, asFromBoard);
		asGUIvisible = false;
		});

#pragma endregion

#pragma region Stockfish 2
	advancedStockfish2.loadWidgetsFromFile("assets/ui/advanced_stockfish_2.txt");
	advancedStockfish2.get<tgui::Panel>("Panel")->setOrigin(0.5f, 0.5f);
	advancedStockfish2.get<tgui::Panel>("Panel")->setPosition("50%", "50%");
	advancedStockfish2.get<tgui::Panel>("Arrows")->setOrigin(0.5f, 0.5f);
	advancedStockfish2.get<tgui::Panel>("Arrows")->setPosition("50%", "50%");
	tgui::CheckBox::Ptr asUCI_LimitStrength = advancedStockfish2.get<tgui::CheckBox>("UCI_LimitStrength");
	tgui::EditBox::Ptr asUCI_Elo = advancedStockfish2.get<tgui::EditBox>("UCI_Elo");
	tgui::SpinButton::Ptr asUCI_EloSpin = advancedStockfish2.get<tgui::SpinButton>("UCI_Elo Spin");
	tgui::CheckBox::Ptr asUCI_Show_WDL = advancedStockfish2.get<tgui::CheckBox>("UCI_ShowWDL");
	tgui::TextArea::Ptr asSyzygyPath = advancedStockfish2.get<tgui::TextArea>("SyzygyPath");
	tgui::EditBox::Ptr asSyzygyProbeDepth = advancedStockfish2.get<tgui::EditBox>("SyzygyProbeDepth");
	tgui::SpinButton::Ptr asSyzygyProbeDepthSpin = advancedStockfish2.get<tgui::SpinButton>("SyzygyProbeDepth Spin");
	tgui::CheckBox::Ptr asSyzygy50MoveRule = advancedStockfish2.get<tgui::CheckBox>("Syzygy50MoveRule");
	tgui::EditBox::Ptr asSyzygyProbeLimit = advancedStockfish2.get<tgui::EditBox>("SyzygyProbeLimit");
	tgui::SpinButton::Ptr asSyzygyProbeLimitSpin = advancedStockfish2.get<tgui::SpinButton>("SyzygyProbeLimit Spin");
	tgui::CheckBox::Ptr asUseNNUE = advancedStockfish2.get<tgui::CheckBox>("Use NNUE");
	tgui::TextArea::Ptr asEvalFile = advancedStockfish2.get<tgui::TextArea>("EvalFile");
	tgui::CheckBox::Ptr asTsumeMode = advancedStockfish2.get<tgui::CheckBox>("TsumeMode");
	tgui::TextArea::Ptr asVariantPath = advancedStockfish2.get<tgui::TextArea>("VariantPath");
	tgui::TextArea::Ptr asRunCommand = advancedStockfish2.get<tgui::TextArea>("Run Command");
	tgui::BitmapButton::Ptr asRunButton = advancedStockfish2.get<tgui::BitmapButton>("Run Button");
	tgui::BitmapButton::Ptr asLeft = advancedStockfish2.get<tgui::BitmapButton>("Left");
	tgui::BitmapButton::Ptr asSave2 = advancedStockfish2.get<tgui::BitmapButton>("Save");
	tgui::BitmapButton::Ptr asCancel2 = advancedStockfish2.get<tgui::BitmapButton>("Cancel");
	tgui::BitmapButton::Ptr asClose2 = advancedStockfish2.get<tgui::BitmapButton>("Close Button");
	tgui::BitmapButton::Ptr asMain2 = advancedStockfish2.get<tgui::BitmapButton>("Main");

	asUCI_LimitStrength->onChange([&](bool c) {
		selectedSFSettings.UCI_LimitStrength = c;
		});
	asUCI_Elo->onTextChange([&]() {
		asUCI_Elo->onTextChange.setEnabled(false);
		asUCI_Elo->setText(std::to_string(std::clamp(asUCI_Elo->getText().toInt(), 500, 2850)));
		asUCI_Elo->onTextChange.setEnabled(true);
		selectedSFSettings.UCI_Elo = asUCI_Elo->getText().toInt();
		});
	asUCI_EloSpin->onValueChange([&](float value) {
		asUCI_EloSpin->onValueChange.setEnabled(false);
		asUCI_EloSpin->setValue(1);
		asUCI_EloSpin->onValueChange.setEnabled(true);
		asUCI_Elo->setText(std::to_string(int((value - 1) + selectedSFSettings.UCI_Elo)));
		});
	asUCI_Show_WDL->onChange([&](bool c) {
		selectedSFSettings.UCI_ShowWDL = c;
		});
	asSyzygyPath->onTextChange([&]() {
		selectedSFSettings.SyzygyPath = asSyzygyPath->getText().toStdString();
		});
	asSyzygyProbeDepth->onTextChange([&]() {
		asSyzygyProbeDepth->onTextChange.setEnabled(false);
		asSyzygyProbeDepth->setText(std::to_string(std::clamp(asSyzygyProbeDepth->getText().toInt(), 1, 100)));
		asSyzygyProbeDepth->onTextChange.setEnabled(true);
		selectedSFSettings.SyzygyProbeDepth = asSyzygyProbeDepth->getText().toInt();
		});
	asSyzygyProbeDepthSpin->onValueChange([&](float value) {
		asSyzygyProbeDepthSpin->onValueChange.setEnabled(false);
		asSyzygyProbeDepthSpin->setValue(1);
		asSyzygyProbeDepthSpin->onValueChange.setEnabled(true);
		asSyzygyProbeDepth->setText(std::to_string(int((value - 1) + selectedSFSettings.SyzygyProbeDepth)));
		});
	asSyzygy50MoveRule->onChange([&](bool c) {
		selectedSFSettings.Syzygy50MoveRule = c;
		});
	asSyzygyProbeLimit->onTextChange([&]() {
		asSyzygyProbeLimit->onTextChange.setEnabled(false);
		asSyzygyProbeLimit->setText(std::to_string(std::clamp(asSyzygyProbeLimit->getText().toInt(), 0, 7)));
		asSyzygyProbeLimit->onTextChange.setEnabled(true);
		selectedSFSettings.SyzygyProbeLimit = asSyzygyProbeLimit->getText().toInt();
		});
	asSyzygyProbeLimitSpin->onValueChange([&](float value) {
		asSyzygyProbeLimitSpin->onValueChange.setEnabled(false);
		asSyzygyProbeLimitSpin->setValue(1);
		asSyzygyProbeLimitSpin->onValueChange.setEnabled(true);
		asSyzygyProbeLimit->setText(std::to_string(int((value - 1) + selectedSFSettings.SyzygyProbeLimit)));
		});
	asUseNNUE->onChange([&](bool c) {
		selectedSFSettings.useNNUE = c;
		});
	asEvalFile->onTextChange([&]() {
		selectedSFSettings.EvalFile = asEvalFile->getText().toStdString();
		});
	asTsumeMode->onChange([&](bool c) {
		selectedSFSettings.TsumeMode = c;
		});
	asVariantPath->onTextChange([&]() {
		selectedSFSettings.VariantPath = asVariantPath->getText().toStdString();
		});
	asRunButton->onClick([&]() {
		if (selectedBoard) {
			selectedBoard->getEngine().runCommand(asRunCommand->getText().toStdString());
		}
		});
	asLeft->onPress([&]() { asLoadedGUI--; });

	asSave2->onClick(saveAdvancedStockfish);
	asCancel2->onClick([&]() {
		asLoadedGUI = 0;
		asGUIvisible = false;
		});
	asClose2->onClick([&]() {
		asLoadedGUI = 0;
		asGUIvisible = false;
		});
	asMain2->onPress([&]() {
		if (asFromBoard && !selectedBoard) { return; }
		loadStockfishGUI(bP, selectedSFSettings, asFromBoard);
		asGUIvisible = false;
		});

	loadAdvancedStockfishGUI = [&](const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings, bool fromBoard) {
		selectedSFSettings = stockfishSettings;
		bP = boardSettings;
		asFromBoard = fromBoard;
		asDebugLogFile->setText(stockfishSettings.debugLogFile);
		asThreads->setText(std::to_string(stockfishSettings.sfThreads));
		asHash->setText(std::to_string(stockfishSettings.sfMemory));
		asClearHash->setEnabled(asFromBoard);
		asPonder->setChecked(stockfishSettings.ponder);
		asMultiPV->setText(std::to_string(stockfishSettings.multiPV));
		asSkillLevel->setText(std::to_string(stockfishSettings.skillLevel));
		asMoveOverhead->setText(std::to_string(stockfishSettings.moveOverhead));
		asSlowMover->setText(std::to_string(stockfishSettings.slowMover));
		asNodestime->setText(std::to_string(stockfishSettings.nodestime));
		asUCI_Chess960->setChecked(stockfishSettings.UCI_Chess960);
		asUCI_Variant->setText(stockfishSettings.UCI_Variant);
		asUCI_AnalyseMode->setChecked(stockfishSettings.UCI_AnalyseMode);
		asUCI_LimitStrength->setChecked(stockfishSettings.UCI_LimitStrength);
		asUCI_Elo->setText(std::to_string(stockfishSettings.UCI_Elo));
		asUCI_Show_WDL->setChecked(stockfishSettings.UCI_ShowWDL);
		asSyzygyPath->setText(stockfishSettings.SyzygyPath);
		asSyzygyProbeDepth->setText(std::to_string(stockfishSettings.SyzygyProbeDepth));
		asSyzygy50MoveRule->setChecked(stockfishSettings.Syzygy50MoveRule);
		asSyzygyProbeLimit->setText(std::to_string(stockfishSettings.SyzygyProbeLimit));
		asUseNNUE->setChecked(stockfishSettings.useNNUE);
		asEvalFile->setText(stockfishSettings.EvalFile);
		asTsumeMode->setChecked(stockfishSettings.TsumeMode);
		asVariantPath->setText(stockfishSettings.VariantPath);
		asRunCommand->setEnabled(asFromBoard);
		asRunButton->setEnabled(asFromBoard);
		asGUIvisible = true;
		};

#pragma endregion

#pragma endregion

#pragma region Header GUI

	sf::Clock headerClock;
	headerClock.reset();
	bool hBoardHover = false, hBoardBoundsHover = false, hEngineHover = false, hEngineBoundsHover = false, hTopHover = false, hTopHovered = false, hBackgroundHover = false;
	bool hidHeader = false;

	headerGUI.loadWidgetsFromFile("assets/ui/header.txt");
	headerGUI2.loadWidgetsFromFile("assets/ui/header_2.txt");
	headerGUI3.loadWidgetsFromFile("assets/ui/header_3.txt");
	tgui::ToggleButton::Ptr hBoard = headerGUI.get<tgui::ToggleButton>("Board Settings");
	tgui::ToggleButton::Ptr hEngine = headerGUI.get<tgui::ToggleButton>("Engine Settings");
	tgui::ToggleButton::Ptr hBackground = headerGUI.get<tgui::ToggleButton>("Change Background");
	tgui::Group::Ptr hBoardGroup = headerGUI.get<tgui::Group>("Board");
	tgui::BitmapButton::Ptr hBoardBounds = headerGUI2.get<tgui::BitmapButton>("Board");
	tgui::BitmapButton::Ptr hCreateBoard = headerGUI.get<tgui::BitmapButton>("Create Board");
	tgui::BitmapButton::Ptr hCreateBoardWithSettings = headerGUI.get<tgui::BitmapButton>("Create Board With Settings");
	tgui::BitmapButton::Ptr hNewBoardSettings = headerGUI.get<tgui::BitmapButton>("New Board Settings");
	tgui::BitmapButton::Ptr hSelectedBoardSettings = headerGUI.get<tgui::BitmapButton>("Selected Board");
	tgui::BitmapButton::Ptr hNewBoardAdvanced = headerGUI.get<tgui::BitmapButton>("New Board Advanced");
	tgui::BitmapButton::Ptr hSelectedBoardAdvanced = headerGUI.get<tgui::BitmapButton>("Selected Board Advanced");
	tgui::BitmapButton::Ptr hCopyFEN = headerGUI.get<tgui::BitmapButton>("Copy FEN");
	tgui::BitmapButton::Ptr hLoadFEN = headerGUI.get<tgui::BitmapButton>("Load FEN");
	tgui::BitmapButton::Ptr hDeleteSelectedBoard = headerGUI.get<tgui::BitmapButton>("Delete Selected Board");
	tgui::BitmapButton::Ptr hDeleteAllBoards = headerGUI.get<tgui::BitmapButton>("Delete All Boards");
	tgui::Label::Ptr hSelectedBoardText = headerGUI.get<tgui::Label>("Selected Board Text");
	tgui::Label::Ptr hSelectedBoardAdvancedText = headerGUI.get<tgui::Label>("Selected Board Advanced Text");
	tgui::Label::Ptr hCopyFENText = headerGUI.get<tgui::Label>("Copy FEN Text");
	tgui::Label::Ptr hLoadFENText = headerGUI.get<tgui::Label>("Load FEN Text");
	tgui::Label::Ptr hDeleteSelectedBoardText = headerGUI.get<tgui::Label>("Delete Selected Board Text");
	tgui::Label::Ptr hSelectedBoardShortcut = headerGUI.get<tgui::Label>("Selected Board Shortcut");
	tgui::Label::Ptr hSelectedBoardAdvancedShortcut = headerGUI.get<tgui::Label>("Selected Board Advanced Shortcut");
	tgui::Label::Ptr hCopyFENShortcut = headerGUI.get<tgui::Label>("Copy FEN Shortcut");
	tgui::Label::Ptr hLoadFENShortcut = headerGUI.get<tgui::Label>("Load FEN Shortcut");
	tgui::Label::Ptr hDeleteSelectedBoardShortcut = headerGUI.get<tgui::Label>("Delete Board Shortcut");
	tgui::Label::Ptr hDeleteAllBoardsLabel = headerGUI.get<tgui::Label>("Delete All Boards Label");
	tgui::Label::Ptr hDeleteAllBoardsShortcut = headerGUI.get<tgui::Label>("Delete All Boards Shortcut");
	tgui::Group::Ptr hEngineGroup = headerGUI.get<tgui::Group>("Engine");
	tgui::BitmapButton::Ptr hEngineBounds = headerGUI2.get<tgui::BitmapButton>("Engine");
	tgui::BitmapButton::Ptr hNewBoardEngineSettings = headerGUI.get<tgui::BitmapButton>("New Board Engine");
	tgui::BitmapButton::Ptr hSelectedBoardEngineSettings = headerGUI.get<tgui::BitmapButton>("Selected Board Engine");
	tgui::BitmapButton::Ptr hNewEngineAdvancedSettings = headerGUI.get<tgui::BitmapButton>("New Engine Advanced");
	tgui::BitmapButton::Ptr hSelectedEngineAdvancedSettings = headerGUI.get<tgui::BitmapButton>("Selected Engine Advanced");
	tgui::Label::Ptr hSelectedBoardEngineText = headerGUI.get<tgui::Label>("Selected Board Engine Text");
	tgui::Label::Ptr hSelectedEngineAdvancedText = headerGUI.get<tgui::Label>("Selected Engine Advanced Text");
	tgui::Label::Ptr hSelectedBoardEngineShortcut = headerGUI.get<tgui::Label>("Selected Board Engine Shortcut");
	tgui::Label::Ptr hSelectedEngineAdvancedShortcut = headerGUI.get<tgui::Label>("Selected Engine Advanced Shortcut");
	tgui::Panel::Ptr hShadow = headerGUI3.get<tgui::Panel>("Shadow");
	tgui::Panel::Ptr hShadow2 = headerGUI3.get<tgui::Panel>("Show Shadow");

	hBoard->onMouseEnter([&]() { hBoardHover = true; });
	hBoard->onMouseLeave([&]() { hBoardHover = false; });
	hBoardBounds->onMouseEnter([&]() { hBoardBoundsHover = true; });
	hBoardBounds->onMouseLeave([&]() { hBoardBoundsHover = false; });
	hEngine->onMouseEnter([&]() { hEngineHover = true; });
	hEngine->onMouseLeave([&]() { hEngineHover = false; });
	hEngineBounds->onMouseEnter([&]() { hEngineBoundsHover = true; });
	hEngineBounds->onMouseLeave([&]() { hEngineBoundsHover = false; });
	hBackground->onMouseEnter([&]() { hBackgroundHover = true; });
	hBackground->onMouseLeave([&]() { hBackgroundHover = false; });
	

	hBoard->onToggle([&](bool c) {
		hBoardGroup->setIgnoreMouseEvents(!c);
		hBoardBounds->setIgnoreMouseEvents(!c);
		hEngineGroup->setIgnoreMouseEvents(c);
		hEngineBounds->setIgnoreMouseEvents(c);
		hBoardGroup->setEnabled(c);
		if (hBoard->isMouseDown() && !c) {
			hBoard->setDown(true);
		}
		if (!hBoard->isDown()) { headerClock.restart(); }
		});
	hEngine->onToggle([&](bool c) {
		hEngineGroup->setIgnoreMouseEvents(!c);
		hEngineBounds->setIgnoreMouseEvents(!c);
		hBoardGroup->setIgnoreMouseEvents(c);
		hBoardBounds->setIgnoreMouseEvents(c);
		hEngineGroup->setEnabled(c);
		if (hEngine->isMouseDown() && !c) {
			hEngine->setDown(true);
		}
		if (!hEngine->isDown()) { headerClock.restart(); }
		});
	hBackground->onToggle([&](bool c) {
		if (c) {
			++backgroundIndex %= backgroundTextures.size();
			backgroundTexture = &backgroundTextures.at(backgroundIndex);
			backgroundSprite.setTexture(*backgroundTexture, true);
			backgroundSprite.setScale(sf::Vector2f{ window.getSize().x / static_cast<float>(backgroundTexture->getSize().x),
													window.getSize().y / static_cast<float>(backgroundTexture->getSize().y) });
			backgroundSprite.setOrigin(backgroundSprite.getLocalBounds().getCenter());
			hBackground->setDown(false);
		}
		});

	hCreateBoard->onPress([&]() {
		newBoard();
		hBoard->setDown(false);
		});
	hCreateBoardWithSettings->onPress([&]() {
		loadBoardGUI(1, boardSettings, sfSettings);
		hBoard->setDown(false);
		});
	hNewBoardSettings->onPress([&]() {
		loadBoardGUI(0, boardSettings, sfSettings);
		hBoard->setDown(false);
		});
	hSelectedBoardSettings->onPress([&]() {
		loadBoardGUI(2, selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings());
		hBoard->setDown(false);
		});
	hNewBoardAdvanced->onPress([&]() {
		loadAdvancedBoard(boardSettings, sfSettings, false, false);
		hBoard->setDown(false);
		});
	hSelectedBoardAdvanced->onPress([&]() {
		loadAdvancedBoard(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true, false);
		hBoard->setDown(false);
		});
	hCopyFEN->onPress([&]() {
		if (selectedBoard) { clip::set_text(selectedBoard->getCurrentFEN()); }
		hBoard->setDown(false);
		});
	hLoadFEN->onPress([&]() {
		if (selectedBoard) { loadFENGUI(selectedBoard->getBoardSettings()); }
		hBoard->setDown(false);
		});
	hDeleteSelectedBoard->onPress([&]() {
		for (auto it = boardList.begin(); it != boardList.end(); it++) {
			if (*it == selectedBoard) {
				boardList.erase(it);
				selectedBoard.reset();
				break;
			}
		}
		hBoard->setDown(false);
		});
	hDeleteAllBoards->onPress([&]() {
		selectedBoard.reset();
		for (auto it = boardList.begin(); it != boardList.end();) {
			it = boardList.erase(it);
		}
		hBoard->setDown(false);
		});
	hNewBoardEngineSettings->onPress([&]() {
		loadStockfishGUI(boardSettings, sfSettings, false);
		hEngine->setDown(false);
		});
	hSelectedBoardEngineSettings->onPress([&]() {
		loadStockfishGUI(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true);
		hEngine->setDown(false);
		});
	hNewEngineAdvancedSettings->onPress([&]() {
		loadAdvancedStockfishGUI(boardSettings, sfSettings, false);
		hEngine->setDown(false);
		});
	hSelectedEngineAdvancedSettings->onPress([&]() {
		loadAdvancedStockfishGUI(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true);
		hEngine->setDown(false);
		});
	hShadow->onMouseEnter([&]() { hTopHover = true; hTopHovered = true; });
	hShadow->onMouseLeave([&]() { hTopHover = false; });

	bool guiVisible = sGUIvisible || bGUIvisible || aBGUIVisible || sGUIvisible,
		guiVisibleFromBoard = (sGUIvisible && sFromBoard) || (bGUIvisible && bMode == 2) || (aBGUIVisible && aFromBoard) || (asGUIvisible && asFromBoard);

	auto updateHeader = [&]() {
		if (selectedBoard) {
			hSelectedBoardText->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardAdvancedText->getRenderer()->setTextColor("#EBEBEB");
			hCopyFENText->getRenderer()->setTextColor("#EBEBEB");
			hLoadFENText->getRenderer()->setTextColor("#EBEBEB");
			hDeleteSelectedBoardText->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardShortcut->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardAdvancedShortcut->getRenderer()->setTextColor("#EBEBEB");
			hCopyFENShortcut->getRenderer()->setTextColor("#EBEBEB");
			hLoadFENShortcut->getRenderer()->setTextColor("#EBEBEB");
			hDeleteSelectedBoardShortcut->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardEngineText->getRenderer()->setTextColor("#EBEBEB");
			hSelectedEngineAdvancedText->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardEngineShortcut->getRenderer()->setTextColor("#EBEBEB");
			hSelectedEngineAdvancedShortcut->getRenderer()->setTextColor("#EBEBEB");
			hSelectedBoardSettings->setEnabled(true);
			hSelectedBoardAdvanced->setEnabled(true);
			hCopyFEN->setEnabled(true);
			hLoadFEN->setEnabled(true);
			hDeleteSelectedBoard->setEnabled(true);
			hSelectedBoardEngineSettings->setEnabled(true);
			hSelectedBoardAdvanced->setEnabled(true);
			hSelectedEngineAdvancedSettings->setEnabled(true);
		}
		else {
			hSelectedBoardText->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardAdvancedText->getRenderer()->setTextColor("#A5A5A5");
			hCopyFENText->getRenderer()->setTextColor("#A5A5A5");
			hLoadFENText->getRenderer()->setTextColor("#A5A5A5");
			hDeleteSelectedBoardText->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardShortcut->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardAdvancedShortcut->getRenderer()->setTextColor("#A5A5A5");
			hCopyFENShortcut->getRenderer()->setTextColor("#A5A5A5");
			hLoadFENShortcut->getRenderer()->setTextColor("#A5A5A5");
			hDeleteSelectedBoardShortcut->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardEngineText->getRenderer()->setTextColor("#A5A5A5");
			hSelectedEngineAdvancedText->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardEngineShortcut->getRenderer()->setTextColor("#A5A5A5");
			hSelectedEngineAdvancedShortcut->getRenderer()->setTextColor("#A5A5A5");
			hSelectedBoardSettings->setEnabled(false);
			hSelectedBoardAdvanced->setEnabled(false);
			hCopyFEN->setEnabled(false);
			hLoadFEN->setEnabled(false);
			hDeleteSelectedBoard->setEnabled(false);
			hSelectedBoardEngineSettings->setEnabled(false);
			hSelectedBoardAdvanced->setEnabled(false);
			hSelectedEngineAdvancedSettings->setEnabled(false);
		}
		if (boardList.empty() || guiVisibleFromBoard) {
			hDeleteAllBoardsLabel->getRenderer()->setTextColor("#A5A5A5");
			hDeleteAllBoardsShortcut->getRenderer()->setTextColor("#A5A5A5");
			hDeleteAllBoards->setEnabled(false);
		}
		else {
			hDeleteAllBoardsLabel->getRenderer()->setTextColor("#EBEBEB");
			hDeleteAllBoardsShortcut->getRenderer()->setTextColor("#EBEBEB");
			hDeleteAllBoards->setEnabled(true);
		}
		};
	tgui::Panel::Ptr shadowPanel = headerGUI2.get<tgui::Panel>("Shadow");

#pragma endregion

	// Icon Setup
	sf::Sprite iconTexture{ *selectedIcon };
	sf::RenderTexture renderTexture(selectedIcon->getSize());
	window.setIcon(makeBoardIcon(boardSpriteSheet, renderTexture, iconTexture).getSize(), makeBoardIcon(boardSpriteSheet, renderTexture, iconTexture).getPixelsPtr());
	deltaClock.restart();

	while (window.isOpen()) {
		guiVisible = (sGUIvisible || bGUIvisible || aBGUIVisible || sGUIvisible || fGUIvisible);
		guiVisibleFromBoard = (sGUIvisible && sFromBoard) || (bGUIvisible && bMode == 2) || (aBGUIVisible && aFromBoard) || (asGUIvisible && asFromBoard);
		deltaTime = deltaClock.restart();
		float dT = deltaTime.asSeconds();
		previousMousePos = mousePos;
		mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
		if ((hTopHover || boardList.size() == 0) && !headerVisible && hidHeader) {
			for (auto& widget : headerGUI.getWidgets()) {
				widget->showWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			}
			hShadow2->showWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			headerVisible = true;
		}
		else if (!hTopHover && !hBoard->isDown() && !hEngine->isDown() && headerVisible && boardList.size() > 0 && hTopHovered) {
			for (auto& widget : headerGUI.getWidgets()) {
				widget->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			}
			hShadow2->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			headerVisible = false;
			hidHeader = true;
		}
		if (isFocused) {
			updateHeader();
			if (boardList.empty()) { selectedBoard.reset(); }
			if (!makingBoard && !guiVisible) {
				for (auto it = boardList.rbegin(); it != boardList.rend(); it++) {
					auto& b = *it;
					if (b && b->intersects(mousePos)) {
						if (b != selectedBoard) {
							selectedBoard = b;
						}
						break;
					}
				}
			}
			if (selectedBoard) {
				selectedBoard->updateS(mousePos, isFocused);
			}
		}
		while (const std::optional event = window.pollEvent())
		{
			headerGUI3.handleEvent(*event);
			headerGUI2.handleEvent(*event);
			headerGUI.handleEvent(*event);
			if (bGUIvisible) {
				boardGUI.handleEvent(*event);
			}
			if (sGUIvisible) {
				stockfishGUI.handleEvent(*event);
			}
			if (fGUIvisible) {
				fenGUI.handleEvent(*event);
			}
			if (aBGUIVisible) {
				switch (loadedGUI) {
				case 0:
					advancedBoardPosition.handleEvent(*event);
					break;
				case 1:
					advancedBoardTime.handleEvent(*event);
					break;
				case 2:
					advancedBoardAppearance.handleEvent(*event);
					break;
				case 3:
					advancedBoardAI.handleEvent(*event);
					break;
				}
			}
			if (asGUIvisible) {
				switch (asLoadedGUI) {
				case 0:
					advancedStockfish.handleEvent(*event);
					break;
				case 1:
					advancedStockfish2.handleEvent(*event);
					break;
				}
			}
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else {
				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (isFocused) {
						if (controlClicked) {
							if (keyPressed->code == sf::Keyboard::Key::Delete) {
								if (!guiVisible) {
									selectedBoard.reset();
									boardList.clear();
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::E) {
								if (!guiVisible) {
									loadBoardGUI(1, boardSettings, sfSettings);
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::S) {
								if (!guiVisible) {
									loadBoardGUI(0, boardSettings, sfSettings);
								}
								else {
									if (bGUIvisible && bMode != 1) {
										if (bMode == 0) {
											boardSettings = bP;
											sfSettings = selectedSFSettings;
										}
										else if (bMode == 2 && selectedBoard) {
											selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
											selectedBoard->setStockfishSettings(sfSettings);
										}
										bGUIvisible = false;
									}
									else if (sGUIvisible) {
										if (sFromBoard && selectedBoard) {
											selectedBoard->setStockfishSettings(selectedSFSettings);
											selectedBoard->loadFromBoardArgs(bP, boardSpriteSheet);
										}
										else if (!sFromBoard) {
											sfSettings = selectedSFSettings;
											boardSettings = bP;
										}
										sGUIvisible = false;
									}
									else if (aBGUIVisible && !aCreateBoard) {
										saveAdvancedBoard();
									}
									else if (asGUIvisible) {
										saveAdvancedStockfish();
									}
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::A) {
								if (!guiVisible) {
									loadStockfishGUI(boardSettings, sfSettings, false);
									sGUIvisible = true;
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::L) {
								if (!guiVisible) {
									loadAdvancedStockfishGUI(boardSettings, sfSettings, false);
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::D) {
								if (!guiVisible) {
									loadAdvancedBoard(boardSettings, sfSettings, false, false);
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::F) {
								if (selectedBoard) { clip::set_text(selectedBoard->getCurrentFEN()); }
							}
						}
						else if (shiftClicked) {
							if (keyPressed->code == sf::Keyboard::Key::F) {
								if (!guiVisible && selectedBoard) {
									loadFENGUI(selectedBoard->getBoardSettings());
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::D) {
								if (!guiVisible && selectedBoard) {
									loadAdvancedBoard(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true, false);
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::S) {
								if (!guiVisible && selectedBoard) {
									loadBoardGUI(2, selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings());
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::A) {
								if (!guiVisible && selectedBoard) {
									loadStockfishGUI(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true);
								}
							}
							else if (keyPressed->code == sf::Keyboard::Key::L) {
								if (!guiVisible && selectedBoard) {
									loadAdvancedStockfishGUI(selectedBoard->getBoardSettings(), selectedBoard->getStockfishSettings(), true);
								}
							}
						}
						else {
							if (keyPressed->code == sf::Keyboard::Key::E && !guiVisible) {
								newBoard();
							}
							else if (keyPressed->code == sf::Keyboard::Key::Delete && !guiVisible) {
								for (auto it = boardList.begin(); it != boardList.end(); it++) {
									if (*it == selectedBoard) {
										boardList.erase(it);
										break;
									}
								}
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
				guiVisible = (sGUIvisible || bGUIvisible || aBGUIVisible || sGUIvisible || fGUIvisible);
				if (selectedBoard && !guiVisible) { selectedBoard->handleEvent(event, mousePos, boardSpriteSheet, isFocused, controlClicked); }
			}
		}

		controlClicked = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl));
		shiftClicked = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift));

		window.clear();
		window.draw(backgroundSprite);
		for (auto& b : boardList) {
			b->update(mousePos, isFocused, dT);
			b->draw(window);
		}
		if (makingBoard) { window.setMouseCursor(mouseCursors.at(3)); }
		else if (selectedBoard && !guiVisible) {
			window.setMouseCursor(mouseCursors.at(selectedBoard->getMouseCursor(blockCursor)));
		}

		shadowPanel->setEnabled(guiVisible);
		headerGUI2.draw();
		headerGUI3.draw();
		if (bGUIvisible) {
			bSidePanel->setVisible(bFENArea->isFocused() || bSideHover);
			if (bFENArea->isFocused() || bSideHover) {
				largePositionGUI.setOpacity(1.0f);
				bTopPanel->getRenderer()->setTextureBackground("assets/ui/textures/rounded_rectangle_6.png");
			}
			else {
				largePositionGUI.setOpacity(0.0f);
				bTopPanel->getRenderer()->setTextureBackground("assets/ui/textures/rounded_rectangle_4.png");
			}
			boardGUI.draw();
			if (!bVariantPanel->isVisible()) {
				positionGUI.draw();
				largePositionGUI.draw();
			}
		}
		if (sGUIvisible) {
			stockfishGUI.draw();
		}
		if (fGUIvisible) {
			fenGUI.draw();
			fenPositionGUI.draw();
		}
		if (aBGUIVisible) {
			switch (loadedGUI) {
			case 0:
				advancedBoardPosition.draw();
				if (!aVariantPanel->isVisible()) {
					aPositionGUI.draw();
					aRepeatPositionGUI.draw();
				}
				break;
			case 1:
				advancedBoardTime.draw();
				break;
			case 2:
				advancedBoardAppearance.draw();
				break;
			case 3:
				advancedBoardAI.draw();
				break;
			}
		}
		if (asGUIvisible) {
			switch (asLoadedGUI) {
			case 0:
				advancedStockfish.draw();
				break;
			case 1:
				advancedStockfish2.draw();
				break;
			}
		}
		if (!hBoard->isDown() && hBoard->isMouseDown()) { hBoard->setDown(true); }
		else if (!hBoard->isDown() && hBoardHover && headerClock.isRunning() && headerClock.getElapsedTime().asMilliseconds() < 30) {
			hBoard->setDown(true);
			headerClock.reset();
		}
		else if (hBoard->isDown() && !hBoardHover && !hBoardBoundsHover) {
			hBoard->setDown(false);
			if (!hTopHover && boardList.size() > 0 && hTopHovered) {
				for (auto& widget : headerGUI.getWidgets()) {
					widget->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
				}
				hShadow2->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			}
			hidHeader = true;
		}

		if (!hEngine->isDown() && hEngine->isMouseDown()) { hEngine->setDown(true); }
		else if (!hEngine->isDown() && hEngineHover && headerClock.isRunning() && headerClock.getElapsedTime().asMilliseconds() < 30) {
			hEngine->setDown(true);
			headerClock.reset();
		}
		else if (hEngine->isDown() && !hEngineHover && !hEngineBoundsHover) {
			hEngine->setDown(false);
			if (!hTopHover && boardList.size() > 0 && hTopHovered) {
				for (auto& widget : headerGUI.getWidgets()) {
					widget->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
				}
				hShadow2->hideWithEffect(tgui::ShowEffectType::Fade, sf::seconds(0.25f));
			}
			hidHeader = true;
		}
		headerGUI.draw();
		window.display();
	}

	return 0;
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

sf::Image makeBoardIcon(const sf::Image& boardSpriteSheet, sf::RenderTexture& renderTexture, const sf::Sprite& iconTexture)
{
	int x = 0;
	int y = 0;
	auto tex = Chess::loadBoard(boardSpriteSheet, 0, { 3, 3 });
	sf::Sprite currentBoardC{ tex };
	currentBoardC.setOrigin(currentBoardC.getLocalBounds().getCenter());
	currentBoardC.setPosition({ renderTexture.getSize().x / 2.0f, renderTexture.getSize().y / 2.0f });
	renderTexture.clear();
	renderTexture.draw(currentBoardC);
	renderTexture.draw(iconTexture);
	renderTexture.display();
	return renderTexture.getTexture().copyToImage();
	
}

sf::Image makeAIStrengthTexture(const sf::Texture& gradient, sf::Color color)
{
	if (RGBtoHSV(color).s > 0.05) { color = HSVtoRGB({ RGBtoHSV(color).h, RGBtoHSV(color).s * 1.75f, RGBtoHSV(color).v }); }

	sf::Sprite gradientSprite(gradient);
	sf::RectangleShape background((sf::Vector2f)gradient.getSize());
	background.setFillColor(color);
	background.setPosition({ 0, 0 });
	gradientSprite.setPosition({ 0, 0 });
	sf::RenderTexture renderTexture(gradient.getSize());
	renderTexture.clear();
	renderTexture.draw(background);
	renderTexture.draw(gradientSprite);
	renderTexture.display();
	return renderTexture.getTexture().copyToImage();
}

std::vector<Chess::VariantType> loadVariantTypes(const std::string& path, const std::vector<std::shared_ptr<Chess::Variant>>& variantList)
{
	std::ifstream stream(path);
	std::string line;
	std::vector<Chess::VariantType> variantTypes;
	Chess::VariantType type("", "");
	while (std::getline(stream, line, '\n')) {
		if (line.empty() || (line.size() > 1 && line.front() == '#' && line.at(1) != ' ')) { continue; }
		if (line.size() > 1 && line.front() == '!' && line.at(1) == ' ') {
			auto find = line.find(':');
			if (find != line.npos) {
				if (type.name != "") {
					variantTypes.push_back(std::move(type));
					type = Chess::VariantType("", "");
				}
				std::string name = line.substr(2, find - 2);
				std::string description = line.substr(find + 2);
				type.name = name;
				type.description = description;
			}
		}
		else if (line.find('|') != line.npos) {
			std::string vName = line.substr(0, line.find_first_of('|'));
			std::string vDisplayName = line.substr(findNthOf(line, '|', 2) + 1, findNthOf(line, '|', 3) - (findNthOf(line, '|', 2) + 1));
			std::string vDescription = line.substr(findNthOf(line, '|', 3) + 1, findNthOf(line, '|', 4) - (findNthOf(line, '|', 3) + 1));
			for (auto& v : variantList) {
				if (v->name == vName) {
					v->displayName = vDisplayName;
					v->description = vDescription;
					type.add(v);
				}
			}
		}
	}
	if (type.name != "") {
		variantTypes.push_back(std::move(type));
	}
	type = Chess::VariantType("", "");
	type.name = "Other Variants";
	type.description = "Other Variants not included in the other sections. Contains variants with a variety of fairy pieces and board sizes.";
	for (auto& v : variantList) {
		if (std::none_of(variantTypes.begin(), variantTypes.end(), [&](const Chess::VariantType& variantType) {
			return std::any_of(variantType.variants.begin(), variantType.variants.end(),
				[&](const std::shared_ptr<Chess::Variant>& variant) { return variant->name == v->name; }); })) {
			v->description = " ";
			type.add(v);
		}
	}
	if (type.variants.size() > 0) {
		variantTypes.push_back(std::move(type));
	}
	return variantTypes;
}

void makeBoard(std::shared_ptr<Board>& selectedBoard, const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings, const std::vector<std::map<char, sf::Texture>>& pieceTextures,
	const std::map<std::string, std::map<char, sf::Texture>>& variantPieceTextures, sf::Vector2u windowSize,
	const sf::Image& boardSpriteSheet, const sf::Font& textFont, const std::vector<sf::Texture>& boardTextures, const std::vector<sf::SoundBuffer>& soundBuffers, const std::set<std::string>& endgameVariants, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard)
{
	std::shared_ptr<Board> board = std::make_shared<Board>(boardSettings, stockfishSettings, windowSize, pieceTextures, variantPieceTextures, boardSpriteSheet, textFont, boardTextures, soundBuffers, endgameVariants, false);
	boardList.push_back(board);
	selectedBoard = board;
	makingBoard = false;
}

std::string timeToStr(const sf::Time& time, std::ostringstream& oss, float multiplier)
{
	oss.str("");
	oss.clear();
	oss << time.asSeconds() * multiplier;
	return oss.str();
}

std::string floatToStr(float value, std::ostringstream& oss)
{
	oss.str("");
	oss.clear();
	oss << value;
	std::string s = oss.str();
	s.erase(s.find_last_not_of('0') + 1, std::string::npos);
	if (!s.empty() && s.back() == '.') { s.pop_back(); }
	if (s.empty() || s == ".") { s = "0"; }
	return s;
}