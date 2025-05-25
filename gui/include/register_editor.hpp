#ifndef REGISTER_EDITOR_HPP
#define REGISTER_EDITOR_HPP

#include "hex_spin_box.hpp"
#include "../../core/include/common_definitions.hpp"

class RegisterEditor final : public QWidget {
    Q_OBJECT

public:
    explicit RegisterEditor(QWidget* parent = nullptr);

    void SetReadOnly(bool read_only) const;

    HexSpinBox* accumulator_edit;
    HexSpinBox* auxiliary_edit;
    QSpinBox* instruction_pointer_edit;

signals:
    void AccumulatorEdited(int value);
    void AuxiliaryEdited(int value);
    void InstructionPointerEdited(int value);

private:
    static HexSpinBox* NewHexEdit(QWidget* parent);
    static QSpinBox* NewDecEdit(QWidget* parent);
};

#endif
