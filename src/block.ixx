export module block;

import <SFML/Graphics.hpp>;
import game_object;

export class Block : public GameObject {
public:
    Block(float x, float y, float width, float height, sf::Color color);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isDestroyed() const override;
    virtual bool hit() = 0;
    virtual bool isBonusBlock() const { return false; }
protected:
    sf::RectangleShape shape;
    bool destroyed = false;
};

export class NormalBlock : public Block {
public:
    NormalBlock(float x, float y, sf::Color color = sf::Color::Green);
    bool hit() override;
private:
    int hitsRemaining;
};

export class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y);
    bool hit() override;
};

export class BonusBlock : public Block {
public:
    BonusBlock(float x, float y);
    bool hit() override;
    bool isBonusBlock() const override;
};