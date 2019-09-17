#include "./expression/grammar.hpp"
#include "./Frog.h"

#include <iostream>
#include <map>
#include <string>
#include <exception>
#include <sstream>
#include <utility>
#include <chrono>

int main(int argc, char* argv[]) {
    
    std::string str = std::string(argv[1]);
    auto result = frog::Parse(str, expression::grammar::Expression);
   
    if (result.success) {
        size_t loc = 0;
        int x = expression::action::Expression(result);
        std::cout << x << std::endl;
    } else {
        std::cout << "error: unable to parse" << std::endl;
    }
}