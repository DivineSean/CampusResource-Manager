#include "Database.hpp"
#include "Json.hpp"
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

Database::Database(const std::string& name, const std::string& data_dir)
    : _name(name), _data_dir(data_dir) {}

Collection& Database::createCollection(const std::string& name) {
    if (_collections.find(name) == _collections.end())
        _collections[name] = Collection(name);
    return _collections[name];
}

Collection& Database::getCollection(const std::string& name) {
    return _collections.at(name);
}

const Collection& Database::getCollection(const std::string& name) const {
    return _collections.at(name);
}

bool Database::hasCollection(const std::string& name) const {
    return _collections.find(name) != _collections.end();
}

bool Database::dropCollection(const std::string& name) {
    return _collections.erase(name) > 0;
}

std::vector<std::string> Database::collectionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : _collections)
        names.push_back(pair.first);
    return names;
}

void Database::save() const {
    MKDIR(_data_dir.c_str());

    for (const auto& pair : _collections) {
        std::string filepath = _data_dir + "/" + pair.first + ".jsonl";
        std::ofstream file(filepath);
        if (!file.is_open()) continue;

        std::vector<Document> docs = pair.second.all();
        for (const auto& doc : docs)
            file << Json::serialize(doc) << "\n";
        file.close();
    }
}

void Database::load() {
    for (auto& pair : _collections) {
        std::string filepath = _data_dir + "/" + pair.first + ".jsonl";
        std::ifstream file(filepath);
        if (!file.is_open()) continue;

        pair.second.clear();
        std::string line;
        unsigned int max_id = 0;

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            try {
                Document doc = Json::parseDocument(line);
                if (doc.has("_id")) {
                    try {
                        unsigned int num_id = std::stoi(doc.id());
                        if (num_id > max_id) max_id = num_id;
                    } catch (...) {}
                }
                pair.second.insert(doc);
            } catch (const std::exception& e) {
                std::cerr << "Warning: skipping malformed document in "
                          << pair.first << ": " << e.what() << std::endl;
            }
        }

        pair.second.setNextId(max_id + 1);
        file.close();
    }
}

const std::string& Database::name() const { return _name; }
const std::string& Database::dataDir() const { return _data_dir; }
