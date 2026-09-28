#include "inspector.hpp"

/* RawDataView */
RawDataView::RawDataView(QWidget* parent) : QPlainTextEdit(parent) {
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}
void RawDataView::setData(const QByteArray& data) {
    QString output;

    for (int offset = 0; offset < data.size(); offset += 16) {
        output += QString("%1  ").arg(offset, 8, 16, QChar('0')).toUpper();
        for (int i = 0; i < 16; ++i) {
            if (offset + i < data.size())
                output += QString("%1 ").arg(static_cast<unsigned char>(data[offset + i]), 2, 16, QChar('0')).toUpper();
            else
                output += "   ";
            if (i == 7)
                output += " ";
        }
        output += " ";
        for (int i = 0; i < 16 && offset + i < data.size(); ++i) {
            unsigned char c = data[offset + i];
            if (c >= 32 && c <= 126)
                output += QChar(c);
            else
                output += '.';
        }
        output += '\n';
    }
    setPlainText(output);
}
std::size_t RawDataView::getOffsetPos(std::size_t offset) const {
    std::size_t row = offset / 16;
    std::size_t col = offset % 16;
    std::size_t pos = row * 77 + 10 + col * 3;
    if (col > 7)
        pos++;
    return pos;
}
std::size_t RawDataView::getOffsetPosAscii(std::size_t offset) const {
    std::size_t row = offset / 16;
    std::size_t col = offset % 16;
    return row * 77 + 60 + col;
}
std::vector<std::vector<std::size_t>> RawDataView::getSelectionPos(std::size_t offset, std::size_t len) const {
    std::vector<std::vector<std::size_t>> positions;
    QPoint start(offset % 16, offset / 16);
    QPoint end((offset + len) % 16, (offset + len) / 16);
    QPoint currentPoint = start;
    std::size_t posStart, posEnd;
    while (currentPoint.y() != end.y()) {
        positions.push_back({});
        posStart = getOffsetPos(currentPoint.y() * 16 + currentPoint.x()); // raw bytes
        posEnd   = getOffsetPos(currentPoint.y() * 16 + 15) + 3;
        positions.back().push_back(posStart);
        positions.back().push_back(posEnd - 1);
        posStart = getOffsetPosAscii(currentPoint.y() * 16 + currentPoint.x()); // ascii
        posEnd   = getOffsetPosAscii(currentPoint.y() * 16 + 15) + 1;
        positions.back().push_back(posStart);
        positions.back().push_back(posEnd);
        currentPoint.setX(0);
        currentPoint.setY(currentPoint.y() + 1);
    }
    positions.push_back({});
    posStart = getOffsetPos(currentPoint.y() * 16 + currentPoint.x());
    posEnd   = getOffsetPos(currentPoint.y() * 16 + end.x());
    positions.back().push_back(posStart);
    positions.back().push_back(posEnd - 1);
    posStart = getOffsetPosAscii(currentPoint.y() * 16 + currentPoint.x());
    posEnd   = getOffsetPosAscii(currentPoint.y() * 16 + end.x());
    positions.back().push_back(posStart);
    positions.back().push_back(posEnd);
    return positions;
}
void RawDataView::highlightRange(std::size_t offset, std::size_t len) {
    QList<QTextEdit::ExtraSelection> selections;
    // I should actually get both at the same time
    auto positions       = getSelectionPos(offset, len);
    auto createSelection = [this, &selections](std::size_t start, std::size_t end) {
        QTextEdit::ExtraSelection selection;
        selection.cursor = QTextCursor(document());
        selection.cursor.setPosition(start);
        selection.cursor.setPosition(end, QTextCursor::KeepAnchor);
        selection.format.setBackground(QColor("#010736"));
        selection.format.setForeground(QColor("#FCECD8"));
        selections.append(selection);
    };
    for (auto const& pos : positions) {
        // Raw bytes
        createSelection(pos[0], pos[1]);
        // ASCII
        createSelection(pos[2], pos[3]);
    }
    setExtraSelections(selections);
}
/* InspectorField */
InspectorField::InspectorField(std::size_t offset, std::size_t size, std::string name, std::string value) : offset(offset), size(size),
                                                                                                            name(std::move(name)),
                                                                                                            value(std::move(value)) {}
