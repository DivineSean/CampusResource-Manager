#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include <iostream>
#include <string>

class Resource {
    private:
        unsigned int id;
        std::string name;
        std::string type;
        bool is_available;

    public:
        Resource();
        Resource(unsigned int id, const std::string& name, 
                const std::string& type, bool available = true);
        ~Resource();

        unsigned int getId() const;
        std::string getName() const;
        std::string getType() const;
        bool isAvailable() const;

        void setName(const std::string& name);
        void setType(const std::string& type);
        void setAvailability(bool available);

        void displayInfo() const;
        std::string toString() const;
};

#endif