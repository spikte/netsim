#ifndef NETSIM_SIGNAL_HPP
#define NETSIM_SIGNAL_HPP

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

// Names are weird to not collides with Qt signals
template <typename... Args>
struct Signal {
    using Callback = std::function<void(Args...)>;
    using Id       = std::size_t;

    struct ConnectionEntry {
        Id id;
        Callback callback;
    };

    std::vector<ConnectionEntry> _connections;
    Id _nextId = 0;

    Id subscribe(Callback callback) {
        Id id = _nextId++;
        _connections.push_back({id, std::move(callback)});
        return id;
    }

    void unsubscribe(Id id) {
        _connections.erase(
            std::remove_if(
                _connections.begin(),
                _connections.end(),
                [id](const ConnectionEntry& connection) {
                    return connection.id == id;
                }),
            _connections.end());
    }

    void fire(Args... args) {
        for (auto& connection : _connections) {
            connection.callback(args...);
        }
    }
};

#endif
