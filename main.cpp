#include "blell.hpp"

#include <iostream>
#include <string>
#include <optional>

int main(void) {
    std::string line;

    while(true) {
        std::getline(std::cin, line);
        std::optional<struct blell> blell = parseMessage(line);
        if(blell) {
            std::cout << *blell << std::endl << std::endl;
        }
    }
}
