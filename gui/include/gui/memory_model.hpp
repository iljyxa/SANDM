#ifndef MEMORY_MODEL_HPP
#define MEMORY_MODEL_HPP

#include <QAbstractTableModel>

#include "virtual_machine_controller.hpp"
#include "core/common_definitions.hpp"
#include "core/virtual_machine.hpp"

class MemoryModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit MemoryModel(VirtualMachineController& virtual_machine, QObject* parent = nullptr) :
        QAbstractTableModel(parent), vm_controller_(virtual_machine) {
    }

    [[nodiscard]] snm::DoubleByte Address(const QModelIndex& index) const {
        return Address(index.row(), index.column());
    }

    [[nodiscard]] snm::DoubleByte Address(const int row, const int column) const {
        return row * columnCount() + column;
    }

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override { // NOLINT(*-default-arguments)
        Q_UNUSED(parent);
        return snm::CODE_MEMORY_SIZE / columnCount(); // NOLINT(*-narrowing-conversions)
    }

    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override { // NOLINT(*-default-arguments)
        Q_UNUSED(parent);
        return 8;
    }

    [[nodiscard]] QVariant data(const QModelIndex& index, const int role) const override {
        if (!index.isValid()) {
            return {};
        }

        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            snm::Bytes value = vm_controller_.ReadMemory(Address(index));

            std::string hex = value.ToHexString();

            return QString(hex.data()).toUpper();
        }

        return {};
    }

    [[nodiscard]] QVariant
    headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            return QString("%1").arg(section, 2, 16, QChar('0')).toUpper();
        }

        if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
            return QString("%1").arg(section, 2, 16, QChar('0')).toUpper();
        }

        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, const int role) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;


        const QString source_string = value.toString();

        bool ok;
        const snm::Word source_value = source_string.toUInt(&ok, 16);
        if (!ok) {
            return false;
        }

        vm_controller_.WriteMemory(Address(index), snm::Bytes(source_value));

        emit dataChanged(index, index);

        return true;
    }

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

private:
    VirtualMachineController& vm_controller_;
};

#endif
