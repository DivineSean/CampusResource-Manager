#include "outils.hpp"
#include <thread>
#include <chrono>
#include <cstdlib>

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
