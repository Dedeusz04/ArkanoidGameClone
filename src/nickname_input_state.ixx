export module nickname_input_state;

import <SFML/Graphics.hpp>;
import <string>;
import <regex>;
import <filesystem>;
import game_state;
import game_state_manager;
import audio;
import highscore;

export class NicknameInputState : public GameState {
public:
    NicknameInputState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int startLevel = 1); // Added startLevel
    void handleEvents(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
private:
    sf::Font font;
    sf::Text promptText;
    sf::Text nicknameText;
    sf::Text errorText;
    std::string nickname;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    int startLevel; // Added to store selected level
};