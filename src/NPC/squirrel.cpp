#include "../../include/NPC/squirrel.h"

Squirrel::Squirrel(const std::string& name, float x, float y) 
    : NPC(name, x, y) {}

NPCType Squirrel::getType() const { 
    return NPCType::SQUIRREL; 
}

bool Squirrel::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::ELF;
}