#ifndef PROCESSOR_OBSERVER_HPP
#define PROCESSOR_OBSERVER_HPP

#include "core/common_definitions.hpp"

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
    virtual void OnRegisterIpChanged(const snm::DoubleByte& instruction_pointer) = 0;
    /**
     * @brief Вызывается при изменении значения аккумулятора.
     * @param accumulator Новое значение аккумулятора.
     */
    virtual void OnRegisterAccChanged(const snm::Bytes& accumulator) = 0;
    /**
     * @brief Вызывается при изменении значения вспомогательного регистра.
     * @param auxiliary Новое значение вспомогательного регистра.
     */
    virtual void OnRegisterAuxChanged(const snm::Bytes& auxiliary) = 0;
    /**
     * @brief Вызывается при изменении памяти.
     * @param address Адрес измененной ячейки памяти.
     */
    virtual void OnMemoryChanged(const snm::DoubleByte& address) = 0;
    /**
     * @brief Вызывается при изменении статуса процессора.
     * @param state Текущее состояние процессора.
     * @see snm::ProcessorState
     */
    virtual void OnStateChanged(const snm::ProcessorState& state) = 0;

    virtual ~ProcessorObserver() = default;
};

#endif
