#include "../../include/Game/MultithreadedBattleVisitor.h"
#include <iostream>
#include <sstream>

MultithreadedBattleVisitor::MultithreadedBattleVisitor(float range, std::mutex& mutex)
    : BattleVisitor(range), outputMutex(mutex) {}

void MultithreadedBattleVisitor::processDiceBattle(std::shared_ptr<NPC> attacker,
                                                  std::shared_ptr<NPC> defender) {
    if (!attacker->isAlive() || !defender->isAlive()) return;
    
    
    std::string attackerType;
    std::string defenderType;
    
    switch(attacker->getType()) {
        case NPCType::SQUIRREL: attackerType = "squirrel"; break;
        case NPCType::ELF: attackerType = "elf"; break;
        case NPCType::BANDIT: attackerType = "bandit"; break;
    }
    
    switch(defender->getType()) {
        case NPCType::SQUIRREL: defenderType = "squirrel"; break;
        case NPCType::ELF: defenderType = "elf"; break;
        case NPCType::BANDIT: defenderType = "bandit"; break;
    }
    
    
    int attackerAttack = Dice::attackRoll();
    int defenderDefense = Dice::defenseRoll();
    int defenderAttack = defender->canKill(attacker) ? Dice::attackRoll() : 0;
    int attackerDefense = attacker->canKill(defender) ? Dice::defenseRoll() : 0;
    
    std::string battleLog;
    bool attackerKilled = false;
    bool defenderKilled = false;
    
    
    if (attacker->canKill(defender) && attackerAttack > defenderDefense) {
        defender->kill();
        defenderKilled = true;
        
        battleLog = attacker->getName() + "(" + attackerType +
                   ") killed " + defender->getName() + " with attack " + 
                   std::to_string(attackerAttack) + " > defense " + 
                   std::to_string(defenderDefense);
    }
    
    
    if (defender->canKill(attacker) && defender->isAlive() && 
        defenderAttack > attackerDefense) {
        attacker->kill();
        attackerKilled = true;
        
        if (!battleLog.empty()) battleLog += " and ";
        battleLog += defender->getName() + "(" + defenderType +
                    ") killed " + attacker->getName() + " with attack " + 
                    std::to_string(defenderAttack) + " > defense " + 
                    std::to_string(attackerDefense);
    }
    
    
    if (attackerKilled) {
        notifyObservers(defender->getName(), attacker->getName());
    }
    if (defenderKilled) {
        notifyObservers(attacker->getName(), defender->getName());
    }
    
    
    if (!attackerKilled && !defenderKilled && 
        (attacker->canKill(defender) || defender->canKill(attacker))) {
        battleLog = attacker->getName() + " and " + defender->getName() + 
                   " fought but both survived (attack <= defense)";
    }

    if (battleLog.find("killed") == std::string::npos) {
        
        static std::chrono::steady_clock::time_point lastNoKillLog = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNoKillLog).count();
        
        if (elapsed < 500) { 
            return;
        }
        lastNoKillLog = now;
    }
    
    
    if (!battleLog.empty()) {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[BATTLE] " << battleLog << std::endl;
    }
}

void MultithreadedBattleVisitor::visitPair(std::shared_ptr<NPC> npc1, 
                                          std::shared_ptr<NPC> npc2) {
    
    if (!npc1->isAlive() || !npc2->isAlive()) return;
    
    
    float distance = npc1->distanceTo(npc2);
    if (distance > battleRange) return;
    
    
    bool can1kill2 = npc1->canKill(npc2);
    bool can2kill1 = npc2->canKill(npc1);
    
    if (!can1kill2 && !can2kill1) return;
    
    
    processDiceBattle(npc1, npc2);
}

void MultithreadedBattleVisitor::visitPair(std::shared_ptr<GameNPC> npc1,
                                          std::shared_ptr<GameNPC> npc2) {
    
    visitPair(std::static_pointer_cast<NPC>(npc1), 
              std::static_pointer_cast<NPC>(npc2));
}