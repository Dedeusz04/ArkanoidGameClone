module bonus;

import <iostream>;

Bonus::Bonus(float x, float y, BonusType bonusType) : type(bonusType) {
    shape.setSize({20.0f, 20.0f});
    shape.setPosition(x, y);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);
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

void Bonus::update(float deltaTime) {
    if (!collected) {
        shape.move(0, 100.0f * deltaTime);
        if (shape.getPosition().y > 600) {
            collected = true;
        }
    }
}

void Bonus::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(shape);
    }
}

sf::FloatRect Bonus::getBounds() const {
    return shape.getGlobalBounds();
}

bool Bonus::isDestroyed() const {
    return collected;
}

BonusType Bonus::getType() const {
    return type;
}

void Bonus::collect() {
    collected = true;
}