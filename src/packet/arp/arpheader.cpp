#include "../../../include/packet/arp/arpheader.hpp"
#include <inttypes.h>

const char* ARPHeader::htype_to_string(ARPHardwareType htype) {
    switch (htype) {
        case ARPHardwareType::Reserved:
            return "Reserved";
        case ARPHardwareType::Ethernet_10Mb:
            return "Ethernet (10Mb)";
        case ARPHardwareType::Experimental_Ethernet_3Mb:
            return "Experimental Ethernet (3Mb)";
        case ARPHardwareType::Amateur_Radio_AX25:
            return "Amateur Radio AX.25";
        case ARPHardwareType::Proteon_ProNET_Token_Ring:
            return "Proteon ProNET Token Ring";
        case ARPHardwareType::Chaos:
            return "Chaos";
        case ARPHardwareType::IEEE_802_Networks:
            return "IEEE 802 Networks";
        case ARPHardwareType::ARCNET:
            return "ARCNET";
        case ARPHardwareType::Hyperchannel:
            return "Hyperchannel";
        case ARPHardwareType::Lanstar:
            return "Lanstar";
        case ARPHardwareType::Autonet_Short_Address:
            return "Autonet Short Address";
        case ARPHardwareType::LocalTalk:
            return "LocalTalk";
        case ARPHardwareType::LocalNet:
            return "LocalNet";
        case ARPHardwareType::Ultra_Link:
            return "Ultra link";
        case ARPHardwareType::SMDS:
            return "SMDS";
        case ARPHardwareType::Frame_Relay:
            return "Frame Relay";
        case ARPHardwareType::ATM_16:
        case ARPHardwareType::ATM_19:
        case ARPHardwareType::ATM_21:
            return "ATM";
        case ARPHardwareType::HDLC:
            return "HDLC";
        case ARPHardwareType::Fibre_Channel:
            return "Fibre Channel";
        case ARPHardwareType::Serial_Line:
            return "Serial Line";
        case ARPHardwareType::MIL_STD_188_220:
            return "MIL-STD-188-220";
        case ARPHardwareType::Metricom:
            return "Metricom";
        case ARPHardwareType::IEEE_1394_1995:
            return "IEEE 1394.1995";
        case ARPHardwareType::MAPOS:
            return "MAPOS";
        case ARPHardwareType::Twinaxial:
            return "Twinaxial";
        case ARPHardwareType::EUI_64:
            return "EUI-64";
        case ARPHardwareType::HIPARP:
            return "HIPARP";
        case ARPHardwareType::IP_and_ARP_over_ISO_7816_3:
            return "IP and ARP over ISO 7816-3";
        case ARPHardwareType::ARPSec:
            return "ARPSec";
        case ARPHardwareType::IPsec_Tunnel:
            return "IPsec tunnel";
        case ARPHardwareType::InfiniBand:
            return "InfiniBand";
        case ARPHardwareType::TIA_102_Project_25_CAI:
            return "TIA-102 Project 25 CAI";
        case ARPHardwareType::Wiegand_Interface:
            return "Wiegand Interface";
        case ARPHardwareType::Pure_IP:
            return "Pure IP";
        case ARPHardwareType::EXP1:
            return "HW_EXP1";
        case ARPHardwareType::HFI:
            return "HFI";
        case ARPHardwareType::Unified_Bus_UB:
            return "Unified Bus (UB)";
        case ARPHardwareType::EXP2:
            return "HW_EXP2";
        case ARPHardwareType::AEthernet:
            return "AEthernet";
        case ARPHardwareType::Reserved_65535:
            return "Reserved";
        default:
            return "No string rep";
    }
}
const char* ARPHeader::oper_to_string(ARPOperation oper) {
    switch (oper) {
        case ARPOperation::Reserved:
            return "Reserved";
        case ARPOperation::REQUEST:
            return "REQUEST";
        case ARPOperation::REPLY:
            return "REPLY";
        case ARPOperation::Request_Reverse:
            return "request Reverse";
        case ARPOperation::Reply_Reverse:
            return "reply Reverse";
        case ARPOperation::DRARP_Request:
            return "DRARP-Request";
        case ARPOperation::DRARP_Reply:
            return "DRARP-Reply";
        case ARPOperation::DRARP_Error:
            return "DRARP-Error";
        case ARPOperation::InARP_Request:
            return "InARP-Request";
        case ARPOperation::InARP_Reply:
            return "InARP-Reply";
        case ARPOperation::ARP_NAK:
            return "ARP-NAK";
        case ARPOperation::MARS_Request:
            return "MARS-Request";
        case ARPOperation::MARS_Multi:
            return "MARS-Multi";
        case ARPOperation::MARS_MServ:
            return "MARS-MServ";
        case ARPOperation::MARS_Join:
            return "MARS-Join";
        case ARPOperation::MARS_Leave:
            return "MARS-Leave";
        case ARPOperation::MARS_NAK:
            return "MARS-NAK";
        case ARPOperation::MARS_Unserv:
            return "MARS-Unserv";
        case ARPOperation::MARS_SJoin:
            return "MARS-SJoin";
        case ARPOperation::MARS_SLeave:
            return "MARS-SLeave";
        case ARPOperation::MARS_Grouplist_Request:
            return "MARS-Grouplist-Request";
        case ARPOperation::MARS_Grouplist_Reply:
            return "MARS-Grouplist-Reply";
        case ARPOperation::MARS_Redirect_Map:
            return "MARS-Redirect-Map";
        case ARPOperation::MAPOS_UNARP:
            return "MAPOS-UNARP";
        case ARPOperation::EXP1:
            return "OP_EXP1";
        case ARPOperation::EXP2:
            return "OP_EXP2";
        case ARPOperation::Reserved_65535:
            return "Reserved";
        default:
            return "No string rep";
    }
}

