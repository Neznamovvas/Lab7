#ifndef BANDIT_H
#define BANDIT_H

#include "NPC.h"

class Bandit : public NPC {
public:
    Bandit(const std::string& name, float x, float y);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

#endif