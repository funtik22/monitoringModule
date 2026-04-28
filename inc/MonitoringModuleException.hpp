//ConfigModuleException
#pragma once

#include <stdexcept>

class MonitoringModuleException : public std::runtime_error {
public:
    explicit MonitoringModuleException(const std::string& message)
        :  std::runtime_error(message){};
 
};