/* InspectorBuilder */
InspectorBuilder::InspectorBuilder(std::vector<InspectorField>& fields) : fields(fields), offset(0) {}
void InspectorBuilder::string(std::string name, std::string value, std::size_t size) {
    fields.emplace_back(offset, size, std::move(name), std::move(value));
    offset += size;
}
void InspectorBuilder::hex(std::string name, const std::vector<std::uint8_t>& value) {
    fields.emplace_back(offset, value.size(), std::move(name), bytes2str(value));
    offset += value.size();
}
void InspectorBuilder::mac(std::string name, std::uint64_t value, std::size_t size) {
    fields.emplace_back(offset, size, std::move(name), mac2str(value));
    offset += size;
}
void InspectorBuilder::ipv4(std::string name, std::uint32_t value, std::size_t size) {
    fields.emplace_back(offset, size, std::move(name), ipv42str(value));
    offset += size;
}
void InspectorBuilder::bitsBin(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize) {
    const std::uint8_t mask  = static_cast<std::uint8_t>((1u << bitSize) - 1u);
    const std::uint8_t field = static_cast<std::uint8_t>((value >> bitOffset) & mask);
    fields.emplace_back(offset, 1, std::move(name), std::format("{:0{}b}", field, bitSize));
}
void InspectorBuilder::bitsHex(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize) {
    const std::uint8_t mask  = static_cast<std::uint8_t>((1u << bitSize) - 1u);
    const std::uint8_t field = static_cast<std::uint8_t>((value >> bitOffset) & mask);
    fields.emplace_back(offset, 1, std::move(name), std::format("{:X}", field));
}
void InspectorBuilder::bitsDec(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize) {
    const std::uint8_t mask  = static_cast<std::uint8_t>((1u << bitSize) - 1u);
    const std::uint8_t field = static_cast<std::uint8_t>((value >> bitOffset) & mask);
    fields.emplace_back(offset, 1, std::move(name), std::format("{}", field));
}
void InspectorBuilder::skip(std::size_t size) {
    offset += size;
}
std::size_t InspectorBuilder::row() const {
    return fields.size();
}

