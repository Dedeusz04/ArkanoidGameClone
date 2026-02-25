export module ball;

import <SFML/Graphics.hpp>;
import game_object;
import paddle;
import audio;

export class Ball : public GameObject {
public:
    Ball(float x, float y, float initialSpeed = 300.0f, std::string texturePath = "assets/ball.png");
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOutOfBounds() const;
    void reset(float x, float y);
    void bounceFromPaddle(const Paddle& paddle, AudioManager& audioManager);
    void bounceFromBlock(AudioManager& audioManager, sf::FloatRect blockBounds, bool isIndestructible);
    void setVelocity(const sf::Vector2f& newVelocity);
    void increaseSpeed(float factor);
    float getSpeed() const;
private:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity{ 300.0f, -300.0f };
    float speed;
    bool outOfBounds = false;

    void normalizeVelocity();
};