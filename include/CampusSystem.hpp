#ifndef CAMPUSSYSTEM_HPP
#define CAMPUSSYSTEM_HPP

#include "ResourceManager.hpp"
#include "EventManager.hpp"
#include <iostream>
#include <string>

class CampusSystem {
    private:
        ResourceManager resource_manager;
        EventManager event_manager;
        bool is_running;

    public:
        CampusSystem();
        void run();
        void loadAllData();
        void saveAllData();
        bool isRunning() const;
        ~CampusSystem();
            
        void handleMainMenu();
        void handleResourceMenu(); 
        void handleEventMenu();
        
        bool isNumericInput(const std::string& input) const;
        unsigned int stringToUInt(const std::string& str) const;
        
        bool isValidDate(const std::string& date) const;
        bool isValidTime(const std::string& time) const;
        
        void displaySystemStatus() const;
};

#endif