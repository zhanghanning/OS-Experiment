#include "MiniOS.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    MiniOS os;

    if (argc >= 2) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            os.runDemo();
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            std::cout << "用法：\n"
                      << "  os_experiment          进入交互式菜单\n"
                      << "  os_experiment --demo   直接演示全部实验\n";
            return 0;
        }
    }

    os.runInteractive();
    return 0;
}
