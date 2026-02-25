export module game_objects;

import <SFML/Graphics.hpp>;
import <memory>;
import <vector>;
import <ranges>;
import <string>;
import <cmath>;
import <random>;
import <iostream>;
import <stdexcept>;
import audio;

// Forward declarations
export class GameObject;
export class Paddle;
export class Ball;
export class Block;
export class NormalBlock;
export class IndestructibleBlock;
export class BonusBlock;
export class Bonus;
export class GameObjectManager;
export class CollisionSystem;
export enum class BonusType;

// Bazowa klasa dla obiektów gry (polimorfizm)
export class GameObject {
public:
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual bool isDestroyed() const { return false; }
    virtual ~GameObject() = default;
};

// Typy bonusów
export enum class BonusType {
    ExtendPaddle,
    MultiBall,
    SpeedUp
};

// Klasa dla bonusów
export class Bonus : public GameObject {
    sf::RectangleShape shape;
    BonusType type;
    bool collected = false;

public:
    Bonus(float x, float y, BonusType bonusType) : type(bonusType) {
        shape.setSize({20.0f, 20.0f});
        shape.setPosition(x, y);
        switch (type) {
        case BonusType::ExtendPaddle:
            shape.setFillColor(sf::Color::Cyan);
            break;
        case BonusType::MultiBall:
            shape.setFillColor(sf::Color::Magenta);
            break;
        case BonusType::SpeedUp:
            shape.setFillColor(sf::Color::Red);
            break;
        }
        std::cout << "Bonus created at (" << x << ", " << y << ") type: " << static_cast<int>(type) << "\n";
    }

    void update(float deltaTime) override {
        if (!collected) {
            shape.move(0, 100.0f * deltaTime);
            if (shape.getPosition().y > 600) {
                collected = true;
            }
        }
    }

    void draw(sf::RenderWindow& window) override {
        if (!collected) {
            window.draw(shape);
        }
    }

    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    bool isDestroyed() const override {
        return collected;
    }

    BonusType getType() const {
        return type;
    }

    void collect() {
        collected = true;
    }
};

// Klasa dla paddle'a
export class Paddle : public GameObject {
    sf::RectangleShape shape;
    float speed = 500.0f;
    float defaultWidth;

public:
    Paddle(float x, float y, float width = 100.0f) : defaultWidth(width) {
        shape.setSize({width, 20.0f});
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(x, y);
        std::cout << "Paddle created at (" << x << ", " << y << ") width: " << width << "\n";
    }

    void update(float deltaTime) override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && shape.getPosition().x > 0)
            shape.move(-speed * deltaTime, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && shape.getPosition().x < 800 - shape.getSize().x)
            shape.move(speed * deltaTime, 0);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    void extend(float factor = 1.5f) {
        float currentX = shape.getPosition().x;
        shape.setSize({defaultWidth * factor, 20.0f});
        shape.setPosition(currentX, shape.getPosition().y);
    }

    void resetWidth() {
        float currentX = shape.getPosition().x;
        shape.setSize({defaultWidth, 20.0f});
        shape.setPosition(currentX, shape.getPosition().y);
    }
};

// Klasa dla piłki
export class Ball : public GameObject {
    sf::CircleShape shape;
    sf::Vector2f velocity{300.0f, -300.0f};
    float speed;
    bool outOfBounds = false;
    sf::Color color;


public:
    Ball(float x, float y, float initialSpeed = 300.0f, sf::Color ballColor = sf::Color::White)
        : speed(initialSpeed), color(ballColor) {
        shape.setRadius(10.0f);
        shape.setPosition(x, y);
        shape.setFillColor(color);
        normalizeVelocity();
        std::cout << "Ball created at (" << x << ", " << y << ") speed: " << initialSpeed << "\n";
    }

