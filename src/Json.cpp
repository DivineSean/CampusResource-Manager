#include "Json.hpp"
#include <sstream>
#include <stdexcept>

std::string Json::serialize(const Value& value) {
    return serializeValue(value);
}

std::string Json::serialize(const Document& doc) {
    return serializeValue(doc.toValue());
}

std::string Json::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n";  break;
            case '\t': result += "\\t";  break;
            case '\r': result += "\\r";  break;
            default:   result += c;
        }
    }
    return result;
}

std::string Json::serializeValue(const Value& value) {
    switch (value.type()) {
        case ValueType::Null:
            return "null";
        case ValueType::Bool:
            return value.asBool() ? "true" : "false";
        case ValueType::Int:
            return std::to_string(value.asInt());
        case ValueType::Double: {
            std::ostringstream oss;
            oss << value.asDouble();
            return oss.str();
        }
        case ValueType::String:
            return "\"" + escapeString(value.asString()) + "\"";
        case ValueType::Array: {
            std::string result = "[";
            const auto& arr = value.asArray();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) result += ", ";
                result += serializeValue(arr[i]);
            }
            return result + "]";
        }
        case ValueType::Object: {
            std::string result = "{";
            const auto& obj = value.asObject();
            bool first = true;
            for (const auto& pair : obj) {
                if (!first) result += ", ";
                result += "\"" + escapeString(pair.first) + "\": "
                        + serializeValue(pair.second);
                first = false;
            }
            return result + "}";
        }
    }
    return "null";
}

Value Json::parse(const std::string& json) {
    size_t pos = 0;
    skipWhitespace(json, pos);
    return parseValue(json, pos);
}

Document Json::parseDocument(const std::string& json) {
    return Document::fromValue(parse(json));
}

void Json::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t' ||
            json[pos] == '\n' || json[pos] == '\r'))
        pos++;
}

Value Json::parseValue(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);
    if (pos >= json.size())
        throw std::runtime_error("Unexpected end of JSON");

    char c = json[pos];
    if (c == '"')                            return Value(parseString(json, pos));
    if (c == '{')                            return parseObject(json, pos);
    if (c == '[')                            return parseArray(json, pos);
    if (c == '-' || (c >= '0' && c <= '9'))  return parseNumber(json, pos);
    return parseLiteral(json, pos);
}

std::string Json::parseString(const std::string& json, size_t& pos) {
    pos++;
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') {
            pos++;
            if (pos >= json.size())
                throw std::runtime_error("Unexpected end of string escape");
            switch (json[pos]) {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  result += '\r'; break;
                case '/':  result += '/';  break;
                default:   result += json[pos];
            }
        } else {
            result += json[pos];
        }
        pos++;
    }
    if (pos >= json.size())
        throw std::runtime_error("Unterminated string");
    pos++;
    return result;
}

Value Json::parseNumber(const std::string& json, size_t& pos) {
    size_t start = pos;
    bool is_double = false;

    if (json[pos] == '-') pos++;
    while (pos < json.size() && json[pos] >= '0' && json[pos] <= '9') pos++;

    if (pos < json.size() && json[pos] == '.') {
        is_double = true;
        pos++;
        while (pos < json.size() && json[pos] >= '0' && json[pos] <= '9') pos++;
    }

    std::string num_str = json.substr(start, pos - start);
    if (is_double) return Value(std::stod(num_str));
    return Value(std::stoi(num_str));
}

Value Json::parseArray(const std::string& json, size_t& pos) {
    pos++;
    std::vector<Value> arr;

    skipWhitespace(json, pos);
    if (pos < json.size() && json[pos] == ']') { pos++; return Value(arr); }

    while (true) {
        arr.push_back(parseValue(json, pos));
        skipWhitespace(json, pos);
        if (pos >= json.size())
            throw std::runtime_error("Unterminated array");
        if (json[pos] == ']') { pos++; break; }
        if (json[pos] != ',')
            throw std::runtime_error("Expected ',' in array");
        pos++;
    }
    return Value(arr);
}

Value Json::parseObject(const std::string& json, size_t& pos) {
    pos++;
    std::map<std::string, Value> obj;

    skipWhitespace(json, pos);
    if (pos < json.size() && json[pos] == '}') { pos++; return Value(obj); }

    while (true) {
        skipWhitespace(json, pos);
        std::string key = parseString(json, pos);

        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != ':')
            throw std::runtime_error("Expected ':' after key");
        pos++;

        obj[key] = parseValue(json, pos);

        skipWhitespace(json, pos);
        if (pos >= json.size())
            throw std::runtime_error("Unterminated object");
        if (json[pos] == '}') { pos++; break; }
        if (json[pos] != ',')
            throw std::runtime_error("Expected ',' in object");
        pos++;
    }
    return Value(obj);
}

Value Json::parseLiteral(const std::string& json, size_t& pos) {
    if (json.compare(pos, 4, "true") == 0)  { pos += 4; return Value(true); }
    if (json.compare(pos, 5, "false") == 0) { pos += 5; return Value(false); }
    if (json.compare(pos, 4, "null") == 0)  { pos += 4; return Value(); }
    throw std::runtime_error(
        "Unexpected character '" + std::string(1, json[pos]) + "' at position " + std::to_string(pos));
}
