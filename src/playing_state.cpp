module playing_state;

import menu_state;
import game_over_state;
import ball;
import <stdexcept>;
import <filesystem>;
import <iostream>;

PlayingState::PlayingState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int startLevel)
    : stateManager(sm), audioManager(am), highScoreManager(hsm), collisionSystem(am), stateChanged(false), isLevelComplete(false), currentLevel(startLevel) {
    maxLevel = 0;
    while (std::filesystem::exists("assets/level" + std::to_string(maxLevel + 1) + ".txt")) {
        ++maxLevel;
    }
    std::cout << "Found " << maxLevel << " levels\n";
    if (maxLevel == 0) {
        throw std::runtime_error("No level files found in assets/");
    }
    if (startLevel < 1 || startLevel > maxLevel) {
        throw std::runtime_error("Invalid start level: " + std::to_string(startLevel));
    }

    // Load the specified level
    try {
        config = loadLevelConfig("assets/level" + std::to_string(currentLevel) + ".txt");
        std::cout << "Loaded level " << currentLevel << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading level " << currentLevel << ": " << e.what() << "\n";
        config = { 100, 300.0f, {} };
    }

    gameObjects.reset(config.paddleWidth, config.ballSpeed, config.levelLayout);

    if (!font.loadFromFile("assets/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    stateManager.setcurrentLevel(currentLevel);
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    backgroundSprite.setTexture(backgroundTexture);

    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(700, 10);

    pauseText.setFont(font);
    pauseText.setString("Paused");
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(300, 150);

    pauseOverlay.setSize({ 800, 600 });
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 128));

    resumeText.setFont(font);
    resumeText.setString("Resume");
    resumeText.setCharacterSize(30);
    resumeText.setFillColor(sf::Color::White);
    resumeText.setPosition(300, 250);

    restartText.setFont(font);
    restartText.setString("Restart");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(300, 300);

    mainMenuText.setFont(font);
    mainMenuText.setString("Main Menu");
    mainMenuText.setCharacterSize(30);
    mainMenuText.setFillColor(sf::Color::White);
    mainMenuText.setPosition(300, 350);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(30);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(300, 400);

    levelCompleteText.setFont(font);
    levelCompleteText.setString("Level " + std::to_string(currentLevel) + " Complete!");
    levelCompleteText.setCharacterSize(50);
    levelCompleteText.setFillColor(sf::Color::White);
    levelCompleteText.setPosition(250, 150);

    nextLevelText.setFont(font);
    nextLevelText.setString("Next Level");
    nextLevelText.setCharacterSize(30);
    nextLevelText.setFillColor(sf::Color::White);
    nextLevelText.setPosition(300, 250);
}

void PlayingState::handleEvents(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        window.close();
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape && !isLevelComplete) {
        isPaused = !isPaused;
    }
    sf::Vector2f mousePos;
    if (event.type == sf::Event::MouseMoved && (isPaused || isLevelComplete)) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        // Aktualizuj kolory przycisków podczas ruchu myszy
        if (isPaused) {
            resumeText.setFillColor(resumeText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            restartText.setFillColor(restartText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            mainMenuText.setFillColor(mainMenuText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            exitText.setFillColor(exitText.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
        }
        else if (isLevelComplete) {
            nextLevelText.setFillColor((currentLevel < maxLevel && nextLevelText.getGlobalBounds().contains(mousePos)) ? sf::Color::Green : (currentLevel < maxLevel ? sf::Color::White : sf::Color(128, 128, 128)));
            restartText.setFillColor(restartText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            mainMenuText.setFillColor(mainMenuText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
            exitText.setFillColor(exitText.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && (isPaused || isLevelComplete)) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        std::cout << "Mouse clicked at (" << mousePos.x << ", " << mousePos.y << ")\n";

        if (isPaused) {
            if (resumeText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Resume clicked\n";
                isPaused = false;
            }
            else if (restartText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Restart clicked for level " << currentLevel << "\n";
                stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager, currentLevel);
            }
            else if (mainMenuText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Main Menu clicked\n";
                stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);
            }
            else if (exitText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Exit clicked\n";
                window.close();
            }
        }
        else if (isLevelComplete) {
            if (currentLevel < maxLevel && nextLevelText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Next Level clicked\n";
                currentLevel++;
                stateManager.setcurrentLevel(currentLevel);
                try {
                    config = loadLevelConfig("assets/level" + std::to_string(currentLevel) + ".txt");
                    std::cout << "Loaded level " << currentLevel << "\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error loading level " << currentLevel << ": " << e.what() << "\n";
                    config = { 100, 300.0f, {} };
                }
                gameObjects.reset(config.paddleWidth, config.ballSpeed, config.levelLayout);
                lives = 3;
                isLevelComplete = false;
                levelCompleteText.setString("Level " + std::to_string(currentLevel) + " Complete!");
            }
            else if (restartText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Restart clicked for level " << currentLevel << "\n";
                stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager, currentLevel);
            }
            else if (mainMenuText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Main Menu clicked\n";
                stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);
            }
            else if (exitText.getGlobalBounds().contains(mousePos)) {
                std::cout << "Exit clicked\n";
                window.close();
            }
        }
    }
}

void PlayingState::update(float deltaTime) {
    if (isPaused || isLevelComplete || stateChanged) {
        return;
    }
    gameObjects.update(deltaTime);
    collisionSystem.checkCollisions(gameObjects);

    if (gameObjects.allDestructibleBlocksDestroyed()) {
        isLevelComplete = true;
        audioManager.playDestroySound();
        highScoreManager.addScore(stateManager.getNickname(), gameObjects.getScore(),currentLevel);
        std::cout << "Level " << currentLevel << " completed\n";
        return;
    }

    auto ball = gameObjects.getBall();
    if (ball && ball->isOutOfBounds()) {
        if (--lives <= 0) {
            stateManager.setState<GameOverState>(stateManager, audioManager, highScoreManager, gameObjects.getScore());
            stateChanged = true;
            return;
        }
        else {
            ball->reset(400, 300);
        }
    }

    scoreText.setString("Score: " + std::to_string(gameObjects.getScore()));
    livesText.setString("Lives: " + std::to_string(lives));
}

void PlayingState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(backgroundSprite);
    gameObjects.draw(window);
    window.draw(scoreText);
    window.draw(livesText);
    if (isPaused) {
        window.draw(pauseOverlay);
        window.draw(pauseText);
        window.draw(resumeText);
        window.draw(restartText);
        window.draw(mainMenuText);
        window.draw(exitText);
    }
    else if (isLevelComplete) {
        window.draw(pauseOverlay);
        window.draw(levelCompleteText);
        window.draw(nextLevelText);
        window.draw(restartText);
        window.draw(mainMenuText);
        window.draw(exitText);
    }
    window.display();
}