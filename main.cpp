#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>

enum class ShipDirection {
    HORIZONTAL,
    VERTICAL
};

struct Ship {
    int length;
    int startRow;
    int startCol;
    ShipDirection direction;
    sf::Color color;
};

// Button Struct
struct Button {
    sf::Text text;
    sf::RectangleShape shape;
    sf::Color defaultColor;
    sf::Color hoverColor;
    sf::Color pressedColor;
    bool isPressed = false;

    Button(const std::string& label, sf::Font& font, sf::Color defaultColor, sf::Color hoverColor, sf::Color pressedColor, float x, float y)
        : defaultColor(defaultColor), hoverColor(hoverColor), pressedColor(pressedColor) {
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(defaultColor);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f); // Center text
        text.setPosition(x, y);

        shape.setSize(sf::Vector2f(textRect.width + 20, textRect.height + 10)); // Add some padding
        shape.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent background
        shape.setOrigin(shape.getLocalBounds().left + shape.getLocalBounds().width / 2.0f, shape.getLocalBounds().top + shape.getLocalBounds().height / 2.0f);
        shape.setPosition(x, y);
    }

    void setPosition(float x, float y) {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(x, y);
        shape.setPosition(x, y);
    }
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
    bool isMouseOver(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return shape.getGlobalBounds().contains(mousePos.x, mousePos.y);
    }
    void update(sf::RenderWindow& window) {
        if (isMouseOver(window)) {
            text.setFillColor(hoverColor);
        }
        else {
            text.setFillColor(defaultColor);
        }
    }
    void setPressed(bool pressed) {
        isPressed = pressed;
        if (isPressed) {
            text.setFillColor(pressedColor);
        }
    }
};


