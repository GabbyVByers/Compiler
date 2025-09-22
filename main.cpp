#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <cassert>

struct MetaLine {
    std::string line;
    std::vector<MetaLine> children;

    void print(int depth) {
        std::cout << line << "\n";
        for (MetaLine& meta_line : children) {
            for (int i = 0; i < depth + 1; i++) { std::cout << "   "; }
            meta_line.print(depth + 1);
        }
    }
};

std::vector<MetaLine> recursive_childbirth(std::vector<std::string>& lines, int& i) {
    std::vector<MetaLine> children;
    while (true) {
        if (i >= lines.size())
            break;
        std::string& line = lines[i]; i++;
        if (line == "{") {
            assert(children.size() > 0);
            children[children.size() - 1].children = recursive_childbirth(lines, i);
            continue;
        }
        if (line == "}") {
            break;
        }
        MetaLine child;
        child.line = line;
        children.push_back(child);
    }
    return children;
}

std::vector<std::string> tokenize(std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        result.push_back(word);
    }
    return result;
}

void compile(std::vector<MetaLine>& meta_lines, std::ofstream& output) {
    for (MetaLine& meta_line : meta_lines) {
        std::vector<std::string> tokens = tokenize(meta_line.line);


    }
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

    std::cout << "\n\n\n\n";

    int current_index = 0;
    MetaLine mother;
    mother.line = "I am the mother of all tokens, my scope contains all of your code, gaze upon my children...";
    mother.children = recursive_childbirth(lines, current_index);
    mother.print(0);

    std::ofstream output("assembly.txt");
    compile(mother.children, output);

    return 0;
}

