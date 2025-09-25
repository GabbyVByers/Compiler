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

void assert_loop_exists(std::string token, std::vector<std::string>& lines) {
    bool loop_exists = false;
    for (std::string line : lines) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens[0] == "loop") {
            if (tokens[1] == token) {
                loop_exists = true;
            }
        }
    }
    if (!loop_exists) {
        std::cout << "CATASTROPHIC ERROR: loop '" << token << "' does not exist!\n";
        assert(false);
    }
}

void assert_goto_label_exists(std::string token, std::vector<std::string>& lines) {
    bool goto_label_exists = false;
    for (std::string line : lines) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens[0] == "label") {
            if (tokens[1] == token) {
                goto_label_exists = true;
            }
        }
    }
    if (!goto_label_exists) {
        std::cout << "CATASTROPHIC ERROR: goto label '" << token << "' does not exist!\n";
        assert(false);
    }
}

bool string_is_all_numbers(std::string token) {
    for (char ch : token) {
        if ((ch == '0') ||
            (ch == '1') ||
            (ch == '2') ||
            (ch == '3') ||
            (ch == '4') ||
            (ch == '5') ||
            (ch == '6') ||
            (ch == '7') ||
            (ch == '8') ||
            (ch == '9')) {
            continue;
        }
        else {
            return false;
        }
    }
    return true;
}

void compile_tokens(std::vector<std::string>& tokens, std::ofstream& output, std::vector<std::string>& lines) {
    int num_tokens = tokens.size();

    for (int i = 0; i < num_tokens; i++) {
        std::string token = tokens[i];
        
        if (token == "var") {
            assert(i == 0);
            assert(tokens.size() == 2);
            std::string ASM = "\tvar " + tokens[1] + "\n";
            output << ASM;
            break;
        }

        if (token == "continue") {
            assert(i == 0);
            assert(tokens.size() == 2);
            std::string ASM = "\tJMP .LOOP_START_" + tokens[1] + "\n";
            assert_loop_exists(tokens[1], lines);
            output << ASM;
            break;
        }

        if (token == "break") {
            assert(i == 0);
            assert(tokens.size() == 2);
            std::string ASM = "\tJMP .LOOP_END_" + tokens[1] + "\n";
            assert_loop_exists(tokens[1], lines);
            output << ASM;
            break;
        }

        if (token == "->") {
            assert((i + 2) == tokens.size());
            std::string argument = tokens[i + 1];

            size_t open_braket_index = argument.find('[');
            size_t close_braket_index = argument.find(']');

            assert(((open_braket_index == std::string::npos) && (close_braket_index == std::string::npos)) ||
                   ((open_braket_index != std::string::npos) && (close_braket_index != std::string::npos)));

            if (open_braket_index == std::string::npos) {
                output << "\tSTR r0 &" + argument + "\n";
                break;
            }
            else {
                std::string array_index = argument.substr(open_braket_index + 1, argument.size() - open_braket_index - 2);
                std::string array = argument.substr(0, open_braket_index);
                if (string_is_all_numbers(array_index)) {
                    output << "\tLDI r2 &" + array + "\n\tLDI r3 #" + array_index + "\n\tADD r2 r3\n\tSTX r0 r2\n";
                    break;
                }
                else {
                    output << "\tLDI r2 &" + array + "\n\tLDR r3 &" + array_index + "\n\tADD r2 r3\n\tSTX r0 r2\n";
                    break;
                }
            }
        }

        if (token == "label") {
            assert(tokens.size() == 2);
            output << "label GOTO_" + tokens[1] + "\n";
            break;
        }

        if (token == "goto") {
            assert(tokens.size() == 2);
            assert_goto_label_exists(tokens[1], lines);
            output << "\tJMP .GOTO_" + tokens[1] + "\n";
            break;
        }

        if (token.size() >= 3) {
            if (token.substr(token.size() - 2, 2) == "()") {
                std::string func_name = token.substr(0, token.size() - 2);
                if (i != 0) {
                    std::cout << "CATASTROPHIC ERROR: Invoking a function must load accumulator (" << func_name << ")\n";
                    assert(false);
                }
                output << "\tJSR .FUNCTION_" + func_name + "\n";
                continue;
            }
        }

        if (token == ">>") {
            output << "\tSHR r0\n";
            continue;
        }

        if (token == "<<") {
            output << "\tSHL r0\n";
            continue;
        }

        if ((token == "+")  ||
            (token == "-")  ||
            (token == "*")  ||
            (token == "/")  ||
            (token == "%")  ||
            (token == "&")  ||
            (token == "|")  ||
            (token == "^")  ||
            (token == ">")  ||
            (token == "<")  ||
            (token == "==") ||
            (token == "!=") ||
            (token == ">=") ||
            (token == "<=")) {
            std::string argument = tokens[i + 1];
            i++;

            size_t open_braket_index = argument.find('[');
            size_t close_braket_index = argument.find(']');
            assert(((open_braket_index == std::string::npos) && (close_braket_index == std::string::npos)) ||
                   ((open_braket_index != std::string::npos) && (close_braket_index != std::string::npos)));

            if (string_is_all_numbers(argument)) {
                output << "\tLDI r1 #" + argument + "\n";
            }
            else {
                if (open_braket_index == std::string::npos) {
                    output << "\tLDR r1 &" + argument + "\n";
                }
                else {
                    std::string array_index = argument.substr(open_braket_index + 1, argument.size() - open_braket_index - 2);
                    std::string array = argument.substr(0, open_braket_index);
                    if (string_is_all_numbers(array_index)) {
                        output << "\tLDI r2 &" + array + "\n\tLDI r3 #" + array_index + "\n\tADD r2 r3\n\tSTX r1 r2\n";
                    }
                    else {
                        output << "\tLDI r2 &" + array + "\n\tLDR r3 &" + array_index + "\n\tADD r2 r3\n\tSTX r1 r2\n";
                    }
                }
            }

            if (token == "+") {
                output << "\tADD r0 r1\n";
                continue;
            }

            if (token == "-") {
                output << "\tSUB r0 r1\n";
                continue;
            }

            if (token == "*") {
                output << "\tJSR .MULTIPLICATION_SUBROUTINE\n";
                continue;
            }

            if (token == "/") {
                output << "\tJSR .DIVISION_SUBROUTINE\n";
                continue;
            }

            if (token == "%") {
                output << "\tJSR .MODULOS_SUBROUTINE\n";
                continue;
            }

            if (token == "&") {
                output << "\tAND r0 r1\n";
                continue;
            }

            if (token == "|") {
                output << "\tORR r0 r1\n";
                continue;
            }

            if (token == "^") {
                output << "\tXOR r0 r1\n";
                continue;
            }

            if (token == ">") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 FLAG_GREATER_THAN\n";
                continue;
            }

            if (token == "<") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 FLAG_LESS_THAN\n";
                continue;
            }

            if (token == "==") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 FLAG_EQUAL\n";
                continue;
            }

            if (token == "!=") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 !FLAG_EQUAL\n";
                continue;
            }

            if (token == ">=") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 !FLAG_LESS_THAN\n";
                continue;
            }

            if (token == "<=") {
                output << "\tCMP r0 r1\n\tLDI r0 #0\n\tLDI r0 #1 !FLAG_GREATER_THAN\n";
                continue;
            }
        }

        // load accumulator
        if (i == 0) {
            if (string_is_all_numbers(token)) {
                output << "\tLDI r0 #" + token + "\n";
                continue;
            }
            else {
                size_t open_braket_index = token.find('[');
                size_t close_braket_index = token.find(']');
                assert(((open_braket_index == std::string::npos) && (close_braket_index == std::string::npos)) ||
                       ((open_braket_index != std::string::npos) && (close_braket_index != std::string::npos)));
                if (open_braket_index == std::string::npos) {
                    output << "\tLDR r0 &" + token + "\n";
                    continue;
                }
                else {
                    std::string array_index = token.substr(open_braket_index + 1, token.size() - open_braket_index - 2);
                    std::string array = token.substr(0, open_braket_index);
                    if (string_is_all_numbers(array_index)) {
                        output << "\tLDI r2 &" + array + "\n\tLDI r3 #" + array_index + "\n\tADD r2 r3\n\tLDX r0 r2\n";
                        continue;
                    }
                    else {
                        output << "\tLDI r2 &" + array + "\n\tLDR r3 &" + array_index + "\n\tADD r2 r3\n\tLDX r0 r2\n";
                        continue;
                    }
                }
            }
        }
    }
}

