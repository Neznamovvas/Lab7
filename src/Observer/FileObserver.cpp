#include "../../include/Observer/FileObserver.h"
#include <iostream>

FileObserver::FileObserver() {
    logFile.open("log.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log.txt file" << std::endl;
    }
}

FileObserver::~FileObserver() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void FileObserver::onKill(const std::string& killer, const std::string& victim) {
    if (logFile.is_open()) {
        logFile << "[BATTLE] " << killer << " killed " << victim << std::endl;
    }
}