void ARPHeader::init() {
    *this = ARPHeader{};
}
std::size_t ARPHeader::size() const {
    return 8 + 2 * hlen + 2 * plen;
}

void ARPHeader::set_htype(ARPHardwareType value) {
    htype = static_cast<std::uint16_t>(value);
}
void ARPHeader::set_ptype(std::uint16_t value) {
    ptype = value;
}
void ARPHeader::set_hlen(std::uint8_t value) {
    hlen = value;
}
void ARPHeader::set_plen(std::uint8_t value) {
    plen = value;
}
void ARPHeader::set_oper(ARPOperation value) {
    oper = static_cast<std::uint16_t>(value);
}
void ARPHeader::set_sha(std::uint64_t value) {
    sha = value;
}
void ARPHeader::set_spa(std::uint32_t value) {
    spa = value;
}
void ARPHeader::set_tha(std::uint64_t value) {
    tha = value;
}
void ARPHeader::set_tpa(std::uint32_t value) {
    tpa = value;
}

void ARPHeader::from_bytes(const std::uint8_t* buffer) {
    std::size_t offset = 0;

    init();

    htype = pack_u16_at(buffer, &offset);
    ptype = pack_u16_at(buffer, &offset);

    hlen = buffer[offset++];
    plen = buffer[offset++];

    oper = pack_u16_at(buffer, &offset);

    sha = pack_uN_at(buffer, hlen, &offset);
    spa = pack_uN_at(buffer, plen, &offset);
    tha = pack_uN_at(buffer, hlen, &offset);
    tpa = pack_uN_at(buffer, plen, &offset);
}
std::size_t ARPHeader::to_bytes(std::uint8_t* buffer) const {
    std::size_t offset = 0;

    unpack_u16_at(buffer, &offset, htype);
    unpack_u16_at(buffer, &offset, ptype);

    buffer[offset++] = hlen;
    buffer[offset++] = plen;

    unpack_u16_at(buffer, &offset, oper);

    unpack_uN_at(buffer, hlen, &offset, sha);
    unpack_uN_at(buffer, plen, &offset, spa);
    unpack_uN_at(buffer, hlen, &offset, tha);
    unpack_uN_at(buffer, plen, &offset, tpa);

    return offset;
}
std::uint8_t* ARPHeader::to_bytes_alloc(std::size_t* outputSize) const {
    const std::size_t bufferSize = size();

    if (outputSize)
        *outputSize = bufferSize;
    auto* buffer = static_cast<std::uint8_t*>(std::malloc(bufferSize));
    if (buffer)
        to_bytes(buffer);

    return buffer;
}
