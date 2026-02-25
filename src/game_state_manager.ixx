export module game_state_manager;

import <memory>;
import <string>;
import <type_traits>;
import <SFML/Graphics.hpp>;
import <utility>;
import game_state;

export class GameStateManager {
public:
    template<typename T, typename... Args>
    void setState(Args&&... args) {
        static_assert(std::is_base_of_v<GameState, T>, "T must derive from GameState");
        currentState = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void setNickname(const std::string& nickname);
	void setcurrentLevel(const int& lvl);
	int FcurrentLevel();
    std::string getNickname() const;
    void handleEvents(sf::RenderWindow& window, sf::Event& event);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    int currentLevel;
private:
    std::unique_ptr<GameState> currentState;
    std::string currentNickname;
};