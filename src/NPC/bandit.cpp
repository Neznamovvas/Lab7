#include "../../include/NPC/bandit.h"

Bandit::Bandit(const std::string& name, float x, float y) 
    : NPC(name, x, y) {}

NPCType Bandit::getType() const { 
    return NPCType::BANDIT; 
}

bool Bandit::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::SQUIRREL;
}