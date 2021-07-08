#include "blell.hpp"

#include <iostream>
#include <string>
#include <optional>

int main(void) {
    std::string line;
     //blelll;

    while(true) {
        std::getline(std::cin, line);
        std::cout << line << '\n';
        std::optional<struct blell> blell = parseMessage(line);
        std::cout << *blell << std::endl << std::endl;
    }

    return 0;
}
