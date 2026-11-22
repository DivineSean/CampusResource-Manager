#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "Collection.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Database {
    private:
        std::string _name;
        std::unordered_map<std::string, Collection> _collections;
        std::string _data_dir;

    public:
        Database(const std::string& name, const std::string& data_dir = "data");

        Collection& createCollection(const std::string& name);
        Collection& getCollection(const std::string& name);
        const Collection& getCollection(const std::string& name) const;
        bool hasCollection(const std::string& name) const;
        bool dropCollection(const std::string& name);
        std::vector<std::string> collectionNames() const;

        void save() const;
        void load();

        const std::string& name() const;
        const std::string& dataDir() const;
};

#endif