bool canPlaceShip(const std::vector<Ship>& ships, int row, int col, int length, ShipDirection direction, int gridRows, int gridCols) {
    for (const auto& ship : ships) {
        if (ship.direction == direction) {
            
            if (direction == ShipDirection::HORIZONTAL) {
                
                for (int i = 0; i < length; ++i) {
                    for (int j = 0; j < ship.length; ++j) {
                        if (row == ship.startRow && col + i == ship.startCol + j) {
                            return false;  
                        }
                    }
                }
            }
            else
            {
                for (int i = 0; i < length; ++i) {
                    for (int j = 0; j < ship.length; ++j) {
                        if (row + i == ship.startRow + j && col == ship.startCol) {
                            return false;  
                        }
                    }
                }
            }
        }
        else
        {
            if (direction == ShipDirection::HORIZONTAL && ship.direction == ShipDirection::VERTICAL) {
                for (int i = 0; i < length; i++) {
                    for (int j = 0; j < ship.length; j++) {
                        if (row == ship.startRow + j && col + i == ship.startCol) {
                            return false;
                        }
                    }
                }
            }
            else if (direction == ShipDirection::VERTICAL && ship.direction == ShipDirection::HORIZONTAL) {
                for (int i = 0; i < length; i++) {
                    for (int j = 0; j < ship.length; j++) {
                        if (row + i == ship.startRow && col == ship.startCol + j) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    if (direction == ShipDirection::HORIZONTAL && (col + length > gridCols)) {
        return false;
    }
    if (direction == ShipDirection::VERTICAL && (row + length > gridRows)) {
        return false;
    }
    return true;
}


bool canPlaceShipWithGap(const std::vector<Ship>& ships, int row, int col, int length, ShipDirection direction, int gridRows, int gridCols) {
    
    if (!canPlaceShip(ships, row, col, length, direction, gridRows, gridCols)) {
        return false;
    }

    for (const auto& ship : ships) {
        
        if (direction == ShipDirection::HORIZONTAL && ship.direction == ShipDirection::HORIZONTAL) {
            for (int i = 0; i < length; ++i) {
                for (int j = 0; j < ship.length; ++j) {
                    if (abs(row - ship.startRow) <= 1 && abs((col + i) - (ship.startCol + j)) <= 1) {
                        return false; 
                    }
                }
            }
        }
        else if (direction == ShipDirection::VERTICAL && ship.direction == ShipDirection::VERTICAL) {
            for (int i = 0; i < length; ++i) {
                for (int j = 0; j < ship.length; ++j) {
                    if (abs((row + i) - (ship.startRow + j)) <= 1 && abs(col - ship.startCol) <= 1) {
                        return false; 
                    }
                }
            }
        }
        else if (direction == ShipDirection::HORIZONTAL && ship.direction == ShipDirection::VERTICAL) {
            for (int i = 0; i < length; i++) {
                for (int j = 0; j < ship.length; j++) {
                    if (abs(row - (ship.startRow + j)) <= 1 && abs((col + i) - ship.startCol) <= 1) {
                        return false;
                    }
                }
            }
        }
        else if (direction == ShipDirection::VERTICAL && ship.direction == ShipDirection::HORIZONTAL) {
            for (int i = 0; i < length; i++) {
                for (int j = 0; j < ship.length; j++) {
                    if (abs((row + i) - ship.startRow) <= 1 && abs(col - (ship.startCol + j)) <= 1) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}


void autoPlaceShipsInPlacement(std::vector<Ship>& ships, int gridRows, int gridCols) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, gridRows - 1);
    std::uniform_int_distribution<> colDist(0, gridCols - 1);
    std::uniform_int_distribution<> dirDist(0, 1); // 0 - horizontal, 1 - vertical

    
    std::vector<int> shipLengths = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
    std::vector<sf::Color> shipColors = { sf::Color::Cyan, sf::Color::Magenta, sf::Color::Magenta, sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, sf::Color::Green, sf::Color::Green, sf::Color::Green, sf::Color::Green };
    ships.clear();
    for (size_t i = 0; i < shipLengths.size(); ++i) {
        bool placed = false;
        while (!placed) {
            int row = rowDist(gen);
            int col = colDist(gen);
            ShipDirection dir = (dirDist(gen) == 0) ? ShipDirection::HORIZONTAL : ShipDirection::VERTICAL;

            if (canPlaceShipWithGap(ships, row, col, shipLengths[i], dir, gridRows, gridCols)) {
                ships.push_back({ shipLengths[i], row, col, dir, shipColors[i] });
                placed = true;
            }
        }
    }
}

void runSeaBattleGame(std::vector<Ship> playerShips) { // Receive player's ships
    
    const int windowWidth = 1400;
    const int windowHeight = 700;

    
    const int boardMarginLeft = 50;
    const int boardMarginTop = 50;
    const int boardMarginBottom = 50;
    const int boardMarginRight = 50;

    
    const int gridRows = 10;
    const int gridCols = 10;

    
    const int spaceBetweenBoards = 250;

    
    const float cellSizeX = static_cast<float>(windowWidth - boardMarginLeft - boardMarginRight - spaceBetweenBoards) / (2 * gridCols);
    const float cellSizeY = static_cast<float>(windowHeight - boardMarginTop - boardMarginBottom) / gridRows;

    
    sf::Texture playerBoardTexture;
    if (!playerBoardTexture.loadFromFile("field.png")) {
        std::cerr << "Error loading player board image!" << std::endl;
        return;
    }

    sf::Texture opponentBoardTexture;
    if (!opponentBoardTexture.loadFromFile("field.png")) {
        std::cerr << "Error loading opponent board image!" << std::endl;
        return;
    }

   
    sf::Sprite playerBoardSprite;
    playerBoardSprite.setTexture(playerBoardTexture);
    playerBoardSprite.setScale(cellSizeX / (playerBoardTexture.getSize().x / gridCols), cellSizeY / (playerBoardTexture.getSize().y / gridRows));
    playerBoardSprite.setPosition(boardMarginLeft, boardMarginTop);

    sf::Sprite opponentBoardSprite;
    opponentBoardSprite.setTexture(opponentBoardTexture);
    opponentBoardSprite.setScale(cellSizeX / (opponentBoardTexture.getSize().x / gridCols), cellSizeY / (opponentBoardTexture.getSize().y / gridRows));
    opponentBoardSprite.setPosition(boardMarginLeft + gridCols * cellSizeX + spaceBetweenBoards, boardMarginTop);

    
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    
    std::vector<sf::Text> columnLetters(gridCols);
    for (int i = 0; i < gridCols; ++i) {
        columnLetters[i].setFont(font);
        columnLetters[i].setCharacterSize(20);
        columnLetters[i].setFillColor(sf::Color::White);
        columnLetters[i].setString(static_cast<char>('A' + i));
        sf::FloatRect textRect = columnLetters[i].getLocalBounds();
        columnLetters[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        columnLetters[i].setPosition(boardMarginLeft + i * cellSizeX + cellSizeX / 2.0f, boardMarginTop - 30);  // Centered
    }

    std::vector<sf::Text> columnLettersOpponent(gridCols);
    for (int i = 0; i < gridCols; ++i) {
        columnLettersOpponent[i].setFont(font);
        columnLettersOpponent[i].setCharacterSize(20);
        columnLettersOpponent[i].setFillColor(sf::Color::White);
        columnLettersOpponent[i].setString(static_cast<char>('A' + i));
        sf::FloatRect textRect = columnLettersOpponent[i].getLocalBounds();
        columnLettersOpponent[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        columnLettersOpponent[i].setPosition(boardMarginLeft + gridCols * cellSizeX + spaceBetweenBoards + i * cellSizeX + cellSizeX / 2.0f, boardMarginTop - 30); // Centered
    }

    
    std::vector<sf::Text> rowNumbers(gridRows);
    for (int i = 0; i < gridRows; ++i) {
        rowNumbers[i].setFont(font);
        rowNumbers[i].setCharacterSize(20);
        rowNumbers[i].setFillColor(sf::Color::White);
        rowNumbers[i].setString(std::to_string(i + 1));
        sf::FloatRect textRect = rowNumbers[i].getLocalBounds();
        rowNumbers[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        rowNumbers[i].setPosition(boardMarginLeft - 20, boardMarginTop + i * cellSizeY + cellSizeY / 2.0f);   // Centered
    }

    std::vector<sf::Text> rowNumbersOpponent(gridRows);
    for (int i = 0; i < gridRows; ++i) {
        rowNumbersOpponent[i].setFont(font);
        rowNumbersOpponent[i].setCharacterSize(20);
        rowNumbersOpponent[i].setFillColor(sf::Color::White);
        rowNumbersOpponent[i].setString(std::to_string(i + 1));
        sf::FloatRect textRect = rowNumbersOpponent[i].getLocalBounds();
        rowNumbersOpponent[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        rowNumbersOpponent[i].setPosition(boardMarginLeft + gridCols * cellSizeX + spaceBetweenBoards - 20, boardMarginTop + i * cellSizeY + cellSizeY / 2.0f);   // Centered
    }

    Button pauseButton("Pause", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, windowWidth / 2.0f, windowHeight / 2.0f - 50); // Centered
    Button exitButton("Exit", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, windowWidth / 2.0f, windowHeight / 2.0f + 50);  // Centered

    bool isPaused = false;
    
    sf::RenderWindow gameWindow(sf::VideoMode(windowWidth, windowHeight), "Sea Battle - Game");

   
    while (gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                gameWindow.close();


            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Get Mouse Position 
                    sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);

                   
                    if (mousePos.x >= boardMarginLeft && mousePos.x < boardMarginLeft + gridCols * cellSizeX && mousePos.y >= boardMarginTop && mousePos.y < boardMarginTop + gridRows * cellSizeY) {
                       
                        int col = (mousePos.x - boardMarginLeft) / cellSizeX;
                        int row = (mousePos.y - boardMarginTop) / cellSizeY;
                        std::cout << "Clicked on cell: " << row << ", " << col << std::endl; 
                        
                    }
                    //  Pause/Continue Button 
                    if (pauseButton.isMouseOver(gameWindow)) {
                        pauseButton.setPressed(true);
                    }
                    //  Exit Button 
                    if (exitButton.isMouseOver(gameWindow)) {
                        exitButton.setPressed(true);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    //  Pause/Continue Button 
                    if (pauseButton.isPressed) {
                        isPaused = !isPaused;
                        if (isPaused) {
                            pauseButton.text.setString("Continue");
                        }
                        else {
                            pauseButton.text.setString("Pause");
                        }
                        
                        sf::FloatRect textRect = pauseButton.text.getLocalBounds();
                        pauseButton.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                        pauseButton.setPosition(windowWidth / 2.0f, windowHeight / 2.0f - 50);

                    }
                    pauseButton.setPressed(false);

                    // Exit Button 
                    if (exitButton.isPressed) {
                        gameWindow.close();
                    }
                    exitButton.setPressed(false);
                }
            }
        }
        pauseButton.update(gameWindow);
        exitButton.update(gameWindow);

        gameWindow.clear(sf::Color::Black); 


        gameWindow.draw(playerBoardSprite);
        gameWindow.draw(opponentBoardSprite);


        for (auto& letter : columnLetters) {
            gameWindow.draw(letter);
        }
        for (auto& letter : columnLettersOpponent) {
            gameWindow.draw(letter);
        }

        for (auto& number : rowNumbers) {
            gameWindow.draw(number);
        }
        for (auto& number : rowNumbersOpponent) {
            gameWindow.draw(number);
        }

        for (const auto& ship : playerShips) {
            for (int i = 0; i < ship.length; ++i) {
                sf::RectangleShape shipPart(sf::Vector2f(cellSizeX, cellSizeY));
                if (ship.direction == ShipDirection::HORIZONTAL) {
                    shipPart.setPosition(boardMarginLeft + (ship.startCol + i) * cellSizeX, boardMarginTop + ship.startRow * cellSizeY);
                }
                else {
                    shipPart.setPosition(boardMarginLeft + ship.startCol * cellSizeX, boardMarginTop + (ship.startRow + i) * cellSizeY);
                }

                shipPart.setFillColor(ship.color);
                shipPart.setOutlineThickness(1);
                shipPart.setOutlineColor(sf::Color::Black);
                gameWindow.draw(shipPart);
            }

        }
        // Draw Pause/Continue Button
        pauseButton.draw(gameWindow);
        // Draw Exit Button
        exitButton.draw(gameWindow);

        gameWindow.display();
    }
}


void showShipPlacementWindow(sf::RenderWindow& mainWindow, sf::Font& font) {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const int boardSize = 500; 

    sf::RenderWindow placementWindow(sf::VideoMode(windowWidth, windowHeight), "Ship Placement");


    const int gridRows = 10;
    const int gridCols = 10;

    const float cellSizeX = static_cast<float>(boardSize) / gridCols;
    const float cellSizeY = static_cast<float>(boardSize) / gridRows;

    // Board Margin 
    const int boardMarginLeft = 50;
    const int boardMarginTop = 50;

    //  Position of Ships:
    const int shipPanelPositionX = boardMarginLeft + boardSize + 50;

    // Ships to Place
    std::vector<Ship> shipsToPlace = {
        { 4, 0, 0, ShipDirection::HORIZONTAL, sf::Color::Cyan },   // 1x  (length 4)
        { 3, 0, 0, ShipDirection::HORIZONTAL, sf::Color::Magenta },  // 2x (length 3)
        { 2, 0, 0, ShipDirection::HORIZONTAL, sf::Color::Blue },    // 3x (length 2)
        { 1, 0, 0, ShipDirection::HORIZONTAL, sf::Color::Green }    // 4x (length 1)
    };


    std::vector<Ship> placedShips;
    autoPlaceShipsInPlacement(placedShips, gridRows, gridCols);

    //  Create Buttons 
    Button autoButton("Auto", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, boardMarginLeft + boardSize / 2.0f - 50, windowHeight - 50);
    Button battleButton("To Battle", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, boardMarginLeft + boardSize + 100, windowHeight - 50);
    Button rotateButton("Rotate", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, shipPanelPositionX, 50);


    sf::Texture seaBattleTexture;
    if (!seaBattleTexture.loadFromFile("field.png")) {
        std::cerr << "Error loading image!" << std::endl;
        return;
    }

    sf::Sprite seaBattleSprite;
    seaBattleSprite.setTexture(seaBattleTexture);
    seaBattleSprite.setScale(cellSizeX / (seaBattleTexture.getSize().x / gridCols), cellSizeY / (seaBattleTexture.getSize().y / gridRows));
    seaBattleSprite.setPosition(boardMarginLeft, boardMarginTop);

    while (placementWindow.isOpen()) {
        sf::Event event;
        while (placementWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                placementWindow.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    //  Auto Button 
                    if (autoButton.isMouseOver(placementWindow)) {
                        autoButton.setPressed(true);
                    }
                    // Battle Button 
                    if (battleButton.isMouseOver(placementWindow)) {
                        battleButton.setPressed(true);
                    }
                    //  Rotate Button 
                    if (rotateButton.isMouseOver(placementWindow)) {
                        rotateButton.setPressed(true);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Auto Button 
                    if (autoButton.isPressed) {
                        placedShips.clear();  
                        autoPlaceShipsInPlacement(placedShips, gridRows, gridCols);
                    }
                    autoButton.setPressed(false);
                    //  Battle Button 
                    if (battleButton.isPressed) {
                        placementWindow.close();
                        runSeaBattleGame(placedShips); 
                    }
                    battleButton.setPressed(false);
                    //  Rotate Button 
                    if (rotateButton.isPressed) {
                        std::cout << "Rotate" << std::endl;

                    }
                    rotateButton.setPressed(false);
                }
            }
        }

        autoButton.update(placementWindow);
        battleButton.update(placementWindow);
        rotateButton.update(placementWindow);

        placementWindow.clear(sf::Color::Black);


        placementWindow.draw(seaBattleSprite);


        for (const auto& ship : placedShips) {
            for (int j = 0; j < ship.length; ++j) {
                sf::RectangleShape shipPart(sf::Vector2f(cellSizeX, cellSizeY));
                if (ship.direction == ShipDirection::HORIZONTAL) {
                    shipPart.setPosition(boardMarginLeft + (ship.startCol + j) * cellSizeX, boardMarginTop + ship.startRow * cellSizeY);
                }
                else {
                    shipPart.setPosition(boardMarginLeft + ship.startCol * cellSizeX, boardMarginTop + (ship.startRow + j) * cellSizeY);
                }
                shipPart.setFillColor(ship.color);
                shipPart.setOutlineThickness(1);
                shipPart.setOutlineColor(sf::Color::Black);
                placementWindow.draw(shipPart);
            }
        }

        
        for (size_t i = 0; i < shipsToPlace.size(); ++i) {
            for (int j = 0; j < shipsToPlace[i].length; ++j) {
                sf::RectangleShape shipPart(sf::Vector2f(cellSizeX, cellSizeY));

                shipPart.setPosition(shipPanelPositionX, 100 + i * 100 + j * cellSizeY);  // Adjusted position

                shipPart.setFillColor(shipsToPlace[i].color);
                shipPart.setOutlineThickness(1);
                shipPart.setOutlineColor(sf::Color::Black);
                placementWindow.draw(shipPart);
            }
        }

        //Draw Buttons
        autoButton.draw(placementWindow);
        battleButton.draw(placementWindow);
        rotateButton.draw(placementWindow);
        placementWindow.display();
    }
}


void showGameModeMenu(sf::RenderWindow& mainWindow, sf::Font& font) {

    const int menuWidth = 400;
    const int menuHeight = 400;

  
    Button withComputerButton("With Computer", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, menuWidth / 2.0f, menuHeight / 2.0f - 50);
    Button withFriendButton("With Friend", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, menuWidth / 2.0f, menuHeight / 2.0f + 50);
    Button backButton("Back", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, menuWidth / 2.0f, menuHeight / 2.0f + 150);

    sf::RenderWindow gameModeMenu(sf::VideoMode(menuWidth, menuHeight), "Select Game Mode");


    while (gameModeMenu.isOpen()) {
        sf::Event event;
        while (gameModeMenu.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                gameModeMenu.close();


            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    //  With Computer Button 
                    if (withComputerButton.isMouseOver(gameModeMenu)) {
                        withComputerButton.setPressed(true);
                    }
                    // With Friend Button 
                    if (withFriendButton.isMouseOver(gameModeMenu)) {
                        withFriendButton.setPressed(true);
                    }
                    // Back Button 
                    if (backButton.isMouseOver(gameModeMenu)) {
                        backButton.setPressed(true);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    //  With Computer Button 
                    if (withComputerButton.isPressed) {
                        gameModeMenu.close();
                        showShipPlacementWindow(mainWindow, font);


                    }
                    withComputerButton.setPressed(false);
                    //  With Friend Button 
                    if (withFriendButton.isPressed) {
                        gameModeMenu.close();
                        runSeaBattleGame(std::vector<Ship>()); // Pass empty vector for player ships
                    }
                    withFriendButton.setPressed(false);

                    //  Back Button 
                    if (backButton.isPressed) {
                        gameModeMenu.close();  // Close the game mode menu
                    }
                    backButton.setPressed(false);
                }
            }
        }

        withComputerButton.update(gameModeMenu);
        withFriendButton.update(gameModeMenu);
        backButton.update(gameModeMenu);

        gameModeMenu.clear(sf::Color::Cyan);  

        withComputerButton.draw(gameModeMenu);
        withFriendButton.draw(gameModeMenu);
        backButton.draw(gameModeMenu);

        gameModeMenu.display();
    }
}

int main() {
    //  First window: Main Menu 
    const int menuWindowWidth = 400;
    const int menuWindowHeight = 300;

    sf::RenderWindow mainWindow(sf::VideoMode(menuWindowWidth, menuWindowHeight), "Main Menu");


    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return EXIT_FAILURE;
    }
    //  Create Buttons 
    Button playButton("Play", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, menuWindowWidth / 2.0f, menuWindowHeight / 2.0f - 50); // Centered

    Button exitButton("Exit", font, sf::Color::White, sf::Color::Yellow, sf::Color::Red, menuWindowWidth / 2.0f, menuWindowHeight / 2.0f + 50);  // Centered

    // Game Loop for Main Menu 
    while (mainWindow.isOpen()) {
        sf::Event event;
        while (mainWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                mainWindow.close();

            // Play button
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    //  Play button clicked: 
                    if (playButton.isMouseOver(mainWindow)) {
                        playButton.setPressed(true);
                    }
                    //  Exit button clicked: 
                    if (exitButton.isMouseOver(mainWindow)) {
                        exitButton.setPressed(true);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Play button clicked:
                    if (playButton.isPressed) {
                        showGameModeMenu(mainWindow, font);
                    }
                    playButton.setPressed(false);

                    // Exit button clicked: 
                    if (exitButton.isPressed) {
                        mainWindow.close();
                    }
                    exitButton.setPressed(false);
                }
            }
        }

        playButton.update(mainWindow);
        exitButton.update(mainWindow);

        mainWindow.clear(sf::Color::Black);
        playButton.draw(mainWindow);
        exitButton.draw(mainWindow);
        mainWindow.display();
    }

    return 0;
}
