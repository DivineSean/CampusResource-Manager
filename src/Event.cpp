#include "Event.hpp"
#include <sstream>
#include <iomanip>

Event::Event() {
    id = 0;
    title = "Default Event";
    date = "2025-01-01";
    start_time = "00:00";
    duration_minutes = 60;
    resource_ids.clear();
}

Event::Event(unsigned int id, const std::string& title, const std::string& date,
             const std::string& start_time, int duration_minutes, 
             const std::vector<unsigned int>& resource_ids) {
    this->id = id;
    this->title = title;
    this->date = date;
    this->start_time = start_time;
    this->duration_minutes = duration_minutes;
    this->resource_ids = resource_ids;
}

Event::~Event() {
}

unsigned int Event::getId() const {
    return id;
}

std::string Event::getTitle() const {
    return title;
}

std::string Event::getDate() const {
    return date;
}

std::string Event::getStartTime() const {
    return start_time;
}

int Event::getDurationMinutes() const {
    return duration_minutes;
}

std::vector<unsigned int> Event::getResourceIds() const {
    return resource_ids;
}

void Event::setTitle(const std::string& title) {
    this->title = title;
}

void Event::setDate(const std::string& date) {
    this->date = date;
}

void Event::setStartTime(const std::string& start_time) {
    this->start_time = start_time;
}

void Event::setDurationMinutes(int duration_minutes) {
    this->duration_minutes = duration_minutes;
}

void Event::setResourceIds(const std::vector<unsigned int>& resource_ids) {
    this->resource_ids = resource_ids;
}

void Event::addResourceId(unsigned int resource_id) {
    for (unsigned int i = 0; i < resource_ids.size(); i++) {
        if (resource_ids[i] == resource_id) {
        }
    }
    resource_ids.push_back(resource_id);
}

void Event::removeResourceId(unsigned int resource_id) {
    for (unsigned int i = 0; i < resource_ids.size(); i++) {
        if (resource_ids[i] == resource_id) {
            resource_ids.erase(resource_ids.begin() + i);
            return;
        }
    }
}

void Event::displayInfo() const {
    std::cout << "=================================================" << std::endl;
    std::cout << "Event ID: " << id << std::endl;
    std::cout << "Title: " << title << std::endl;
    std::cout << "Date: " << date << std::endl;
    std::cout << "Time: " << start_time << " - " << getEndTime() << std::endl;
    std::cout << "Duration: " << duration_minutes << " minutes" << std::endl;
    std::cout << "Resource IDs: ";
    if (resource_ids.empty()) {
        std::cout << "None";
    } else {
        for (unsigned int i = 0; i < resource_ids.size(); i++) {
            std::cout << resource_ids[i];
            if (i < resource_ids.size() - 1) {
                std::cout << ", ";
            }
        }
    }
    std::cout << std::endl;
    std::cout << "=================================================" << std::endl;
}

std::string Event::toString() const {
    std::string resource_ids_str = "";
    for (unsigned int i = 0; i < resource_ids.size(); i++) {
        resource_ids_str += std::to_string(resource_ids[i]);
        if (i < resource_ids.size() - 1) {
            resource_ids_str += ",";
        }
    }
    
    return std::to_string(id) + "|" + title + "|" + date + "|" + 
           start_time + "|" + std::to_string(duration_minutes) + "|" + 
           resource_ids_str;
}

std::string Event::getEndTime() const {
    int hours = std::stoi(start_time.substr(0, 2));
    int minutes = std::stoi(start_time.substr(3, 2));
    
    minutes += duration_minutes;
    hours += minutes / 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" 
       << std::setfill('0') << std::setw(2) << minutes;
    
    return ss.str();
}