#include "shell.hpp"

/* == Lexer == */
std::vector<Token> command_to_token(std::string command) {
    std::vector<std::string> words;
    std::vector<Token> tokens;

    words = split(command, " ");
    for (auto word : words)
        tokens.push_back(Token(word, TokenType::WORD));

    return tokens;
}
