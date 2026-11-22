#include "Value.hpp"
#include <sstream>

Value::Value() : _type(ValueType::Null), _bool_val(false), _int_val(0), _double_val(0.0) {}
Value::Value(bool val) : _type(ValueType::Bool), _bool_val(val), _int_val(0), _double_val(0.0) {}
Value::Value(int val) : _type(ValueType::Int), _bool_val(false), _int_val(val), _double_val(0.0) {}
Value::Value(double val) : _type(ValueType::Double), _bool_val(false), _int_val(0), _double_val(val) {}
Value::Value(const std::string& val) : _type(ValueType::String), _bool_val(false), _int_val(0), _double_val(0.0), _string_val(val) {}
Value::Value(const char* val) : _type(ValueType::String), _bool_val(false), _int_val(0), _double_val(0.0), _string_val(val) {}
Value::Value(const std::vector<Value>& val) : _type(ValueType::Array), _bool_val(false), _int_val(0), _double_val(0.0), _array_val(val) {}
Value::Value(const std::map<std::string, Value>& val) : _type(ValueType::Object), _bool_val(false), _int_val(0), _double_val(0.0), _object_val(val) {}

ValueType Value::type() const { return _type; }
bool Value::isNull() const { return _type == ValueType::Null; }
bool Value::isBool() const { return _type == ValueType::Bool; }
bool Value::isInt() const { return _type == ValueType::Int; }
bool Value::isDouble() const { return _type == ValueType::Double; }
bool Value::isString() const { return _type == ValueType::String; }
bool Value::isArray() const { return _type == ValueType::Array; }
bool Value::isObject() const { return _type == ValueType::Object; }
bool Value::isNumber() const { return _type == ValueType::Int || _type == ValueType::Double; }

bool Value::asBool() const { return _bool_val; }
int Value::asInt() const { return _int_val; }
double Value::asDouble() const { return _double_val; }
const std::string& Value::asString() const { return _string_val; }
const std::vector<Value>& Value::asArray() const { return _array_val; }
std::vector<Value>& Value::asArray() { return _array_val; }
const std::map<std::string, Value>& Value::asObject() const { return _object_val; }
std::map<std::string, Value>& Value::asObject() { return _object_val; }

std::string Value::toIndexKey() const {
    switch (_type) {
        case ValueType::Null:   return "null";
        case ValueType::Bool:   return _bool_val ? "true" : "false";
        case ValueType::Int:    return std::to_string(_int_val);
        case ValueType::Double: return std::to_string(_double_val);
        case ValueType::String: return _string_val;
        default: return "";
    }
}

std::string Value::toDisplayString() const {
    switch (_type) {
        case ValueType::Null: return "null";
        case ValueType::Bool: return _bool_val ? "true" : "false";
        case ValueType::Int:  return std::to_string(_int_val);
        case ValueType::Double: {
            std::ostringstream oss;
            oss << _double_val;
            return oss.str();
        }
        case ValueType::String: return _string_val;
        case ValueType::Array: {
            std::string result = "[";
            for (size_t i = 0; i < _array_val.size(); i++) {
                if (i > 0) result += ", ";
                result += _array_val[i].toDisplayString();
            }
            return result + "]";
        }
        case ValueType::Object: {
            std::string result = "{";
            bool first = true;
            for (const auto& pair : _object_val) {
                if (!first) result += ", ";
                result += pair.first + ": " + pair.second.toDisplayString();
                first = false;
            }
            return result + "}";
        }
    }
    return "";
}

bool Value::operator==(const Value& other) const {
    if (_type != other._type) return false;
    switch (_type) {
        case ValueType::Null:   return true;
        case ValueType::Bool:   return _bool_val == other._bool_val;
        case ValueType::Int:    return _int_val == other._int_val;
        case ValueType::Double: return _double_val == other._double_val;
        case ValueType::String: return _string_val == other._string_val;
        case ValueType::Array:  return _array_val == other._array_val;
        case ValueType::Object: return _object_val == other._object_val;
    }
    return false;
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

bool Value::operator<(const Value& other) const {
    if (_type != other._type) return static_cast<int>(_type) < static_cast<int>(other._type);
    switch (_type) {
        case ValueType::Null:   return false;
        case ValueType::Bool:   return _bool_val < other._bool_val;
        case ValueType::Int:    return _int_val < other._int_val;
        case ValueType::Double: return _double_val < other._double_val;
        case ValueType::String: return _string_val < other._string_val;
        default: return false;
    }
}