void compile(std::vector<MetaLine>& meta_lines, std::ofstream& output, std::vector<std::string>& lines) {
    static int if_statement_counter = -1;
    
    for (MetaLine& meta_line : meta_lines) {
        std::string line = meta_line.line;
        std::vector<std::string> tokens = tokenize(line);

        if (tokens[0] == "if") {
            assert(tokens.size() >= 2);
            assert(tokens[1][0] == '(');
            assert(tokens[tokens.size() - 1][tokens[tokens.size() - 1].size() - 1] == ')');
            std::vector<std::string> local_tokens;
            for (int i = 1; i < tokens.size(); i++) {
                std::string token = tokens[i];
                if (i == 1) {
                    token.erase(0, 1);
                }
                if (i == tokens.size() - 1) {
                    token.erase(token.size() - 1);
                }
                local_tokens.push_back(token);
            }
            compile_tokens(local_tokens, output, lines);
            if_statement_counter++;
            std::string ASM_BEGIN = "\tCMP r0 r0\n\tJMP .END_IF_" + std::to_string(if_statement_counter) + " FLAG_ZERO\n";
            std::string ASM_END = "label END_IF_" + std::to_string(if_statement_counter) + "\n";
            output << ASM_BEGIN;
            compile(meta_line.children, output, lines);
            output << ASM_END;
            continue;
        }

        if (tokens[0] == "func") {
            assert(tokens.size() == 2);
            assert(tokens[1].size() >= 3);
            assert(tokens[1].substr(tokens[1].size() - 2, 2) == "()");
            std::string func_name = tokens[1].substr(0, tokens[1].size() - 2);
            std::string ASM_BEGIN = "label FUNCTION_" + func_name + "\n";
            std::string ASM_END = "\tRTS\n";
            output << ASM_BEGIN;
            compile(meta_line.children, output, lines);
            output << ASM_END;
            continue;
        }

        if (tokens[0] == "loop") {
            assert(tokens.size() == 2);
            std::string ASM_BEGIN = "label LOOP_START_" + tokens[1] + "\n";
            std::string ASM_END = "\tJMP .LOOP_START_" + tokens[1] + "\nlabel LOOP_END_" + tokens[1] + "\n";
            output << ASM_BEGIN;
            compile(meta_line.children, output, lines);
            output << ASM_END;
            continue;
        }

        compile_tokens(tokens, output, lines);
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
    compile(mother.children, output, lines);

    return 0;
}

