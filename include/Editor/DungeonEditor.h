#ifndef DUNGEONEDITOR_H
#define DUNGEONEDITOR_H

#include <vector>
#include <memory>
#include <string>
#include "../NPC/NPC.h"
#include "../Observer/BattleObserver.h"

class ConsoleObserver;
class FileObserver;
class BattleVisitor;

class DungeonEditor {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    std::shared_ptr<ConsoleObserver> consoleObserver;
    std::shared_ptr<FileObserver> fileObserver;
    
    std::string extractValueFromJSON(const std::string& jsonStr, const std::string& key) const;
    
public:
    DungeonEditor();
    
    void addNPC(const std::string& type, const std::string& name, float x, float y);
    void printAll() const;
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    void startBattle(float range);
    void clear();
    
    size_t getNPCCount() const;
};

#endif