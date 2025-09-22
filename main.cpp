#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>

std::vector<std::string> tokenize(std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        result.push_back(word);
    }
    return result;
}

bool is_only_whitespace(std::string line) {
    for (unsigned char ch : line) {
        if (!std::isspace(ch)) {
            return false;
        }
    }
    return true;
}

int main() {
    std::ifstream source_file("snake.vc");
    std::string content((std::istreambuf_iterator<char>(source_file)), std::istreambuf_iterator<char>());

    std::string temp_content = content;
    content.clear();
    for (char& ch : temp_content) {
        if (ch == '\n') {
            ch = ';';
        }
        content.push_back(ch);
    }

    temp_content = content;
    content.clear();
    for (char& ch : temp_content) {
        if (ch == '\t')
            continue;
        content.push_back(ch);
    }

    std::vector<std::string> lines;
    std::string current_line;
    for (char& ch : content) {
        if (ch == '{') {
            lines.push_back(current_line);
            lines.push_back("{");
            current_line.clear();
            continue;
        }
        if (ch == '}') {
            lines.push_back(current_line);
            lines.push_back("}");
            current_line.clear();
            continue;
        }
        if (ch == ';') {
            lines.push_back(current_line);
            current_line.clear();
            continue;
        }
        current_line.push_back(ch);
    }

    while (true) {
        int num_lines = lines.size();
        bool is_in_order = true;
        for (int i = 0; i < num_lines - 1; i++) {
            std::string& curr_line = lines[i];
            std::string& next_line = lines[i + 1];
            if (curr_line.substr(0, 3) == "var") {
                if (next_line.substr(0, 3) != "var") {
                    std::string temp_line = curr_line;
                    curr_line = next_line;
                    next_line = temp_line;
                    is_in_order = false;
                }
            }
        }
        if (is_in_order)
            break;
    }

    std::vector<std::string> temp_lines;
    for (std::string line : lines) {
        temp_lines.push_back(line);
    }
    lines.clear();
    for (std::string line : temp_lines) {
        if (is_only_whitespace(line))
            continue;
        if (line.substr(0, 2) == "//")
            continue;
        lines.push_back(line);
    }

    for (std::string& line : lines) {
        std::cout << line << "\n";
    }
    return 0;
}

