module nickname_input_state;

import playing_state;
import <stdexcept>;
import <cctype>;
import <iostream>;

NicknameInputState::NicknameInputState(GameStateManager& sm, AudioManager& am, HighScoreManager& hsm, int startLevel)
    : stateManager(sm), audioManager(am), highScoreManager(hsm), startLevel(startLevel) {
    if (!font.loadFromFile("assets/arial.ttf")) {
        throw std::runtime_error("Failed to load font: assets/arial.ttf");
    }
    if (!backgroundTexture.loadFromFile("assets/background2.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    backgroundSprite.setTexture(backgroundTexture);

    promptText.setFont(font);
    promptText.setString("Enter your nickname (start with capital, no spaces, max 10 chars, alphanumeric):");
    promptText.setCharacterSize(20);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(50, 200);

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

    std::cout << "NicknameInputState initialized with startLevel: " << startLevel << "\n";
}

void NicknameInputState::handleEvents(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128) {
            char c = static_cast<char>(event.text.unicode);
            if (c == '\b' && !nickname.empty()) { // Backspace
                nickname.pop_back();
                errorText.setString("");
            }
            else if (c == ' ') { // Handle space explicitly
                errorText.setString("Spaces are not allowed!");
            }
            else if (c >= 32) {
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
                std::cout << "Nickname entered: " << nickname << ", starting level " << startLevel << "\n";
                stateManager.setNickname(nickname);
                stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager, startLevel);
            }
            else if (highScoreManager.isNicknameUnique(nickname)) {
                std::cout << "Nickname entered: " << nickname << ", starting level " << startLevel << "\n";
                stateManager.setNickname(nickname);
                stateManager.setState<PlayingState>(stateManager, audioManager, highScoreManager, startLevel);
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

void NicknameInputState::update(float deltaTime) {
    nicknameText.setString(nickname);
}

void NicknameInputState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(backgroundSprite);
    window.draw(promptText);
    window.draw(nicknameText);
    window.draw(errorText);
    window.display();
}