#include <iostream>
module game_over_state;

import nickname_input_state;
import menu_state;
import <stdexcept>;
import <vector>;

GameOverState::GameOverState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm,int score)
    : stateManager(sm), audioManager(am), highScoreManager(hsm), finalScore(score) {
    if (!font.loadFromFile("assets/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    if (!backgroundTexture.loadFromFile("assets/background2.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    backgroundSprite.setTexture(backgroundTexture);
    highScoreManager.addScore(stateManager.getNickname(), finalScore, stateManager.FcurrentLevel());

    overlay.setSize({ 800, 600 });
    overlay.setFillColor(sf::Color(0, 0, 0, 128));

    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(300, 100);

    scoreText.setFont(font);
    scoreText.setString("Your Score: " + std::to_string(finalScore));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(300, 200);

    highscoreText.setFont(font);
    updateHighscoreText();
    highscoreText.setCharacterSize(20);
    highscoreText.setFillColor(sf::Color::White);
    highscoreText.setPosition(300, 250);

    restartText.setFont(font);
    restartText.setString("Restart");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(300, 350);

    menuText.setFont(font);
    menuText.setString("Main Menu");
    menuText.setCharacterSize(30);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition(300, 400);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(30);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(300, 450);
}
void GameOverState::handleEvents(sf::RenderWindow& window, sf::Event& event) {
   sf::Vector2f mousePos;
    if (event.type == sf::Event::MouseMoved) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        // Aktualizuj kolory przycisków podczas ruchu myszy
        restartText.setFillColor(restartText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
        menuText.setFillColor(menuText.getGlobalBounds().contains(mousePos) ? sf::Color::Green : sf::Color::White);
        exitText.setFillColor(exitText.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        std::cout << "Mouse clicked at (" << mousePos.x << ", " << mousePos.y << ")\n";

        if (restartText.getGlobalBounds().contains(mousePos)) {
            std::cout << "Restart clicked\n";
            stateManager.setState<NicknameInputState>(stateManager, audioManager, highScoreManager);
        }
        else if (menuText.getGlobalBounds().contains(mousePos)) {
            std::cout << "Main Menu clicked\n";
            stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);
        }
        else if (exitText.getGlobalBounds().contains(mousePos)) {
            std::cout << "Exit clicked\n";
            window.close();
        }
    }
}

void GameOverState::update(float deltaTime) {
}

void GameOverState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(backgroundSprite);
    window.draw(overlay);
    window.draw(gameOverText);
    window.draw(scoreText);
    window.draw(highscoreText);
    window.draw(restartText);
    window.draw(menuText);
    window.draw(exitText);
    window.display();
}

void GameOverState::updateHighscoreText() {
    auto scores = highScoreManager.getHighScores();
    std::string text = "High Scores:\n";
    for (size_t i = 0; i < scores.size() && i < 3; ++i) {
        text += scores[i].first + ": " + std::to_string(scores[i].second) + "\n";
    }
    highscoreText.setString(text);
}