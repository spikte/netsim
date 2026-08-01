#include "test_interface.hpp"

void test_interface() {
    Interface int1;
    Interface int2;
    PPLinkUni l1to2;
    PPLinkUni l2to1;

    l1to2.connect_src(&int1);
    l1to2.connect_dst(&int2);
    l2to1.connect_src(&int2);
    l2to1.connect_dst(&int1);

    SOFT_ASSERT_EQUAL(int1.to, &l1to2);
    SOFT_ASSERT_EQUAL(int1.from, &l2to1);
    SOFT_ASSERT_EQUAL(int2.to, &l2to1);
    SOFT_ASSERT_EQUAL(int2.from, &l1to2);

    ethernet_t eth;
    ethernetheader_init(&eth.header);
    ethernetheader_set_destination(&eth.header, int2.address);
    ethernetheader_set_source(&eth.header, int1.address);
    ethernetheader_set_has_tag(&eth.header, false);
    ethernetheader_set_eth_type(&eth.header, ETHERTYPE_ARP);
    // Temp for testing, need to implement ARP in my packet lib
    uint8_t buffer[] = "This is a test";
    eth.payload      = buffer;
    eth.payload_size = sizeof(buffer) - 1;
    eth.tail.crc     = ethernet_compute_crc(&eth);

    global_tick = 0;
    PacketRaw packet;
    packet.resize(ethernet_size(&eth));
    ethernet_build_bytes(packet.data(), &eth, false);
    int1.send_packet(packet);
    SOFT_ASSERT_EQUAL(int1.send_queue.size(), 1);
    SOFT_ASSERT_EQUAL(int2.recv_queue.size(), 0);
    SOFT_ASSERT_MEMORY_EQUAL(int1.send_queue.front().packet.data(), packet.data(), packet.size());

    int1.update();
    SOFT_ASSERT_EQUAL(int1.send_queue.size(), 1);
    SOFT_ASSERT_EQUAL(int2.recv_queue.size(), 0);
    SOFT_ASSERT_MEMORY_EQUAL(int1.send_queue.front().packet.data(), packet.data(), packet.size());

    global_tick = int1.send_queue.front().delivery_tick;
    int1.update();
    SOFT_ASSERT_EQUAL(int1.send_queue.size(), 0);
    SOFT_ASSERT_EQUAL(int2.recv_queue.size(), 1);
    SOFT_ASSERT_MEMORY_EQUAL(int2.recv_queue.front().data(), packet.data(), packet.size());

    global_tick++;
    int1.update();
    SOFT_ASSERT_EQUAL(int1.send_queue.size(), 0);
    SOFT_ASSERT_EQUAL(int2.recv_queue.size(), 1);
    SOFT_ASSERT_MEMORY_EQUAL(int2.recv_queue.front().data(), packet.data(), packet.size());
}
