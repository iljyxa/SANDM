#ifndef REGISTER_EDITOR_HPP
#define REGISTER_EDITOR_HPP

#include <QFormLayout>

#include "core/common_definitions.hpp"
#include "gui/hex_spin_box.hpp"
#include "gui/spin_box_hoverable.hpp"

/**
 * @class RegisterEditor
 * @brief Класс для редактирования и управления регистрами.
 *
 * Класс предоставляет функциональность для модификации, чтения и управления
 * регистрами.
 *
 */
class RegisterEditor final : public QWidget {
    Q_OBJECT

public:
    explicit RegisterEditor(QWidget* parent = nullptr);

    void SetReadOnly(bool read_only) const;

    HexSpinBox* accumulator_edit;
    HexSpinBox* auxiliary_edit;
    SpinBoxHoverable* instruction_pointer_edit;

signals:
    void AccumulatorEdited(int value);
    void AuxiliaryEdited(int value);
    void InstructionPointerEdited(int value);
    void AccumulatorHovered(bool state);
    void AuxiliaryHovered(bool state);
    void InstructionPointerHovered(bool state);

private:
    static HexSpinBox* NewHexEdit(QWidget* parent);
    static SpinBoxHoverable* NewDecEdit(QWidget* parent);
};

#endif
