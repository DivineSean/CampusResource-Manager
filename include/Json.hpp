#ifndef JSON_HPP
#define JSON_HPP

#include "Value.hpp"
#include "Document.hpp"
#include <string>

class Json {
    public:
        static std::string serialize(const Value& value);
        static std::string serialize(const Document& doc);

        static Value parse(const std::string& json);
        static Document parseDocument(const std::string& json);

    private:
        static std::string serializeValue(const Value& value);
        static std::string escapeString(const std::string& str);

        static Value parseValue(const std::string& json, size_t& pos);
        static void skipWhitespace(const std::string& json, size_t& pos);
        static std::string parseString(const std::string& json, size_t& pos);
        static Value parseNumber(const std::string& json, size_t& pos);
        static Value parseArray(const std::string& json, size_t& pos);
        static Value parseObject(const std::string& json, size_t& pos);
        static Value parseLiteral(const std::string& json, size_t& pos);
};

#endif
