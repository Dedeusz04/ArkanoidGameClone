export module highscore;

import <vector>;
import <algorithm>;
import <fstream>;
import <string>;
import <sstream>;
import <filesystem>;
import <stdexcept>;

export class HighScoreManager {
    std::vector<std::pair<std::string, int>> highScores;
    const std::string highScoreFile = "assets/highscores.txt";
    const std::string playersDir = "players";

public:
    HighScoreManager() {
        // Create players directory if it doesn't exist
        std::filesystem::create_directory(playersDir);
        loadHighScores();
    }

    void addScore(const std::string& nickname, int score) {
        // Add score to player's individual file
        addPlayerScore(nickname, score);

        // Update high scores list
        auto it = std::find_if(highScores.begin(), highScores.end(),
            [&nickname](const auto& entry) { return entry.first == nickname; });

        if (it != highScores.end()) {
            // Update existing score if new score is higher
            if (score > it->second) {
                it->second = score;
            }
        }
        else {
            // Add new score
            highScores.emplace_back(nickname, score);
        }

        // Sort and limit to top 10
        std::sort(highScores.begin(), highScores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        if (highScores.size() > 10) {
            highScores.resize(10);
        }

        saveHighScores();
    }

    bool isNicknameUnique(const std::string& nickname) const {
        return !std::filesystem::exists(playersDir + "/" + nickname + ".txt");
    }

    std::vector<std::pair<std::string, int>> getHighScores() const {
        return highScores;
    }

private:
    void loadHighScores() {
        highScores.clear();
        // Load scores from player files
        for (const auto& entry : std::filesystem::directory_iterator(playersDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string nickname = entry.path().stem().string();
                auto scores = loadPlayerScores(nickname);
                if (!scores.empty()) {
                    // Use the highest score for highscores.txt
                    int maxScore = *std::max_element(scores.begin(), scores.end());
                    highScores.emplace_back(nickname, maxScore);
                }
            }
        }

        // Load existing highscores.txt for consistency
        std::ifstream file(highScoreFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string nickname;
                int score;
                if (iss >> nickname >> score) {
                    auto it = std::find_if(highScores.begin(), highScores.end(),
                        [&nickname](const auto& entry) { return entry.first == nickname; });
                    if (it == highScores.end()) {
                        highScores.emplace_back(nickname, score);
                    }
                    else if (score > it->second) {
                        it->second = score;
                    }
                }
            }
        }

        std::sort(highScores.begin(), highScores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        if (highScores.size() > 10) {
            highScores.resize(10);
        }
    }

    void saveHighScores() const {
        std::ofstream file(highScoreFile);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open highscores file for writing: " + highScoreFile);
        }
        for (const auto& entry : highScores) {
            file << entry.first << " " << entry.second << "\n";
        }
    }

    std::vector<int> loadPlayerScores(const std::string& nickname) const {
        std::vector<int> scores;
        std::ifstream file(playersDir + "/" + nickname + ".txt");
        if (file.is_open()) {
            int score;
            while (file >> score) {
                scores.push_back(score);
            }
        }
        return scores;
    }

    void addPlayerScore(const std::string& nickname, int score) {
        std::ofstream file(playersDir + "/" + nickname + ".txt", std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open player score file for writing: " + nickname);
        }
        file << score << "\n";
    }
};