export module bonus;

import <SFML/Graphics.hpp>;
import game_object;

export enum class BonusType {
    ExtendPaddle,
    MultiBall,
    SpeedUp
};

export class Bonus : public GameObject {
public:
    Bonus(float x, float y, BonusType bonusType);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isDestroyed() const override;
    BonusType getType() const;
    void collect();
private:
    sf::RectangleShape shape;
    BonusType type;
    bool collected = false;
};