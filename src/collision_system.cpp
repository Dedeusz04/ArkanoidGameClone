module collision_system;

import <random>;
import <iostream>;
import <ranges>;
import <limits>;

CollisionSystem::CollisionSystem(AudioManager& audio) : audioManager(audio) {}

void CollisionSystem::checkCollisions(GameObjectManager& manager) {
    auto paddle = manager.getPaddle();
    auto blocks = manager.getBlocks();
    auto bonuses = manager.getBonuses();
    auto balls = manager.getBalls();

    for (auto ball : balls) {
        // Kolizja z paletką
        ball->bounceFromPaddle(*paddle, audioManager);

        // Kolizja z blokami
        float minOverlap = std::numeric_limits<float>::max();
        Block* closestBlock = nullptr;
        sf::FloatRect ballBounds = ball->getBounds();

        for (auto block : blocks | std::ranges::views::filter([](GameObject* obj) { return !obj->isDestroyed(); })) {
            if (ballBounds.intersects(block->getBounds())) {
                auto blockPtr = dynamic_cast<Block*>(block);
                if (blockPtr) {
                    sf::FloatRect blockBounds = block->getBounds();
                    float overlapLeft = ballBounds.left + ballBounds.width - blockBounds.left;
                    float overlapRight = blockBounds.left + blockBounds.width - ballBounds.left;
                    float overlapTop = ballBounds.top + ballBounds.height - blockBounds.top;
                    float overlapBottom = blockBounds.top + blockBounds.height - ballBounds.top;
                    float overlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });
                    if (overlap < minOverlap) {
                        minOverlap = overlap;
                        closestBlock = blockPtr;
                    }
                }
            }
        }

        if (closestBlock) {
            bool isIndestructible = dynamic_cast<IndestructibleBlock*>(closestBlock) != nullptr;
            ball->bounceFromBlock(audioManager, closestBlock->getBounds(), isIndestructible);
            bool destroyed = closestBlock->hit();
            if (destroyed) {
                audioManager.playDestroySound();
                manager.addScore(100);
                if (closestBlock->isBonusBlock()) {
                    manager.addScore(500);
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, 2);
                    BonusType type = static_cast<BonusType>(dis(gen));
                    auto pos = closestBlock->getBounds();
                    manager.add(std::make_unique<Bonus>(pos.left + pos.width / 2, pos.top + pos.height, type));
                    std::cout << "Bonus spawned from BonusBlock\n";
                }
            }
            else {
                audioManager.playHitSound();
            }
            std::cout << "Collision with " << (isIndestructible ? "IndestructibleBlock" : "NormalBlock/BonusBlock")
                << " at (" << closestBlock->getBounds().left << ", " << closestBlock->getBounds().top << ")\n";
        }
    }

    // Kolizje z bonusami
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
                        ball->getBounds().left + 10.0f, ball->getBounds().top, ball->getSpeed(), "assets/bonus_ball.png")); // Usunięto ballColor
                }
                break;
            }
            case BonusType::SpeedUp:
                for (auto ball : balls) {
                    ball->increaseSpeed(1.2f);
                }
                break;
            }
            audioManager.playHitSound();
        }
    }
}