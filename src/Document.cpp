#include "Document.hpp"
#include <iostream>

Document::Document() {}

Document::Document(const std::map<std::string, Value>& fields) : _fields(fields) {}

void Document::set(const std::string& key, const Value& value) {
    _fields[key] = value;
}

Value Document::get(const std::string& key) const {
    auto it = _fields.find(key);
    if (it != _fields.end()) return it->second;
    return Value();
}

bool Document::has(const std::string& key) const {
    return _fields.find(key) != _fields.end();
}

void Document::remove(const std::string& key) {
    _fields.erase(key);
}

std::string Document::id() const {
    auto it = _fields.find("_id");
    if (it != _fields.end() && it->second.isString())
        return it->second.asString();
    return "";
}

void Document::setId(const std::string& id) {
    _fields["_id"] = Value(id);
}

const std::map<std::string, Value>& Document::fields() const {
    return _fields;
}

Value Document::toValue() const {
    return Value(_fields);
}

Document Document::fromValue(const Value& val) {
    if (val.isObject())
        return Document(val.asObject());
    return Document();
}

void Document::display() const {
    std::cout << "=================================================" << std::endl;
    for (const auto& pair : _fields) {
        std::cout << "  " << pair.first << ": "
                  << pair.second.toDisplayString() << std::endl;
    }
    std::cout << "=================================================" << std::endl;
}
