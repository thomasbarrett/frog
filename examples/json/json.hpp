#ifndef JSON_H
#define JSON_H

#include <vector>
#include <map>
#include <variant>
#include <iostream>

namespace json {

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
 
    struct Array;
    struct Object;

    using Value = std::variant<int, double, std::string, Array, Object>; 

    struct Array: public std::vector<Value> {
        Array(): std::vector<Value>{} {}
        Array(const std::initializer_list<Value> &e): std::vector<Value> {e} {}
        Array(const Array &arr): std::vector<Value> {arr} {}
        Array(Array &&arr): std::vector<Value> {arr} {}
        Array& operator=(const Array& arr) = default;	
    };

    struct Object: public std::map<std::string, Value> {
        Object(): std::map<std::string, Value> {} {}
        Object(const std::initializer_list<std::pair<const std::string, Value>> &e): std::map<std::string, Value> {e} {}
        Object(const Object &obj): std::map<std::string, Value> {obj} {}
        Object(Object &&obj): std::map<std::string, Value> {obj} {}
        Object& operator=(const Object& obj) = default;	
    };

    std::ostream& operator<<(std::ostream& os, const json::Value& value) {
        std::visit(overloaded {
            [&os](int v) { os << v; },
            [&os](double v) { os << v; },
            [&os](std::string v) { os << '"' << v << '"'; },
            [&os](Array v) {
                os << '[';
                for (auto it = v.begin(); it != v.end(); it++) {
                    if (it != v.begin()) os << ",";
                    os << *it;
                }
                os << ']';
            },
            [&os](Object v) {
                os << '{';
                for (auto it = v.begin(); it != v.end(); it++) {
                    if (it != v.begin()) os << ",";
                    os << '"' << it->first << "\":" << it->second;
                }
                os << '}';
            }
        }, value);
        return os;
    } 
}

#endif /* JSON_H */