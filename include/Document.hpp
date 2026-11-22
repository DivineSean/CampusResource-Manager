#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include "Value.hpp"
#include <string>
#include <map>

class Document {
    private:
        std::map<std::string, Value> _fields;

    public:
        Document();
        Document(const std::map<std::string, Value>& fields);

        void set(const std::string& key, const Value& value);
        Value get(const std::string& key) const;
        bool has(const std::string& key) const;
        void remove(const std::string& key);

        std::string id() const;
        void setId(const std::string& id);

        const std::map<std::string, Value>& fields() const;

        Value toValue() const;
        static Document fromValue(const Value& val);

        void display() const;
};

#endif
