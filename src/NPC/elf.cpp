#include "../../include/NPC/elf.h"

Elf::Elf(const std::string& name, float x, float y) 
    : NPC(name, x, y) {}

NPCType Elf::getType() const { 
    return NPCType::ELF; 
}

bool Elf::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::BANDIT;
}