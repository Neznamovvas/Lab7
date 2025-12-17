#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <random>
#include <chrono>
#include <thread>

namespace Constants {
    
    constexpr int MAP_WIDTH = 100;
    constexpr int MAP_HEIGHT = 100;
    
    
    struct MoveDistance {
        static constexpr int SQUIRREL = 5;
        static constexpr int ELF = 10;
        static constexpr int BANDIT = 10;
    };
    
    struct KillDistance {
        static constexpr int SQUIRREL = 5;
        static constexpr int ELF = 50;
        static constexpr int BANDIT = 10;
    };
    
    
    constexpr int GAME_DURATION = 30;
    constexpr int PRINT_INTERVAL = 1; 
    
    
    static thread_local std::mt19937 random_generator{
        static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count() ^ 
            std::hash<std::thread::id>{}(std::this_thread::get_id())
        )
    };
    
    static int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(random_generator);
    }
    
    static float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(random_generator);
    }
}

#endif 
