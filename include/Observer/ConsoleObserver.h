#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include "BattleObserver.h"

class ConsoleObserver : public BattleObserver {
public:
    void onKill(const std::string& killer, const std::string& victim) override;
};

#endif