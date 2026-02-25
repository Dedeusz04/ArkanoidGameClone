module paddle;

import <iostream>;

Paddle::Paddle(float x, float y, float width) : defaultWidth(width) {
    shape.setSize({ width, 10.0f });
    shape.setFillColor(sf::Color::Blue);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);
    shape.setPosition(x, y);
    velocity = { 0.0f, 0.0f }; // Inicjalizacja prędkości
    std::cout << "Paddle created at (" << x << ", " << y << ") width: " << width << "\n";
}

void Paddle::update(float deltaTime) {
    // Reset prędkości w każdej klatce
    velocity = { 0.0f, 0.0f };

    // Aktualizacja pozycji i prędkości na podstawie klawiszy
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && shape.getPosition().x > 0) {
        shape.move(-speed * deltaTime, 0);
        velocity.x = -speed; // Ustaw prędkość w lewo
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && shape.getPosition().x < 800 - shape.getSize().x) {
        shape.move(speed * deltaTime, 0);
        velocity.x = speed; // Ustaw prędkość w prawo
    }

    // Obsługa timera wydłużenia
    if (isExtended) {
        extendTimer -= deltaTime;
        if (extendTimer <= 0.0f) {
            resetWidth();
            isExtended = false;
            std::cout << "Paddle width reset after bonus expired\n";
        }
    }
}

void Paddle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Paddle::getBounds() const {
    return shape.getGlobalBounds();
}

void Paddle::extend(float factor) {
    float currentX = shape.getPosition().x;
    shape.setSize({ defaultWidth * factor, 10.0f });
    shape.setPosition(currentX, shape.getPosition().y);
    isExtended = true;
    extendTimer = 10.0f; // Ustaw timer na 10 sekund
    std::cout << "Paddle extended with factor: " << factor << " for 10 seconds\n";
}

void Paddle::resetWidth() {
    float currentX = shape.getPosition().x;
    shape.setSize({ defaultWidth, 10.0f });
    shape.setPosition(currentX, shape.getPosition().y);
}

sf::Vector2f Paddle::getVelocity() const {
    return velocity;
}