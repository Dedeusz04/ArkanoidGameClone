export module paddle;

import <SFML/Graphics.hpp>;
import game_object;

export class Paddle : public GameObject {
public:
    Paddle(float x, float y, float width = 100.0f);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void extend(float factor = 1.5f);
    void resetWidth();
    sf::Vector2f getVelocity() const; // Dodana metoda getVelocity
private:
    sf::RectangleShape shape;
    float speed = 500.0f;
    float defaultWidth;
    bool isExtended = false;
    float extendTimer = 0.0f;
    sf::Vector2f velocity; // Dodana zmienna do przechowywania prędkości
};