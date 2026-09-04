#include "test_switch.hpp"

void test_switch_no_vlan() {
    Switch switch1;
    Switch switch2;
    Link link;

    switch1.on = true;
    switch1.ports[0].name = "FastEthernet 0";
    switch1.interface_add(0, 1);
    SOFT_ASSERT_EQUAL(switch1.ports[0].interface.address, 1);
    SOFT_ASSERT_CHECK(switch1.ports[0].has_interface, "");
    SOFT_ASSERT_EQUAL(switch1.name_to_slot["FastEthernet 0"], 0);

    switch2.on = true;
    switch2.ports[0].name = "FastEthernet 0";
    switch2.interface_add(0, 2);
    SOFT_ASSERT_EQUAL(switch2.ports[0].name, "FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch2.ports[0].interface.address, 2);
    SOFT_ASSERT_CHECK(switch2.ports[0].has_interface, "");
    SOFT_ASSERT_EQUAL(switch2.name_to_slot["FastEthernet 0"], 0);

    switch1.interface_turn_on("FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch1.ports[0].interface.on, true);
    switch2.interface_turn_on("FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch2.ports[0].interface.on, true);

    switch1.link("FastEthernet 0", switch2, "FastEthernet 0", link);
    SOFT_ASSERT_EQUAL(link[0].src, &switch1.ports[0].interface);
    SOFT_ASSERT_EQUAL(link[0].dst, &switch2.ports[0].interface);
    SOFT_ASSERT_EQUAL(link[1].src, &switch2.ports[0].interface);
    SOFT_ASSERT_EQUAL(link[1].dst, &switch1.ports[0].interface);

    ethernet_t eth;
    ethernetheader_init(&eth.header);
    ethernetheader_set_destination(&eth.header, switch2.ports[0].interface.address);
    ethernetheader_set_source(&eth.header, switch1.ports[0].interface.address);
    ethernetheader_set_has_tag(&eth.header, false);
    ethernetheader_set_eth_type(&eth.header, ETHERTYPE_ARP);
    uint8_t buffer[] = "This is a test";
    eth.payload      = buffer;
    eth.payload_size = sizeof(buffer) - 1;
    eth.tail.crc     = ethernet_compute_crc(&eth);
    PacketRaw packet;
    packet.resize(ethernet_size(&eth));
    ethernet_build_bytes(packet.data(), &eth, false);

    global_tick = 0;
    switch1.send_packet(0, packet);
    global_tick = switch1.ports[0].interface.send_queue.front().delivery_tick;
    switch1.ports[0].interface.update();
    switch2.update();

    auto it = switch2.mac_table.table.find({1, switch1.ports[0].interface.address});
    SOFT_ASSERT_NOT_EQUAL(it, switch2.mac_table.table.end());
    if (it == switch2.mac_table.table.end())
        return;
    SOFT_ASSERT_EQUAL(it->second.interface_id, 0);

    global_tick = it->second.aging_tick - 1;
    switch2.update();
    it = switch2.mac_table.table.find({1, switch1.ports[0].interface.address});
    SOFT_ASSERT_NOT_EQUAL(it, switch2.mac_table.table.end());
    if (it == switch2.mac_table.table.end())
        return;
    SOFT_ASSERT_EQUAL(it->second.interface_id, 0);

    global_tick++;
    switch2.update();
    it = switch2.mac_table.table.find({1, switch1.ports[0].interface.address});
    SOFT_ASSERT_EQUAL(it, switch2.mac_table.table.end());
}
void test_switch_vlan() {
    Switch switch1;
    Switch switch2;
    Link link;

    /* Switch 1 */
    switch1.on = true;
    // F0
    switch1.ports[0].name = "FastEthernet 0";
    switch1.interface_add(0, 10);
    switch1.interface_set_type("FastEthernet 0", SwitchPortType::ACCESS);
    switch1.interface_add_vlan("FastEthernet 0", 10);
    // F1
    switch1.ports[1].name = "FastEthernet 1";
    switch1.interface_add(1, 11);
    switch1.interface_set_type("FastEthernet 1", SwitchPortType::TRUNK);
    switch1.interface_add_vlan("FastEthernet 1", 10);

    /* Switch 2 */
    switch2.on = true;
    // F0
    switch2.ports[0].name = "FastEthernet 0";
    switch2.interface_add(0, 20);
    switch2.interface_set_type("FastEthernet 0", SwitchPortType::ACCESS);
    switch2.interface_add_vlan("FastEthernet 0", 10);
    // F1
    switch2.ports[1].name = "FastEthernet 1";
    switch2.interface_add(1, 21);
    switch2.interface_set_type("FastEthernet 1", SwitchPortType::TRUNK);
    switch2.interface_add_vlan("FastEthernet 1", 10);

    switch1.interface_turn_on("FastEthernet 0");
    switch1.interface_turn_on("FastEthernet 1");
    switch2.interface_turn_on("FastEthernet 0");
    switch2.interface_turn_on("FastEthernet 1");

    switch1.link("FastEthernet 1", switch2, "FastEthernet 1", link);
    Interface* value = &switch1.ports[0].interface;
    SOFT_ASSERT_EQUAL(link[0].src, &switch1.ports[1].interface);
    SOFT_ASSERT_EQUAL(link[0].dst, &switch2.ports[1].interface);
    SOFT_ASSERT_EQUAL(link[1].src, &switch2.ports[1].interface);
    SOFT_ASSERT_EQUAL(link[1].dst, &switch1.ports[1].interface);


    ethernet_t eth;
    ethernetheader_init(&eth.header);
    ethernetheader_set_destination(&eth.header, switch2.ports[0].interface.address);
    ethernetheader_set_source(&eth.header, switch1.ports[0].interface.address);
    ethernetheader_set_has_tag(&eth.header, false);
    ethernetheader_set_eth_type(&eth.header, ETHERTYPE_ARP);
    uint8_t buffer[] = "This is a test";
    eth.payload      = buffer;
    eth.payload_size = sizeof(buffer) - 1;
    eth.tail.crc     = ethernet_compute_crc(&eth);
    PacketRaw packet;
    packet.resize(ethernet_size(&eth));
    ethernet_build_bytes(packet.data(), &eth, false);

    // f0 SW1 f1 -- f1 SW2 f0
    global_tick = 0;
    switch1.ports[0].interface.recv_packet(packet);
    switch1.update();
    PacketRaw packet_tagged = packet_raw_tag(packet, 10);
    SOFT_ASSERT_MEMORY_EQUAL(switch1.ports[1].interface.send_queue.front().packet.data(), packet_tagged.data(), packet_tagged.size());
    global_tick = switch1.ports[1].interface.send_queue.front().delivery_tick;
    switch1.ports[1].interface.update();
    SOFT_ASSERT_MEMORY_EQUAL(switch2.ports[1].interface.recv_queue.front().data(), packet_tagged.data(), packet_tagged.size());
    switch2.update();
    SOFT_ASSERT_CHECK(switch2.ports[1].interface.recv_queue.empty(), "");
}
void test_switch() {
    test_switch_no_vlan();
    test_switch_vlan();
}
