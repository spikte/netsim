#include "test_switch.hpp"

void test_switch() {
    Switch switch1;
    Switch switch2;
    Link link;

    switch1.on = true;
    switch1.add_interface(0, 1, "FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch1.interfaces[0].name, "FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch1.interfaces[0].address, 1);
    SOFT_ASSERT_CHECK(switch1.slot_has_interface[0], "");
    SOFT_ASSERT_EQUAL(switch1.name_to_interface["FastEthernet 0"], 0);

    switch2.on = true;
    switch2.add_interface(0, 2, "FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch2.interfaces[0].name, "FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch2.interfaces[0].address, 2);
    SOFT_ASSERT_CHECK(switch2.slot_has_interface[0], "");
    SOFT_ASSERT_EQUAL(switch2.name_to_interface["FastEthernet 0"], 0);

    switch1.turn_on_interface("FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch1.interfaces[0].on, true);
    switch2.turn_on_interface("FastEthernet 0");
    SOFT_ASSERT_EQUAL(switch2.interfaces[0].on, true);

    switch1.link(0, switch2, 0, link);
    SOFT_ASSERT_EQUAL(link[0].src, &switch1.interfaces[0]);
    SOFT_ASSERT_EQUAL(link[0].dst, &switch2.interfaces[0]);
    SOFT_ASSERT_EQUAL(link[1].src, &switch2.interfaces[0]);
    SOFT_ASSERT_EQUAL(link[1].dst, &switch1.interfaces[0]);

    ethernet_t eth;
    ethernetheader_init(&eth.header);
    ethernetheader_set_destination(&eth.header, switch2.interfaces[0].address);
    ethernetheader_set_source(&eth.header, switch1.interfaces[0].address);
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
    global_tick = switch1.interfaces[0].send_queue.front().delivery_tick;
    switch1.interfaces[0].update();
    switch2.update();

    auto it = switch2.mac_table.table.find(switch1.interfaces[0].address);
    SOFT_ASSERT_NOT_EQUAL(it, switch2.mac_table.table.end());
    if (it == switch2.mac_table.table.end())
        return;
    SOFT_ASSERT_EQUAL(it->second.interface_id, 0);

    global_tick = it->second.aging_tick - 1;
    switch2.update();
    it = switch2.mac_table.table.find(switch1.interfaces[0].address);
    SOFT_ASSERT_NOT_EQUAL(it, switch2.mac_table.table.end());
    if (it == switch2.mac_table.table.end())
        return;
    SOFT_ASSERT_EQUAL(it->second.interface_id, 0);

    global_tick++;
    switch2.update();
    it = switch2.mac_table.table.find(switch1.interfaces[0].address);
    SOFT_ASSERT_EQUAL(it, switch2.mac_table.table.end());
}
