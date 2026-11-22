#include "Collection.hpp"
#include <algorithm>

Collection::Collection() : _name(""), _next_id(1) {}

Collection::Collection(const std::string& name) : _name(name), _next_id(1) {}

std::string Collection::generateId() {
    return std::to_string(_next_id++);
}

void Collection::indexDocument(const Document& doc) {
    std::string doc_id = doc.id();
    for (auto& idx : _indexes) {
        const std::string& field = idx.first;
        if (doc.has(field)) {
            std::string key = doc.get(field).toIndexKey();
            idx.second[key].insert(doc_id);
        }
    }
}

void Collection::unindexDocument(const Document& doc) {
    std::string doc_id = doc.id();
    for (auto& idx : _indexes) {
        const std::string& field = idx.first;
        if (doc.has(field)) {
            std::string key = doc.get(field).toIndexKey();
            auto it = idx.second.find(key);
            if (it != idx.second.end()) {
                it->second.erase(doc_id);
                if (it->second.empty())
                    idx.second.erase(it);
            }
        }
    }
}

std::string Collection::insert(Document doc) {
    if (!doc.has("_id") || doc.id().empty())
        doc.setId(generateId());
    std::string id = doc.id();
    _docs[id] = doc;
    indexDocument(doc);
    return id;
}

Document* Collection::find(const std::string& id) {
    auto it = _docs.find(id);
    if (it != _docs.end()) return &it->second;
    return nullptr;
}

std::vector<Document> Collection::filter(const std::string& field,
                                         const std::string& value) const {
    std::vector<Document> results;

    auto idx_it = _indexes.find(field);
    if (idx_it != _indexes.end()) {
        auto val_it = idx_it->second.find(value);
        if (val_it != idx_it->second.end()) {
            for (const std::string& doc_id : val_it->second) {
                auto doc_it = _docs.find(doc_id);
                if (doc_it != _docs.end())
                    results.push_back(doc_it->second);
            }
        }
        return results;
    }

    for (const auto& pair : _docs) {
        if (pair.second.has(field) &&
            pair.second.get(field).toIndexKey() == value)
            results.push_back(pair.second);
    }
    return results;
}

std::vector<Document> Collection::search(const std::string& field,
                                         const std::string& value) const {
    std::vector<Document> results;
    std::string search_lower;
    for (char c : value)
        search_lower += (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;

    for (const auto& pair : _docs) {
        if (pair.second.has(field)) {
            std::string field_val = pair.second.get(field).toIndexKey();
            std::string field_lower;
            for (char c : field_val)
                field_lower += (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
            if (field_lower.find(search_lower) != std::string::npos)
                results.push_back(pair.second);
        }
    }
    return results;
}

bool Collection::update(const std::string& id,
                        const std::map<std::string, Value>& updates) {
    auto it = _docs.find(id);
    if (it == _docs.end()) return false;

    unindexDocument(it->second);
    for (const auto& pair : updates) {
        if (pair.first == "_id") continue;
        it->second.set(pair.first, pair.second);
    }
    indexDocument(it->second);
    return true;
}

bool Collection::remove(const std::string& id) {
    auto it = _docs.find(id);
    if (it == _docs.end()) return false;

    unindexDocument(it->second);
    _docs.erase(it);
    return true;
}

std::vector<Document> Collection::all() const {
    std::vector<Document> results;
    results.reserve(_docs.size());
    for (const auto& pair : _docs)
        results.push_back(pair.second);

    std::sort(results.begin(), results.end(),
        [](const Document& a, const Document& b) {
            try { return std::stoi(a.id()) < std::stoi(b.id()); }
            catch (...) { return a.id() < b.id(); }
        });
    return results;
}

int Collection::count() const { return _docs.size(); }

bool Collection::exists(const std::string& id) const {
    return _docs.find(id) != _docs.end();
}

void Collection::createIndex(const std::string& field) {
    if (_indexes.find(field) != _indexes.end()) return;
    _indexes[field] = {};
    for (const auto& pair : _docs) {
        if (pair.second.has(field)) {
            std::string key = pair.second.get(field).toIndexKey();
            _indexes[field][key].insert(pair.first);
        }
    }
}

bool Collection::hasIndex(const std::string& field) const {
    return _indexes.find(field) != _indexes.end();
}

std::vector<std::string> Collection::indexedFields() const {
    std::vector<std::string> fields;
    for (const auto& pair : _indexes)
        fields.push_back(pair.first);
    return fields;
}

const std::string& Collection::name() const { return _name; }

void Collection::clear() {
    _docs.clear();
    for (auto& idx : _indexes)
        idx.second.clear();
    _next_id = 1;
}

void Collection::setNextId(unsigned int id) { _next_id = id; }

unsigned int Collection::nextId() const { return _next_id; }
