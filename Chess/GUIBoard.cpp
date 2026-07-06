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

#include "GUIBoard.h"

GUIBoard::GUIBoard(const tgui::Picture::Ptr& boardPicture, tgui::Gui& gui) : gui(&gui)
{
	setBoard(boardPicture, {8, 8});
}

void GUIBoard::setPieces(const std::string& fen, const Chess::Variant& variant, const std::map<char, tgui::Texture>& pieceTextures, bool reversed)
{
	gui->removeAllWidgets();
	previewPieces = generatePositionFromFENID(fen, variant, pieceTextures, boardOffset, boardSize, boardMultiplier, reversed);
	for (auto& piece : previewPieces) {
		piece.addGUI(*gui);
	}
}

void GUIBoard::setBoard(const tgui::Picture::Ptr& boardPicture, sf::Vector2u boardSquares)
{
	boardScale = boardPicture->getSize().x / 1024.0f;
	boardMultiplier.x = boardPicture->getSize().x / static_cast<float>(boardSquares.x);
	boardMultiplier.y = boardPicture->getSize().y / static_cast<float>(boardSquares.y);
	boardSize = boardPicture->getSize();
	boardOffset = boardPicture->getAbsolutePosition();
}


SVGPiece::SVGPiece(const tgui::Texture& texture, int x, int y, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, sf::Vector2f boardMultiplier, bool reversed) : pieceImage(tgui::Picture::create(texture))
{
	pieceImage->setSize({ boardMultiplier.x, boardMultiplier.y });
	pieceImage->setOrigin(0.5f, 0.5f);
	pieceImage->setPosition(tgui::Layout2d{ Chess::getGlobalPosition(sf::Vector2i{x, y}, boardOffset, boardSize, boardMultiplier, boardSquares, reversed) });
}

void SVGPiece::addGUI(tgui::Gui& gui)
{
	gui.add(pieceImage);
}

// Bishop, King, Knight, Pawn, Queen, Rook
// Black --> White
std::vector<SVGPiece> generatePositionFromFENID(const std::string& code, const Chess::Variant& variant, const std::map<char, tgui::Texture>& pieceTextures, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2f boardMultiplier, bool reversed) {

	std::vector<SVGPiece> pieces;
	std::vector<std::string> splitString = split(code, ' ');
	std::vector<std::string> ranks = split(splitString.front(), '/');
	// ========= RANKS =============
	for (int i = 0; i < ranks.size(); i++) {
		int x = 1;
		int y = Chess::reverseY(i + 1, variant.boardSize);
		std::string rank = ranks.at(i);
		for (int j = 0; j < rank.size(); j++) {
			char letter = rank.at(j);
			if (std::isdigit(letter)) {
				int k = j;
				for (; k < rank.size(); k++) {
					if (!std::isdigit(rank.at(k))) {
						k--;
						break;
					}
				}
				x += std::stoi(rank.substr(j, k - j + 1));
				j = k;
			}
			else if (std::isalpha(letter) && Chess::isValidSquare({ x, y }, variant.boardSize)) {
				if (variant.hasPiece(letter)) {
					Chess::PColor color = std::isupper(letter) ? Chess::PColor::White : Chess::PColor::Black;
					if (j > 0 && rank.at(j - 1) == '+') {
						auto find = variant.promotedPieceTypes.find(std::tolower(letter));
						if (find != variant.promotedPieceTypes.end()) {
							letter = find->second;
						}
					}
					if (pieceTextures.find(letter) != pieceTextures.end()) {
						pieces.emplace_back(pieceTextures.find(letter)->second, x, y, boardOffset, boardSize, variant.boardSize, boardMultiplier, reversed);
					}
					else {
						pieces.emplace_back(pieceTextures.begin()->second, x, y, boardOffset, boardSize, variant.boardSize, boardMultiplier, reversed);
					}					
				}
				x++;
			}
		}
	}
	return pieces;
}