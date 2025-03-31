#include "Main.h"

int main()
{
    srand(time(0));
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "SF Chess", sf::Style::None);
    float windowSizeX = window.getSize().x;
    float windowSizeY = window.getSize().y;
    sf::Texture placeholder;
    placeholder.loadFromFile("assets/placeholder.png");

    // Chess Boards
    int boardSize = 1024;
    sf::Image boardSpriteSheet;
    boardSpriteSheet.loadFromFile("assets/board/board_matrix.jpg");
    sf::Texture boardTexture;
    boardTexture.loadFromImage(boardSpriteSheet, false, sf::IntRect({0, 0}, {boardSize, boardSize}));
    sf::Sprite board{ boardTexture };
    Main::loadBoard(boardSpriteSheet, board, boardTexture, (std::rand() % 24) + 1, boardSize);
    float ScaleX = windowSizeX / boardTexture.getSize().x;
    float ScaleY = windowSizeY / boardTexture.getSize().y;
    board.setScale({ std::min(ScaleX, ScaleY), std::min(ScaleX, ScaleY) });
    board.setOrigin(board.getLocalBounds().getCenter());
    board.setPosition({ windowSizeX / 2.0f, windowSizeY / 2.0f });

    // Pieces
    int halfMoves = 0, fullMoves = 0;
    bool whiteToPlay = true;
    float pieceSize = 320;
    float pieceScale = (std::min(ScaleX, ScaleY) * 128.0f) / (float)pieceSize;
    float boardOffset = (windowSizeX / 2.0f) - ((boardTexture.getSize().x * board.getScale().x) / 2);
    float boardSquareOffset = ((boardTexture.getSize().x * board.getScale().x) / 8);
    sf::Image pieceSpriteSheetAlpha, pieceSpriteSheetCburnett, pieceSpriteSheetMerida, pieceSpriteSheetDisguised;
    pieceSpriteSheetAlpha.loadFromFile("assets/piece/pieces_matrix_alpha.png");
    pieceSpriteSheetCburnett.loadFromFile("assets/piece/pieces_matrix_cburnett.png");
    pieceSpriteSheetMerida.loadFromFile("assets/piece/pieces_matrix_merida.png");
    pieceSpriteSheetDisguised.loadFromFile("assets/piece/pieces_matrix_disguised.png");
    sf::Image pieceStyle = pieceSpriteSheetAlpha;
    sf::Texture blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT;
    sf::Texture whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT;
    std::vector<std::reference_wrapper<sf::Texture>> pieceTextures{ blackBishopT, blackKingT, blackKnightT, blackPawnT, blackQueenT, blackRookT, 
    whiteBishopT, whiteKingT, whiteKnightT, whitePawnT, whiteQueenT, whiteRookT };
    Main::loadPieceSet(pieceStyle, pieceTextures, pieceSize);
    std::vector<std::unique_ptr<Piece>> pieceList = Main::generatePositionFromFENID("rnbqkbnr/8/8/2p5/4P3/8/8/RNBQK2R w KQkq c6 0 2",
        pieceTextures, pieceScale, boardOffset, boardSquareOffset, whiteToPlay, halfMoves, fullMoves);
    Main::calculateCastlingPossibilities(pieceList);
    Main::isValidPosition(pieceList);

    // ==== MAIN ====

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(board);
        for (auto& piece : pieceList) {
            piece->draw(window);
        }
        window.display();
    }
}