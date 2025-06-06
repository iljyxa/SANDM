#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

// ReSharper disable once CppUnusedIncludeDirective
#include <bitset>
#include <functional>
// ReSharper disable once CppUnusedIncludeDirective
#include <thread>

#include "core/common_definitions.hpp"
#include "core/memory_manager.hpp"
#include "core/processor_io.hpp"
#include "core/processor_observer.hpp"

/**
 * @struct Registers
 * @brief Структура, представляющая регистры процессора.
 *
 * Содержит основные регистры, необходимые для выполнения инструкций процессора,
 * такие как аккумулятор, вспомогательный регистр и указатель на текущую инструкцию.
 */
struct Registers {
    snm::Bytes accumulator{};
    snm::Bytes auxiliary{};
    snm::Address instruction_pointer = 0;
};

class Processor {
public:
    explicit Processor(MemoryManager& memory, ProcessorObserver* observer = nullptr, ProcessorIo* io = nullptr);

    /**
     * @brief Запускает выполнение инструкций процессора.
     *
     * Цикл продолжает выполняться до изменения состояния `is_running_` на `false`, что может быть выполнено
     * посредством вызова других методов, таких как Stop() или до установки регистра IP на адрес, по которому нет инструкций.
     *
     */
    void Run();
    /**
     * @brief Выполняет одну инструкцию процессора.
     *
     * Метод выполняет одну инструкцию, определенную текущим значением регистра указателя инструкций (IP).
     * На время выполнения инструкции статус устанавливается в активный.
     */
    void Step();
    /**
     * @brief Останавливает выполнение инструкций процессора.
     *
     * Устанавливает внутреннее состояние процессора `is_running_` в значение `false`.
     * Это состояние используется для завершения активного цикла выполнения инструкций.
     */
    void Stop();
    /**
     * @brief Выполняет сброс состояния процессора.
     *
     * Устанавливает аккумулятор, вспомогательный регистр и регистр указателя инструкций в начальное состояние (значение 0).
     * Сбрасывает статус выполнения процессора.
     */
    void Reset();
    /**
     * @brief Возвращает структуру, содержащую текущие значения всех регистров процессора.
     *
     * Метод предоставляет доступ к текущему состоянию регистров, включая аккумулятор, вспомогательный регистр
     * и указатель инструкций. Эти данные могут быть использованы для анализа или отладки выполнения инструкций процессора.
     *
     * @return Константная ссылка на структуру Registers, содержащую значения регистров процессора.
     */
    [[nodiscard]] const Registers& GetRegisters() const;
    /**
     * @brief Устанавливает наблюдателя для процессора.
     *
     * Позволяет задать объект наблюдателя для отслеживания изменений состояния процессора.
     * Если наблюдатель не указан (равен nullptr), текущий наблюдатель будет удалён.
     *
     * @param observer Указатель на объект класса ProcessorObserver для получения уведомлений
     * о событиях процессора. По умолчанию равен nullptr.
     */
    void SetObserver(ProcessorObserver* observer);
    /**
     * @brief Устанавливает объект ввода-вывода для процессора.
     *
     * Метод позволяет задать объект класса ProcessorIo, который будет использоваться процессором
     * для выполнения операций ввода-вывода. При вызове данного метода текущий объект ввода-вывода заменяется.
     *
     * @param io Указатель на объект ProcessorIo, который будет использоваться для операций ввода-вывода.
     */
    void SetIo(ProcessorIo* io);
    /**
     * @brief Возвращает текущее значение аккумулятора процессора.
     *
     * Аккумулятор используется для хранения результатов вычислений и операций процессора.
     * Данный метод предоставляет доступ к текущему значению аккумулятора.
     *
     * @return Константная ссылка на объект типа `snm::Bytes`, содержащий текущее значение аккумулятора.
     */
    [[nodiscard]] const snm::Bytes& GetAccumulator() const;
    /**
     * @brief Возвращает текущее значение регистра указателя инструкций (IP).
     *
     * Указатель инструкций используется для определения адреса следующей инструкции,
     * которая будет выполнена процессором.
     *
     * @return Константная ссылка на объект типа `snm::Address`, содержащий текущее значение регистра указателя инструкций.
     */
    [[nodiscard]] const snm::Address& GetInstructionPointer() const;
    /**
     * @brief Устанавливает значение регистра указателя инструкций (IP).
     *
     * Метод обновляет регистр указателя инструкций значением, переданным в качестве аргумента.
     * При изменении значения регистра вызывается уведомление наблюдателя. Если значение превышает
     * размер доступной памяти процессора, работа процессора завершится путём изменения состояния.
     *
     * @param value Новое значение указателя инструкций, передаваемое в виде объекта типа `snm::Address`.
     */
    void SetInstructionPointer(snm::Address value);
    /**
     * @brief Возвращает текущее значение вспомогательного регистра.
     *
     * Метод позволяет получить доступ к вспомогательным данным, которые хранятся
     * в соответствующем регистре процессора.
     *
     * @return Константная ссылка на объект типа `snm::Address`, содержащий текущее значение вспомогательного регистра.
     */
    [[nodiscard]] const snm::Bytes& GetAuxiliary() const;
    /**
     * @brief Возвращает текущий статус процессора.
     *
     * @return Константная ссылка на текущее состояние процессора типа ProcessorState.
     */
    [[nodiscard]] const snm::ProcessorState& GetState() const;