    void update(float deltaTime) override {
        shape.move(velocity * deltaTime);

        auto bounds = shape.getGlobalBounds();
        if (bounds.left <= 0 || bounds.left + bounds.width >= 800) {
            velocity.x *= -1;
            normalizeVelocity();
        }
        if (bounds.top <= 0) {
            velocity.y *= -1;
            normalizeVelocity();
        }
        if (bounds.top > 600) {
            outOfBounds = true;
        }
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    bool isOutOfBounds() const { return outOfBounds; }

    void reset(float x, float y) {
        shape.setPosition(x, y);
        velocity = {300.0f, -300.0f};
        normalizeVelocity();
        outOfBounds = false;
    }

    void bounceFromPaddle(const Paddle& paddle, AudioManager& audioManager) {
        if (getBounds().intersects(paddle.getBounds())) {
            velocity.y = -std::abs(velocity.y);
            float relativeHit = (shape.getPosition().x - paddle.getBounds().left) / paddle.getBounds().width;
            velocity.x = speed * (relativeHit - 0.5f) * 2;
            normalizeVelocity();
            audioManager.playHitSound();
        }
    }

    void bounceFromBlock(AudioManager& audioManager, const sf::FloatRect& blockBounds, bool isIndestructible) {
        auto ballBounds = getBounds();
        float dx = (ballBounds.left + ballBounds.width / 2) - (blockBounds.left + blockBounds.width / 2);
        float dy = (ballBounds.top + ballBounds.height / 2) - (blockBounds.top + blockBounds.height / 2);

        bool hitHorizontal = std::abs(dx) > std::abs(dy) && ((dx > 0 && velocity.x < 0) || (dx < 0 && velocity.x > 0));
        bool hitVertical = !hitHorizontal && ((dy > 0 && velocity.y < 0) || (dy < 0 && velocity.y > 0));

        if (hitHorizontal) {
            velocity.x *= -1;
        } else if (hitVertical) {
            velocity.y *= -1;
        }

        normalizeVelocity();
        audioManager.playHitSound();
    }

    void setVelocity(const sf::Vector2f& newVelocity) {
        velocity = newVelocity;
        normalizeVelocity();
    }

    void increaseSpeed(float factor) {
        speed *= factor;
        normalizeVelocity();
    }

    float getSpeed() const { return speed; }

private:
    void normalizeVelocity() {
        float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (currentSpeed > 0) {
            velocity = velocity * (speed / currentSpeed);
        }
    }
};

// Bazowa klasa dla bloków
export class Block : public GameObject {
protected:
    sf::RectangleShape shape;
    bool destroyed = false;

public:
    Block(float x, float y, float width, float height, sf::Color color) {
        shape.setSize({width, height});
        shape.setFillColor(color);
        shape.setPosition(x, y);
    }

    void update(float deltaTime) override {}

    void draw(sf::RenderWindow& window) override {
        if (!destroyed) window.draw(shape);
    }

    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    bool isDestroyed() const override { return destroyed; }

    virtual void onHit(AudioManager& audioManager, GameObjectManager& manager) = 0;
};

// Zwykły blok (niszczalny)
export class NormalBlock : public Block {
    int hitsRemaining;

public:
    NormalBlock(float x, float y, sf::Color color = sf::Color::Green)
        : Block(x, y, 50.0f, 20.0f, color), hitsRemaining(1) {
        std::cout << "NormalBlock created at (" << x << ", " << y << ")\n";
    }

    void onHit(AudioManager& audioManager, GameObjectManager& manager) override {
        if (--hitsRemaining <= 0) {
            destroyed = true;
            audioManager.playDestroySound();
            std::cout << "NormalBlock destroyed\n";
        } else {
            audioManager.playHitSound();
        }
    }
};

// Blok niezniszczalny
export class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y)
        : Block(x, y, 50.0f, 20.0f, sf::Color(100, 100, 100)) {
        std::cout << "IndestructibleBlock created at (" << x << ", " << y << ")\n";
    }

    void onHit(AudioManager& audioManager, GameObjectManager& manager) override {
        audioManager.playHitSound();
        std::cout << "IndestructibleBlock hit (not destroyed)\n";
    }
};

// Blok bonusowy
export class BonusBlock : public Block {
public:
    BonusBlock(float x, float y)
        : Block(x, y, 50.0f, 20.0f, sf::Color::Yellow) {
        std::cout << "BonusBlock created at (" << x << ", " << y << ")\n";
    }

    void onHit(AudioManager& audioManager, GameObjectManager& manager) override {
        destroyed = true;
        audioManager.playDestroySound();
        std::cout << "BonusBlock destroyed\n";
    }
};

// Manager obiektów gry
export class GameObjectManager {
    std::vector<std::unique_ptr<GameObject>> objects;
    int score = 0;

public:
    void add(std::unique_ptr<GameObject> obj) {
        objects.push_back(std::move(obj));
    }

    void update(float deltaTime) {
        for (auto& obj : objects) {
            if (!obj->isDestroyed()) obj->update(deltaTime);
        }
    }

    void draw(sf::RenderWindow& window) {
        auto visibleObjects = objects | std::ranges::views::filter([](const auto& obj) {
            return !obj->isDestroyed() && obj->getBounds().top < 600;
        });
        for (const auto& obj : visibleObjects) {
            obj->draw(window);
        }
    }

