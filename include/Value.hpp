#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <vector>
#include <map>

enum class ValueType { Null, Bool, Int, Double, String, Array, Object };

class Value {
    private:
        ValueType _type;
        bool _bool_val;
        int _int_val;
        double _double_val;
        std::string _string_val;
        std::vector<Value> _array_val;
        std::map<std::string, Value> _object_val;

    public:
        Value();
        Value(bool val);
        Value(int val);
        Value(double val);
        Value(const std::string& val);
        Value(const char* val);
        Value(const std::vector<Value>& val);
        Value(const std::map<std::string, Value>& val);

        ValueType type() const;
        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isDouble() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;
        bool isNumber() const;

        bool asBool() const;
        int asInt() const;
        double asDouble() const;
        const std::string& asString() const;
        const std::vector<Value>& asArray() const;
        std::vector<Value>& asArray();
        const std::map<std::string, Value>& asObject() const;
        std::map<std::string, Value>& asObject();

        std::string toIndexKey() const;
        std::string toDisplayString() const;

        bool operator==(const Value& other) const;
        bool operator!=(const Value& other) const;
        bool operator<(const Value& other) const;
};

#endif
