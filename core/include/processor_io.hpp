
#ifndef PROCESSOR_IO_HPP
#define PROCESSOR_IO_HPP

// TODO: добавить I

#include "common_definitions.hpp"

class ProcessorIo {
public:
    virtual void Input(common::Bytes& bytes, common::Type& type) = 0;
    virtual void Output(common::Bytes& bytes, common::Type& type) = 0;

    virtual ~ProcessorIo() = default;
};

#endif //PROCESSOR_IO_HPP
