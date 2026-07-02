#ifndef OS_EXPERIMENT_MINI_OS_H
#define OS_EXPERIMENT_MINI_OS_H

#include "MemoryManager.h"
#include "Process.h"
#include "Scheduler.h"

#include <string>
#include <vector>

class MiniOS {
public:
    void runInteractive();
    void runDemo();

private:
    std::vector<PCB> createDefaultProcesses() const;
    std::vector<PCB> createProcessesWithPages(const std::vector<int>& pageStream) const;

    void runProcessorSchedulingExperiment();
    void runMemoryExperiment(bool useDefaultInput = false);
    void runIntegratedExperiment(bool useDefaultInput = false);

    void printPCBTable(const std::vector<PCB>& processes) const;
    void printReadyQueue(const std::vector<PCB>& processes, const std::vector<int>& readyQueue) const;
    void printFrames(const std::vector<int>& frames, int frameCount) const;
    void printPageStream(const std::vector<int>& pageStream, int maxCount = 320) const;

    static int readInt(const std::string& prompt, int defaultValue, int minValue, int maxValue);
};

#endif // OS_EXPERIMENT_MINI_OS_H
