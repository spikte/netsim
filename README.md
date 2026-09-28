# Netsim

Network simulator in C++ and Qt.
I was having a hard time learning all those networking concepts and understanding how it all connect together so i decided to implement a network simulator.
Everything is implemented from scratch (except some cryptographic component, for exemple the ethernet frame check sequence).

## Exemple

<video src="./videos/arp_request.mp4" controls></video>

## Features

### Per Layer

* Layer 2
    - [X] Switch forwarding
    - [X] MAC Table
    - [X] ARP Table and ARP request
    - [X] VLAN (Access/Trunk port)
    - [~] LLDP (Partially, only the most important TLV)
    - [~] STP
* Layer 3
    - [X] IPv4 forwarding
    - [X] ICMP

### Commands

* Basic:
    - `hostname <arg>`
* MAC Table:
    - `show mac address-table`
* VLAN
    - `interface <arg> switchport mode <arg>`
    - `interface <arg> access vlan <arg>`
    - `interface <arg> trunk allowed vlan <arg>`
    - `show vlan`
    - `vlan <arg> <arg>`
* ARP:
    - `arp -a`
    - `arp -n <arg>` (works like arping, TODO: change it)
* IPv4:
    - On single-interface device (computer)
        * `ip address <arg> <arg>`
        * `ip address`
    - On multi-interface device (switch/router)
        * `interface <arg> ip address <arg> <arg>`
        * `show ip interface`
    - `show ip route`
    - `ip route <arg> <arg> <arg>`
    - `ping <arg>`
    - `traceroute <arg>`
