#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <functional>
#include <unordered_map>

#include "common_definitions.hpp"
#include "memory_manager.hpp"
#include "processor_io.hpp"
#include "processor_observer.hpp"

struct Registers {
    common::Bytes accumulator{};
    common::Bytes auxiliary{};
    common::DoubleByte instruction_pointer = 0;
    common::Byte page_table_index = 0;
};

class Processor {
public:
    explicit Processor(MemoryManager& memory, ProcessorObserver* observer = nullptr, ProcessorIo* io = nullptr);
    Registers registers;

    void Run();
    void Step();
    void Stop();
    void Reset();
    const Registers& GetRegisters() const;
    void SetObserver(ProcessorObserver* observer);
    void SetIo(ProcessorIo* io);
    const common::Bytes& GetAccumulator() const;
    const common::DoubleByte& GetInstructionPointer() const;
    void SetInstructionPointer(common::DoubleByte value);
    const common::Bytes& GetAuxiliary() const;
    const common::Byte& GetPageTableIndex() const;
    void SetPageTableIndex(common::Byte value);
    const bool& IsRunning() const;

    template <typename T>
    void SetAccumulator(const T &value) {
        registers.accumulator = value;

        if (observer_) {
            observer_->OnRegisterAccChanged(registers.accumulator);
        }
    }

    template <typename T>
    void SetAuxiliary(const T &value) {
        registers.auxiliary = value;

        if (observer_) {
            observer_->OnRegisterAuxChanged(registers.auxiliary);
        }
    }

private:
    MemoryManager& memory_;
    ProcessorObserver* observer_;
    ProcessorIo* io_;
    bool is_running_;
    std::array<std::function<void()>, 256> instructions_handlers_;

    void ExecuteInstruction();
    void NextInstruction();
    void SetStatus(bool is_running);

    template <class T>
    static common::Type TypeIo();

    void Nope();

    template <typename T>
    void Add();

    template <class T>
    void Sub();

    template <class T>
    void Mul();

    template <class T>
    void Div();

    template <class T>
    void Jmp();

    template <class T>
    void Set();

    template <class T>
    void Jnz();

    template <class T>
    void Jgz();

    template <class T>
    void Cpsg();

    template <class T>
    void Cpse();

    template <class T>
    void Load();

    template <class T>
    void Mod();

    template <class T>
    void Page();

    template <class T>
    void Save();

    template <class T>
    void Read();

    template <class T>
    void Write();

};

#endif
