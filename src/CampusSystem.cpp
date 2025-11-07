#include "CampusSystem.hpp"
#include "outils.hpp"
#include <limits>
#include <sstream>
#include <cstdlib>

CampusSystem::CampusSystem() : event_manager(&resource_manager) {
    is_running = false;
    resource_manager.loadFromFile();
    event_manager.loadFromFile();
}

bool CampusSystem::isNumericInput(const std::string& input) const {
    if (input.empty()) return false;
    
    for (unsigned int i = 0; i < input.length(); i++) {
        if (!std::isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

unsigned int CampusSystem::stringToUInt(const std::string& str) const {
    std::stringstream ss(str);
    unsigned int result;
    ss >> result;
    return result;
}

bool CampusSystem::isValidDate(const std::string& date) const {
    if (date.length() != 10) {
        return false;
    }
    if (date[4] != '-' || date[7] != '-') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) {
            return false;
        }
    }
    int year = stringToUInt(date.substr(0, 4));
    int month = stringToUInt(date.substr(5, 2));
    int day = stringToUInt(date.substr(8, 2));
    
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap_year) {
        days_in_month[1] = 29;
    }
    if (day > days_in_month[month - 1]) {
        return false;
    }
    
    return true;
}

bool CampusSystem::isValidTime(const std::string& time) const {
    if (time.length() != 5) {
        return false;
    }
    
    if (time[2] != ':') {
        return false;
    }
    
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!std::isdigit(time[i])) {
            return false;
        }
    }
    
    int hours = stringToUInt(time.substr(0, 2));
    int minutes = stringToUInt(time.substr(3, 2));
    
    if (hours < 0 || hours > 23) return false;
    if (minutes < 0 || minutes > 59) return false;
    
    return true;
}

void CampusSystem::handleMainMenu() {
    systemMainMenu();
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        std::cout << "Error: Please choose a valid option." << std::endl;
        std::cout << "\nPress Enter to choose your option again...";
        std::cin.get();
        clearScreen();
        return;
    }
    
    if (!isNumericInput(input)) {
        std::cout << "Error: Please choose only numbers." << std::endl;
        std::cout << "\nPress Enter to choose your option again...";
        std::cin.get();
        clearScreen();
        return;
    }

    int choice = stringToUInt(input);
    
    switch (choice) {
        case 1:
            clearScreen();
            handleResourceMenu();
            break;
        case 2:
            clearScreen();
            handleEventMenu();
            clearScreen();
            break;
        case 3:
            clearScreen();
            displaySystemStatus();
            std::cout << "\nPress Enter to return to main menu...";
            std::cin.get();
            clearScreen();
            break;
        case 4:
            is_running = false;
            break;
        default:
            std::cout << "Error: Invalid option. Please choose 1-4." << std::endl;
            std::cout << "\nPress Enter to choose your option again...";
            std::cin.get();
            clearScreen();
            break;
    }
}

