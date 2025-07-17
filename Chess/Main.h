#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include "Board.h"


static void block_until_gained_focus(sf::Window& window);

void addTexture(std::vector<sf::Texture>& textures, const std::string& path, bool smooth = false);

void addImage(std::vector<sf::Image>& images, const std::string& path);

void addSoundBuffer(std::vector<sf::SoundBuffer>& soundBuffers, const std::string& path);

sf::Image makeIconFromBoard(std::shared_ptr<Board>& board, sf::RenderTexture& renderTexture, sf::Sprite& iconTexture);

void makeBoard(std::shared_ptr<Board>& selectedBoard, Chess::Variant VARIANT, std::optional<std::string> FEN, float X_OFFSET, float Y_OFFSET, int boardSize, bool animated, sf::Vector2u windowSize, float BOARD_SCALE, int BOARD_SET, int PIECE_SET, std::vector<sf::Image> pieceSpriteSheets, sf::Image& boardSpriteSheet, sf::Font& textFont, sf::Texture& boardTexture, bool AI, bool AI_ONLY, std::optional<bool> WHITE, bool REPEAT_FEN, bool CHESS960, bool ENDGAME, std::vector<sf::Texture>& pieceTextures, std::vector<sf::Texture>& boardTextures, std::vector<sf::SoundBuffer>& soundBuffers, bool TIME_ENABLED, sf::Time WHITE_TIME, sf::Time BLACK_TIME, sf::Time TIME_INCREMENT, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard);