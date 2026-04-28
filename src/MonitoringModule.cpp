#include "MonitoringModule.hpp"
#include "MonitoringModuleException.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <limits>
#include <optional>
#include <sstream>
#include <utility>


MonitoringModule::MonitoringModule(){
    setName("MonitoringModule");
    setStatus(ProcessStatus::STOPPED);
}

MonitoringModule::~MonitoringModule() {
    if (running_.load()) {
        try { stop(); } catch (...) { /* dtor: swallow */ }
    }
}

void MonitoringModule::initialize() {
    try {
        messageBus = std::make_unique<BaseMemory>(MESSAGE_BUS_NAME);
        Result res = messageBus->createConnection();
        if (!res.result) {
            throw MonitoringModuleException(
                "Failed to create MessageBus connection: " + res.message);
        }
        messageBus->subscribeTag(SUBSCRIBE_TO_TAG);
        if (!res.result) {
            throw MonitoringModuleException(
                "Failed to subscribe to tag: " + res.message);
        }

        messageBus->subscribeTag(SUBSCRIBE_TO_TAG);
        //bus_->subscribeTag("metrics_query");

        sources["cpu"]  = std::make_unique<CpuCollector>();
        sources["mem"]  = std::make_unique<MemoryCollector>();
        sources["proc"] = std::make_unique<ProcCollector>();

        registerStandardMetrics();

        setStatus(ProcessStatus::STOPPED);
        publishLogInfo("MonitoringModule initialized");
    } catch (const std::exception& e) {
        setStatus(ProcessStatus::FAILED);
        publishLogError(std::string("initialize() failed: ") + e.what());
        throw;
    }
}

void MonitoringModule::start() {
    if (running_.exchange(true)) return;          
    setStatus(ProcessStatus::STARTING);
    try {
        collectorThread_ = std::thread(&MonitoringModule::collectorLoop, this);
        reporterThread_  = std::thread(&MonitoringModule::reporterLoop,  this);
        listenerThread_  = std::thread(&MonitoringModule::listenerLoop,  this);
        setStatus(ProcessStatus::RUNNING);
        publishLogInfo("MonitoringModule started");
    } catch (const std::exception& e) {
        running_ = false;
        setStatus(ProcessStatus::FAILED);
        publishLogError(std::string("start() failed: ") + e.what());
        throw;
    }
}


