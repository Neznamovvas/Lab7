#include "../../include/Game/GameNPC.h"
#include "../../include/utils/constants.h"
#include <mutex>


GameNPC::GameNPC(const std::string& name, float x, float y, int id)
    : NPC(name, x, y), id(id) {}

float GameNPC::getXSafe() const {
    std::shared_lock lock(positionMutex);
    return x;
}

float GameNPC::getYSafe() const {
    std::shared_lock lock(positionMutex);
    return y;
}

void GameNPC::setPositionSafe(float newX, float newY) {
    std::unique_lock lock(positionMutex);
    
    
    if (newX < 1) newX = 1;
    if (newX > Constants::MAP_WIDTH - 1) newX = Constants::MAP_WIDTH - 1;
    if (newY < 1) newY = 1;
    if (newY > Constants::MAP_HEIGHT - 1) newY = Constants::MAP_HEIGHT - 1;
    
    x = newX;
    y = newY;
}

int GameNPC::getMoveDistance() const {
    switch(getType()) {
        case NPCType::SQUIRREL: return Constants::MoveDistance::SQUIRREL;
        case NPCType::ELF: return Constants::MoveDistance::ELF;
        case NPCType::BANDIT: return Constants::MoveDistance::BANDIT;
        default: return 0;
    }
}

int GameNPC::getKillDistance() const {
    switch(getType()) {
        case NPCType::SQUIRREL: return Constants::KillDistance::SQUIRREL;
        case NPCType::ELF: return Constants::KillDistance::ELF;
        case NPCType::BANDIT: return Constants::KillDistance::BANDIT;
        default: return 0;
    }
}

std::unique_lock<std::shared_mutex> GameNPC::getPositionLock() {
    return std::unique_lock<std::shared_mutex>(positionMutex);
}

std::shared_lock<std::shared_mutex> GameNPC::getPositionSharedLock() const {
    return std::shared_lock<std::shared_mutex>(positionMutex);
}


GameSquirrel::GameSquirrel(const std::string& name, float x, float y, int id)
    : GameNPC(name, x, y, id) {}

NPCType GameSquirrel::getType() const { return NPCType::SQUIRREL; }

bool GameSquirrel::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::ELF;
}


GameElf::GameElf(const std::string& name, float x, float y, int id)
    : GameNPC(name, x, y, id) {}

NPCType GameElf::getType() const { return NPCType::ELF; }

bool GameElf::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::BANDIT;
}


GameBandit::GameBandit(const std::string& name, float x, float y, int id)
    : GameNPC(name, x, y, id) {}

NPCType GameBandit::getType() const { return NPCType::BANDIT; }

bool GameBandit::canKill(const std::shared_ptr<NPC>& other) const {
    return other->getType() == NPCType::SQUIRREL;
}