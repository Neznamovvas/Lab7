#ifndef SQUIRREL_H
#define SQUIRREL_H

#include "NPC.h"

class Squirrel : public NPC {
public:
    Squirrel(const std::string& name, float x, float y);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

#endif