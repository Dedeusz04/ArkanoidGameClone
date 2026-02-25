export module playing_state;

import <SFML/Graphics.hpp>;
import game_state;
import game_state_manager;
import game_object_manager;
import collision_system;
import audio;
import highscore;
import config;

export class PlayingState : public GameState {
public:
    PlayingState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int startLevel = 1);
    int FcurrentLevel();
    void handleEvents(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
private:
    GameObjectManager gameObjects;
    CollisionSystem collisionSystem;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;
    LevelConfig config;
    int lives = 3;
    bool isPaused = false;
    bool stateChanged = false;
    bool isLevelComplete = false;
    int maxLevel = 3;
    int currentLevel = 1;
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text pauseText;
    sf::RectangleShape pauseOverlay;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text mainMenuText;
    sf::Text exitText;
    sf::Text levelCompleteText;
    sf::Text nextLevelText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
};