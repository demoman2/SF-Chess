#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include "Board.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <clipboardxx.hpp>

class SVGPiece {
	tgui::Picture::Ptr pieceImage;
public:
	SVGPiece(const tgui::Texture& texture, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed);
	void addGUI(tgui::Gui& gui);
	void draw(sf::RenderWindow& window);
};

struct GUIBoard {
	tgui::Gui* gui;
	std::vector<SVGPiece> previewPieces;
	float boardScale, pieceScale, boardMultiplier;
	sf::Vector2f boardOffset, boardSize, startingOffset;
	GUIBoard(const tgui::Picture::Ptr& boardPicture, tgui::Gui& gui, std::vector<tgui::Texture>& pieceTextures);
	void setPieces(std::string fen, std::vector<tgui::Texture>& pieceTextures, bool reversed = false);
};

std::vector<tgui::Texture> loadSVGSet(std::string path);

static void block_until_gained_focus(sf::Window& window);

void addTexture(std::vector<sf::Texture>& textures, const std::string& path, bool smooth = false);

void addImage(std::vector<sf::Image>& images, const std::string& path);

void addSoundBuffer(std::vector<sf::SoundBuffer>& soundBuffers, const std::string& path);

std::vector<sf::Image> makeBoardIcons(sf::Image& boardSpriteSheet, sf::RenderTexture& renderTexture, sf::Sprite& iconTexture);

void makeBoard(std::shared_ptr<Board>& selectedBoard, const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings, sf::Vector2u windowSize, std::vector<sf::Image>& pieceSpriteSheets, sf::Image& boardSpriteSheet, sf::Font& textFont, sf::Texture& boardTexture, std::vector<sf::Texture>& pieceTextures, std::vector<sf::Texture>& boardTextures, std::vector<sf::SoundBuffer>& soundBuffers, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard);

sf::Image makeAIStrengthTexture(sf::Texture& gradient, sf::Color color);

std::vector<SVGPiece> generatePositionFromFENID(std::string code, std::vector<tgui::Texture>& pieceTextures, float pieceScale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed = false);

std::string timeToStr(const sf::Time& time, std::ostringstream& oss, float multiplier = 1.0f);

std::string floatToStr(float value, std::ostringstream& oss);