#include "outils.hpp"
#include <thread>
#include <chrono>

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void sleepSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void SystemStart(){
    clearScreen();
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
}

void systemEventMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "|              EVENT MANAGEMENT                  |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "| 1 | Add New Event                              |" << std::endl;
    std::cout << "| 2 | View All Events                            |" << std::endl;
    std::cout << "| 3 | Update Event                               |" << std::endl;
    std::cout << "| 4 | Delete Event                               |" << std::endl;
    std::cout << "| 5 | Search Events                              |" << std::endl;
    std::cout << "| 6 | Return to Main Menu                        |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl <<"Please choose your option (1-6): ";
}

void systemMainMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "|                   MAIN MENU                    |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "| 1 | Resource Management                        |" << std::endl;
    std::cout << "| 2 | Event Management                           |" << std::endl;
    std::cout << "| 3 | View System Status                         |" << std::endl;
    std::cout << "| 4 | Exit                                       |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl <<"Please choose your option (1-4): ";
}

void systemResourceMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "|             RESOURCE MANAGEMENT                |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "| 1 | Add New Resource                           |" << std::endl;
    std::cout << "| 2 | View All Resources                         |" << std::endl;
    std::cout << "| 3 | Update Resource                            |" << std::endl;
    std::cout << "| 4 | Delete Resource                            |" << std::endl;
    std::cout << "| 5 | Search Resources                           |" << std::endl;
    std::cout << "| 6 | Return to Main Menu                        |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl <<"Please choose your option (1-6): ";
}

void systemResourceSearchMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "|            SEARCH RESOURCES BY                 |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "| 1 | Search by ID                               |" << std::endl;
    std::cout << "| 2 | Search by Name                             |" << std::endl;
    std::cout << "| 3 | Search by Type                             |" << std::endl;
    std::cout << "| 4 | Return to Resource Menu                    |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl <<"Please choose your option (1-4): ";
}

void systemEventSearchMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "|              SEARCH EVENTS BY                  |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "| 1 | Search by ID                               |" << std::endl;
    std::cout << "| 2 | Search by Title                            |" << std::endl;
    std::cout << "| 3 | Return to Event Menu                       |" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl <<"Please choose your option (1-3): ";
}