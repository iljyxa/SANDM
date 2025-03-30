#ifndef PROCESSOR_OBSERVER_HPP
#define PROCESSOR_OBSERVER_HPP
#include "common_definitions.hpp"

/**
 * @class ProcessorObserver
 * @brief Абстрактный класс наблюдателя за процессором.
 *
 * Этот класс определяет интерфейс для объектов, которые хотят получать
 * уведомления об изменениях состояния процессора.
 */
class ProcessorObserver {
public:
    /**
     * @brief Вызывается при изменении указателя инструкций.
     * @param instruction_pointer Новое значение указателя инструкций.
     */
    virtual void OnRegisterIpChanged(common::DoubleByte& instruction_pointer) = 0;
    /**
     * @brief Вызывается при изменении значения аккумулятора.
     * @param accumulator Новое значение аккумулятора.
     */
    virtual void OnRegisterAccChanged(const common::Bytes& accumulator) = 0;
    /**
     * @brief Вызывается при изменении значения вспомогательного регистра.
     * @param auxiliary Новое значение вспомогательного регистра.
     */
    virtual void OnRegisterAuxChanged(const common::Bytes& auxiliary) = 0;
    /**
     * @brief Вызывается при изменении индекса таблицы страниц.
     * @param page_table_index Новое значение индекса таблицы страниц.
     */
    virtual void OnRegisterPtiChanged(common::Byte& page_table_index) = 0;
    /**
     * @brief Вызывается при изменении памяти.
     * @param address Адрес измененной ячейки памяти.
     */
    virtual void OnMemoryChanged(common::DoubleByte& address) = 0;
    /**
     * @brief Вызывается при изменении статуса процессора.
     * @param is_running Флаг, указывающий, запущен ли процессор.
     */
    virtual void OnStatusChanged(bool& is_running) = 0;

    virtual ~ProcessorObserver() = default;
};

#endif
