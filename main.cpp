#include "scheme.h"
#include <iostream>
#include <fstream>

int main() {

//    std::freopen("input.txt","r", stdin);
    Compilation proc;
    std::string line;
//    std::ifstream in1;
//    in1.open("input.txt");
    while (getline(std::cin , line)) {
        std::cout << proc.Build(line) << "\n";
    }
    return 0;
}

