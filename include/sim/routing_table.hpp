#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include "ip_trie.hpp"
#include "type.hpp"
#include <algorithm>
#include <queue>

enum class RoutingTableProtocol {
    Local,
    Connected,
    Static,
    BGP,
    RIB,
    IS_IS,
};
std::string_view rtb2str(RoutingTableProtocol protocol);
struct RouteData {
    IPv4CIDR ipv4_cidr;
    IPv4Address next_hop;
    std::string interface;
    int metric;
    RoutingTableProtocol protocol;
    int administrative_distance;
    bool operator==(const RouteData& data) const;
    bool is_direct() const;
};
struct IPTrieRIBPolicy {
    static void insert(std::vector<RouteData>& stored, const RouteData& data);
    static void remove(std::vector<RouteData>& stored, const RouteData& data);
    static void remove_at(std::vector<RouteData>& stored, std::size_t index);
    static void remove_all(std::vector<RouteData>& stored);
    static bool empty(const std::vector<RouteData>& stored);
};
struct RoutingTable {
    IPTrie<std::vector<RouteData>, IPTrieRIBPolicy> ip_trie;
    void add_entry(IPv4CIDR ipv4_cidr, IPv4Address next_hop, std::string interface, int metric, RoutingTableProtocol protocol, int administrative_distance);
    void del_entry(IPv4CIDR ipv4_cidr, std::size_t index);
    void del_entry(const RouteData& data);
    std::vector<RouteData>* find_all(IPv4Address ipv4_address, std::uint8_t len = 32, bool exact = false) const;
    RouteData* find(IPv4Address ipv4_address, std::uint8_t len = 32, bool exact = false) const;
    RouteData* resolve(IPv4Address ipv4_address) const;
    void del_entry_connected(IPv4CIDR ipv4_cidr, const std::string& interface);
    void del_entry_local(IPv4Address ipv4_address, const std::string& interface);
    std::vector<const IPTrieNode<std::vector<RouteData>&>> get_nodes() const;
};

struct ForwardData {
    std::string interface;
    IPv4Address next_hop;
};
struct IPTrieFIBPolicy {
    static void insert(std::unique_ptr<ForwardData>& stored, const ForwardData& data);
    static void remove(std::unique_ptr<ForwardData>& stored, const std::unique_ptr<ForwardData>& data);
    static void remove_at(std::unique_ptr<ForwardData>& stored, std::size_t index);
    static void remove_all(std::unique_ptr<ForwardData>& stored);
    static bool empty(const std::unique_ptr<ForwardData>& stored);
};
struct ForwardingTable {
    IPTrie<std::unique_ptr<ForwardData>, IPTrieFIBPolicy> ip_trie;
    ForwardData* resolve(IPv4Address ipv4_address);
    void build_recursive(const RoutingTable& routing_table, const IPTrieNode<std::vector<RouteData>>* node);
    void build(const RoutingTable& routing_table);
};

#endif
