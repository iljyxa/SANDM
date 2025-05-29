
#ifndef VIRTUAL_MACHINE_HPP
#define VIRTUAL_MACHINE_HPP

#include "memory_manager.hpp"
#include "processor.hpp"
#include "processor_io.hpp"

class VirtualMachine : public ProcessorIo {
public:
    explicit VirtualMachine(ProcessorIo* processor_io = nullptr);

    ~VirtualMachine() override = default;

    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine& operator=(const VirtualMachine&) = delete;

    VirtualMachine(VirtualMachine&&) = default;
    VirtualMachine& operator=(VirtualMachine&&) = default;

    virtual void Load(const common::ByteCode& byte_code);
    [[nodiscard]] virtual common::Bytes ReadMemory(const common::Byte& address);
    virtual void WriteMemory(const common::Byte& address, const common::Bytes& data);

    virtual void Run();
    virtual void Stop();
    virtual void Step();
    virtual void Reset();

    [[nodiscard]] virtual bool IsRunning();
    [[nodiscard]] virtual Registers GetRegisters();

    virtual void SetInstructionPointer(common::DoubleByte value);
    virtual void SetAccumulator(common::Byte value);
    virtual void SetAccumulator(common::Word value);
    virtual void SetAccumulator(common::SignedWord value);
    virtual void SetAccumulator(common::Real value);
    virtual void SetAuxiliary(common::Byte value);
    virtual void SetAuxiliary(common::Word value);
    virtual void SetAuxiliary(common::SignedWord value);
    virtual void SetAuxiliary(common::Real value);

    void SetProcessorObserver(ProcessorObserver* observer) const;
    void SetProcessorIo(ProcessorIo* processor_io) const;

    void Output(common::Bytes& bytes, common::Type& type) override;
    void InputAsync(common::Type type, InputCallback callback) override;

    static std::string BytesToString(const common::Bytes& bytes, const common::Type& type);
    static common::Bytes BytesFromString(const std::string& string, const common::Type& type);

protected:
    std::unique_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<Processor> processor_;
};

#endif
