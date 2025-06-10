
#ifndef VIRTUAL_MACHINE_HPP
#define VIRTUAL_MACHINE_HPP

#include <iostream>

#include "core/memory_manager.hpp"
#include "core/processor.hpp"
#include "core/processor_io.hpp"

/**
 * @class VirtualMachine
 * @brief Класс представляет виртуальную машину для выполнения программ.
 *
 * VirtualMachine предоставляет функциональность для загрузки, выполнения
 * программ и управления их состоянием. Виртуальная машина может интерпретировать
 * команды, управлять памятью и обрабатывать исключения.
 */
class VirtualMachine : public ProcessorIo {
public:
    explicit VirtualMachine(ProcessorIo* processor_io = nullptr);

    ~VirtualMachine() override = default;

    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine& operator=(const VirtualMachine&) = delete;

    VirtualMachine(VirtualMachine&&) = default;
    VirtualMachine& operator=(VirtualMachine&&) = default;

    virtual void Load(const snm::ByteCode& byte_code);
    [[nodiscard]] virtual snm::Bytes ReadMemory(const snm::Address& address);
    virtual void WriteMemory(const snm::Address& address, const snm::Bytes& data);

    virtual void Run();
    virtual void Stop();
    virtual void Step();
    virtual void Reset();

    [[nodiscard]] virtual bool IsRunning();
    [[nodiscard]] virtual const snm::ProcessorState& GetState();
    [[nodiscard]] virtual Registers GetRegisters();

    virtual void SetInstructionPointer(snm::Address value);
    virtual void SetAccumulator(snm::Byte value);
    virtual void SetAccumulator(snm::Word value);
    virtual void SetAccumulator(snm::SignedWord value);
    virtual void SetAccumulator(snm::Real value);
    virtual void SetAuxiliary(snm::Byte value);
    virtual void SetAuxiliary(snm::Word value);
    virtual void SetAuxiliary(snm::SignedWord value);
    virtual void SetAuxiliary(snm::Real value);

    void SetProcessorObserver(ProcessorObserver* observer) const;
    void SetProcessorIo(ProcessorIo* processor_io) const;

    void OutputRequest(snm::Bytes bytes, snm::Type type) override;
    void InputRequest(snm::Type type, InputCallback callback) override;

    static std::string BytesToString(const snm::Bytes& bytes, const snm::Type& type);
    static snm::Bytes BytesFromString(const std::string& string, const snm::Type& type);

protected:
    std::unique_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<Processor> processor_;
};

#endif
