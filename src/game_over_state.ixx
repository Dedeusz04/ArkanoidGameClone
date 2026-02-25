export module game_over_state;

import <SFML/Graphics.hpp>;
import <string>;
import game_state;
import game_state_manager;
import audio;
import highscore;

export class GameOverState : public GameState {
public:
    GameOverState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int score);
    void handleEvents(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
private:
    sf::Font font;
    sf::Text gameOverText;
    sf::Text scoreText;
    sf::Text highscoreText;
    sf::Text restartText;
    sf::Text menuText;
    sf::Text exitText;
    sf::RectangleShape overlay;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;
    int finalScore;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    void updateHighscoreText();
};