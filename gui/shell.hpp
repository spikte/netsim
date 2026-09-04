#ifndef GUI_SHELL_HPP
#define GUI_SHELL_HPP

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

std::vector<std::string> split(std::string s, const std::string& delimiter);

template <typename T>
using Handler = std::function<void(const std::vector<std::string>&, T*)>;
/* == Lexer == */
enum class TokenType {
    WORD, // Actually i don't think i'll ever need more for my simple need
};
struct Token {
    std::string text;
    TokenType type;
};
std::vector<Token> command_to_token(std::string command);

/* == Command Trie == */
enum class CommandTrieNodeType {
    KEYWORD,
    ARG
};
template <typename T>
struct CommandTrieNode {
    explicit CommandTrieNode(std::string keyword = "") : keyword(keyword), type(CommandTrieNodeType::KEYWORD), handler(std::nullopt) {};
    std::string keyword;
    CommandTrieNodeType type;
    std::unordered_map<std::string, CommandTrieNode> children;
    std::optional<Handler<T>> handler;
};
template <typename T>
struct Command {
    Handler<T> handler;
    std::vector<std::string> args;
};
template <typename T>
struct CommandTrie {
    CommandTrieNode<T> root;
    void add(std::string command, Handler<T> handler) {
        std::vector<std::string> words = split(command, " ");
        size_t index                   = 0;
        CommandTrieNode<T>* node       = &root;
        while (index < words.size()) {
            auto it = node->children.find(words[index]);
            if (it != node->children.end())
                node = &it->second;
            else {
                node->children[words[index]] = CommandTrieNode<T>(words[index]);
                node                         = &node->children[words[index]];
                if (words[index] == "<arg>")
                    node->type = CommandTrieNodeType::ARG;
                else
                    node->type = CommandTrieNodeType::KEYWORD;
            }
            index++;
        }
        node->handler = handler;
    }
    std::optional<Command<T>> get_handler(const std::string& input) {
        std::vector<std::string> words = split(input, " ");
        size_t index                   = 0;
        CommandTrieNode<T>* node       = &root;
        Command<T> command;

        while (index < words.size()) {
            auto it = node->children.find(words[index]);
            if (it != node->children.end())
                node = &it->second;
            else {
                auto arg_it = node->children.find("<arg>");
                if (arg_it != node->children.end()) {
                    command.args.push_back(words[index]);
                    node = &arg_it->second;
                } else
                    return std::nullopt;
            }
            index++;
        }
        if (!node->handler)
            return std::nullopt;
        command.handler = *(node->handler);
        return command;
    }
};

/* == Shell == */
template <typename T>
struct Shell {
    T* device;
    std::vector<Command<T>> history;
    CommandTrie<T>* command_trie;
    bool execute_command(const std::string& input) {
        auto command = command_trie->get_handler(input);
        if (!command)
            return false;
        command->handler(command->args, device);
        history.push_back(*command);
        return true;
    }
};

#endif
