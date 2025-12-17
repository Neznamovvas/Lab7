#ifndef GAMENPCFACTORY_H
#define GAMENPCFACTORY_H

#include <memory>
#include <string>
#include "GameNPC.h"

class GameNPCFactory {
public:
    static std::shared_ptr<GameNPC> createGameNPC(const std::string& typeStr,
                                                 const std::string& name,
                                                 float x, float y,
                                                 int id);
    
    static std::shared_ptr<GameNPC> createRandomGameNPC(int id);
    
private:
    static std::string getRandomSquirrelName();
    static std::string getRandomElfName();
    static std::string getRandomBanditName();
};

#endif