/**
 * @file Frog.h
 *
 * @brief Single header parsing library focusing on simplicity.
 *
 * The Frog parsing library defines pattern types allowing the user to define
 * parsing expression grammars. This library is non-opinionated and imposes
 * very little structure on the user. The goal of Frog is to allow users to
 * get a project up and running as quickly as possible.
 * 
 * Parsing complex grammars is a tedious task which is only faced on occassion
 * by most programmers. Moreover, parsing tasks in need of high performance or
 * good error handling are often implemented as hand written recursive descent
 * parsers. A good example of this is the clang compiler. Rather than create a
 * feature rich library with documentation that nobody wants to read, Frog
 * attempts to create a parsing infrastructure that can be used right away.
 * 
 * Other popular C++ parsing libraries include boost::spirit and tao::pegtl.
 * Unlike boost::spirit, the Frog library is light weight and header only which
 * allows easy integration into projects of all sizes. The Frog library differs
 * from tao::pegtl since it uses functions.
 * 
 * @author Thomas Barrett
 */

#ifndef FROG_H
#define FROG_H

#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <variant>

namespace frog {

    using Character = char;
    using CharacterSet = std::vector<char>;
    using String = std::string;
    using RegularExpression = std::regex;

    struct Choice;
    struct Sequence;
    struct Epsilon {};

    using Pattern = std::variant<
        Character,
        CharacterSet,
        String,
        RegularExpression,
        Choice,
        Sequence,
        Epsilon
    >;

    using Symbol = std::variant<Pattern, Pattern (*)()>;

    struct Sequence: public std::vector<Symbol> {
        Sequence(): std::vector<Symbol>{} {}
        Sequence(const Sequence &e): std::vector<Symbol>{e} {}
        Sequence(Sequence &&e): std::vector<Symbol>{e} {}
        Sequence(std::initializer_list<Symbol> e) : std::vector<Symbol>{e} {}
    };
    
    struct Choice: public std::vector<Symbol> {
        Choice(): std::vector<Symbol>{} {}
        Choice(const Choice &e): std::vector<Symbol>{e} {}
        Choice(Choice &&e): std::vector<Symbol>{e} {}
        Choice(std::initializer_list<Symbol> e) : std::vector<Symbol>{e} {}

    };

    struct Result {
        bool success;
        int index;
        std::string_view str;
        size_t start;
        size_t length;
        std::vector<Result> children;

        const Result& operator[](int i) const {
            return children[i];
        }
    };

    struct ParseContext {
        const std::string &str;
        size_t &loc;
    };

    
    Result ParseSymbol(const ParseContext &ctx, Symbol sym);

    /**
     * Parse a single character in the given context.
     * 
     * @param ctx: the context
     * @param c: the character
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, Character c) {
        if(ctx.str[ctx.loc] == c) {
            return {true, 0, ctx.str, ctx.loc++, 1};
        } else {
            return {false, 0, ctx.str, ctx.loc, 0};
        }
    }

    /**
     * Parse a character from a set in the given context.
     * 
     * @param ctx: the context
     * @param c: the character set
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, const CharacterSet &chars) {
        if(std::find(chars.begin(), chars.end(), ctx.str[ctx.loc]) != chars.end()) {
            return {true, 0, ctx.str, ctx.loc++, 1};
        } else {
            return {false, 0, ctx.str, ctx.loc, 0};
        }
    }

    /**
     * Parse a string from in the given context.
     * 
     * @param ctx: the context
     * @param c: the string
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, const String &s) {
        if (ctx.str.substr(ctx.loc, s.size()) == s) {
            size_t start_loc = ctx.loc;
            ctx.loc += s.size();
            return {true, 0, ctx.str, start_loc, s.size()};
        } else {
            return {false, 0, ctx.str, ctx.loc, 0};
        }
    }

    /**
     * Parse a regular expression from in the given context.
     * 
     * @param ctx: the context
     * @param c: the string
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, const RegularExpression &rgx) {
        std::match_results<decltype(ctx.str.begin())> m;
        if (std::regex_search(ctx.str.begin() + ctx.loc, ctx.str.end(), m, rgx)) {
            size_t start_loc = ctx.loc;
            ctx.loc += m.length(0);
            return {true, 0, ctx.str, start_loc, static_cast<size_t>(m.length())};
        } else {
            return {false, 0, ctx.str, ctx.loc, 0};
        } 
    }

    /**
     * Parse a pattern from a list of choices from in the given context.
     * 
     * The choice operation is ordered. This operation will choose the
     * first pattern to succeed. In the case of error, the longest errenous
     * pattern will be chosen.
     * 
     * @param ctx: the context
     * @param c: the string
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, const Choice &c) {

        int index = 0;
        Result longest;
        for (auto sym: c) {
            long temp_loc = ctx.loc;
            auto result = ParseSymbol(ctx, sym);
            result.index = index;
            if (result.success) {
                return result;
            } else if (result.length > longest.length) {
                longest = result;
            }
            ctx.loc = temp_loc;
            index++;
        }

        return longest;
    }

    /**
     * Parse a sequence of patterns in the given context.
     * 
     * @param ctx: the context
     * @param c: the string
     * @return the parse tree
     */
    Result Parse(const ParseContext &ctx, const Sequence &seq) {
        std::vector<Result> children;
        bool success = true;

        for (auto s: seq) {
            Result result = ParseSymbol(ctx, s);
            children.push_back(result);
            if (!result.success) {
                success = false;
                break;
            }
        }

        if (children.size() > 0) {
            size_t start = children.front().start;
            size_t length = children.back().start + children.back().length - start;
            return {success, 0, ctx.str, start, length, children};
        } else {
            return {success, 0, ctx.str, ctx.loc, 0};
        }
    }

    Result Parse(const ParseContext &ctx, const Epsilon &s) {
        return {true, 0, ctx.str, ctx.loc, 0};
    }

    Result Parse(const ParseContext &ctx, const Pattern &p) {
        return std::visit([&ctx](const auto &p) -> Result {
            return Parse(ctx, p);
        }, p);
    }

    Result ParseSymbol(const ParseContext &ctx, Symbol sym) {
        return std::visit([&ctx](auto sym) -> Result {
            using T = std::decay_t<decltype(sym)>;
            if constexpr (std::is_same_v<T, Pattern>) {
                return Parse(ctx, sym);
            } else {
                return Parse({ctx.str, ctx.loc}, sym());
            }
        }, sym);
    }

    Result Parse(const std::string &str, Symbol sym) {
        size_t loc = 0;
        return ParseSymbol({str, loc}, sym);
    }

    Choice operator/(Choice a, Symbol b) {
        a.push_back(b);
        return a;
    }

    Choice operator/(Symbol a, Symbol b) {
        Choice choice;
        choice.push_back(a);
        choice.push_back(b);
        return std::move(choice);
    }

}

#endif /* FROG_H */