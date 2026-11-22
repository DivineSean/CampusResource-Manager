#include "CampusApp.hpp"
#include <iostream>

int main() {
    try {
        CampusApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
