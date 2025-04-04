#ifndef VIRTUAL_MACHINE_CONTROLLER_HPP
#define VIRTUAL_MACHINE_CONTROLLER_HPP

#include <QMap>
#include <QSet>
#include <QThreadPool>

#include "../../core/include/processor_observer.hpp"
#include "../../core/include/virtual_machine.hpp"

/**
 * @enum VmState
 * @brief Перечисление состояний виртуальной машины.
 */
enum VmState {
    STOPPED, ///< Виртуальная машина остановлена
    RUNNING, ///< Виртуальная машина выполняется
    PAUSED ///< Виртуальная машина приостановлена
};

/**
 * @class VirtualMachineController
 * @brief Контроллер виртуальной машины, управляющий её состоянием и взаимодействием.
 *
 * Этот класс обеспечивает интерфейс для управления виртуальной машиной, включая
 * загрузку байт-кода, управление точками останова, выполнение и отладку программы.
 */
class VirtualMachineController final : public QObject, public VirtualMachine, public ProcessorObserver {
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса VirtualMachineController.
     * @param processor_io Указатель на объект ProcessorIo для взаимодействия с вводом/выводом.
     * @param parent Родительский QObject (по умолчанию nullptr).
     */
    explicit VirtualMachineController(ProcessorIo* processor_io = nullptr, QObject* parent = nullptr);

    /**
     * @brief Загружает байт-код и карту соответствия исходного кода байт-коду.
     * @param byte_code Байт-код для загрузки.
     * @param source_to_bytecode_map Карта соответствия исходного кода байт-коду.
     */
    void Load(const common::ByteCode& byte_code, common::SourceToBytecodeMap& source_to_bytecode_map);
    /**
     * @brief Возвращает текущее состояние виртуальной машины.
     * @return Текущее состояние VmState.
     */
    VmState GetState() const;
    /**
     * @brief Возвращает номер текущей строки исполняемого кода.
     * @return Номер текущей строки кода.
     */
    unsigned int GetCurrentCodeLine();
    /**
     * @brief Сбрасывает состояние процессора включая регистры, но не сбрасывает память
     */
    void ResetProcessor() const;

    // === Методы-наблюдатели, реализующие интерфейс ProcessorObserver ===

    void OnRegisterPtiChanged(common::Byte& page_table_index) override;
    void OnMemoryChanged(common::DoubleByte& address) override;
    void OnStatusChanged(bool& is_running) override;
    void OnRegisterAccChanged(const common::Bytes& accumulator) override;
    void OnRegisterAuxChanged(const common::Bytes& auxiliary) override;
    void OnRegisterIpChanged(common::DoubleByte& instruction_pointer) override;

public slots:
    // === Управление состоянием машины ===
    /**
     * @brief Запускает виртуальную машину.
     */
    void OnRun();
    /**
     * @brief Останавливает выполнение виртуальной машины.
     */
    void OnStop();
    /**
     * @brief Выполняет один шаг программы.
     */
    void OnStep();
    /**
     * @brief Останавливает и сбрасывает полностью состояние виртуальной машины.
     */
    void OnReset();
    /**
     * @brief Запускает виртуальную машину в режиме отладки.
     */
    void OnDebug();
    /**
     * @brief Приостанавливает или продолжает выполнение программы.
     */
    void OnPauseContinue();

    // === Точки останова ===

    /**
     * @brief Устанавливает точку останова.
     * @param breakpoint Номер строки исходного кода для установки точки останова.
     */
    void OnInsertBreakpoint(unsigned int breakpoint);
    /**
     * @brief Удаляет точку останова.
     * @param breakpoint Номер строки исходного кода для удаления точки останова.
     */
    void OnRemoveBreakpoint(unsigned int breakpoint);
    /**
     * @brief Очищает все точки останова.
     */
    void OnClearBreakpoints();

    // === Изменение регистров ===

    /**
     * @brief Обрабатывает изменение значения аккумулятора.
     * @param value Новое значение аккумулятора.
     */
    void OnAccumulatorEdited(int value);
    /**
     * @brief Обрабатывает изменение значения вспомогательного регистра.
     * @param value Новое значение вспомогательного регистра.
     */
    void OnAuxiliaryEdited(int value);
    /**
     * @brief Обрабатывает изменение значения указателя инструкций.
     * @param value Новое значение указателя инструкций.
     */
    void OnInstructionPointerEdited(int value);
    /**
     * @brief Обрабатывает изменение значения индекса таблицы страниц.
     * @param value Новое значение индекса таблицы страниц.
     */
    void OnPageTableIndexEdited(int value);

signals:
    /**
     * @brief Сигнал об изменении состояния виртуальной машины.
     * @param state Новое состояние виртуальной машины.
     * @param debugging Флаг, указывающий на режим отладки.
     */
    void StateChanged(VmState state, bool debugging);
    /**
     * @brief Сигнал для обновления интерфейса.
     */
    void Update();
    /**
     * @brief Сигнал о сбросе виртуальной машины.
     */
    void Reseted();
    /**
     * @brief Сигнал о возникновении ошибки.
     * @param error Описание ошибки.
     */
    void ErrorOccurred(const QString& error);

private:
    VmState state_; ///< Текущее состояние
    bool debugging_; ///< Признак работы в режиме отладки
    QSet<common::DoubleByte> breakpoints_; ///< Точки останова для байт-кода
    QSet<unsigned int> source_breakpoints_; ///< Точки останова для исходного кода
    common::SourceToBytecodeMap source_to_bytecode_map_; ///< Карта соответствия исходного кода байт-коду
    common::BytecodeToSourceMap bytecode_to_source_map_; ///< Карта соответствия байт-кода исходному коду

    /**
     * @brief Устанавливает новое состояние виртуальной машины.
     * @param state Новое состояние.
     */
    void SetState(VmState state);
    /**
     * @brief Обновляет точки останова в соответствии с байт-кодом
     */
    void UpdateBreakpoints();
};

#endif //VIRTUAL_MACHINE_CONTROLLER_HPP
