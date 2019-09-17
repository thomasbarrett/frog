#ifndef JSON_GRAMMAR_H
#define JSON_GRAMMAR_H

#include "../Frog.h"

namespace expression::grammar { 

    frog::Pattern Number() {
        return frog::RegularExpression {"^[0-9]+"};
    }

    frog::Pattern Expression();

    frog::Pattern Factor() {
        return Number / frog::Sequence{'(', Expression, ')'};
    }

    frog::Pattern Term_() {
        return frog::Sequence {'*', Factor, Term_}
             / frog::Sequence {'/', Factor, Term_}
             / frog::Epsilon {};
    }

    frog::Pattern Term() {
        return frog::Sequence {Factor, Term_};
    }

    frog::Pattern Expression_() {
        return frog::Sequence {'+', Term, Expression_}
             / frog::Sequence {'-', Term, Expression_}
             / frog::Epsilon {};
    }

    frog::Pattern Expression() {
        return frog::Sequence{Term, Expression_};
    }

};

namespace expression::action {

    int Number(frog::Result tree) {
        std::string lexeme { tree.str.substr(tree.start, tree.length) };
        return std::stoi(lexeme);
    }

    int Expression(frog::Result tree);

    int Factor(frog::Result tree) {
        switch (tree.index) {
            case 0: return Number(tree);
            case 1: return Expression(tree[1]);
            default: throw std::logic_error{"unreachable"};
        }
    }

    int Term_(int x, frog::Result tree) {
        switch (tree.index) {
            case 0: return Term_(x * Factor(tree[1]), tree[2]);
            case 1: return Term_(x / Factor(tree[1]), tree[2]);
            case 2: return x;
            default: throw std::logic_error{"unreachable"};
        }
    }

    int Term(frog::Result tree) {
        return Term_(Factor(tree[0]), tree[1]);
    }

    int Expression_(int x, frog::Result tree) {
        switch (tree.index) {
            case 0: return Expression_(x + Term(tree[1]), tree[2]);
            case 1: return Expression_(x - Term(tree[1]), tree[2]);
            case 2: return x;
            default: throw std::logic_error{"unreachable"};
        }
    }

    int Expression(frog::Result tree) {
        return Expression_(Term(tree[0]), tree[1]);
    }
}

#endif /* JSON_GRAMMAR_H */