/* InspectorLayer */
InspectorLayer::InspectorLayer(QWidget* parent) : QWidget(parent) {
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Offset", "Name", "Value", "Size"});

    table = new QTableView(this);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setModel(model);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(table);
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection&) {
        if (selected.isEmpty())
            return;
        const QModelIndex index  = selected.indexes().first();
        const std::size_t offset = model->item(index.row(), 0)->text().toULongLong();
        const std::size_t size   = model->item(index.row(), 3)->text().toULongLong();
    });
}
void InspectorLayer::setFields(const std::vector<InspectorField>& fields) {
    model->removeRows(0, model->rowCount());
    for (std::size_t row = 0; row < fields.size(); ++row)
        setItem(row, fields[row]);
}
void InspectorLayer::setItem(std::size_t row, const InspectorField& field) {
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QString::number(static_cast<qulonglong>(field.offset))));
    items.append(new QStandardItem(QString::fromStdString(field.name)));
    auto* valueItem = new QStandardItem(QString::fromStdString(field.value));
    valueItem->setToolTip(QString::fromStdString(field.value));
    items.append(valueItem);
    items.append(new QStandardItem(QString::number(static_cast<qulonglong>(field.size))));
    model->insertRow(static_cast<int>(row), items);
}
std::size_t InspectorLayer::width() const {
    int tableWidth = table->verticalHeader()->width() + table->horizontalHeader()->length() + 2 * table->frameWidth();
    return tableWidth;
}
std::size_t InspectorLayer::height() const {
    int tableHeight = table->horizontalHeader()->height() + table->verticalHeader()->length() + 2 * table->frameWidth();
    return static_cast<std::size_t>(tableHeight);
}
void InspectorLayer::unselect() {
    if (!table->selectionModel())
        return;
    table->selectionModel()->clearSelection();
    table->setCurrentIndex(QModelIndex());
}
void InspectorLayer::setEthernet(const Ethernet& frame) {
    std::vector<InspectorField> fields;
    InspectorBuilder b(fields);
    b.mac("Destination", frame.header.destination, 6);
    b.mac("Source", frame.header.source, 6);
    for (std::size_t i = 0; i < frame.header.tags.size(); ++i) {
        const VlanTag& tag     = frame.header.tags[i];
        const std::string vlan = std::format("VLAN {}", tag.get_vid());
        b.numberHex(std::format("{} PCP", vlan), tag.get_pcp());
        b.numberHex(std::format("{} DEI", vlan), tag.get_dei());
        b.numberHex(std::format("{} VID", vlan), tag.get_vid());
    }
    const std::string ethTypeStr = std::format("{} (0x{:X})", ethheader_ethtype2str(static_cast<EthType>(frame.header.eth_type)), frame.header.eth_type);
    b.string("Ethernet Type", ethTypeStr, 2);
    setFields(fields);
}
void InspectorLayer::setEthernetFCS(const Ethernet& frame) {
    std::vector<InspectorField> fields;
    InspectorBuilder b(fields);
    b.numberHex("FCS", frame.tail.crc);
    setFields(fields);
}
void InspectorLayer::setARP(const ARPHeader& arp) {
    std::vector<InspectorField> fields;
    InspectorBuilder b(fields);
    const std::string hTypeStr = std::format("{} (0x{:X})",
                                             ARPHeader::htype_to_string(static_cast<ARPHardwareType>(arp.htype)),
                                             arp.htype);
    b.string("Hardware Type", hTypeStr, 2);
    const std::string pTypeStr = std::format("{} (0x{:X})",
                                             ethheader_ethtype2str(static_cast<EthType>(arp.ptype)),
                                             arp.ptype);
    b.string("Protocol Type", pTypeStr, 2);
    b.numberHex("Hardware Length", arp.hlen);
    b.numberHex("Protocol Length", arp.plen);
    const std::string opStr = std::format("{} (0x{:X})",
                                          ARPHeader::oper_to_string(static_cast<ARPOperation>(arp.oper)),
                                          arp.oper);
    b.string("Operation", opStr, 2);
    const bool isHTypeMAC = static_cast<ARPHardwareType>(arp.htype) == ARPHardwareType::Ethernet_10Mb;
    const bool isPTypeIP  = static_cast<EthType>(arp.ptype) == EthType::IP;

    if (isHTypeMAC) {
        b.mac("Sender Hardware Address", arp.sha, arp.hlen);
    } else {
        const std::uint8_t* data = reinterpret_cast<const std::uint8_t*>(&arp.sha);
        std::vector<std::uint8_t> vec(data + 8 - arp.hlen, data + 8);
        b.hex("Sender Hardware Address", vec);
    }

    if (isPTypeIP) {
        b.ipv4("Sender Protocol Address", arp.spa, arp.plen);
    } else {
        const std::uint8_t* data = reinterpret_cast<const std::uint8_t*>(&arp.spa);
        std::vector<std::uint8_t> vec(data + 8 - arp.plen, data + 8);
        b.hex("Sender Protocol Address", vec);
    }

    if (isHTypeMAC) {
        b.mac("Target Hardware Address", arp.tha, arp.hlen);
    } else {
        const std::uint8_t* data = reinterpret_cast<const std::uint8_t*>(&arp.tha);
        std::vector<std::uint8_t> vec(data + 8 - arp.hlen, data + 8);
        b.hex("Target Hardware Address", vec);
    }

    if (isPTypeIP) {
        b.ipv4("Target Protocol Address", arp.tpa, arp.plen);
    } else {
        const std::uint8_t* data = reinterpret_cast<const std::uint8_t*>(&arp.tpa);
        std::vector<std::uint8_t> vec(data + 8 - arp.plen, data + 8);
        b.hex("Target Protocol Address", vec);
    }
    setFields(fields);
}
void InspectorLayer::setLLDP(const LLDP& lldp) {
    std::vector<InspectorField> fields;
    InspectorBuilder b(fields);
    for (const auto& tlv : lldp.tlvs) {
        const auto type           = static_cast<LLDP::TLVType>(tlv.get_type());
        const std::string typeStr = std::format("{} (0x{:X})",
                                                LLDP::tlvtype_to_str(type),
                                                tlv.get_type() << 1);
        b.string("TLV Type", typeStr, 1);
        if (type != LLDP::TLVType::EndOfLLDPDU) {
            const std::string lenStr = std::format("{}", tlv.get_len());
            b.string("TLV Length", lenStr, 1);
        }
        switch (type) {
            case LLDP::TLVType::ChassisId: {
                if (tlv.data.empty())
                    break;
                const auto subtype           = static_cast<LLDP::ChassisIdSubtype>(tlv.data[0]);
                const std::string subtypeStr = std::format("{} (0x{:X})",
                                                           LLDP::chassisidsubtype_to_str(subtype),
                                                           tlv.data[0]);
                b.string("Chassis ID Subtype", subtypeStr, 1);
                if (subtype == LLDP::ChassisIdSubtype::MACAddress) {
                    if (tlv.data.size() >= 7) {
                        const std::uint64_t mac = pack_u48(tlv.data.data() + 1);
                        b.mac("Chassis ID", mac, 6);
                    }
                } else {
                    std::vector<std::uint8_t> id(tlv.data.begin() + 1, tlv.data.end());
                    b.hex("Chassis ID", id);
                }
                break;
            }
            case LLDP::TLVType::PortId: {
                if (tlv.data.empty())
                    break;
                const auto subtype = static_cast<LLDP::PortIdSubtype>(tlv.data[0]);
                const std::string subtypeStr = std::format("{} (0x{:X})",
                                                           LLDP::portidsubtype_to_str(subtype),
                                                           tlv.data[0]);
                b.string("Port ID Subtype", subtypeStr, 1);
                if (subtype == LLDP::PortIdSubtype::MACAddress) {
                    if (tlv.data.size() >= 7) {
                        const std::uint64_t mac = pack_u48(tlv.data.data() + 1);
                        b.mac("Port ID", mac, 6);
                    }
                } else if (subtype == LLDP::PortIdSubtype::InterfaceName) {
                    const std::string interfaceName(tlv.data.begin() + 1, tlv.data.end());
                    b.string("Port ID", interfaceName, interfaceName.size());
                } else {
                    std::vector<std::uint8_t> id(tlv.data.begin() + 1, tlv.data.end());
                    b.hex("Port ID", id);
                }
                break;
            }
            case LLDP::TLVType::TimeToLive: {
                if (tlv.data.size() >= 2) {
                    const std::uint16_t ttl = static_cast<std::uint16_t>((static_cast<std::uint16_t>(tlv.data[0]) << 8) |
                                                                         static_cast<std::uint16_t>(tlv.data[1]));
                    b.number("Time To Live", ttl);
                }
                break;
            }
            case LLDP::TLVType::EndOfLLDPDU:
                break;
            default:
                b.hex(std::format("Unknown (or display not implemented) TLV ({})", tlv.get_type()), tlv.data);
                break;
        }
    }

    setFields(fields);
}
void InspectorLayer::setIPv4(const IPv4& packet) {
    std::vector<InspectorField> fields;
    InspectorBuilder b(fields);

    b.bitsDec("Version", packet.header.version_length, 4, 4);
    b.bitsDec("Header Length", packet.header.version_length, 0, 4);
    b.numberHex("Type of Service", packet.header.type_of_service);
    b.number("Total Length", packet.header.total_length);
    b.number("Identification", packet.header.identification);
    b.bitsBin("Flags", packet.header.fragment_offset, 16, 3);
    b.bitsDec("Fragment offset", packet.header.fragment_offset, 0, 13);
    b.number("Time To Live", packet.header.time_to_live);
    const std::string protStr = std::format("{} ({:X})", ipv4header_format2str(static_cast<IPv4AddressFormat>(packet.header.protocol)), packet.header.protocol);
    b.string("Protocol", protStr, 1);
    b.numberHex("Checksum", packet.header.checksum);
    b.ipv4("Source address", packet.header.source);
    b.ipv4("Destination address", packet.header.destination);
    setFields(fields);
}
std::size_t InspectorLayer::offset() const {
    std::size_t layerSize = 0;
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* sizeItem = model->item(row, 3);
        const std::size_t size  = sizeItem->text().toULongLong();
        layerSize += size;
    }
    return layerSize;
}

