# Frog

The Frog parsing library defines pattern types allowing the user to define
parsing expression grammars. This library is non-opinionated and imposes
very little structure on the user. The goal of Frog is to allow users to
get a project up and running as quickly as possible.

Parsing complex grammars is a tedious task which is only faced on occassion
by most programmers. Moreover, parsing tasks in need of high performance or
good error handling are often implemented as hand written recursive descent
parsers. A good example of this is the clang compiler. Rather than create a
feature rich library with documentation that nobody wants to read, Frog
attempts to create a parsing infrastructure that can be used right away.

Other popular C++ parsing libraries include boost::spirit and tao::pegtl.
Unlike boost::spirit, the Frog library is light weight and header only which
allows easy integration into projects of all sizes. The Frog library differs
from tao::pegtl since it uses functions.

## Usage
This is a simple example grammar for parsing expression with operator precedence.
Note that since PEG grammars cannot handle left recursion, epsilon rules are
used to remove left recursion.
```c++

namespace expression::grammar { 

    using namespace frog;

    Pattern Number() {
        return RegularExpression {"^[0-9]+"};
    }

    Pattern Expression();

    Pattern Factor() {
        return Sequence {'(', Expression, ')'}
             / Number;
    }

    Pattern Term_() {
        return Sequence {'*', Factor, Term_}
             / Sequence {'/', Factor, Term_}
             / Epsilon {};
    }

    Pattern Term() {
        return Sequence {Factor, Term_};
    }

    Pattern Expression_() {
        return Sequence {'+', Term, Expression_}
             / Sequence {'-', Term, Expression_}
             / Epsilon {};
    }

    Pattern Expression() {
        return Sequence{Term, Expression_};
    }

};

```