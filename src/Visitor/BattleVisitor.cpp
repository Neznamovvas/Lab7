#include "../../include/Visitor/BattleVisitor.h"
#include <algorithm>

BattleVisitor::BattleVisitor(float range) : battleRange(range) {}

void BattleVisitor::addObserver(std::shared_ptr<BattleObserver> observer) {
    observers.push_back(observer);
}

void BattleVisitor::visit(std::vector<std::shared_ptr<NPC>>& npcs) {
    std::vector<bool> killed(npcs.size(), false);
    
    // Check all NPC pairs
    for (size_t i = 0; i < npcs.size(); i++) {
        if (!npcs[i]->isAlive() || killed[i]) continue;
        
        for (size_t j = 0; j < npcs.size(); j++) {
            if (i == j) continue;
            if (!npcs[j]->isAlive() || killed[j]) continue;
            
            // Check distance
            if (npcs[i]->distanceTo(npcs[j]) <= battleRange) {
                // Check if i can kill j
                if (npcs[i]->canKill(npcs[j])) {
                    // i kills j
                    npcs[j]->kill();
                    killed[j] = true;
                    notifyObservers(npcs[i]->getName(), npcs[j]->getName());
                    
                    // If j can also kill i, they kill each other
                    if (npcs[j]->canKill(npcs[i])) {
                        npcs[i]->kill();
                        killed[i] = true;
                        notifyObservers(npcs[j]->getName(), npcs[i]->getName());
                        break;
                    }
                }
                // Check if j can kill i
                else if (npcs[j]->canKill(npcs[i])) {
                    // j kills i
                    npcs[i]->kill();
                    killed[i] = true;
                    notifyObservers(npcs[j]->getName(), npcs[i]->getName());
                    break;
                }
            }
        }
    }
    
    // Remove dead NPCs
    npcs.erase(
        std::remove_if(npcs.begin(), npcs.end(),
            [](const std::shared_ptr<NPC>& npc) { return !npc->isAlive(); }),
        npcs.end()
    );
}

void BattleVisitor::notifyObservers(const std::string& killer, const std::string& victim) {
    for (auto& observer : observers) {
        observer->onKill(killer, victim);
    }
}