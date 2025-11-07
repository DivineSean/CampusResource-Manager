#include "Resource.hpp"
#include <iomanip>

Resource::Resource() {
    id = 0;
    name = "default";
    type = "default";
    is_available = true;
}

Resource::Resource(unsigned int id, const std::string& name, const std::string& type, bool available) {
    this->id = id;
    this->name = name;
    this->type = type;
    this->is_available = available;
}

Resource::~Resource() {
}

unsigned int Resource::getId() const {
    return id;
}

std::string Resource::getName() const {
    return name;
}

std::string Resource::getType() const {
    return type;
}

bool Resource::isAvailable() const {
    return is_available;
}

void Resource::setName(const std::string& name) {
    this->name = name;
}

void Resource::setType(const std::string& type) {
    this->type = type;
}

void Resource::setAvailability(bool available) {
    this->is_available = available;
}

void Resource::displayInfo() const {
    std::cout << "=================================================" << std::endl;
    std::cout << "Resource ID: " << id << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Type: " << type << std::endl;
    std::cout << "Status: " << (is_available ? "Available" : "Not Available") << std::endl;
    std::cout << "=================================================" << std::endl;
}

std::string Resource::toString() const {
    return std::to_string(id) + "|" + name + "|" + type + "|" + (is_available ? "1" : "0");
}