/* Inspector */
Inspector::Inspector(QWidget* parent, DataRaw bytes) : QWidget(parent) {
    rawDataView = new RawDataView(this);
    QByteArray qbytes(bytes);
    rawDataView->setData(qbytes);
    rawDataView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    layersLayout = new QVBoxLayout();
    layersLayout->setContentsMargins(0, 0, 0, 0);
    layersLayout->setSpacing(0);
    mainLayout->addLayout(layersLayout);
    mainLayout->addWidget(rawDataView);
    setMinimumWidth(1000);

    build(bytes);
}
void Inspector::build(DataRaw bytes) {
    for (auto* layer : layers)
        delete layer;
    layers.clear();
    while (QLayoutItem* item = layersLayout->takeAt(0))
        delete item;

    Ethernet frame;
    frame.from_bytes(bytes.data(), bytes.size());
    InspectorLayer* layer2 = new InspectorLayer(this);
    layer2->setEthernet(frame);
    layers.push_back(layer2);
    layersLayout->addWidget(layer2, 0, Qt::AlignTop);
    InspectorLayer* layer3 = new InspectorLayer(this);
    bool has_valid_layer3  = false;
    switch (static_cast<EthType>(frame.header.eth_type)) {
        case EthType::ARP: {
            ARPHeader arp;
            arp.from_bytes(frame.payload.data());
            layer3->setARP(arp);
            has_valid_layer3 = true;
            break;
        }
        case EthType::LLDP: {
            LLDP lldp;
            lldp.from_bytes(frame.payload.data(), frame.payload.size());
            layer3->setLLDP(lldp);
            has_valid_layer3 = true;
            break;
        }
        case EthType::IP: {
            IPv4 packet;
            packet.from_bytes(frame.payload.data(), frame.payload.size());
            layer3->setIPv4(packet);
            has_valid_layer3 = true;
            break;
        }
        default:
            break;
    }
    if (has_valid_layer3) {
        layers.push_back(layer3);
        layersLayout->addWidget(layer3, 0, Qt::AlignTop);
    }
    // Add ethernet FCS
    InspectorLayer* layerFCS = new InspectorLayer(this);
    layerFCS->setEthernetFCS(frame);
    layers.push_back(layerFCS);
    layersLayout->addWidget(layerFCS, 0, Qt::AlignTop);
    for (std::size_t i = 0; i < layers.size(); ++i) {
        connect(layers[i]->table->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                this,
                [this, i](const QItemSelection& selected, const QItemSelection&) {
                    if (selected.isEmpty())
                        return;
                    const int row = selected.indexes().first().row();
                    selectRow(i, row);
                });
    }
    resize();
}
void Inspector::resize() {
    // Layers
    std::array<int, 4> columnWidths{0, 0, 0, 0};
    for (auto* layer : layers) {
        auto* table = layer->table;
        table->resizeColumnsToContents();
        table->resizeRowsToContents();
        for (int column = 0; column < 4; ++column)
            columnWidths[column] = std::max(columnWidths[column], table->columnWidth(column));
    }
    int columnsWidth = 0;
    for (int width : columnWidths)
        columnsWidth += width;
    for (auto* layer : layers) {
        auto* table = layer->table;
        for (int column = 0; column < 4; ++column)
            table->setColumnWidth(column, columnWidths[column]);
        const int tableWidth  = columnsWidth + table->verticalHeader()->width() + 2 * table->frameWidth();
        const int tableHeight = table->horizontalHeader()->height() + table->verticalHeader()->length() + 2 * table->frameWidth();
        table->setFixedSize(tableWidth + 10, tableHeight);
        layer->setFixedSize(tableWidth + 10, tableHeight);
    }
    // Raw bytes
    QFontMetrics fm(rawDataView->font());
    int rawByteWidth        = 0;
    const QStringList lines = rawDataView->toPlainText().split('\n');
    for (const QString& line : lines)
        rawByteWidth = qMax(rawByteWidth, fm.horizontalAdvance(line));
    rawByteWidth += 50; // Some padding

    setMinimumWidth(columnsWidth + rawByteWidth);
}
void Inspector::selectRow(std::size_t layerIndex, int row) {
    if (layerIndex >= layers.size())
        return;
    for (std::size_t i = 0; i < layers.size(); ++i) {
        if (i != layerIndex && layers[i] && layers[i]->table)
            layers[i]->table->clearSelection();
    }
    InspectorLayer* layer = layers[layerIndex];
    if (!layer || !layer->table)
        return;
    auto* model                   = layer->table->model();
    const QModelIndex offsetIndex = model->index(row, 0);
    const QModelIndex sizeIndex   = model->index(row, 3);
    if (!offsetIndex.isValid() || !sizeIndex.isValid())
        return;
    std::size_t offset = offsetIndex.data().toULongLong();
    std::size_t size   = sizeIndex.data().toULongLong();
    for (std::size_t i = 0; i < layerIndex; i++)
        offset += layers[i]->offset();
    rawDataView->highlightRange(offset, size);
}
