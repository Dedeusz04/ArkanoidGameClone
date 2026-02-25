import <SFML/Graphics.hpp>;
import game_state_manager;
import menu_state;
import audio;
import highscore;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "projectArkanoid");
    window.setFramerateLimit(120);

    AudioManager audioManager;
    HighScoreManager highScoreManager;
    GameStateManager stateManager;
    stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            stateManager.handleEvents(window, event);
        }

        float deltaTime = clock.restart().asSeconds();
        stateManager.update(deltaTime);
        stateManager.draw(window);
    }

    return 0;
}