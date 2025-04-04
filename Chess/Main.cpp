#include "Main.h"

int main()
{
    srand(time(0));
    const auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
    const auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
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
    Piece* selectedPiece = nullptr;
    sf::Vector2f selectedPos{ 0.0f, 0.0f };
    sf::Vector2i mousePos;

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
    sf::Sprite selectedPieceBackground{ selectedPieceTexture };
    selectedPieceBackground.setScale(sf::Vector2f{ (pieceScale * 320.0f) / 128.0f, (pieceScale * 320.0f) / 128.0f });

    // Setup
    Main::loadPieceSet(pieceStyle, pieceTextures, pieceSize);
    std::vector<std::shared_ptr<Piece>> pieceList = Main::generatePositionFromFENID("r3k2r/pppppppp/6N1/8/8/2n5/PPPPPPPP/R3K2R w KQkq - 0 1",
        pieceTextures, pieceScale, boardOffset, boardMultiplier, whiteToPlay, halfMoves, fullMoves);
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

    while (window.isOpen())
    {
        mousePos = sf::Mouse::getPosition();
        selectedPos = { std::ceil(((float)mousePos.x - boardOffset) / boardMultiplier),  9 - std::ceil((float)mousePos.y / boardMultiplier) };
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    Piece* p = Main::getPieceFromPosition(sf::Vector2i(selectedPos), pieceList);
                    if (p != nullptr) {
                        // Side to Play == Color
                        if (whiteToPlay == (p->color == Piece::PColor::White)) {
                            if (selectedPiece != nullptr && selectedPiece->name == "King") {
                                King* king = dynamic_cast<King*>(selectedPiece);
                                if (std::any_of(king->captureCastlePositions.cbegin(), king->captureCastlePositions.cend(), [selectedPos](sf::Vector2i pos) { return pos == (sf::Vector2i)selectedPos; })) {
                                    selectedPiece = nullptr;
                                }
                                else {
                                    selectedPiece = p;
                                }
                            }
                            else {
                                selectedPiece = p;
                            }
                        }
                        else {
                            selectedPiece = nullptr;
                        }
                    } else {
                        selectedPiece = nullptr;
                    }
                }
            }
        }   

        // Draw
        window.clear();
        window.draw(board);
        if (selectedPiece != nullptr) {
            for (auto& sprite : selectedPiece->selectionSquares) {
                if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    sprite.setTexture(extraTextures.at(4));
                } else { sprite.setTexture(extraTextures.at(0)); }
                window.draw(sprite);
            }
            for (auto& sprite : selectedPiece->captureSquares) {
                if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    sprite.setTexture(extraTextures.at(4));
                }
                else { sprite.setTexture(extraTextures.at(1)); }
                window.draw(sprite);
            }
            if (selectedPiece->name == "King") {
                King* king = dynamic_cast<King*>(selectedPiece);
                for (auto& sprite : king->castleSquares) {
                    if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        sprite.setTexture(extraTextures.at(4));
                    }
                    else { sprite.setTexture(extraTextures.at(0)); }
                    window.draw(sprite);
                }
                for (auto& sprite : king->castleCaptureSquares) {
                    if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        sprite.setTexture(extraTextures.at(4));
                    }
                    else { sprite.setTexture(extraTextures.at(1)); }
                    window.draw(sprite);
                }
            }
            if (selectedPiece->name == "Pawn") {
                Pawn* pawn = dynamic_cast<Pawn*>(selectedPiece);
                for (auto& sprite : pawn->enPassantSquares) {
                    if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        sprite.setTexture(extraTextures.at(4));
                    }
                    else { sprite.setTexture(extraTextures.at(0)); }
                    window.draw(sprite);
                }
            }
            selectedPieceBackground.setPosition(selectedPiece->sprite.getPosition());
            window.draw(selectedPieceBackground);
        }
        for (auto& piece : pieceList) {
            if (piece->sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                window.setMouseCursor(handCursor);
            }
            piece->draw(window);
        }
        window.display();
    }
}