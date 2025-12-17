#ifndef BATTLEOBSERVER_H
#define BATTLEOBSERVER_H

#include <string>

class BattleObserver {
public:
    virtual ~BattleObserver() = default;
    virtual void onKill(const std::string& killer, const std::string& victim) = 0;
};

#endif