export module game_state;

// Systemowe importy
import <SFML/Graphics.hpp>;
import <memory>;
import <string>;
import <stdexcept>;
import <type_traits>;
import <cctype>;
import <regex>;

// Własne moduły
import game_objects;
import audio;
import highscore;
import config;

// Eksport klas
export class GameState;
export class GameStateManager;
export class MenuState;
export class PlayingState;
export class GameOverState;
export class NicknameInputState;

// Definicja GameState
export class GameState {
public:
    virtual void handleEvents(sf::RenderWindow& window, sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~GameState() = default;
};

// GameStateManager
export class GameStateManager {
    std::unique_ptr<GameState> currentState;
    std::string currentNickname; // Przechowuje nick gracza

public:
    template<typename T, typename... Args>
    void setState(Args&&... args) {
        static_assert(std::is_base_of_v<GameState, T>,
            "T must derive from GameState");
        currentState = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void setNickname(const std::string& nickname) {
        currentNickname = nickname;
    }

    std::string getNickname() const {
        return currentNickname;
    }

    void handleEvents(sf::RenderWindow& window, sf::Event& event) {
        if (currentState) currentState->handleEvents(window, event);
    }

    void update(float deltaTime) {
        if (currentState) currentState->update(deltaTime);
    }

    void draw(sf::RenderWindow& window) {
        if (currentState) currentState->draw(window);
    }
};

// MenuState
export class MenuState : public GameState {
    sf::Font font;
    sf::Text startText;
    sf::Text highscoreText;
    sf::Text exitText;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;

public:
    MenuState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm)
        : stateManager(sm), audioManager(am), highScoreManager(hsm) {
        if (!font.loadFromFile("assets/arial.ttf")) {
            throw std::runtime_error("Failed to load font: assets/arial.ttf");
        }

        startText.setFont(font);
        startText.setString("Start Game");
        startText.setCharacterSize(40);
        startText.setFillColor(sf::Color::White);
        startText.setPosition(300, 250);

        highscoreText.setFont(font);
        updateHighscoreText();
        highscoreText.setCharacterSize(20);
        highscoreText.setFillColor(sf::Color::White);
        highscoreText.setPosition(300, 350);

        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(40);
        exitText.setFillColor(sf::Color::White);
        exitText.setPosition(300, 450);
    }

    void handleEvents(sf::RenderWindow& window, sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            auto mousePos = sf::Mouse::getPosition(window);
            if (startText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                stateManager.setState<NicknameInputState>(stateManager, audioManager, highScoreManager);
            }
            else if (exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                window.close();
            }
        }
    }

    void update(float deltaTime) override {
        auto mousePos = sf::Mouse::getPosition();
        startText.setFillColor(startText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Green : sf::Color::White);
        exitText.setFillColor(exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Red : sf::Color::White);
    }

    void draw(sf::RenderWindow& window) override {
        window.clear(sf::Color::Black);
        window.draw(startText);
        window.draw(highscoreText);
        window.draw(exitText);
        window.display();
    }

private:
    void updateHighscoreText() {
        auto scores = highScoreManager.getHighScores();
        std::string text = "High Scores:\n";
        for (size_t i = 0; i < scores.size() && i < 4; ++i) {
            text += scores[i].first + ": " + std::to_string(scores[i].second) + "\n";
        }
        highscoreText.setString(text);
    }
};

// NicknameInputState
export class NicknameInputState : public GameState {
    sf::Font font;
    sf::Text promptText;
    sf::Text nicknameText;
    sf::Text errorText;
    std::string nickname;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;

public:
    NicknameInputState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm)
        : stateManager(sm), audioManager(am), highScoreManager(hsm) {
        if (!font.loadFromFile("assets/arial.ttf")) {
            throw std::runtime_error("Failed to load font: assets/arial.ttf");
        }

        promptText.setFont(font);
        promptText.setString("Enter your nickname (start with capital, no spaces, max 10 chars,):");
        promptText.setCharacterSize(20);
        promptText.setFillColor(sf::Color::White);
        promptText.setPosition(100, 200);

        nicknameText.setFont(font);
        nicknameText.setString("");
        nicknameText.setCharacterSize(30);
        nicknameText.setFillColor(sf::Color::Yellow);
        nicknameText.setPosition(300, 250);

        errorText.setFont(font);
        errorText.setString("");
        errorText.setCharacterSize(20);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(100, 300);
    }

    void handleEvents(sf::RenderWindow& window, sf::Event& event) override {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                if (c == '\b' && !nickname.empty()) { // Backspace
                    nickname.pop_back();
                    errorText.setString("");
                }
                else if (c >= 32 && c != ' ') {
                    // Create temporary nickname to test with new character
                    std::string tempNickname = nickname + c;
                    std::regex charRegex(nickname.empty() ? "^[A-Z]$" : "^[A-Z][A-Za-z0-9]{0,9}$");
                    if (tempNickname.length() <= 10 && std::regex_match(tempNickname, charRegex)) {
                        nickname += c;
                        errorText.setString("");
                    }
                    else {
                        if (nickname.empty() && (c < 'A' || c > 'Z')) {
                            errorText.setString("Must start with a capital letter!");
                        }
                        else if (!std::isalnum(c)) {
                            errorText.setString("Only letters and numbers allowed!");
                        }
                        else if (tempNickname.length() > 10) {
                            errorText.setString("Nickname must be 10 characters or less!");
                        }
                    }
                }
                nicknameText.setString(nickname);
            }
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !nickname.empty()) {
            std::regex nicknameRegex("^[A-Z][A-Za-z0-9]{0,9}$");
            if (std::regex_match(nickname, nicknameRegex)) {
                if (!highScoreManager.isNicknameUnique(nickname) && std::filesystem::exists("players/" + nickname + ".txt")) {
                    // Allow existing nickname
                    stateManager.setNickname(nickname);
                    stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager);
                }
                else if (highScoreManager.isNicknameUnique(nickname)) {
                    // New nickname
                    stateManager.setNickname(nickname);
                    stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager);
                }
                else {
                    errorText.setString("Nickname already taken!");
                }
            }
            else {
                errorText.setString("Invalid nickname format!");
            }
        }
    }

    void update(float deltaTime) override {
        nicknameText.setString(nickname);
    }

    void draw(sf::RenderWindow& window) override {
        window.clear(sf::Color::Black);
        window.draw(promptText);
        window.draw(nicknameText);
        window.draw(errorText);
        window.display();
    }
};

