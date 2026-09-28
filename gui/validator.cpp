#include "validator.hpp"

QValidator::State MACValidator::validate(QString& input, int& pos) const {
    Q_UNUSED(pos);
    if (input.isEmpty())
        return Intermediate;
    // Hexadecimal with ':' -> 00:11:22:33:44:55
    static const QRegularExpression colonHex(R"(^(?:[0-9A-Fa-f]{2}:){0,5}[0-9A-Fa-f]{0,2}$)");
    if (colonHex.match(input).hasMatch())
        return Acceptable;
    return Invalid;
}
QValidator::State IPv4Validator::validate(QString& input, int& pos) const {
    Q_UNUSED(pos);
    if (input.isEmpty())
        return Intermediate;
    // Decimal with '.' -> 192.168.1.10
    static const QRegularExpression dottedDecimal(R"(^(?:[0-9]{1,3}\.){0,3}[0-9]{0,3}$)");
    if (dottedDecimal.match(input).hasMatch())
        return Acceptable;

    return Invalid;
}
