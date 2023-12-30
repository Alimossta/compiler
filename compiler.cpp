#include <iostream>
#include <cstdlib>
#include <cctype>

enum TokenType {
    INT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    int value;  // For INT tokens
};

class Lexer {
public:
    Lexer(const std::string& input) : input_(input), position_(0) {}

    Token getNextToken() const {
        while (position_ < input_.length() && isspace(input_[position_])) {
            // Skip whitespace
            position_++;
        }

        if (position_ >= input_.length()) {
            // End of file
            return {END_OF_FILE, 0};
        }

        char currentChar = input_[position_];
        switch (currentChar) {
            case '+':
                position_++;
                return {PLUS, 0};
            case '-':
                position_++;
                return {MINUS, 0};
            case '*':
                position_++;
                return {MULTIPLY, 0};
            case '/':
                position_++;
                return {DIVIDE, 0};
            default:
                if (isdigit(currentChar)) {
                    // Parse integer
                    int value = 0;
                    while (position_ < input_.length() && isdigit(input_[position_])) {
                        value = value * 10 + (input_[position_] - '0');
                        position_++;
                    }
                    return {INT, value};
                } else {
                    // Invalid character
                    std::cerr << "Error: Invalid character '" << currentChar << "'" << std::endl;
                    exit(1);
                }
        }
    }

private:
    std::string input_;
    mutable size_t position_;  // 'mutable' allows modification of 'position_' in a const member function
};

class Parser {
public:
    Parser(const Lexer& lexer) : lexer_(lexer), currentToken_(lexer_.getNextToken()) {}

    int parseExpression() {
        int result = parseTerm(); // Parse the first term in the expression

        while (currentToken_.type == PLUS || currentToken_.type == MINUS) {
            Token op = currentToken_;
            if (op.type == PLUS) {
                eat(PLUS);
                result += parseTerm();
            } else if (op.type == MINUS) {
                eat(MINUS);
                result -= parseTerm();
            }
        }

        return result;
    }

    void parse() {
        parseExpression();
        if (currentToken_.type != END_OF_FILE) {
            std::cerr << "Error: Unexpected token after expression" << std::endl;
            exit(1);
        }
    }

private:
    const Lexer& lexer_;
    Token currentToken_;

    void eat(TokenType expectedType) {
        if (currentToken_.type == expectedType) {
            currentToken_ = lexer_.getNextToken();
        } else {
            std::cerr << "Error: Unexpected token type" << std::endl;
            exit(1);
        }
    }

    int parseFactor() {
        if (currentToken_.type == INT) {
            int value = currentToken_.value;
            eat(INT);
            return value;
        } else if (currentToken_.type == PLUS) {
            eat(PLUS);
            return parseFactor();
        } else if (currentToken_.type == MINUS) {
            eat(MINUS);
            return -parseFactor();
        } else {
            std::cerr << "Error: Unexpected token type in factor" << std::endl;
            exit(1);
        }
    }

    int parseTerm() {
        int result = parseFactor();

        while (currentToken_.type == MULTIPLY || currentToken_.type == DIVIDE) {
            Token op = currentToken_;
            if (op.type == MULTIPLY) {
                eat(MULTIPLY);
                result *= parseFactor();
            } else if (op.type == DIVIDE) {
                eat(DIVIDE);
                int divisor = parseFactor();
                if (divisor == 0) {
                    std::cerr << "Error: Division by zero" << std::endl;
                    exit(1);
                }
                result /= divisor;
            }
        }

        return result;
    }
};

int main() {
    std::string input;
    std::cout << "Enter an arithmetic expression: ";
    std::getline(std::cin, input);

    Lexer lexer(input);
    Parser parser(lexer);

    parser.parse();

    std::cout << "Parsing successful!" << std::endl;

    return 0;
}
