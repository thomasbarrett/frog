#ifndef JSON_ACTION_H
#define JSON_ACTION_H

#include "./json.hpp"
#include "./grammar.hpp"
#include "../Frog.h"

namespace json::action {

    int Number(const frog::ParseContext &ctx, frog::Result res) {
        std::string lexeme { ctx.str.substr(res.start, res.length) };
        return std::stoi(lexeme);
    }

    std::string String(const frog::ParseContext &ctx, frog::Result res) {
        std::string lexeme { ctx.str.substr(res.start + 1, res.length - 2) };
        return lexeme;
    }

    json::Value Value(const frog::ParseContext &ctx, frog::Result res);

    json::Array ValueList(const frog::ParseContext &ctx, frog::Result res) {
        auto choice = res.children[0];
        switch (choice.index) {
            case 0: {
                json::Array result = ValueList(ctx, choice.children[0].children[2]);
                result.insert(result.begin(), Value(ctx, choice.children[0].children[0]));
                return result;
            }
            case 1: return { Value(ctx, choice.children[0]) };
            default: throw std::logic_error{""};
        }
    }

    json::Array Array(const frog::ParseContext &ctx, frog::Result res) {
        auto choice = res.children[0];
        switch (choice.index) {
            case 0:
                return { ValueList(ctx, choice.children[0].children[1]) };
            case 1: 
            return {};
            default: throw std::logic_error{""};
        }
    }

    std::pair<const std::string, json::Value> Property(const frog::ParseContext &ctx, frog::Result res) {
        auto sequence = res.children[0];
        return { String(ctx, sequence.children[0]), Value(ctx, sequence.children[2]) };
    }

    json::Object PropertyList(const frog::ParseContext &ctx, frog::Result res) {
        auto choice = res.children[0];
        switch (choice.index) {
            case 0: {
                json::Object result = PropertyList(ctx, choice.children[0].children[2]);
                result.emplace(Property(ctx, choice.children[0].children[0]));
                return result;
            }
            case 1:
                return { Property(ctx, choice.children[0]) };
            default: throw std::logic_error{""};
        }
    }

    json::Object Object(const frog::ParseContext &ctx, frog::Result res) {
        auto choice = res.children[0];
        auto sequence = choice.children[0];
        switch (choice.index) {
            case 0: return { PropertyList(ctx, sequence.children[1]) };
            case 1: return {};
            default: throw std::logic_error{""};
        }
    }

    json::Value Value(const frog::ParseContext &ctx, frog::Result res) {
        auto choice = res.children[0];
        switch (choice.index) {
            case 0: return Number(ctx, choice.children[0]);
            case 1: return String(ctx, choice.children[0]);
            case 2: return Array(ctx, choice.children[0]);
            case 3: return Object(ctx, choice.children[0]);
            default: throw std::logic_error {""};
        }
    };

    json::Value Start(const frog::ParseContext &ctx, frog::Result res) {
        auto sequence = res.children[0];
        return Value(ctx, sequence.children[0]);
    } 
}

#endif /* JSON_ACTION_H */