#include "../../include/Game/GameEngine.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include "../../include/utils/ThreadSafeCout.h"

using namespace std::chrono_literals;

GameEngine::GameEngine() 
    : gameRunning(false), aliveNPCs(0), nextNPCId(1) {
    
    
    consoleObserver = std::make_shared<ConsoleObserver>();
    fileObserver = std::make_shared<FileObserver>();
}
GameEngine::~GameEngine() {
    stop();
}

void GameEngine::initialize() {
    createInitialNPCs();
    aliveNPCs = npcs.size();
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        SAFE_COUT << "Game initialized with " << npcs.size() << " NPCs\n";
        SAFE_COUT << "Game duration: " << Constants::GAME_DURATION << " seconds\n";
        SAFE_COUT << "Map size: " << Constants::MAP_WIDTH << "x" 
                 << Constants::MAP_HEIGHT << "\n";
    }
}

void GameEngine::createInitialNPCs() {
    std::unique_lock lock(npcsMutex);
    
    for (int i = 0; i < 50; ++i) {
        auto npc = GameNPCFactory::createRandomGameNPC(nextNPCId++);
        npcs.push_back(npc);
    }
}

void GameEngine::start() {
    gameRunning = true;
    gameStartTime = std::chrono::steady_clock::now(); 
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        SAFE_COUT << "=== STARTING GAME ===" << std::endl;
    }
    
    
    for (int i = 0; i < MOVEMENT_THREADS; ++i) {
        movementThreads.emplace_back(&GameEngine::movementThreadFunc, this);
    }
    
    
    for (int i = 0; i < BATTLE_THREADS; ++i) {
        battleThreads.emplace_back(&GameEngine::battleThreadFunc, this);
    }
    
    
    printThread = std::thread(&GameEngine::printThreadFunc, this);
    
    
    while (gameRunning && getElapsedSeconds() < Constants::GAME_DURATION) {
        std::this_thread::sleep_for(100ms);
    }
    
    stop();
    printSurvivors();
}

void GameEngine::stop() {
    gameRunning = false;
    
    
    {
        std::lock_guard<std::mutex> lock(battleQueueMutex);
        battleQueueCV.notify_all();
    }
    
    
    for (auto& thread : movementThreads) {
        if (thread.joinable()) thread.join();
    }
    
    for (auto& thread : battleThreads) {
        if (thread.joinable()) thread.join();
    }
    
    if (printThread.joinable()) printThread.join();
    
    movementThreads.clear();
    battleThreads.clear();
}

void GameEngine::movementThreadFunc() {
    while (gameRunning) {
        
        std::shared_ptr<GameNPC> currentNPC = nullptr;
        
        {
            std::shared_lock lock(npcsMutex);
            if (npcs.empty()) {
                std::this_thread::sleep_for(50ms);
                continue;
            }
            
            
            std::vector<std::shared_ptr<GameNPC>> aliveNPCsList;
            for (const auto& npc : npcs) {
                auto lock = npc->getPositionSharedLock();
                if (npc->isAlive()) {
                    aliveNPCsList.push_back(npc);
                }
            }
            
            if (aliveNPCsList.empty()) {
                std::this_thread::sleep_for(50ms);
                continue;
            }
            
            int index = Constants::randomInt(0, aliveNPCsList.size() - 1);
            currentNPC = aliveNPCsList[index];
        }
        
        if (currentNPC) {
            
            moveNPC(currentNPC);
            
            
            static const int MAX_BATTLES_PER_CYCLE = 3;
            int battlesFound = 0;
            
            std::shared_lock lock(npcsMutex);
            for (const auto& otherNPC : npcs) {
                if (battlesFound >= MAX_BATTLES_PER_CYCLE) break;
                if (otherNPC == currentNPC || !otherNPC->isAlive()) continue;
                
                if (checkBattleCondition(currentNPC, otherNPC)) {
                    
                    BattleTask task(currentNPC, otherNPC);
                    
                    {
                        std::lock_guard<std::mutex> queueLock(battleQueueMutex);
                        battleQueue.push(task);
                    }
                    battleQueueCV.notify_one();
                    battlesFound++;
                }
            }
        }
        
        std::this_thread::sleep_for(100ms);
    }
}

void GameEngine::moveNPC(std::shared_ptr<GameNPC> npc) {
    if (!npc->isAlive()) return;
    
    
    float angle = Constants::randomFloat(0.0f, 2.0f * 3.14159f);
    int distance = npc->getMoveDistance();
    
    float dx = std::cos(angle) * distance;
    float dy = std::sin(angle) * distance;
    
    float currentX = npc->getXSafe();
    float currentY = npc->getYSafe();
    
    float newX = currentX + dx;
    float newY = currentY + dy;
    
    npc->setPositionSafe(newX, newY);
}

bool GameEngine::checkBattleCondition(std::shared_ptr<GameNPC> npc1, 
                                     std::shared_ptr<GameNPC> npc2) {
    if (!npc1->isAlive() || !npc2->isAlive()) return false;
    
    
    bool can1kill2 = npc1->canKill(npc2);
    bool can2kill1 = npc2->canKill(npc1);
    
    if (!can1kill2 && !can2kill1) return false;
    
    
    float distance = npc1->distanceTo(npc2);
    int requiredDistance = std::max(npc1->getKillDistance(), npc2->getKillDistance());
    
    return distance <= requiredDistance;
}

