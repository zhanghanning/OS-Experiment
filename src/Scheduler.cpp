#include "Scheduler.h"

#include <stdexcept>

void RoundRobinScheduler::reset(const std::vector<PCB>& processes) {
    readyQueue_.clear();
    for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
        if (!processes[i].isEnded()) {
            readyQueue_.push_back(i);
        }
    }
}

bool RoundRobinScheduler::hasReadyProcess() const {
    return !readyQueue_.empty();
}

int RoundRobinScheduler::dispatch() {
    if (readyQueue_.empty()) {
        throw std::runtime_error("没有就绪进程可调度");
    }

    int processIndex = readyQueue_.front();
    readyQueue_.pop_front();
    return processIndex;
}

void RoundRobinScheduler::finishTimeSlice(const PCB& process, int processIndex) {
    if (!process.isEnded()) {
        readyQueue_.push_back(processIndex);
    }
}

std::vector<int> RoundRobinScheduler::readyQueueSnapshot() const {
    return std::vector<int>(readyQueue_.begin(), readyQueue_.end());
}
