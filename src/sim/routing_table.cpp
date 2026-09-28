#include "../../include/sim/routing_table.hpp"

std::string_view rtb2str(RoutingTableProtocol protocol) {
    switch (protocol) {
        case RoutingTableProtocol::Local:
            return "L";
        case RoutingTableProtocol::Connected:
            return "C";
        case RoutingTableProtocol::Static:
            return "S";
        case RoutingTableProtocol::BGP:
            return "B";
        case RoutingTableProtocol::RIB:
            return "R";
        case RoutingTableProtocol::IS_IS:
            return "i";
    }
    return "?";
}
bool RouteData::is_direct() const {
    return protocol == RoutingTableProtocol::Connected || protocol == RoutingTableProtocol::Local;
}
/* RouteData */
bool RouteData::operator==(const RouteData& data) const {
    if (protocol != data.protocol)
        return false;
    if (data.is_direct())
        return ipv4_cidr == data.ipv4_cidr && interface == data.interface;
    else
        return ipv4_cidr == data.ipv4_cidr && next_hop == data.next_hop;
}
/* IPTrieRIBPolicy */
void IPTrieRIBPolicy::insert(std::vector<RouteData>& stored, const RouteData& data) {
    stored.push_back(data);
}
void IPTrieRIBPolicy::remove(std::vector<RouteData>& stored, const RouteData& data) {
    auto it = std::find_if(stored.begin(), stored.end(), [data](const RouteData& data_stored) {
        return data == data_stored;
    });
    if (it != stored.end())
        stored.erase(it);
}
void IPTrieRIBPolicy::remove_at(std::vector<RouteData>& stored, std::size_t index) {
    stored.erase(stored.begin() + index);
}
void IPTrieRIBPolicy::remove_all(std::vector<RouteData>& stored) {
    stored.clear();
}
bool IPTrieRIBPolicy::empty(const std::vector<RouteData>& stored) {
    return stored.empty();
}
/* Routing table */
void RoutingTable::add_entry(IPv4CIDR ipv4_cidr, IPv4Address next_hop, std::string interface, int metric, RoutingTableProtocol protocol, int administrative_distance) {
    RouteData route;

    ipv4_cidr.address             = ipv4_cidr.address & ipv4_cidr.subnet;
    route.ipv4_cidr               = ipv4_cidr;
    route.next_hop                = next_hop;
    route.interface               = interface;
    route.metric                  = metric;
    route.protocol                = protocol;
    route.administrative_distance = administrative_distance;
    ip_trie.insert(ipv4_cidr.address, ipv4_cidr.get_prefix(), route);
}
void RoutingTable::del_entry(IPv4CIDR ipv4_cidr, std::size_t index) {
    ip_trie.remove_data_at(ipv4_cidr.address, ipv4_cidr.get_prefix(), index);
}
void RoutingTable::del_entry(const RouteData& data) {
    ip_trie.remove_data(data.ipv4_cidr.address, data.ipv4_cidr.get_prefix(), data);
}
std::vector<RouteData>* RoutingTable::find_all(IPv4Address ipv4_address, std::uint8_t len, bool exact) const {
    IPTrieNode<std::vector<RouteData>>* node;

    if (exact)
        node = ip_trie.find_exact(ipv4_address, len);
    else
        node = ip_trie.find(ipv4_address, len);
    if (!node)
        return nullptr;

    return &node->data;
}
RouteData* RoutingTable::find(IPv4Address ipv4_address, std::uint8_t len, bool exact) const {
    std::vector<RouteData>* routes = find_all(ipv4_address, len, exact);
    if (!routes)
        return nullptr;
    auto it = std::min_element(routes->begin(), routes->end(), [](const RouteData& left, const RouteData& right) {
        if (left.administrative_distance == right.administrative_distance)
            return left.metric < right.metric;
        return left.administrative_distance < right.administrative_distance;
    });
    if (it == routes->end())
        return nullptr;
    return &*it;
}
RouteData* RoutingTable::resolve(IPv4Address ipv4_address) const {
    RouteData* data;
    constexpr std::size_t max_depth = 32;
    std::size_t depth;

    data  = find(ipv4_address);
    depth = 0;
    while (data && !data->is_direct() && depth++ < max_depth)
        data = find(data->next_hop);

    return data;
}
void RoutingTable::del_entry_connected(IPv4CIDR ipv4_cidr, const std::string& interface) {
    std::vector<RouteData>* routes = find_all(ipv4_cidr.address, ipv4_cidr.get_prefix(), true);
    if (!routes)
        return;
    auto it = std::find_if(routes->begin(), routes->end(), [interface](const RouteData& route_data) {
        return route_data.interface == interface && route_data.protocol == RoutingTableProtocol::Connected;
    });
    if (it == routes->end())
        return;
    routes->erase(it);
    ip_trie.clean(ip_trie.root);
}
void RoutingTable::del_entry_local(IPv4Address ipv4_address, const std::string& interface) {
    std::vector<RouteData>* routes = find_all(ipv4_address, 32, true);
    if (!routes)
        return;
    auto it = std::find_if(routes->begin(), routes->end(), [interface](const RouteData& route_data) {
        return route_data.interface == interface && route_data.protocol == RoutingTableProtocol::Local;
    });
    if (it == routes->end())
        return;
    routes->erase(it);
    ip_trie.clean(ip_trie.root);
}

