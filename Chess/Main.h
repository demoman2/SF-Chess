#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <SFML/Main.hpp>
#include "Board.h"


static void block_until_gained_focus(sf::Window& window) {
	while (const std::optional event = window.waitEvent()) {
		if (event->is<sf::Event::FocusGained>()) {
			return;
		}
	}
}

sf::Image makeIconFromBoard(std::shared_ptr<Board> board, sf::RenderTexture& renderTexture, sf::Sprite& iconTexture) {
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

void makeBoard(std::shared_ptr<Board>& selectedBoard, Chess::Variant VARIANT, std::optional<std::string> FEN, float X_OFFSET, float Y_OFFSET, int boardSize, bool animated, sf::Vector2u windowSize, float BOARD_SCALE, int BOARD_SET, int PIECE_SET, std::vector<sf::Image> pieceSpriteSheets, sf::Image& boardSpriteSheet, sf::Font& textFont, sf::Texture& boardTexture, bool AI, bool AI_ONLY, std::optional<bool> WHITE, bool REPEAT_FEN, bool CHESS960, bool ENDGAME, std::vector<sf::Texture>& pieceTextures, std::vector<sf::Texture>& boardTextures, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard) {
	makingBoard = true;
	std::shared_ptr<Board> board = std::make_shared<Board>(VARIANT, FEN, X_OFFSET, Y_OFFSET, boardSize, animated, windowSize, BOARD_SCALE, BOARD_SET, PIECE_SET, pieceSpriteSheets, boardSpriteSheet, textFont, boardTexture, AI, AI_ONLY, WHITE, REPEAT_FEN, CHESS960, ENDGAME, pieceTextures, boardTextures, false);
	boardList.push_back(board);
	makingBoard = false;
	selectedBoard = board;
	board->generatingMoves = true;
	board->generateLegalMoves(true);
}
