#include "../../gui/include/register_editor.hpp"
#include <QFormLayout>

RegisterEditor::RegisterEditor(QWidget* parent) :
    QWidget(parent) {
    accumulator_edit = NewHexEdit(this);
    auxiliary_edit = NewHexEdit(this);
    instruction_pointer_edit = NewDecEdit(this);

    // ReSharper disable once CppDFAMemoryLeak
    auto* form_layout = new QFormLayout(this);

    form_layout->addRow("Accumulator:", accumulator_edit);
    form_layout->addRow("Auxiliary:", auxiliary_edit);
    form_layout->addRow("Instruction Pointer:", instruction_pointer_edit);

    connect(accumulator_edit, QOverload<int>::of(&QSpinBox::valueChanged), this, &RegisterEditor::AccumulatorEdited);
    connect(auxiliary_edit, QOverload<int>::of(&QSpinBox::valueChanged), this, &RegisterEditor::AuxiliaryEdited);
    connect(instruction_pointer_edit, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &RegisterEditor::InstructionPointerEdited);
}

HexSpinBox* RegisterEditor::NewHexEdit(QWidget* parent) {
    auto* hex_edit = new HexSpinBox(parent);
    hex_edit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return hex_edit;
}

QSpinBox* RegisterEditor::NewDecEdit(QWidget* parent) {
    auto* dec_edit = new QSpinBox(parent);
    dec_edit->setRange(std::numeric_limits<common::DoubleByte>::min(), std::numeric_limits<common::DoubleByte>::max());
    return dec_edit;
}

void RegisterEditor::SetReadOnly(const bool read_only) const {
    accumulator_edit->setReadOnly(read_only);
    auxiliary_edit->setReadOnly(read_only);
    instruction_pointer_edit->setReadOnly(read_only);
}
