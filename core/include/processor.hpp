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
};

class Processor {
public:
    explicit Processor(MemoryManager& memory, ProcessorObserver* observer = nullptr, ProcessorIo* io = nullptr);

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
    const bool& IsRunning() const;

    template <typename T>
    void SetAccumulator(const T &value) {
        registers_.accumulator = value;

        if (observer_) {
            observer_->OnRegisterAccChanged(registers_.accumulator);
        }
    }

    template <typename T>
    void SetAuxiliary(const T &value) {
        registers_.auxiliary = value;

        if (observer_) {
            observer_->OnRegisterAuxChanged(registers_.auxiliary);
        }
    }

private:
    MemoryManager& memory_;
    ProcessorObserver* observer_;
    ProcessorIo* io_;
    std::array<std::function<void()>, std::numeric_limits<common::Byte>::max() + 1> instructions_handlers_;
    Registers registers_;
    bool is_running_;

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
    void Mod();
    void Load();
    void Store();
    template <class T>
    void Input();
    template <class T>
    void Output();
    void Jump();
    template <class T>
    void SkipLower();
    template <class T>
    void SkipGreater();
    template <class T>
    void SkipEqual();
    void JumpAndStore();

};

#endif
