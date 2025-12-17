#ifndef GAMENPC_H
#define GAMENPC_H

#include "../NPC/NPC.h"
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <memory>

class GameNPC : public NPC {
protected:
    mutable std::shared_mutex positionMutex;
    int id;
    std::atomic<bool> inBattle{false};
    
public:
    bool tryEnterBattle() {
        bool expected = false;
        return inBattle.compare_exchange_strong(expected, true);
    }
    
    void exitBattle() {
        inBattle.store(false);
    }

    GameNPC(const std::string& name, float x, float y, int id);
    
    int getId() const { return id; }
    
    float getXSafe() const;
    float getYSafe() const;
    void setPositionSafe(float newX, float newY);
    
    
    int getMoveDistance() const;
    int getKillDistance() const;
    
    
    std::unique_lock<std::shared_mutex> getPositionLock();
    std::shared_lock<std::shared_mutex> getPositionSharedLock() const;
};


class GameSquirrel : public GameNPC {
public:
    GameSquirrel(const std::string& name, float x, float y, int id);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

class GameElf : public GameNPC {
public:
    GameElf(const std::string& name, float x, float y, int id);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

class GameBandit : public GameNPC {
public:
    GameBandit(const std::string& name, float x, float y, int id);
    NPCType getType() const override;
    bool canKill(const std::shared_ptr<NPC>& other) const override;
};

#endif 