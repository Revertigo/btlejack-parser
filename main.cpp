#include "blell.hpp"

#include <iostream>
#include <string>
#include <optional>

int main(void) {
//    std::string line;
//    std::optional<struct blell> blelll;
//    while(true) {
//        std::getline(std::cin, line);
//        std::cout << line << '\n';
//        blell = parseMessage(line);
//    }
    std::optional<struct blell> blell = parseMessage("LL Data: 0e 07 03 00 04 00 0a 17 00");
    std::cout << *blell << std::endl;
    blell = parseMessage("LL Data: 02 07 03 00 04 00 0a 03 00");
    std::cout << *blell << std::endl;

    return 0;
}
