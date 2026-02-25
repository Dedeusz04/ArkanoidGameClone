export module highscore;

import <vector>;
import <string>;
import <filesystem>;
import game_state_manager;

export class HighScoreManager {
public:
    HighScoreManager();
    void addScore(const std::string& nickname, int score,int currentlevel);
    bool isNicknameUnique(const std::string& nickname) const;
    std::vector<std::pair<std::string, int>> getHighScores() const;
private:
    std::vector<std::pair<std::string, int>> highScores;
    const std::string highScoreFile = "assets/highscores.txt";
    const std::string playersDir = "players";

    void loadHighScores();
    void saveHighScores() const;
    std::vector<int> loadPlayerScores(const std::string& nickname) const;
    void addPlayerScore(const std::string& nickname, int score, int currentlevel);
};