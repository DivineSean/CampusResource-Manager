#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include "Document.hpp"
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>

class Collection {
    private:
        std::string _name;
        std::unordered_map<std::string, Document> _docs;
        std::map<std::string, std::map<std::string, std::set<std::string>>> _indexes;
        unsigned int _next_id;

        std::string generateId();
        void indexDocument(const Document& doc);
        void unindexDocument(const Document& doc);

    public:
        Collection();
        Collection(const std::string& name);

        std::string insert(Document doc);
        Document* find(const std::string& id);
        std::vector<Document> filter(const std::string& field, const std::string& value) const;
        std::vector<Document> search(const std::string& field, const std::string& value) const;
        bool update(const std::string& id, const std::map<std::string, Value>& updates);
        bool remove(const std::string& id);

        std::vector<Document> all() const;
        int count() const;
        bool exists(const std::string& id) const;

        void createIndex(const std::string& field);
        bool hasIndex(const std::string& field) const;
        std::vector<std::string> indexedFields() const;

        const std::string& name() const;

        void clear();
        void setNextId(unsigned int id);
        unsigned int nextId() const;
};

#endif
