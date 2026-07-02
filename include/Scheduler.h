#ifndef OS_EXPERIMENT_SCHEDULER_H
#define OS_EXPERIMENT_SCHEDULER_H

#include "Process.h"

#include <deque>
#include <vector>

class RoundRobinScheduler {
public:
    void reset(const std::vector<PCB>& processes);
    bool hasReadyProcess() const;
    int dispatch();
    void finishTimeSlice(const PCB& process, int processIndex);
    std::vector<int> readyQueueSnapshot() const;

private:
    std::deque<int> readyQueue_;
};

#endif // OS_EXPERIMENT_SCHEDULER_H
