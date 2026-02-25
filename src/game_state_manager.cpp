module game_state_manager;

void GameStateManager::setNickname(const std::string& nickname) {
    currentNickname = nickname;
}

std::string GameStateManager::getNickname() const {
    return currentNickname;
}
int GameStateManager::FcurrentLevel() {
    return currentLevel;
}
void GameStateManager::setcurrentLevel(const int& lvl) {
    currentLevel = lvl;
}

void GameStateManager::handleEvents(sf::RenderWindow& window, sf::Event& event) {
    if (currentState) currentState->handleEvents(window, event);
}

void GameStateManager::update(float deltaTime) {
    if (currentState) currentState->update(deltaTime);
}

void GameStateManager::draw(sf::RenderWindow& window) {
    if (currentState) currentState->draw(window);
}