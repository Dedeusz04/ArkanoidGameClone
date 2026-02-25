module block;

import <iostream>;

Block::Block(float x, float y, float width, float height, sf::Color color) {
    shape.setSize({ width, height });
    shape.setFillColor(color);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setPosition(x, y);
}

void Block::update(float deltaTime) {}

void Block::draw(sf::RenderWindow& window) {
    if (!destroyed) window.draw(shape);
}

sf::FloatRect Block::getBounds() const {
    return shape.getGlobalBounds();
}

bool Block::isDestroyed() const {
    return destroyed;
}

NormalBlock::NormalBlock(float x, float y, sf::Color color)
    : Block(x, y, 50.0f, 20.0f, color), hitsRemaining(1) {
    std::cout << "NormalBlock created at (" << x << ", " << y << ")\n";
}

bool NormalBlock::hit() {
    if (--hitsRemaining <= 0) {
        destroyed = true;
        std::cout << "NormalBlock destroyed\n";
        return true;
    }
    return false;
}

IndestructibleBlock::IndestructibleBlock(float x, float y)
    : Block(x, y, 50.0f, 20.0f, sf::Color(100, 100, 100)) {
    std::cout << "IndestructibleBlock created at (" << x << ", " << y << ")\n";
}

bool IndestructibleBlock::hit() {
    std::cout << "IndestructibleBlock hit (not destroyed)\n";
    return false;
}

BonusBlock::BonusBlock(float x, float y)
    : Block(x, y, 50.0f, 20.0f, sf::Color::Yellow) {
    std::cout << "BonusBlock created at (" << x << ", " << y << ")\n";
}

bool BonusBlock::hit() {
    destroyed = true;
    std::cout << "BonusBlock destroyed\n";
    return true;
}

bool BonusBlock::isBonusBlock() const {
    return true;
}