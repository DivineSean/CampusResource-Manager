#include "ResourceManager.hpp"
#include <iostream>

ResourceManager::ResourceManager() {
    data_file = "data/resources.txt";
    next_id = 1;
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::addResource(const std::string& name, const std::string& type, bool is_available) {
    Resource new_resource(next_id, name, type, is_available);
    resources.push_back(new_resource);
    std::cout << "Resource '" << name << "' added successfully with ID: " << next_id << std::endl;
    std::cout << "Status: " << (is_available ? "Available" : "Not Available") << std::endl;
    next_id++;
    saveToFile();
}

void ResourceManager::viewAllResources() const {
    if (resources.empty()) {
        std::cout << "\n=== NO RESOURCES FOUND ===" << std::endl;
        std::cout << "No resources have been added to the system yet." << std::endl;
        std::cout << "Use 'Add New Resource' option to add resources." << std::endl;
        return;
    }

    std::cout << "\n=== ALL RESOURCES ===" << std::endl;
    std::cout << "Total Resources: " << resources.size() << std::endl;
    std::cout << std::endl;
    
    for (unsigned int i = 0; i < resources.size(); i++) {
        resources[i].displayInfo();
        std::cout << std::endl;
    }
}

void ResourceManager::updateResource(unsigned int id, bool can_change_availability) {
    Resource* resource = findResource(id);
    if (resource == NULL) {
        std::cout << "Error: Resource with ID " << id << " not found." << std::endl;
        return;
    }

    std::cout << "Current resource information:" << std::endl;
    resource->displayInfo();
    
    std::cout << "\nEnter new information (press Enter to keep current value):" << std::endl;
    
    std::cout << "New name (current: " << resource->getName() << "): ";
    std::string new_name;
    std::getline(std::cin, new_name);
    if (!new_name.empty()) {
        resource->setName(new_name);
    }
    
    std::cout << "New type (current: " << resource->getType() << "): ";
    std::string new_type;
    std::getline(std::cin, new_type);
    if (!new_type.empty()) {
        resource->setType(new_type);
    }
    
    std::cout << "Change availability status? (y/N): ";
    std::string change_status;
    std::getline(std::cin, change_status);
    
    if (change_status == "y" || change_status == "Y") {
        std::cout << "Set as available? (y/n): ";
        std::string availability_input;
        std::getline(std::cin, availability_input);
        
        bool new_availability = (availability_input == "y" || availability_input == "Y");
        
        if (!new_availability && resource->isAvailable() && !can_change_availability) {
            std::cout << "Error: Cannot set resource to unavailable because it is currently used by one or more events." << std::endl;
            std::cout << "Please delete the events or update them to not use this resource first." << std::endl;
            return;
        }
        
        resource->setAvailability(new_availability);
    }
    
    std::cout << "Resource updated successfully!" << std::endl;
    saveToFile();
}

void ResourceManager::updateResourceByName(const std::string& name, bool can_change_availability) {
    Resource* resource = findResourceByName(name);
    if (resource == NULL) {
        std::cout << "Error: Resource with name '" << name << "' not found." << std::endl;
        return;
    }
    updateResource(resource->getId(), can_change_availability);
}

void ResourceManager::deleteResource(unsigned int id) {
    for (unsigned int i = 0; i < resources.size(); i++) {
        if (resources[i].getId() == id) {
            std::string resource_name = resources[i].getName();
            std::cout << "Deleting resource: " << resource_name << " (ID: " << id << ")" << std::endl;
            resources.erase(resources.begin() + i);
            std::cout << "Resource deleted successfully." << std::endl;
            saveToFile();
            return;
        }
    }
    std::cout << "Error: Resource with ID " << id << " not found." << std::endl;
}

void ResourceManager::deleteResourceByName(const std::string& name) {
    Resource* resource = findResourceByName(name);
    if (resource == NULL) {
        std::cout << "Error: Resource with name '" << name << "' not found." << std::endl;
        return;
    }
    unsigned int id = resource->getId();
    deleteResource(id);
}

std::string ResourceManager::toLowercase(const std::string& str) const {
    std::string result = str;
    for (unsigned int i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + 32;
        }
    }
    return result;
}