/* IPTrieFIBPolicy */
void IPTrieFIBPolicy::insert(std::unique_ptr<ForwardData>& stored, const ForwardData& data) {
    stored = std::make_unique<ForwardData>(data);
}
void IPTrieFIBPolicy::remove(std::unique_ptr<ForwardData>& stored, const std::unique_ptr<ForwardData>& data) {
    stored.reset();
}
void IPTrieFIBPolicy::remove_at(std::unique_ptr<ForwardData>& stored, std::size_t index) {}
void IPTrieFIBPolicy::remove_all(std::unique_ptr<ForwardData>& stored) {
    stored.reset();
}
bool IPTrieFIBPolicy::empty(const std::unique_ptr<ForwardData>& stored) {
    return stored == nullptr;
}

/* ForwardingTable */
ForwardData* ForwardingTable::resolve(IPv4Address ipv4_address) {
    IPTrieNode<std::unique_ptr<ForwardData>>* node;

    node = ip_trie.find(ipv4_address, 32);
    if (!node)
        return nullptr;

    return node->data.get();
}
void ForwardingTable::build_recursive(const RoutingTable& routing_table, const IPTrieNode<std::vector<RouteData>>* node) {
    if (!node)
        return;
    if (!IPTrieRIBPolicy::empty(node->data)) {
        const std::vector<RouteData>& routes = node->data;
        auto it                              = std::min_element(routes.begin(), routes.end(), [](const RouteData& left, const RouteData& right) {
            if (left.administrative_distance == right.administrative_distance)
                return left.metric < right.metric;
            return left.administrative_distance < right.administrative_distance;
        });
        if (it != routes.end()) {
            const RouteData& route = *it;
            if (route.protocol == RoutingTableProtocol::Connected) {
                ip_trie.insert(node->addr, node->len, ForwardData{.interface = route.interface, .next_hop = route.next_hop});
            } else if (route.protocol != RoutingTableProtocol::Local) {
                RouteData* resolved_route = routing_table.resolve(route.next_hop);
                if (resolved_route)
                    ip_trie.insert(node->addr, node->len, ForwardData{.interface = resolved_route->interface, .next_hop = route.next_hop});
            }
        }
    }
    build_recursive(routing_table, node->left.get());
    build_recursive(routing_table, node->right.get());
}
void ForwardingTable::build(const RoutingTable& routing_table) {
    ip_trie.clear();
    build_recursive(routing_table, routing_table.ip_trie.root.get());
}
