#include "scheme.h"
#include <iostream>
#include <fstream>

int main() {

//    std::freopen("input.txt","r", stdin);
    Compilation proc;
    std::string line;
//    std::ifstream in1;
//    in1.open("input.txt");
    std::string type;
    getline(std::cin, type);
    if (type == "monocode") {
        while (getline(std::cin, line)) {
            std::cout << proc.Build(line) << "\n";
        }
    } else if (type == "splitcode") {
        int balance = 0;
        std::string all_line;
        while (getline(std::cin, line)) {
            bool is_quotes = true;
            for (char x: line) {
                if(x != '\'') {
                    is_quotes = false;
                }
                if (x == '(') {
                    balance++;
                } else if (x == ')') {
                    balance--;
                }
                all_line += x;
            }
            if (balance == 0 && !is_quotes) {
                std::cout << proc.Build(all_line) << "\n";
                all_line  = "";
            }
        }
    } else if (type == "file") {
        std::freopen("input.txt","r", stdin);
        int balance = 0;
        std::string all_line;
        while (getline(std::cin, line)) {
            bool is_quotes = true;
            for (char x: line) {
                if(x != '\'') {
                    is_quotes = false;
                }
                if (x == '(') {
                    balance++;
                } else if (x == ')') {
                    balance--;
                }
                all_line += x;
            }
            if (balance == 0 && !is_quotes) {
                std::cout << proc.Build(all_line) << "\n";
                all_line  = "";
            }
        }
    }
    return 0;
}

