#ifndef ELF_H
#define ELF_H

#include "NPC.h"

class Elf : public NPC {
public:
    Elf(const std::string& name, float x, float y);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

#endif