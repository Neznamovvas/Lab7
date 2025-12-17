#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include "GameNPC.h"
#include "GameNPCFactory.h"
#include "MultithreadedBattleVisitor.h"
#include "../../include/Observer/BattleObserver.h"
#include "../../include/Observer/ConsoleObserver.h"
#include "../../include/Observer/FileObserver.h"
#include "../../include/utils/constants.h"


struct BattleTask {
    std::shared_ptr<GameNPC> attacker;
    std::shared_ptr<GameNPC> defender;
    
    BattleTask(std::shared_ptr<GameNPC> a, std::shared_ptr<GameNPC> d)
        : attacker(a), defender(d) {}
};

class GameEngine {
private:
    
    std::vector<std::shared_ptr<GameNPC>> npcs;
    mutable std::shared_mutex npcsMutex;
    
    
    std::queue<BattleTask> battleQueue;
    std::mutex battleQueueMutex;
    std::condition_variable battleQueueCV;
    
    
    std::shared_ptr<ConsoleObserver> consoleObserver;
    std::shared_ptr<FileObserver> fileObserver;
    
    
    std::vector<std::thread> movementThreads;
    std::vector<std::thread> battleThreads;
    std::thread printThread;
    
    
    std::atomic<bool> gameRunning;
    std::atomic<int> aliveNPCs;
    
    
    static inline std::mutex coutMutex;
    
    
    static constexpr int MOVEMENT_THREADS = 4;
    static constexpr int BATTLE_THREADS = 2;
    
    
    std::atomic<int> nextNPCId;

    std::chrono::steady_clock::time_point gameStartTime;
    
    
    int getElapsedSeconds() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
            now - gameStartTime).count();
    }
    
public:
    GameEngine();
    ~GameEngine();
    
    void initialize();
    void start();
    void stop();
    
private:
    void createInitialNPCs();
    void movementThreadFunc();
    void battleThreadFunc();
    void printThreadFunc();
    
    void moveNPC(std::shared_ptr<GameNPC> npc);
    bool checkBattleCondition(std::shared_ptr<GameNPC> npc1, 
                             std::shared_ptr<GameNPC> npc2);
    void processBattle(BattleTask& task);
    
    void printMap();
    void printSurvivors();
    
    std::string getNPCSymbol(NPCType type);
};


#endif 