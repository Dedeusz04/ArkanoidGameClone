export module collision_system;

import <SFML/Graphics.hpp>;
import game_object;
import paddle;
import ball;
import block;
import bonus;
import game_object_manager;
import audio;

export class CollisionSystem {
public:
    CollisionSystem(AudioManager& audio);
    void checkCollisions(GameObjectManager& manager);
private:
    AudioManager& audioManager;
};