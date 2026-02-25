export module menu_state;

import <SFML/Graphics.hpp>;
import game_state;
import game_state_manager;
import audio;
import highscore;
import <vector>;

export class MenuState : public GameState {
public:
    MenuState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm);
    void handleEvents(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
private:
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;
    sf::Font font;
    sf::Text startText;
    sf::Text selectLevelText;
    sf::Text highscoreText;
    sf::Text exitText;
    bool isSelectingLevel = false;
    sf::Text levelSelectTitle;
    sf::RectangleShape overlay;
    std::vector<sf::Text> levelTexts;
    sf::Text backText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    int maxLevel = 0;
    void updateHighscoreText();
};