    /**
     * @brief Устанавливает значение регистра аккумулятор.
     *
     * Метод изменяет текущее значение аккумулятора на заданное. Если определён наблюдатель,
     * уведомляет его об изменении значения аккумулятора.
     *
     * @param value Новое значение, которое будет установлено в аккумуляторе.
     */
    template <typename T>
    void SetAccumulator(const T &value) {
        registers_.accumulator = value;

        if (observer_) {
            observer_->OnRegisterAccChanged(registers_.accumulator);
        }
    }

    /**
     * @brief Устанавливает значение вспомогательного регистра.
     *
     * Метод изменяет значение вспомогательного регистра на указанное.
     * Если зарегистрирован наблюдатель, он уведомляется об изменении значения регистра.
     *
     * @param value Новое значение, которое необходимо установить во вспомогательный регистр.
     */
    template <typename T>
    void SetAuxiliary(const T &value) {
        registers_.auxiliary = value;

        if (observer_) {
            observer_->OnRegisterAuxChanged(registers_.auxiliary);
        }
    }

    /**
     * @brief Проверяет, работает ли процессор.
     *
     * Метод определяет, находится ли процессор в состоянии, отличном от остановленного.
     *
     * @return true, если процессор работает; false, если он остановлен.
     */
    [[nodiscard]] bool IsRunning() const {
        return state_ != snm::ProcessorState::STOPPED;
    }

private:
    MemoryManager& memory_; ///< Менеджер памяти
    ProcessorObserver* observer_; ///< Текущий наблюдатель состояния
    ProcessorIo* io_; ///< Обработчик ввода-вывода
    Registers registers_;
    snm::ProcessorState state_; ///< Состояние процессора в данный момент

    std::array<std::function<void()>, std::numeric_limits<snm::Byte>::max() + 1> instructions_handlers_;
    std::array<snm::ArgModifier, 4> argument_modifiers_{};

    /**
     * @brief Выполняет текущую инструкцию процессора.
     *
     * Метод читает текущую инструкцию на основании указателя инструкции (IP) из памяти,
     * определяет обработчик для выполнения инструкции и вызывает соответствующий функциональный объект.
     * В случае ошибки чтения инструкции, её неправильного формата или отсутствия обработчика
     * устанавливается состояние остановки процессора и может выбрасываться исключение.
     *
     * @throws std::runtime_error Если обработчик инструкции не определен для кодов операции.
     */
    void ExecuteInstruction();
    /**
     * @brief Переходит к выполнению следующей инструкции в процессоре.
     *
     * Увеличивает значение регистра указателя инструкции (IP) на единицу, устанавливая его
     * на адрес следующей инструкции для последующего выполнения.
     */
    void NextInstruction();
    /**
     * @brief Устанавливает указанный статус процессора.
     * @param state Новое состояние процессора типа ProcessorState.
     */
    void SetState(snm::ProcessorState state);

    /**
     * @brief Определяет тип данных в зависимости от переданного шаблонного параметра.
     *
     * Метод сравнивает тип шаблонного параметра T с предопределенными типами, такими как
     * snm::Byte, snm::Word, snm::SignedWord, snm::Real, и возвращает соответствующее значение из перечисления snm::Type.
     *
     * @return Значение перечисления snm::Type, соответствующее типу данных.
     */
    template <class T>
    static snm::Type TypeIo();

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
    template <typename T>
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
