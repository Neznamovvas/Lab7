#ifndef BATTLEVISITOR_H
#define BATTLEVISITOR_H

#include <memory>
#include <vector>
#include "../Observer/BattleObserver.h"
#include "../NPC/NPC.h"

class BattleVisitor {
protected:
    float battleRange;
    std::vector<std::shared_ptr<BattleObserver>> observers;
    
    void notifyObservers(const std::string& killer, const std::string& victim);
    
public:
    BattleVisitor(float range);
    
    void addObserver(std::shared_ptr<BattleObserver> observer);
    void visit(std::vector<std::shared_ptr<NPC>>& npcs);
};

#endif 