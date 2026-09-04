#include "shell.hpp"

/* == Lexer == */
std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}
std::vector<Token> command_to_token(std::string command) {
    std::vector<std::string> words;
    std::vector<Token> tokens;

    words = split(command, " ");
    for (auto word : words)
        tokens.push_back(Token(word, TokenType::WORD));

    return tokens;
}
