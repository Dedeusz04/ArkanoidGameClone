module menu_state;

import nickname_input_state;
import playing_state;
import <stdexcept>;
import <vector>;
import <filesystem>;
import <iostream>;

MenuState::MenuState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm)
    : stateManager(sm), audioManager(am), highScoreManager(hsm), isSelectingLevel(false) {
    // Load font and check for success
    if (!font.loadFromFile("assets/arial.ttf")) {
        throw std::runtime_error("Failed to load font: assets/arial.ttf");
    }

    // Determine maxLevel by checking existing level files
    maxLevel = 0;
    while (std::filesystem::exists("assets/level" + std::to_string(maxLevel + 1) + ".txt")) {
        ++maxLevel;
    }
    std::cout << "MenuState: Found " << maxLevel << " levels\n";
    if (maxLevel == 0) {
        throw std::runtime_error("No level files found in assets/");
    }
    if (!backgroundTexture.loadFromFile("assets/background2.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Initialize main menu texts
    startText.setFont(font);
    startText.setString("Start Game");
    startText.setCharacterSize(40);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(300, 200);

    selectLevelText.setFont(font);
    selectLevelText.setString("Select Level");
    selectLevelText.setCharacterSize(40);
    selectLevelText.setFillColor(sf::Color::White);
    selectLevelText.setPosition(300, 300);

    highscoreText.setFont(font);
    updateHighscoreText();
    highscoreText.setCharacterSize(20);
    highscoreText.setFillColor(sf::Color::White);
    highscoreText.setPosition(300, 400);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(40);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(0, 0);

    // Initialize level selection screen
    levelSelectTitle.setFont(font);
    levelSelectTitle.setString("Select Level");
    levelSelectTitle.setCharacterSize(50);
    levelSelectTitle.setFillColor(sf::Color::White);
    levelSelectTitle.setPosition(300, 150);

    overlay.setSize({ 800, 600 });
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
	am.playBackgroundMusic();

    // Create level buttons
    levelTexts.clear();
    for (int i = 1; i <= maxLevel; ++i) {
        sf::Text levelText;
        levelText.setFont(font);
        levelText.setString("Level " + std::to_string(i));
        levelText.setCharacterSize(30);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(300, 200 + i * 50);
        levelTexts.push_back(levelText);
    }

    backText.setFont(font);
    backText.setString("Back");
    backText.setCharacterSize(30);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(300, 200 + (maxLevel + 1) * 50);
}

void MenuState::handleEvents(sf::RenderWindow& window, sf::Event& event) {
    // Mapuj pozycję myszy na współrzędne widoku
    sf::Vector2f mousePos;
    if (event.type == sf::Event::MouseMoved) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        // Aktualizuj kolory przycisków podczas ruchu myszy
        if (!isSelectingLevel) {
            startText.setFillColor(startText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            selectLevelText.setFillColor(selectLevelText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            exitText.setFillColor(exitText.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
        }
        else {
            for (auto& levelText : levelTexts) {
                levelText.setFillColor(levelText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            }
            backText.setFillColor(backText.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        std::cout << "Mouse clicked at (" << mousePos.x << ", " << mousePos.y << ")\n";

        if (!isSelectingLevel) {
            if (startText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Start Game clicked\n";
                stateManager.setState<NicknameInputState>(stateManager, audioManager, highScoreManager, 1);
            }
            else if (selectLevelText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Select Level clicked\n";
                isSelectingLevel = true;
            }
            else if (exitText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Exit clicked\n";
                window.close();
            }
        }
        else {
            bool stateChanged = false;
            for (int i = 0; i < levelTexts.size(); ++i) {
                if (levelTexts[i].getGlobalBounds().contains(mousePos)) {
                    std::cout << "Level " << (i + 1) << " clicked\n";
                    stateManager.setState<NicknameInputState>(stateManager, audioManager, highScoreManager, i + 1);
                    stateChanged = true;
                    break;
                }
            }
            if (!stateChanged && backText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Back clicked\n";
                isSelectingLevel = false;
            }
        }
    }
}

void MenuState::update(float deltaTime) {
}

void MenuState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(backgroundSprite);
    if (!isSelectingLevel) {
        window.draw(startText);
        window.draw(selectLevelText);
        window.draw(highscoreText);
        window.draw(exitText);
    }
    else {
        window.draw(overlay);
        window.draw(levelSelectTitle);
        for (const auto& levelText : levelTexts) {
            window.draw(levelText);
        }
        window.draw(backText);
    }
    window.display();
}

void MenuState::updateHighscoreText() {
    auto scores = highScoreManager.getHighScores();
    std::string text = "High Scores:\n";
    for (size_t i = 0; i < scores.size() && i < 4; ++i) {
        text += scores[i].first + ": " + std::to_string(scores[i].second) + "\n";
    }
    highscoreText.setString(text);
}