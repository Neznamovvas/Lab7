#include "../../include/Editor/DungeonEditor.h"
#include "../../include/NPC/NPCFactory.h"
#include "../../include/Visitor/BattleVisitor.h"
#include "../../include/Observer/ConsoleObserver.h"
#include "../../include/Observer/FileObserver.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

DungeonEditor::DungeonEditor() {
    consoleObserver = std::make_shared<ConsoleObserver>();
    fileObserver = std::make_shared<FileObserver>();
}

void DungeonEditor::addNPC(const std::string& type, const std::string& name, float x, float y) {
    try {
        auto npc = NPCFactory::createNPC(type, name, x, y);
        npcs.push_back(npc);
        std::cout << "Added NPC: " << npc->getInfo() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error adding NPC: " << e.what() << std::endl;
    }
}

void DungeonEditor::printAll() const {
    std::cout << "\n=== NPC LIST (" << npcs.size() << ") ===" << std::endl;
    if (npcs.empty()) {
        std::cout << "No NPCs in the dungeon." << std::endl;
    } else {
        for (size_t i = 0; i < npcs.size(); i++) {
            std::cout << i + 1 << ". " << npcs[i]->getInfo() << std::endl;
        }
    }
    std::cout << "===========================\n" << std::endl;
}

void DungeonEditor::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "[" << std::endl;
    for (size_t i = 0; i < npcs.size(); i++) {
        auto data = npcs[i]->toMap();
        file << "  {" << std::endl;
        file << "    \"type\": \"" << data["type"] << "\"," << std::endl;
        file << "    \"name\": \"" << data["name"] << "\"," << std::endl;
        file << "    \"x\": " << data["x"] << "," << std::endl;
        file << "    \"y\": " << data["y"] << std::endl;
        file << "  }";
        if (i != npcs.size() - 1) file << ",";
        file << std::endl;
    }
    file << "]" << std::endl;
    
    file.close();
    std::cout << "Saved " << npcs.size() << " NPCs to file: " << filename << std::endl;
}

std::string DungeonEditor::extractValueFromJSON(const std::string& jsonStr, const std::string& key) const {
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = jsonStr.find(searchKey);
    if (keyPos == std::string::npos) return "";
    
    size_t valueStart = keyPos + searchKey.length();
    
    while (valueStart < jsonStr.length() && 
           (jsonStr[valueStart] == ' ' || jsonStr[valueStart] == '\t' || jsonStr[valueStart] == '\n')) {
        valueStart++;
    }
    
    if (valueStart >= jsonStr.length()) return "";
    
    if (jsonStr[valueStart] == '\"') {
        valueStart++; 
        size_t valueEnd = jsonStr.find("\"", valueStart);
        if (valueEnd == std::string::npos) return "";
        return jsonStr.substr(valueStart, valueEnd - valueStart);
    } else {
        size_t valueEnd = valueStart;
        while (valueEnd < jsonStr.length() && 
               (std::isdigit(jsonStr[valueEnd]) || jsonStr[valueEnd] == '.' || 
                jsonStr[valueEnd] == '-' || jsonStr[valueEnd] == '+')) {
            valueEnd++;
        }
        return jsonStr.substr(valueStart, valueEnd - valueStart);
    }
}

void DungeonEditor::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    try {
        npcs.clear();
        
        size_t pos = 0;
        while ((pos = content.find("{", pos)) != std::string::npos) {
            size_t endPos = content.find("}", pos);
            if (endPos == std::string::npos) break;
            
            std::string objStr = content.substr(pos, endPos - pos + 1);

            std::map<std::string, std::string> data;
            data["type"] = extractValueFromJSON(objStr, "type");
            data["name"] = extractValueFromJSON(objStr, "name");
            data["x"] = extractValueFromJSON(objStr, "x");
            data["y"] = extractValueFromJSON(objStr, "y");
            
            if (!data["type"].empty() && !data["name"].empty() && 
                !data["x"].empty() && !data["y"].empty()) {
                auto npc = NPCFactory::createFromMap(data);
                npcs.push_back(npc);
            }
            
            pos = endPos + 1;
        }
        
        std::cout << "Loaded " << npcs.size() << " NPCs from file: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading file: " << e.what() << std::endl;
    }
}

void DungeonEditor::startBattle(float range) {
    std::cout << "\n=== BATTLE START (range: " << range << "m) ===" << std::endl;
    std::cout << "Before battle: " << npcs.size() << " NPCs" << std::endl;
    
    BattleVisitor visitor(range);
    visitor.addObserver(consoleObserver);
    visitor.addObserver(fileObserver);
    
    visitor.visit(npcs);
    
    std::cout << "After battle: " << npcs.size() << " NPCs" << std::endl;
    std::cout << "=== BATTLE END ===\n" << std::endl;
}

void DungeonEditor::clear() {
    npcs.clear();
    std::cout << "All NPCs removed from dungeon." << std::endl;
}

size_t DungeonEditor::getNPCCount() const {
    return npcs.size();
}