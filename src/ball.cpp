module ball;

import <iostream>;
import <cmath>;
import <stdexcept>;

Ball::Ball(float x, float y, float initialSpeed, std::string texturePath)
    : speed(initialSpeed) {
    if (!texture.loadFromFile(texturePath)) {
        throw std::runtime_error("Failed to load texture: assets/ball.png");
    }
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    sprite.setScale(24.0f / spriteBounds.width, 24.0f / spriteBounds.height);
    sprite.setOrigin(spriteBounds.width / 2, spriteBounds.height / 2);
    normalizeVelocity();
    std::cout << "Ball created at (" << x << ", " << y << ") speed: " << initialSpeed << "\n";
}

void Ball::update(float deltaTime) {
    sf::Vector2f nextPos = sprite.getPosition() + velocity * deltaTime;

    // Sprawdź kolizję z bokami
    if (nextPos.x - getBounds().width / 2 <= 0 || nextPos.x + getBounds().width / 2 >= 800) {
        velocity.x *= -1;
    }

    // Sprawdź kolizję z górą
    if (nextPos.y - getBounds().height / 2 <= 0) {
        velocity.y *= -1;
    }

    // Sprawdź czy piłka spadła na dół
    if (nextPos.y - getBounds().height / 2 > 600) {
        outOfBounds = true;
        return;
    }

    // Teraz dopiero przesuń
    sprite.move(velocity * deltaTime);
}



void Ball::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Ball::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Ball::isOutOfBounds() const {
    return outOfBounds;
}

void Ball::reset(float x, float y) {
    sprite.setPosition(x, y);
    velocity = { 300.0f, -300.0f };
    normalizeVelocity();
    outOfBounds = false;
}

void Ball::bounceFromPaddle(const Paddle& paddle, AudioManager& audioManager) {
    if (getBounds().intersects(paddle.getBounds())) {
        // Odbij pionowo
        velocity.y = -std::abs(velocity.y);

        // Minimalny efekt kierunku odbicia
        float hitX = sprite.getPosition().x - paddle.getBounds().left;
        float hitRatio = hitX / paddle.getBounds().width;
        velocity.x = (hitRatio - 0.5f) * speed * 1.5f;

        normalizeVelocity();
        audioManager.playHitSound();

        // Przesuń piłkę ponad paletkę (zapobiega "klejeniu")
        sprite.setPosition(sprite.getPosition().x, paddle.getBounds().top - getBounds().height / 2 - 0.5);
    }
}


void Ball::bounceFromBlock(AudioManager& audioManager, sf::FloatRect blockBounds, bool isIndestructible) {
    float dx = (getBounds().left + getBounds().width / 2) - (blockBounds.left + blockBounds.width / 2);
    float dy = (getBounds().top + getBounds().height / 2) - (blockBounds.top + blockBounds.height / 2);

    if ((dx < 0 && velocity.x > 0) || (dx > 0 && velocity.x < 0)) {
        velocity.x *= -1;
    }
    if ((dy < 0 && velocity.y > 0) || (dy > 0 && velocity.y < 0)) {
        velocity.y *= -1;
    }

    normalizeVelocity();
    audioManager.playHitSound();
    std::cout << "Ball bounced: dx=" << dx << ", dy=" << dy << ", velocity=(" << velocity.x << ", " << velocity.y << ")\n";
}

void Ball::setVelocity(const sf::Vector2f& newVelocity) {
    velocity = newVelocity;
    normalizeVelocity();
}

void Ball::increaseSpeed(float factor) {
    speed *= factor;
    normalizeVelocity();
}

float Ball::getSpeed() const {
    return speed;
}

void Ball::normalizeVelocity() {
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > 0) {
        velocity = velocity * (speed / currentSpeed);
    }
}