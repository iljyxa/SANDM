#ifndef VIRTUAL_MACHINE_CONTROLLER_HPP
#define VIRTUAL_MACHINE_CONTROLLER_HPP

#include <QThreadPool>
#include <qmap.h>
#include <qset.h>

#include "../../core/include/processor_observer.hpp"
#include "../../core/include/virtual_machine.hpp"

enum VmState {
    STOPPED,
    RUNNING,
    PAUSED
};

class VirtualMachineController final : public QObject, public VirtualMachine, public ProcessorObserver {
    Q_OBJECT

public:
    explicit VirtualMachineController(ProcessorIo* processor_io = nullptr, QObject* parent = nullptr);

    void Load(const common::ByteCode& byte_code, common::SourceToBytecodeMap& source_to_bytecode_map);
    VmState GetState() const;
    unsigned int GetCurrentCodeLine();
    void ResetProcessor() const;

    void OnRegisterPtiChanged(common::Byte& page_table_index) override;
    void OnMemoryChanged(common::DoubleByte& address) override;
    void OnStatusChanged(bool& is_running) override;
    void OnRegisterAccChanged(const common::Bytes& accumulator) override;
    void OnRegisterAuxChanged(const common::Bytes& auxiliary) override;
    void OnRegisterIpChanged(common::DoubleByte& instruction_pointer) override;

public slots:
    void OnRun();
    void OnStop();
    void OnStep();
    void OnReset();
    void OnDebug();
    void OnPauseContinue();

    void OnInsertBreakpoint(unsigned int breakpoint);
    void OnRemoveBreakpoint(unsigned int breakpoint);
    void OnClearBreakpoints();

    void OnAccumulatorEdited(int value);
    void OnAuxiliaryEdited(int value);
    void OnInstructionPointerEdited(int value);
    void OnPageTableIndexEdited(int value);

signals:
    void StateChanged(VmState state, bool debugging);
    void Update();
    void Reseted();
    void ErrorOccurred(const QString& error);

private:
    VmState state_;
    bool debugging_;
    QSet<common::DoubleByte> breakpoints_;
    QSet<unsigned int> source_breakpoints_;
    common::SourceToBytecodeMap source_to_bytecode_map_;
    common::BytecodeToSourceMap bytecode_to_source_map_;

    void SetState(VmState state);
    void UpdateBreakpoints();
};

#endif //VIRTUAL_MACHINE_CONTROLLER_HPP
