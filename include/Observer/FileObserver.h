#ifndef FILEOBSERVER_H
#define FILEOBSERVER_H

#include "BattleObserver.h"
#include <fstream>

class FileObserver : public BattleObserver {
    std::ofstream logFile;
    
public:
    FileObserver();
    ~FileObserver();
    
    void onKill(const std::string& killer, const std::string& victim) override;
};

#endif