void CampusSystem::handleResourceMenu() {
    bool in_resource_menu = true;
    
    while (in_resource_menu) {
        systemResourceMenu();
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            std::cout << "Error: Please choose a valid option." << std::endl;
            std::cout << "\nPress Enter to try again...";
            std::cin.get();
            clearScreen();
            continue;
        }
        
        if (!isNumericInput(input)) {
            std::cout << "Error: Please choose only numbers." << std::endl;
            std::cout << "\nPress Enter to try again...";
            std::cin.get();
            clearScreen();
            continue;
        }

        int choice = stringToUInt(input);
        
        switch (choice) {
            case 1: {
                std::cout << "\n=== ADD NEW RESOURCE ===" << std::endl;
                std::string name, type, availability_input;
                bool is_available = true;
                
                bool valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter resource name: ";
                    std::getline(std::cin, name);
                    if (name.empty()) {
                        std::cout << "Error: Resource name cannot be empty!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter resource type (e.g., Room, Projector, Lab): ";
                    std::getline(std::cin, type);
                    if (type.empty()) {
                        std::cout << "Error: Resource type cannot be empty!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    std::cout << "Is this resource available? (y/n): ";
                    std::getline(std::cin, availability_input);
                    if (availability_input == "y" || availability_input == "Y") {
                        is_available = true;
                        valid_input = true;
                    } else if (availability_input == "n" || availability_input == "N") {
                        is_available = false;
                        valid_input = true;
                    } else {
                        std::cout << "Error: Please enter 'y' for yes or 'n' for no." << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                    }
                }
                
                resource_manager.addResource(name, type, is_available);
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 2:
                resource_manager.viewAllResources();
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            case 3: {
                std::cout << "\n=== UPDATE RESOURCE ===" << std::endl;
                resource_manager.viewAllResources();
                
                if (resource_manager.getResourceCount() == 0) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    clearScreen();
                    break;
                }
                
                std::cout << "\nEnter resource ID or Name to update: ";
                std::string identifier;
                std::getline(std::cin, identifier);
                
                bool can_change_availability = true;
                
                if (isNumericInput(identifier)) {
                    unsigned int id = stringToUInt(identifier);
                    if (resource_manager.resourceExists(id)) {
                        can_change_availability = !event_manager.isResourceUsedByEvents(id);
                        resource_manager.updateResource(id, can_change_availability);
                    } else {
                        std::cout << "Error: Resource with ID " << id << " not found!" << std::endl;
                    }
                } else {
                    if (resource_manager.resourceExistsByName(identifier)) {
                        Resource* res = resource_manager.findResourceByName(identifier);
                        if (res != NULL) {
                            can_change_availability = !event_manager.isResourceUsedByEvents(res->getId());
                        }
                        resource_manager.updateResourceByName(identifier, can_change_availability);
                    } else {
                        std::cout << "Error: Resource with name '" << identifier << "' not found!" << std::endl;
                    }
                }
                
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 4: {
                std::cout << "\n=== DELETE RESOURCE ===" << std::endl;
                resource_manager.viewAllResources();
                
                if (resource_manager.getResourceCount() == 0) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    clearScreen();
                    break;
                }
                
                std::cout << "\nEnter resource ID or Name to delete: ";
                std::string identifier;
                std::getline(std::cin, identifier);
                
                std::cout << "Are you sure you want to delete this resource? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
                    if (isNumericInput(identifier)) {
                        unsigned int id = stringToUInt(identifier);
                        if (event_manager.isResourceUsedByEvents(id)) {
                            std::cout << "Warning: This resource is currently used by one or more events." << std::endl;
                            std::cout << "Deleting it will affect those events. Continue? (y/N): ";
                            std::string final_confirm;
                            std::getline(std::cin, final_confirm);
                            if (final_confirm == "y" || final_confirm == "Y") {
                                resource_manager.deleteResource(id);
                            } else {
                                std::cout << "Deletion cancelled." << std::endl;
                            }
                        } else {
                            resource_manager.deleteResource(id);
                        }
                    } else {
                        Resource* res = resource_manager.findResourceByName(identifier);
                        if (res != NULL) {
                            unsigned int id = res->getId();
                            if (event_manager.isResourceUsedByEvents(id)) {
                                std::cout << "Warning: This resource is currently used by one or more events." << std::endl;
                                std::cout << "Deleting it will affect those events. Continue? (y/N): ";
                                std::string final_confirm;
                                std::getline(std::cin, final_confirm);
                                if (final_confirm == "y" || final_confirm == "Y") {
                                    resource_manager.deleteResourceByName(identifier);
                                } else {
                                    std::cout << "Deletion cancelled." << std::endl;
                                }
                            } else {
                                resource_manager.deleteResourceByName(identifier);
                            }
                        } else {
                            std::cout << "Error: Resource not found!" << std::endl;
                        }
                    }
                } else {
                    std::cout << "Deletion cancelled." << std::endl;
                }
                
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 5: {
                clearScreen();
                bool in_search_menu = true;
                
                while (in_search_menu) {
                    systemResourceSearchMenu();
                    std::string input;
                    std::getline(std::cin, input);
                    
                    if (input.empty()) {
                        std::cout << "Error: Please choose a valid option." << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        clearScreen();
                        continue;
                    }
                    
                    if (!isNumericInput(input)) {
                        std::cout << "Error: Please choose only numbers." << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        clearScreen();
                        continue;
                    }

                    int choice = stringToUInt(input);
                    
                    switch (choice) {
                        case 1: {
                            std::cout << "\n=== SEARCH BY ID ===" << std::endl;
                            std::cout << "Enter resource ID: ";
                            std::string id_input;
                            std::getline(std::cin, id_input);
                            
                            if (isNumericInput(id_input) && !id_input.empty()) {
                                unsigned int id = stringToUInt(id_input);
                                resource_manager.searchResourceById(id);
                            } else {
                                std::cout << "Error: Invalid ID format!" << std::endl;
                            }
                            
                            std::cout << "\nPress Enter to continue...";
                            std::cin.get();
                            clearScreen();
                            break;
                        }
                        case 2: {
                            std::cout << "\n=== SEARCH BY NAME ===" << std::endl;
                            std::cout << "Enter resource name: ";
                            std::string name;
                            std::getline(std::cin, name);
                            
                            if (!name.empty()) {
                                resource_manager.searchResourceByName(name);
                            } else {
                                std::cout << "Search term cannot be empty!" << std::endl;
                            }
                            
                            std::cout << "\nPress Enter to continue...";
                            std::cin.get();
                            clearScreen();
                            break;
                        }
                        case 3: {
                            std::cout << "\n=== SEARCH BY TYPE ===" << std::endl;
                            std::cout << "Enter resource type: ";
                            std::string type;
                            std::getline(std::cin, type);
                            
                            if (!type.empty()) {
                                resource_manager.searchResourceByType(type);
                            } else {
                                std::cout << "Search term cannot be empty!" << std::endl;
                            }
                            
                            std::cout << "\nPress Enter to continue...";
                            std::cin.get();
                            clearScreen();
                            break;
                        }
                        case 4:
                            clearScreen();
                            in_search_menu = false;
                            break;
                        default:
                            std::cout << "Error: Invalid option. Please choose 1-4." << std::endl;
                            std::cout << "\nPress Enter to try again...";
                            std::cin.get();
                            clearScreen();
                            break;
                    }
                }
                break;
            }
            case 6:
                clearScreen();
                in_resource_menu = false;
                break;
            default:
                std::cout << "Error: Invalid option. Please choose 1-6." << std::endl;
                std::cout << "\nPress Enter to try again...";
                std::cin.get();
                clearScreen();
                break;
        }
    }
}

