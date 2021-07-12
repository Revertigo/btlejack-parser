#include "blell.hpp"

#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <sys/stat.h>

int main(void) {
    //Create named pipe with default permissions
    mkfifo("btlejack_output", 0666);
    std::fstream pipe("btlejack_output");
    std::string line;

    while(true) {
        std::getline(pipe, line);
        std::optional<struct blell> blell = parseMessage(line);
        if(blell) {
            std::cout << *blell << std::endl << std::endl;
        }
    }
}