    void reset(float paddleWidth, float ballSpeed, const std::vector<std::string>& levelLayout) {
        objects.clear();
        std::cout << "Resetting GameObjectManager...\n";
        add(std::make_unique<Paddle>(350, 550, paddleWidth));
        add(std::make_unique<Ball>(400, 300, ballSpeed));

        const float blockWidth = 50.0f;
        const float blockHeight = 20.0f;
        const float spacing = 10.0f;
        const float startY = 100.0f;

        for (size_t j = 0; j < levelLayout.size(); ++j) {
            const auto& row = levelLayout[j];
            std::cout << "Processing row " << j << ": " << row << "\n";
            if (row.empty()) {
                std::cerr << "Warning: Empty row in levelLayout at index " << j << "\n";
                continue;
            }
            float totalWidth = row.size() * (blockWidth + spacing) - spacing;
            float startX = (800.0f - totalWidth) / 2.0f;
            for (size_t i = 0; i < row.size(); ++i) {
                float x = startX + i * (blockWidth + spacing);
                float y = startY + j * (blockHeight + spacing);
                try {
                    if (row[i] == '1') {
                        add(std::make_unique<NormalBlock>(x, y));
                    } else if (row[i] == '2') {
                        add(std::make_unique<IndestructibleBlock>(x, y));
                    } else if (row[i] == '3') {
                        add(std::make_unique<BonusBlock>(x, y));
                    } else {
                        std::cerr << "Warning: Invalid block type '" << row[i] << "' at row " << j << ", col " << i << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error creating block at (" << x << ", " << y << "): " << e.what() << "\n";
                }
            }
        }
        score = 0;
        std::cout << "GameObjectManager reset completed. Total objects: " << objects.size() << "\n";
    }

    std::vector<GameObject*> getBlocks() {
        std::vector<GameObject*> blocks;
        for (auto& obj : objects) {
            if (dynamic_cast<Block*>(obj.get()) && !obj->isDestroyed()) {
                blocks.push_back(obj.get());
            }
        }
        return blocks;
    }

    std::vector<Bonus*> getBonuses() {
        std::vector<Bonus*> bonuses;
        for (auto& obj : objects) {
            if (auto bonus = dynamic_cast<Bonus*>(obj.get())) {
                if (!bonus->isDestroyed()) {
                    bonuses.push_back(bonus);
                }
            }
        }
        return bonuses;
    }

    std::vector<Ball*> getBalls() {
        std::vector<Ball*> balls;
        for (auto& obj : objects) {
            if (auto ball = dynamic_cast<Ball*>(obj.get())) {
                if (!ball->isOutOfBounds()) {
                    balls.push_back(ball);
                }
            }
        }
        return balls;
    }

    Ball* getBall() {
        for (auto& obj : objects) {
            if (auto ball = dynamic_cast<Ball*>(obj.get())) {
                return ball;
            }
        }
        throw std::runtime_error("Ball not found in GameObjectManager");
    }

    Paddle* getPaddle() {
        for (auto& obj : objects) {
            if (auto paddle = dynamic_cast<Paddle*>(obj.get())) {
                return paddle;
            }
        }
        throw std::runtime_error("Paddle not found in GameObjectManager");
    }

    void addScore(int points) { score += points; }
    int getScore() const { return score; }
};

// System kolizji
export class CollisionSystem {
    AudioManager& audioManager;

public:
    CollisionSystem(AudioManager& audio) : audioManager(audio) {}

    void checkCollisions(GameObjectManager& manager) {
        auto paddle = manager.getPaddle();
        auto blocks = manager.getBlocks();
        auto bonuses = manager.getBonuses();
        auto balls = manager.getBalls();

        // Kolizje piłek
        for (auto ball : balls) {
            ball->bounceFromPaddle(*paddle, audioManager);

            bool hasCollided = false;
            for (auto block : blocks | std::ranges::views::filter([](GameObject* obj) { return !obj->isDestroyed(); })) {
                if (!hasCollided && ball->getBounds().intersects(block->getBounds())) {
                    auto blockPtr = dynamic_cast<Block*>(block);
                    if (blockPtr) {
                        bool isIndestructible = dynamic_cast<IndestructibleBlock*>(blockPtr) != nullptr;
                        ball->bounceFromBlock(audioManager, block->getBounds(), isIndestructible);
                        blockPtr->onHit(audioManager, manager);
                        if (!isIndestructible && blockPtr->isDestroyed()) {
                            manager.addScore(100);
                            if (dynamic_cast<BonusBlock*>(blockPtr)) {
                                std::random_device rd;
                                std::mt19937 gen(rd());
                                std::uniform_int_distribution<> dis(0, 2);
                                BonusType type = static_cast<BonusType>(dis(gen));
                                auto pos = blockPtr->getBounds();
                                manager.add(std::make_unique<Bonus>(pos.left + pos.width / 2, pos.top + pos.height, type));
                                std::cout << "Bonus spawned from BonusBlock\n";
                            }
                        }
                        hasCollided = true;
                        std::cout << "Collision with " << (isIndestructible ? "IndestructibleBlock" : "NormalBlock/BonusBlock") << "\n";
                    }
                }
            }
        }

        // Kolizje bonusów
        for (auto bonus : bonuses) {
            if (bonus->getBounds().intersects(paddle->getBounds())) {
                bonus->collect();
                switch (bonus->getType()) {
                case BonusType::ExtendPaddle:
                    paddle->extend();
                    break;
                case BonusType::MultiBall: {
                    auto balls = manager.getBalls();
                    if (!balls.empty()) {
                        auto ball = balls[0];
                        manager.add(std::make_unique<Ball>(
                            ball->getBounds().left + 10.0f, ball->getBounds().top, ball->getSpeed(), sf::Color::Red));
                    }
                    break;
                }
                case BonusType::SpeedUp:
                    for (auto ball : manager.getBalls()) {
                        ball->increaseSpeed(1.2f);
                    }
                    break;
                }
                audioManager.playHitSound();
            }
        }
    }
};