void GameEngine::battleThreadFunc() {
    while (gameRunning) {
        BattleTask task(nullptr, nullptr);
        
        {
            std::unique_lock<std::mutex> lock(battleQueueMutex);
            
            
            battleQueueCV.wait_for(lock, 100ms, [this]() {
                return !battleQueue.empty() || !gameRunning;
            });
            
            if (!gameRunning && battleQueue.empty()) return;
            
            if (!battleQueue.empty()) {
                task = battleQueue.front();
                battleQueue.pop();
            }
        }
        
        if (task.attacker && task.defender) {
            processBattle(task);
        }
    }
}

void GameEngine::processBattle(BattleTask& task) {
    
    if (!task.attacker->tryEnterBattle() || !task.defender->tryEnterBattle()) {
        return; 
    }
    
    
    std::unique_lock<std::shared_mutex> lock1, lock2;
    
    
    if (task.attacker->getId() < task.defender->getId()) {
        lock1 = task.attacker->getPositionLock();
        lock2 = task.defender->getPositionLock();
    } else {
        lock1 = task.defender->getPositionLock();
        lock2 = task.attacker->getPositionLock();
    }
    
    
    if (!task.attacker->isAlive() || !task.defender->isAlive()) {
        task.attacker->exitBattle();
        task.defender->exitBattle();
        return;
    }
    
    
    if (!checkBattleCondition(task.attacker, task.defender)) {
        task.attacker->exitBattle();
        task.defender->exitBattle();
        return;
    }
    
    
    MultithreadedBattleVisitor visitor(100.0f, coutMutex);
    visitor.addObserver(consoleObserver);
    visitor.addObserver(fileObserver);
    visitor.visitPair(task.attacker, task.defender);
    
    
    task.attacker->exitBattle();
    task.defender->exitBattle();
}

void GameEngine::printThreadFunc() {
    auto lastPrintTime = std::chrono::steady_clock::now();
    
    while (gameRunning) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastPrintTime).count();
        
        if (elapsed >= Constants::PRINT_INTERVAL) {
            printMap();
            lastPrintTime = now;
        }
        
        std::this_thread::sleep_for(100ms);
    }
}

void GameEngine::printMap() {
    std::lock_guard<std::mutex> lock(coutMutex);
    
    int aliveCount = 0;
    std::vector<std::tuple<float, float, std::string>> npcInfo;
    
    {
        std::shared_lock lock(npcsMutex);
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                aliveCount++;
                std::string type;
                switch(npc->getType()) {
                    case NPCType::SQUIRREL: type = "Squirrel"; break;
                    case NPCType::ELF: type = "Elf"; break;
                    case NPCType::BANDIT: type = "Bandit"; break;
                    default: type = "Unknown";
                }
                npcInfo.emplace_back(npc->getXSafe(), npc->getYSafe(), type);
            }
        }
    }
    
    SAFE_COUT << "=== DUNGEON SIMULATOR ===" << std::endl;
    SAFE_COUT << "Time: " << getElapsedSeconds() << "/" 
              << Constants::GAME_DURATION << " seconds" << std::endl;
    SAFE_COUT << "Alive NPCs: " << aliveCount << std::endl;
    SAFE_COUT << "========================\n" << std::endl;
    
    
    SAFE_COUT << "NPC POSITIONS:" << std::endl;
    SAFE_COUT << std::string(50, '-') << std::endl;
    
    for (const auto& [x, y, type] : npcInfo) {
        
        int posX = static_cast<int>(x / 2); 
        std::string position(50, ' ');
        
        SAFE_COUT << std::left << std::setw(10) << type 
                 << " [" << std::setw(5) << std::fixed << std::setprecision(1) << x
                 << "," << std::setw(5) << y << "] " 
                 << position << std::endl;
    }
    
    SAFE_COUT << std::string(50, '-') << std::endl;
}

std::string GameEngine::getNPCSymbol(NPCType type) {
    switch(type) {
        case NPCType::SQUIRREL: return "S";
        case NPCType::ELF: return "E";
        case NPCType::BANDIT: return "B";
        default: return "?";
    }
}

void GameEngine::printSurvivors() {
    std::lock_guard<std::mutex> lock(coutMutex);
    
    SAFE_COUT << "\n=== GAME OVER ===" << std::endl;
    
    std::vector<std::shared_ptr<GameNPC>> survivors;
    {
        std::shared_lock npcsLock(npcsMutex);
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                survivors.push_back(npc);
            }
        }
    }
    
    SAFE_COUT << "Survivors (" << survivors.size() << "):" << std::endl;
    
    for (const auto& npc : survivors) {
        std::string typeStr;
        switch(npc->getType()) {
            case NPCType::SQUIRREL: typeStr = "squirrel"; break;
            case NPCType::ELF: typeStr = "elf"; break;
            case NPCType::BANDIT: typeStr = "bandit"; break;
        }
        
        SAFE_COUT << "  " << npc->getName() << "(" 
                 << typeStr << ") at (" 
                 << std::fixed << std::setprecision(1) 
                 << npc->getXSafe() << ", " << npc->getYSafe() << ")" << std::endl;
    }
    
    SAFE_COUT << "\n=== STATISTICS ===" << std::endl;
    SAFE_COUT << "Total NPCs: " << npcs.size() << std::endl;
    SAFE_COUT << "Survivors: " << survivors.size() << std::endl;
    SAFE_COUT << "Killed: " << (npcs.size() - survivors.size()) << std::endl;
}