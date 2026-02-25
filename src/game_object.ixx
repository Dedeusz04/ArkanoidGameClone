export module game_object;

import <SFML/Graphics.hpp>;

export class GameObject {
public:
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual bool isDestroyed() const { return false; }
    virtual ~GameObject() = default;
};