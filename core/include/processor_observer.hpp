#ifndef PROCESSOR_OBSERVER_HPP
#define PROCESSOR_OBSERVER_HPP
#include "common_definitions.hpp"

// TODO: добавить I

class ProcessorObserver {
public:
    virtual void OnRegisterIpChanged(common::DoubleByte& instruction_pointer) = 0;
    virtual void OnRegisterAccChanged(const common::Bytes& accumulator) = 0;
    virtual void OnRegisterAuxChanged(const common::Bytes& auxiliary) = 0;
    virtual void OnRegisterPtiChanged(common::Byte& page_table_index) = 0;
    virtual void OnMemoryChanged(common::DoubleByte& address) = 0;
    virtual void OnStatusChanged(bool& is_running) = 0;

    virtual ~ProcessorObserver() = default;
};

#endif
