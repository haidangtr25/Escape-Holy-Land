#include "score.h"
void loadMaxScore(int &maxScore) {
    std::ifstream file("max_score.txt");
    if (file.is_open()) {
        file >> maxScore;
        file.close();
    } else {
        maxScore = 0;
    }
}

void saveMaxScore(int &maxScore) {
    std::ofstream file("max_score.txt");
    if (file.is_open()) {
        file << maxScore;
        file.close();
    } else {
        std::cerr << "Failed to save max score!" << std::endl;
    }
}
