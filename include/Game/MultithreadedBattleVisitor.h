#ifndef MULTITHREADEDBATTLEVISITOR_H
#define MULTITHREADEDBATTLEVISITOR_H

#include "dice.h"
#include "GameNPC.h"
#include "../Visitor/BattleVisitor.h"
#include <mutex>
#include <memory>

class MultithreadedBattleVisitor : public BattleVisitor {
private:
    std::mutex& outputMutex;
    
    void processDiceBattle(std::shared_ptr<NPC> attacker,
                          std::shared_ptr<NPC> defender);
    
public:
    MultithreadedBattleVisitor(float range, std::mutex& mutex);

    void visitPair(std::shared_ptr<NPC> npc1, std::shared_ptr<NPC> npc2);
    void visitPair(std::shared_ptr<GameNPC> npc1, std::shared_ptr<GameNPC> npc2);
    
};

#endif
