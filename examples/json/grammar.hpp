#ifndef JSON_GRAMMAR_H
#define JSON_GRAMMAR_H

#include "../Frog.h"

namespace json::grammar { 

    frog::Pattern Number() {
        return frog::RegularExpression {"^[0-9]+"};
    }

    frog::Pattern String() {
        return frog::RegularExpression {"^\\\"[^\\\"]*\\\""};
    }

    frog::Pattern Array();
    frog::Pattern Object();
    
    frog::Pattern Value() {
        return frog::Choice {Number, String, Array, Object};
    }

    frog::Pattern ValueList() {
        return frog::Choice {
            frog::Sequence {Value, ',', ValueList},
            Value
        };
    }
    
    frog::Pattern Array() {
        return frog::Choice {
            frog::Sequence {'[', ValueList, ']'},
            frog::Sequence {'[', ']'}
        };
    }

    frog::Pattern Property() {
        return frog::Sequence {String, ':', Value};
    }

    frog::Pattern PropertyList() {
        return frog::Choice {
            frog::Sequence {Property, ',', PropertyList},
            Property
        };
    }

    frog::Pattern Object() {
        return frog::Choice {
            frog::Sequence {'{', PropertyList, '}' },
            frog::Sequence {'{', '}'}
        };
    }

    frog::Pattern Start() {
        return frog::Sequence {Value, '\0'};
    }

};

#endif /* JSON_GRAMMAR_H */