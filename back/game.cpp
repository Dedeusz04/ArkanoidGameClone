import game_objects;
import game_state;
import highscore;
import audio;
import <SFML/Graphics.hpp>;
import <memory>;

int main() {
    // Utworzenie okna
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid");
    window.setFramerateLimit(30);

    // Inicjalizacja menedżera dźwięku
    AudioManager audioManager;
    try {
        audioManager.playBackgroundMusic();
    }
    catch (const std::exception& e) {
        // Obsługa błędu
    }

    // Inicjalizacja menedżera wyników
    HighScoreManager highScoreManager;

    // Inicjalizacja menedżera stanów
    GameStateManager stateManager;
    stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);

    // Pętla gry
    sf::Clock clock;
    while (window.isOpen()) {
        // Obsługa zdarzeń
        sf::Event event;
        while (window.pollEvent(event)) {
            stateManager.handleEvents(window, event);
        }

        // Aktualizacja
        float deltaTime = clock.restart().asSeconds();
        stateManager.update(deltaTime);

        // Rysowanie
        stateManager.draw(window);
    }

    return 0;
}