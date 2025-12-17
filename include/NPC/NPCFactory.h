#ifndef NPCFACTORY_H
#define NPCFACTORY_H

#include <memory>
#include <string>
#include <map>
#include "NPC.h"

class NPCFactory {
public:
    static std::shared_ptr<NPC> createNPC(const std::string& typeStr, 
                                         const std::string& name, 
                                         float x, float y);
    
    static std::shared_ptr<NPC> createFromMap(const std::map<std::string, std::string>& data);
    
private:
    NPCFactory() = delete; 
};

#endif 