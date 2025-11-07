#include "CampusSystem.hpp"
#include "outils.hpp"
#include <iostream>
#include <exception>
#include <cstdlib>

int main() {

    SystemStart();
    
    try {
        CampusSystem campus_system;

        campus_system.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        std::cerr << "The system encountered an unrecoverable error." << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred." << std::endl;
        return 1;
    }
    
    return 0;
}