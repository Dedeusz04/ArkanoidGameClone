module highscore;

import <algorithm>;
import <fstream>;
import <sstream>;
import <stdexcept>;
import playing_state;

HighScoreManager::HighScoreManager() {
    std::filesystem::create_directory(playersDir);
    loadHighScores();
}

void HighScoreManager::addScore(const std::string& nickname, int score, int currentlevel) {
    addPlayerScore(nickname, score, currentlevel);

    auto it = std::find_if(highScores.begin(), highScores.end(),
        [&nickname](const auto& entry) { return entry.first == nickname; });

    if (it != highScores.end()) {
        if (score > it->second) {
            it->second = score;
        }
    }
    else {
        highScores.emplace_back(nickname, score);
    }

    std::sort(highScores.begin(), highScores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    if (highScores.size() > 10) {
        highScores.resize(10);
    }

    saveHighScores();
}

bool HighScoreManager::isNicknameUnique(const std::string& nickname) const {
    return !std::filesystem::exists(playersDir + "/" + nickname + ".txt");
}

std::vector<std::pair<std::string, int>> HighScoreManager::getHighScores() const {
    return highScores;
}

void HighScoreManager::loadHighScores() {
    highScores.clear();
    for (const auto& entry : std::filesystem::directory_iterator(playersDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::string nickname = entry.path().stem().string();
            auto scores = loadPlayerScores(nickname);
            if (!scores.empty()) {
                int maxScore = *std::max_element(scores.begin(), scores.end());
                highScores.emplace_back(nickname, maxScore);
            }
        }
    }

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

void HighScoreManager::saveHighScores() const {
    std::ofstream file(highScoreFile);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open highscores file for writing: " + highScoreFile);
    }
    for (const auto& entry : highScores) {
        file << entry.first << " " << entry.second << "\n";
    }
}

std::vector<int> HighScoreManager::loadPlayerScores(const std::string& nickname) const {
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

void HighScoreManager::addPlayerScore(const std::string& nickname, int score, int currentlevel) {  
   std::ofstream file(playersDir + "/" + nickname + ".txt", std::ios::app);  
   if (!file.is_open()) {  
       throw std::runtime_error("Failed to open player score file for writing: " + nickname);  
   }  
   file<< "Level " << currentlevel << ": " << score << "\n";
}