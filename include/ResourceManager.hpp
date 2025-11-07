#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP
#include "Resource.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class ResourceManager {
    private:
        std::vector<Resource> resources;
        std::string data_file;
        unsigned int next_id;

    public:
        ResourceManager();
        ~ResourceManager();

        void addResource(const std::string& name, const std::string& type, bool is_available);
        void viewAllResources() const;
        void updateResource(unsigned int id, bool can_change_availability);
        void updateResourceByName(const std::string& name, bool can_change_availability);
        void deleteResource(unsigned int id);
        void deleteResourceByName(const std::string& name);
        
        int getResourceCount() const;
        bool resourceExists(unsigned int id) const;
        bool resourceExistsByName(const std::string& name) const;
        Resource* findResource(unsigned int id); 
        Resource* findResourceByName(const std::string& name);
        
        std::string toLowercase(const std::string& str) const; 
        void searchResourceById(unsigned int id) const;
        void searchResourceByName(const std::string& name) const;
        void searchResourceByType(const std::string& type) const;
        void saveToFile() const;
        void loadFromFile();
};

#endif