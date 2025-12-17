#include <iostream>
#include "./include/Game/GameEngine.h"
#include "./include/utils/ThreadSafeCOut.h"

int main() {
    try {
        
        SAFE_COUT << "=== MULTITHREADED DUNGEON SIMULATOR ===" << std::endl;
        SAFE_COUT << "Starting simulation..." << std::endl;
        
        GameEngine engine;
        engine.initialize();
        engine.start();
        
        SAFE_COUT << "\nSimulation completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}