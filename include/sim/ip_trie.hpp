#ifndef IPTRIE_HPP
#define IPTRIE_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>
#include "type.hpp"

template <typename T>
struct IPTrieNode {
    std::unique_ptr<IPTrieNode> left;
    std::unique_ptr<IPTrieNode> right;
    T data;
    std::uint32_t addr;
    std::uint8_t len;
};
template <typename T, typename DataPolicy>
struct IPTrie {
    std::unique_ptr<IPTrieNode<T>> root;

    // That single function taught me a lot
    template <typename U>
    void insert(const std::uint32_t& addr, const std::uint8_t& len, U&& data) {
        if (len > 32)
            throw std::invalid_argument("invalid prefix length");
        if (!root)
            root = std::make_unique<IPTrieNode<T>>();
        IPTrieNode<T>* node;
        std::uint32_t mask;

        node = root.get();
        mask = 0x80000000u;
        for (std::uint8_t i = 0; i < len; ++i) {
            auto& child = (addr & mask) ? node->right : node->left;
            if (!child)
                child = std::make_unique<IPTrieNode<T>>();
            node = child.get();
            mask >>= 1;
        }
        node->addr = addr & prefix2subnet(len);
        node->len  = len;
        DataPolicy::insert(node->data, std::forward<U>(data));
    }
    template <typename U>
    bool remove_data_recursive(std::unique_ptr<IPTrieNode<T>>& node, const std::uint32_t& addr, const std::uint8_t& len, std::uint8_t depth, const U& data) {
        if (!node)
            return true;
        // If we arrived where we want to delete the data
        if (len == depth) {
            DataPolicy::remove(node->data, data);
            return DataPolicy::empty(node->data) && !node->left && !node->right;
        }
        std::uint32_t mask = 0x80000000u >> depth;
        auto& child        = (addr & mask) ? node->right : node->left;
        bool remove_child  = remove_data_recursive(child, addr, len, depth + 1, data);
        if (remove_child)
            child.reset(); // Practical this thing
        return DataPolicy::empty(node->data) && !node->left && !node->right;
    }
    bool remove_data_at_recursive(std::unique_ptr<IPTrieNode<T>>& node, std::uint32_t addr, std::uint8_t len, std::uint8_t depth, std::size_t index) {
        if (!node)
            return true;
        // If we arrived where we want to delete the data
        if (len == depth) {
            DataPolicy::remove_at(node->data, index);
            return DataPolicy::empty(node->data) && !node->left && !node->right;
        }
        std::uint32_t mask = 0x80000000u >> depth;
        auto& child        = (addr & mask) ? node->right : node->left;
        bool remove_child  = remove_data_at_recursive(child, addr, len, depth + 1, index);
        if (remove_child)
            child.reset(); // Practical this thing
        return DataPolicy::empty(node->data) && !node->left && !node->right;
    }
    bool remove_data_all_recursive(std::unique_ptr<IPTrieNode<T>>& node, const std::uint32_t& addr, const std::uint8_t& len, std::uint8_t depth) {
        if (!node)
            return true;
        // If we arrived where we want to delete the data
        if (len == depth) {
            DataPolicy::remove_all(node->data);
            return DataPolicy::empty(node->data) && !node->left && !node->right;
        }
        std::uint32_t mask = 0x80000000u >> depth;
        auto& child        = (addr & mask) ? node->right : node->left;
        bool remove_child  = remove_data_all_recursive(child, addr, len, depth + 1);
        if (remove_child)
            child.reset(); // Practical this thing
        return DataPolicy::empty(node->data) && !node->left && !node->right;
    }
    template <typename U>
    void remove_data(const std::uint32_t& addr, const std::uint8_t& len, const U& data) {
        remove_data_recursive(root, addr, len, 0, data);
    }
    void remove_data_at(const std::uint32_t& addr, const std::uint8_t& len, std::size_t index) {
        remove_data_at_recursive(root, addr, len, 0, index);
    }
    void remove_data_all(const std::uint32_t& addr, const std::uint8_t& len) {
        remove_data_all_recursive(root, addr, len, 0);
    }
    bool clean(std::unique_ptr<IPTrieNode<T>>& node) {
        if (!node)
            return true;
        if (clean(node->left))
            node->left.reset();
        if (clean(node->right))
            node->right.reset();
        return DataPolicy::empty(node->data) && !node->left && !node->right;
    }
    IPTrieNode<T>* find(const std::uint32_t& addr, const std::uint8_t& len) const {
        if (!root || len > 32)
            return nullptr;

        IPTrieNode<T>* node;
        IPTrieNode<T>* best;
        std::uint32_t mask;

        node = root.get();
        best = !DataPolicy::empty(node->data) ? node : nullptr;
        mask = 0x80000000u;
        for (std::uint8_t i = 0; i < len; ++i) {
            node = (addr & mask) ? node->right.get() : node->left.get();
            if (!node)
                break;
            if (!DataPolicy::empty(node->data))
                best = node;
            mask >>= 1;
        }

        return best;
    }
    IPTrieNode<T>* find_exact(std::uint32_t addr, std::uint8_t len) const {
        if (!root || len > 32)
            return nullptr;

        IPTrieNode<T>* node;
        std::uint32_t mask;

        node = root.get();
        mask = 0x80000000u;
        for (std::uint8_t i = 0; i < len; ++i) {
            node = (addr & mask) ? node->right.get() : node->left.get();
            if (!node)
                return nullptr;
            mask >>= 1;
        }

        return node;
    }
    void clear() {
        root.reset();
    }
    std::vector<const IPTrieNode<T>*> get_nodes() const {
        std::vector<const IPTrieNode<T>*> nodes{};
        if (!root)
            return nodes;
        std::queue<const IPTrieNode<T>*> to_process;

        to_process.push(root.get());
        while (!to_process.empty()) {
            const auto* node = to_process.front();
            to_process.pop();
            if (!node)
                continue;
            if (!DataPolicy::empty(node->data))
                nodes.push_back(node);
            to_process.push(node->left.get());
            to_process.push(node->right.get());
        }

        return nodes;
    }
};

#endif
