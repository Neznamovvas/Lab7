#include "../../include/Observer/ConsoleObserver.h"
#include <iostream>

void ConsoleObserver::onKill(const std::string& killer, const std::string& victim) {
    std::cout << "[BATTLE] " << killer << " killed " << victim << std::endl;
}