module game_object_manager;

import <stdexcept>;
import <iostream>;

void GameObjectManager::add(std::unique_ptr<GameObject> obj) {
    objects.push_back(std::move(obj));
}

void GameObjectManager::update(float deltaTime) {
    for (auto& obj : objects) {
        if (!obj->isDestroyed()) obj->update(deltaTime);
    }
}

void GameObjectManager::draw(sf::RenderWindow& window) {
    auto visibleObjects = objects | std::ranges::views::filter([](const auto& obj) {
        return !obj->isDestroyed() && obj->getBounds().top < 600;
        });
    for (const auto& obj : visibleObjects) {
        obj->draw(window);
    }
}

void GameObjectManager::reset(float paddleWidth, float ballSpeed, const std::vector<std::string>& levelLayout) {
    objects.clear();
    std::cout << "Resetting GameObjectManager...\n";
    add(std::make_unique<Paddle>(450, 575, paddleWidth));
    add(std::make_unique<Ball>(400, 300, ballSpeed)); // Usunięto ballColor

    const float blockWidth = 50.0f;
    const float blockHeight = 20.0f;
    const float spacing = 8.0f;
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
                }
                else if (row[i] == '2') {
                    add(std::make_unique<IndestructibleBlock>(x, y));
                }
                else if (row[i] == '3') {
                    add(std::make_unique<BonusBlock>(x, y));
                }
                else {
                    std::cerr << "Warning: Invalid block type '" << row[i] << "' at row " << j << ", col " << i << "\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error creating block at (" << x << ", " << y << "): " << e.what() << "\n";
            }
        }
    }
    score = 0;
    std::cout << "GameObjectManager reset completed. Total objects: " << objects.size() << "\n";
}

std::vector<GameObject*> GameObjectManager::getBlocks() {
    std::vector<GameObject*> blocks;
    for (auto& obj : objects) {
        if (dynamic_cast<Block*>(obj.get()) && !obj->isDestroyed()) {
            blocks.push_back(obj.get());
        }
    }
    return blocks;
}

std::vector<Bonus*> GameObjectManager::getBonuses() {
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

std::vector<Ball*> GameObjectManager::getBalls() {
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

Ball* GameObjectManager::getBall() {
    for (auto& obj : objects) {
        if (auto ball = dynamic_cast<Ball*>(obj.get())) {
            return ball;
        }
    }
    throw std::runtime_error("Ball not found in GameObjectManager");
}

Paddle* GameObjectManager::getPaddle() {
    for (auto& obj : objects) {
        if (auto paddle = dynamic_cast<Paddle*>(obj.get())) {
            return paddle;
        }
    }
    throw std::runtime_error("Paddle not found in GameObjectManager");
}

void GameObjectManager::addScore(int points) {
    score += points;
}

int GameObjectManager::getScore() const {
    return score;
}

bool GameObjectManager::allDestructibleBlocksDestroyed() const {
    for (const auto& obj : objects) {
        if (auto block = dynamic_cast<Block*>(obj.get())) {
            if (!dynamic_cast<IndestructibleBlock*>(block) && !block->isDestroyed()) {
                return false;
            }
        }
    }
    return true;
}