// PlayingState
export class PlayingState : public GameState {
    GameObjectManager gameObjects;
    CollisionSystem collisionSystem;
    GameStateManager& stateManager;
    AudioManager& audioManager;
    HighScoreManager& highScoreManager;
    LevelConfig config;

    int lives = 3;
    bool isPaused = false;
    bool stateChanged = false;
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text pauseText;
    sf::RectangleShape pauseOverlay;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text mainMenuText;
    sf::Text exitText;

public:
    PlayingState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm)
        : stateManager(sm), audioManager(am), highScoreManager(hsm), collisionSystem(am), stateChanged(false) {
        try {
            config = loadLevelConfig("assets/level1.txt");
        }
        catch (...) {
            config = { 100, 300.0f, {} }; // Domyślne wartości, pusta plansza
        }

        gameObjects.reset(config.paddleWidth, config.ballSpeed, config.levelLayout);

        if (!font.loadFromFile("assets/arial.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

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
    }

    void handleEvents(sf::RenderWindow& window, sf::Event& event) override {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                isPaused = !isPaused;
            }
        }
        if (isPaused && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            auto mousePos = sf::Mouse::getPosition(window);
            if (resumeText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                isPaused = false;
            }
            else if (restartText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager);
            }
            else if (mainMenuText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);
            }
            else if (exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                window.close();
            }
        }
    }

    void update(float deltaTime) override {
        if (isPaused || stateChanged) {
            if (isPaused) {
                auto mousePos = sf::Mouse::getPosition();
                resumeText.setFillColor(resumeText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Green : sf::Color::White);
                restartText.setFillColor(restartText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Green : sf::Color::White);
                mainMenuText.setFillColor(mainMenuText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Red : sf::Color::White);
                exitText.setFillColor(exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Red : sf::Color::White);
            }
            return;
        }

        gameObjects.update(deltaTime);
        collisionSystem.checkCollisions(gameObjects);

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

    void draw(sf::RenderWindow& window) override {
        window.clear(sf::Color::Black);
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
        window.display();
    }
};

// GameOverState
export class GameOverState : public GameState {
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

public:
    GameOverState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int score)
        : stateManager(sm), audioManager(am), highScoreManager(hsm), finalScore(score) {
        if (!font.loadFromFile("assets/arial.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

        // Zapisz wynik z bieżącym nickiem
        highScoreManager.addScore(stateManager.getNickname(), finalScore);

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

    void handleEvents(sf::RenderWindow& window, sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            auto mousePos = sf::Mouse::getPosition(window);
            if (restartText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                stateManager.setState<NicknameInputState>(stateManager, audioManager, highScoreManager);
            }
            else if (menuText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                stateManager.setState<MenuState>(stateManager, audioManager, highScoreManager);
            }
            else if (exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                window.close();
            }
        }
    }

    void update(float deltaTime) override {
        auto mousePos = sf::Mouse::getPosition();
        restartText.setFillColor(restartText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Green : sf::Color::White);
        menuText.setFillColor(menuText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Red : sf::Color::White);
        exitText.setFillColor(exitText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ? sf::Color::Red : sf::Color::White);
    }

    void draw(sf::RenderWindow& window) override {
        window.clear(sf::Color::Black);
        window.draw(overlay);
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(highscoreText);
        window.draw(restartText);
        window.draw(menuText);
        window.draw(exitText);
        window.display();
    }

private:
    void updateHighscoreText() {
        auto scores = highScoreManager.getHighScores();
        std::string text = "High Scores:\n";
        for (size_t i = 0; i < scores.size() && i < 3; ++i) {
            text += scores[i].first + ": " + std::to_string(scores[i].second) + "\n";
        }
        highscoreText.setString(text);
    }
};