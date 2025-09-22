#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

int main() {
    std::ifstream source_file("snake.vc");
    std::string content((std::istreambuf_iterator<char>(source_file)), std::istreambuf_iterator<char>());

    std::string temp_content = content;
    content.clear();
    for (char& ch : temp_content) {
        if (ch == '\n')
            continue;
        if (ch == '\t')
            continue;
        content.push_back(ch);
    }

    std::vector<std::string> lines;
    std::string current_line;


    std::cout << content;
    return 0;
}