int ResourceManager::getResourceCount() const {
    return resources.size();
}

bool ResourceManager::resourceExists(unsigned int id) const {
    for (unsigned int i = 0; i < resources.size(); i++) {
        if (resources[i].getId() == id) {
            return true;
        }
    }
    return false;
}

bool ResourceManager::resourceExistsByName(const std::string& name) const {
    std::string search_lower = toLowercase(name);
    for (unsigned int i = 0; i < resources.size(); i++) {
        if (toLowercase(resources[i].getName()) == search_lower) {
            return true;
        }
    }
    return false;
}

Resource* ResourceManager::findResource(unsigned int id) {
    for (unsigned int i = 0; i < resources.size(); i++) {
        if (resources[i].getId() == id) {
            return &resources[i];
        }
    }
    return NULL;
}

Resource* ResourceManager::findResourceByName(const std::string& name) {
    std::string search_lower = toLowercase(name);
    for (unsigned int i = 0; i < resources.size(); i++) {
        if (toLowercase(resources[i].getName()) == search_lower) {
            return &resources[i];
        }
    }
    return NULL;
}

void ResourceManager::saveToFile() const {
    std::ofstream file(data_file);
    
    if (!file.is_open()) {
        return;
    }
    
    for (unsigned int i = 0; i < resources.size(); i++) {
        file << resources[i].toString() << std::endl;
    }
    
    file.close();
}

void ResourceManager::loadFromFile() {
    std::ifstream file(data_file);
    
    if (!file.is_open()) {
        return;
    }
    
    std::vector<Resource> temp_resources;
    std::string line;
    unsigned int max_id = 0;
    
    while (std::getline(file, line)) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string part;
        
        while (std::getline(ss, part, '|')) {
            parts.push_back(part);
        }
        
        if (parts.size() != 4) {
            file.close();
            return;
        }
        
        unsigned int id;
        std::stringstream(parts[0]) >> id;
        
        Resource loaded_resource(id, parts[1], parts[2], parts[3] == "1");
        temp_resources.push_back(loaded_resource);
        
        max_id = (id > max_id) ? id : max_id;
    }
    
    file.close();
    resources = temp_resources;
    next_id = max_id + 1;
}

void ResourceManager::searchResourceById(unsigned int id) const {
    Resource* resource = const_cast<ResourceManager*>(this)->findResource(id);
    
    if (resource == NULL) {
        std::cout << "No resource found with ID " << id << std::endl;
    } else {
        std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
        std::cout << "Found 1 resource matching ID " << id << ":" << std::endl;
        resource->displayInfo();
        std::cout << std::endl;
    }
}

void ResourceManager::searchResourceByName(const std::string& name) const {
    std::vector<Resource> found_resources;
    std::string search_lower = toLowercase(name);
    
    for (unsigned int i = 0; i < resources.size(); i++) {
        std::string name_lower = toLowercase(resources[i].getName());
        
        if (name_lower.find(search_lower) != std::string::npos) {
            found_resources.push_back(resources[i]);
        }
    }
    
    if (found_resources.empty()) {
        std::cout << "No resources found matching name '" << name << "'" << std::endl;
    } else {
        std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
        std::cout << "Found " << found_resources.size() << " resource(s) matching name '" << name << "':" << std::endl;
        for (unsigned int i = 0; i < found_resources.size(); i++) {
            found_resources[i].displayInfo();
            std::cout << std::endl;
        }
    }
}

void ResourceManager::searchResourceByType(const std::string& type) const {
    std::vector<Resource> found_resources;
    std::string search_lower = toLowercase(type);
    
    for (unsigned int i = 0; i < resources.size(); i++) {
        std::string type_lower = toLowercase(resources[i].getType());
        
        if (type_lower.find(search_lower) != std::string::npos) {
            found_resources.push_back(resources[i]);
        }
    }
    
    if (found_resources.empty()) {
        std::cout << "No resources found matching type '" << type << "'" << std::endl;
    } else {
        std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
        std::cout << "Found " << found_resources.size() << " resource(s) matching type '" << type << "':" << std::endl;
        for (unsigned int i = 0; i < found_resources.size(); i++) {
            found_resources[i].displayInfo();
            std::cout << std::endl;
        }
    }
}