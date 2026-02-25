export module config;

import <string>;
import <vector>;

export struct LevelConfig {
    float paddleWidth;
    float ballSpeed;
    std::vector<std::string> levelLayout;
};

export LevelConfig loadLevelConfig(const std::string& filename);