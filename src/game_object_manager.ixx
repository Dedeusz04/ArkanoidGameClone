export module game_object_manager;

import <memory>;
import <vector>;
import <ranges>;
import <string>;
import <SFML/Graphics.hpp>;
import game_object;
import paddle;
import ball;
import block;
import bonus;

export class GameObjectManager {
public:
	void add(std::unique_ptr<GameObject> obj);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window);
	void reset(float paddleWidth, float ballSpeed, const std::vector<std::string>& levelLayout);
	std::vector<GameObject*> getBlocks();
	std::vector<Bonus*> getBonuses();
	std::vector<Ball*> getBalls();
	Ball* getBall();
	Paddle* getPaddle();
	void addScore(int points);
	int getScore() const;
	bool allDestructibleBlocksDestroyed() const; // Added for level completion
private:
	std::vector<std::unique_ptr<GameObject>> objects;
	int score = 0;
};