#ifndef GUI_VALIDATOR_HPP
#define GUI_VALIDATOR_HPP

#include <QValidator>

struct MACValidator : public QValidator {
    using QValidator::QValidator;
    QValidator::State validate(QString& input, int& pos) const override;
};
struct IPv4Validator : public QValidator {
    using QValidator::QValidator;
    QValidator::State validate(QString& input, int& pos) const override;
};

#endif
