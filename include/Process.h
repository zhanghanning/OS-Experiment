#ifndef OS_EXPERIMENT_PROCESS_H
#define OS_EXPERIMENT_PROCESS_H

#include <cstddef>
#include <string>
#include <vector>

enum class ProcessState {
    Ready,
    Ended
};

char stateToChar(ProcessState state);
std::string stateToText(ProcessState state);

struct PCB {
    std::string name;
    int requiredTime = 0;
    int usedTime = 0;
    ProcessState state = ProcessState::Ready;
    int nextIndex = -1;                 // 模拟实验指导书中的“指针”字段
    std::vector<int> pageStream;        // 综合运行时该进程访问的页号序列
    std::size_t nextPageOffset = 0;

    bool isEnded() const;
    bool hasNextPage() const;
    int currentPage() const;
    void consumeOnePage();
    void runOneTimeSlice();
};

#endif // OS_EXPERIMENT_PROCESS_H
