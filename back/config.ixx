export module config;

import <string>;
import <vector>;
import <fstream>;
import <stdexcept>;
import <regex>;
import <sstream>;

export struct LevelConfig {
    float paddleWidth;
    float ballSpeed;
    std::vector<std::string> levelLayout; // Układ planszy
};

export LevelConfig loadLevelConfig(const std::string& filename) {
    LevelConfig config{ 100.0f, 300.0f, {} }; // Domyślne wartości
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open level file: " + filename);
    }

    std::vector<std::string> layout;
    std::string line;
    std::regex paramPattern(R"(^(paddleWidth|ballSpeed):\s*(\d+(\.\d+)?)\s*$)");
    std::regex blockPattern("^[0123]+$"); // Dopuszcza tylko 0, 1, 2
    size_t rowLength = 0;
    bool readingLayout = false;

    while (std::getline(file, line)) {
        // Usuń białe znaki z początku i końca
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Pomiń puste linie
        if (line.empty()) continue;

        // Sprawdź, czy linia to parametr (paddleWidth lub ballSpeed)
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

        // Sprawdź, czy linia to układ planszy
        if (std::regex_match(line, blockPattern)) {
            readingLayout = true;
            // Sprawdź spójność długości wierszy
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