void CampusSystem::handleEventMenu() {
    bool in_event_menu = true;
    
    while (in_event_menu) {
        systemEventMenu();
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            std::cout << "Error: Please choose a valid option." << std::endl;
            std::cout << "\nPress Enter to try again...";
            std::cin.get();
            clearScreen();
            continue;
        }
        
        if (!isNumericInput(input)) {
            std::cout << "Error: Please choose only numbers." << std::endl;
            std::cout << "\nPress Enter to try again...";
            std::cin.get();
            clearScreen();
            continue;
        }

        int choice = stringToUInt(input);
        
        switch (choice) {
            case 1: {
                std::cout << "\n=== ADD NEW EVENT ===" << std::endl;
                
                if (resource_manager.getResourceCount() == 0) {
                    std::cout << "\nNo resources available! Please add resources first." << std::endl;
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    clearScreen();
                    break;
                }
                
                std::string title, date, start_time, duration_str;
                int duration = 0;
                std::vector<unsigned int> resource_ids;
                
                bool valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter event title: ";
                    std::getline(std::cin, title);
                    if (title.empty()) {
                        std::cout << "Error: Event title cannot be empty!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter date [YYYY-MM-DD] (e.g., 2025-12-25): ";
                    std::getline(std::cin, date);
                    if (!isValidDate(date)) {
                        std::cout << "Error: Invalid date format! Please use YYYY-MM-DD (e.g., 2025-12-25)" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter start time [HH:MM] (e.g., 14:30): ";
                    std::getline(std::cin, start_time);
                    if (!isValidTime(start_time)) {
                        std::cout << "Error: Invalid time format! Please use HH:MM (e.g., 14:30)" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    std::cout << "Enter duration in minutes: ";
                    std::getline(std::cin, duration_str);
                    if (!isNumericInput(duration_str)) {
                        std::cout << "Error: Duration must be a number!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    
                    duration = stringToUInt(duration_str);
                    if (duration <= 0 || duration > 1440) {
                        std::cout << "Error: Duration must be between 1 and 1440 minutes (24 hours)!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                valid_input = false;
                while (!valid_input) {
                    resource_ids = event_manager.selectMultipleResourcesInteractive();
                    
                    if (resource_ids.empty()) {
                        std::cout << "Error: No valid resources selected!" << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        continue;
                    }
                    valid_input = true;
                }
                
                event_manager.addEvent(title, date, start_time, duration, resource_ids);
                
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 2: {
                event_manager.viewAllEvents();
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 3: {
                std::cout << "\n=== UPDATE EVENT ===" << std::endl;
                event_manager.viewAllEvents();
                
                if (event_manager.getEventCount() == 0) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    clearScreen();
                    break;
                }
                
                std::cout << "\nEnter event ID or Title to update: ";
                std::string identifier;
                std::getline(std::cin, identifier);
                
                if (isNumericInput(identifier)) {
                    unsigned int id = stringToUInt(identifier);
                    event_manager.updateEvent(id);
                } else {
                    event_manager.updateEventByTitle(identifier);
                }
                
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 4: {
                std::cout << "\n=== DELETE EVENT ===" << std::endl;
                event_manager.viewAllEvents();
                
                if (event_manager.getEventCount() == 0) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    clearScreen();
                    break;
                }
                
                std::cout << "\nEnter event ID or Title to delete: ";
                std::string identifier;
                std::getline(std::cin, identifier);
                
                std::cout << "Are you sure you want to delete this event? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
                    if (isNumericInput(identifier)) {
                        unsigned int id = stringToUInt(identifier);
                        event_manager.deleteEvent(id);
                    } else {
                        event_manager.deleteEventByTitle(identifier);
                    }
                } else {
                    std::cout << "Deletion cancelled." << std::endl;
                }
                
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                clearScreen();
                break;
            }
            case 5: {
                clearScreen();
                bool in_search_menu = true;
                
                while (in_search_menu) {
                    systemEventSearchMenu();
                    std::string input;
                    std::getline(std::cin, input);
                    
                    if (input.empty()) {
                        std::cout << "Error: Please choose a valid option." << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        clearScreen();
                        continue;
                    }
                    
                    if (!isNumericInput(input)) {
                        std::cout << "Error: Please choose only numbers." << std::endl;
                        std::cout << "\nPress Enter to try again...";
                        std::cin.get();
                        clearScreen();
                        continue;
                    }

                    int choice = stringToUInt(input);
                    
                    switch (choice) {
                        case 1: {
                            std::cout << "\n=== SEARCH BY ID ===" << std::endl;
                            std::cout << "Enter event ID: ";
                            std::string id_input;
                            std::getline(std::cin, id_input);
                            
                            if (isNumericInput(id_input) && !id_input.empty()) {
                                unsigned int id = stringToUInt(id_input);
                                event_manager.searchEventById(id);
                            } else {
                                std::cout << "Error: Invalid ID format!" << std::endl;
                            }
                            
                            std::cout << "\nPress Enter to continue...";
                            std::cin.get();
                            clearScreen();
                            break;
                        }
                        case 2: {
                            std::cout << "\n=== SEARCH BY TITLE ===" << std::endl;
                            std::cout << "Enter event title: ";
                            std::string title;
                            std::getline(std::cin, title);
                            
                            if (!title.empty()) {
                                event_manager.searchEventByTitle(title);
                            } else {
                                std::cout << "Search term cannot be empty!" << std::endl;
                            }
                            
                            std::cout << "\nPress Enter to continue...";
                            std::cin.get();
                            clearScreen();
                            break;
                        }
                        case 3:
                            clearScreen();
                            in_search_menu = false;
                            break;
                        default:
                            std::cout << "Error: Invalid option. Please choose 1-3." << std::endl;
                            std::cout << "\nPress Enter to try again...";
                            std::cin.get();
                            clearScreen();
                            break;
                    }
                }
                break;
            }
            case 6:
                clearScreen();
                in_event_menu = false;
                break;
            default:
                std::cout << "Error: Invalid option. Please choose 1-6." << std::endl;
                std::cout << "\nPress Enter to try again...";
                std::cin.get();
                clearScreen();
                break;
        }
    }
}

void CampusSystem::run(){
    clearScreen();
    is_running = true;
    while (is_running) {
        handleMainMenu();
    }
}

void CampusSystem::saveAllData() {
    resource_manager.saveToFile();
    event_manager.saveToFile();
}

void CampusSystem::loadAllData() {
    resource_manager.loadFromFile();
    event_manager.loadFromFile();
}

void CampusSystem::displaySystemStatus() const {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Resource & Event Management System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Welcome to the Campus Resource & Event Management System!" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "             SYSTEM STATUS REPORT" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "Total Resources: " << resource_manager.getResourceCount() << std::endl;
    std::cout << "Total Events: " << event_manager.getEventCount() << std::endl;
    std::cout << "System Status: Running" << std::endl;
    std::cout << "Data File: " << (resource_manager.getResourceCount() > 0 ? "Has Data" : "Empty") << std::endl;
    std::cout << "==================================================" << std::endl;
}

bool CampusSystem::isRunning() const {
    return is_running;
}

CampusSystem::~CampusSystem(){
    std::cout << "\nExiting Campus Management System..." << std::endl;
    std::cout << "Thank you for using our system!" << std::endl;
    sleepSeconds(1.5);
    clearScreen();
}