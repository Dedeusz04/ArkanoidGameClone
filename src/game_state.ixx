export module game_state;

import <SFML/Graphics.hpp>;

export class GameState {
public:
    virtual void handleEvents(sf::RenderWindow& window, sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~GameState() = default;
};