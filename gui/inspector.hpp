#ifndef GUI_INSPECTOR_HPP
#define GUI_INSPECTOR_HPP

#include "../include/packet/packet.hpp"
#include "../include/sim/type.hpp"
#include <QByteArray>
#include <QHeaderView>
#include <QList>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QVBoxLayout>

/* RawDataView */
struct RawDataView : public QPlainTextEdit {
    Q_OBJECT
  public:
    RawDataView(QWidget* parent = nullptr);
    void setData(const QByteArray& data);
    std::size_t getOffsetPos(std::size_t offset) const;
    std::size_t getOffsetPosAscii(std::size_t offset) const;
    std::vector<std::vector<std::size_t>> getSelectionPos(std::size_t offset, std::size_t len) const;
    void highlightRange(std::size_t offset, std::size_t len);
};
/* InspectorField */
struct InspectorField {
    std::size_t offset = 0;
    std::size_t size   = 0;
    std::string name;
    std::string value;
    InspectorField(std::size_t offset, std::size_t size, std::string name, std::string value);
};
/* InspectorBuilder */
struct InspectorBuilder {
    std::vector<InspectorField>& fields;
    std::size_t offset = 0;

    InspectorBuilder(std::vector<InspectorField>& fields);
    void string(std::string name, std::string value, std::size_t size);
    void hex(std::string name, const std::vector<std::uint8_t>& value);
    template <typename T>
    void number(std::string name, T value) {
        fields.emplace_back(offset, sizeof(T), std::move(name), std::format("{}", value));
        offset += sizeof(T);
    }
    template <typename T>
    void numberHex(std::string name, T value) {
        fields.emplace_back(offset, sizeof(T), std::move(name), std::format("0x{:X}", value));
        offset += sizeof(T);
    }
    template <typename T>
    void numberBin(std::string name, T value) {
        fields.emplace_back(offset, sizeof(T), std::move(name), std::format("{:0{}b}", value, sizeof(T) * 8));
        offset += sizeof(T);
    }
    void mac(std::string name, std::uint64_t value, std::size_t size = 6);
    void ipv4(std::string name, std::uint32_t value, std::size_t size = 4);
    void bitsBin(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize);
    void bitsHex(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize);
    void bitsDec(std::string name, std::uint8_t value, std::size_t bitOffset, std::size_t bitSize);
    void skip(std::size_t size);

    std::size_t row() const;
};
/* InspectorLayer */
struct InspectorLayer : public QWidget {
    QStandardItemModel* model = nullptr;
    QTableView* table         = nullptr;

    void setItem(std::size_t row, const InspectorField& field);
    InspectorLayer(QWidget* parent = nullptr);
    void setFields(const std::vector<InspectorField>& fields);
    void setEthernet(const Ethernet& frame);
    void setEthernetFCS(const Ethernet& frame);
    void setARP(const ARPHeader& arp);
    void setLLDP(const LLDP& lldp);
    void setIPv4(const IPv4& packet);
    std::size_t offset() const;

    std::size_t width() const;
    std::size_t height() const;
    void unselect();
};
/* Inspector */
struct Inspector : public QWidget {
    RawDataView* rawDataView = nullptr;
    std::vector<InspectorLayer*> layers;
    QVBoxLayout* layersLayout;

    Inspector(QWidget* parent = nullptr, DataRaw bytes = {});

    void build(DataRaw bytes);
    void resize();
    void selectRow(std::size_t layerIndex, int row);
};

#endif
