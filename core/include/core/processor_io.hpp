
#ifndef PROCESSOR_IO_HPP
#define PROCESSOR_IO_HPP

#include "core/common_definitions.hpp"

class ProcessorIo {
public:
    using InputCallback = std::function<void(snm::Bytes)>;

    /**
     * Асинхронный ввод данных
     * @param type - тип вводимых данных
     * @param callback - функция обратного вызова для передачи результата
     */
    virtual void InputRequest(snm::Type type, InputCallback callback) = 0;

    /**
     * Вывод данных
     * @param bytes - данные для вывода
     * @param type - тип данных
     */
    virtual void OutputRequest(snm::Bytes bytes, snm::Type type) = 0;

    virtual ~ProcessorIo() = default;
};

#endif //PROCESSOR_IO_HPP
