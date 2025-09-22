#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

std::vector<std::string> tokenize(std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        result.push_back(word);
    }
    return result;
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

    std::vector<std::vector<std::string>> source_tokens;
    for (std::string& line : lines) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens.size() == 0)
            continue;
        if (tokens[0].substr(0, 2) == "//")
            continue;
        source_tokens.push_back(tokens);
    }

    for (std::vector<std::string>& tokens : source_tokens) {
        for (std::string token : tokens) {
            std::cout << token << ",";
        }
        std::cout << "\n";
    }
    return 0;
}

