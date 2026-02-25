module config;

import <fstream>;
import <stdexcept>;
import <regex>;
import <sstream>;

LevelConfig loadLevelConfig(const std::string& filename) {
    LevelConfig config{ 100.0f, 300.0f, {} };
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open level file: " + filename);
    }

    std::vector<std::string> layout;
    std::string line;
    std::regex paramPattern(R"(^(paddleWidth|ballSpeed):\s*(\d+(\.\d+)?)\s*$)");
    std::regex blockPattern("^[0123]+$");
    size_t rowLength = 0;
    bool readingLayout = false;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        std::smatch match;
        if (std::regex_match(line, match, paramPattern)) {
            if (readingLayout) {
                throw std::runtime_error("Parameters must be defined before level layout in: " + filename);
            }
            std::string paramName = match[1].str();
            float value = std::stof(match[2].str());
            if (paramName == "paddleWidth") {
                config.paddleWidth = value;
            }
            else if (paramName == "ballSpeed") {
                config.ballSpeed = value;
            }
            continue;
        }

        if (std::regex_match(line, blockPattern)) {
            readingLayout = true;
            if (rowLength == 0) {
                rowLength = line.size();
            }
            else if (line.size() != rowLength) {
                throw std::runtime_error("Inconsistent row length in level file: " + line);
            }
            layout.push_back(line);
            continue;
        }

        throw std::runtime_error("Invalid line in level file: " + line);
    }

    if (layout.empty()) {
        throw std::runtime_error("Level layout is empty in: " + filename);
    }

    config.levelLayout = layout;
    return config;
}