#include "MiniOS.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>

void MiniOS::runInteractive() {
    while (true) {
        std::cout << "\n========== Mini OS Simulator ==========" << '\n';
        std::cout << "1. 实验一：处理机调度（时间片轮转）" << '\n';
        std::cout << "2. 实验二：虚拟存储页面置换（LRU）" << '\n';
        std::cout << "3. 综合运行：调度 + LRU 存储管理" << '\n';
        std::cout << "4. 一键演示全部实验" << '\n';
        std::cout << "0. 退出" << '\n';
        std::cout << "请选择：";

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入。" << '\n';
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                runProcessorSchedulingExperiment();
                break;
            case 2:
                runMemoryExperiment();
                break;
            case 3:
                runIntegratedExperiment();
                break;
            case 4:
                runDemo();
                break;
            case 0:
                std::cout << "系统退出。" << '\n';
                return;
            default:
                std::cout << "没有该选项，请重新输入。" << '\n';
        }
    }
}

void MiniOS::runDemo() {
    runProcessorSchedulingExperiment();
    runMemoryExperiment(true);
    runIntegratedExperiment(true);
}

std::vector<PCB> MiniOS::createDefaultProcesses() const {
    std::vector<PCB> processes = {
        {"Q1", 2, 0, ProcessState::Ready, 1, {}, 0},
        {"Q2", 3, 0, ProcessState::Ready, 2, {}, 0},
        {"Q3", 1, 0, ProcessState::Ready, 3, {}, 0},
        {"Q4", 2, 0, ProcessState::Ready, 4, {}, 0},
        {"Q5", 4, 0, ProcessState::Ready, 0, {}, 0}
    };
    return processes;
}

std::vector<PCB> MiniOS::createProcessesWithPages(const std::vector<int>& pageStream) const {
    std::vector<PCB> processes = createDefaultProcesses();
    std::size_t cursor = 0;

    for (auto& process : processes) {
        for (int i = 0; i < process.requiredTime; ++i) {
            if (cursor < pageStream.size()) {
                process.pageStream.push_back(pageStream[cursor++]);
            } else {
                process.pageStream.push_back(0);
            }
        }
    }
    return processes;
}

void MiniOS::runProcessorSchedulingExperiment() {
    std::cout << "\n========== 实验一：处理机调度 ==========" << '\n';
    std::cout << "调度算法：时间片轮转法（Round Robin），时间片大小为 1。" << '\n';

    auto processes = createDefaultProcesses();
    RoundRobinScheduler scheduler;
    scheduler.reset(processes);

    std::cout << "\n初始 PCB 状态：" << '\n';
    printPCBTable(processes);
    printReadyQueue(processes, scheduler.readyQueueSnapshot());

    int timeSlice = 0;
    while (scheduler.hasReadyProcess()) {
        ++timeSlice;
        int index = scheduler.dispatch();
        PCB& current = processes[index];

        std::cout << "\n[时间片 " << timeSlice << "] 选中进程：" << current.name << '\n';
        current.runOneTimeSlice();

        scheduler.finishTimeSlice(current, index);
        auto queueAfterFinish = scheduler.readyQueueSnapshot();
        for (auto& p : processes) {
            p.nextIndex = -1;
        }
        for (std::size_t i = 0; i < queueAfterFinish.size(); ++i) {
            int curr = queueAfterFinish[i];
            int next = queueAfterFinish[(i + 1) % queueAfterFinish.size()];
            processes[curr].nextIndex = next;
        }

        std::cout << "运行后 PCB 状态：" << '\n';
        printPCBTable(processes);
        printReadyQueue(processes, scheduler.readyQueueSnapshot());
    }

    std::cout << "\n所有进程均已结束，处理机调度实验完成。" << '\n';
}

void MiniOS::runMemoryExperiment(bool useDefaultInput) {
    std::cout << "\n========== 实验二：虚拟存储页面置换 ==========" << '\n';
    std::cout << "页面置换算法：LRU（最近最少使用）。" << '\n';

    int frameCountForDetail = useDefaultInput ? 4 : readInt("请输入用于详细展示的物理块数 [默认 4，范围 4-32]：", 4, 4, 32);
    unsigned int seed = static_cast<unsigned int>(useDefaultInput ? 2026 : readInt("请输入随机种子 [默认 2026]：", 2026, 1, 999999));

    auto instructions = LRUMemoryManager::generateInstructionStream(320, seed);
    auto pages = LRUMemoryManager::toPageStream(instructions);

    std::cout << "\n生成的页地址流如下：" << '\n';
    printPageStream(pages, 320);

    std::cout << "\n前 40 次访问的内存物理块变化：" << '\n';
    LRUMemoryManager detailManager(frameCountForDetail);
    for (int i = 0; i < 40 && i < static_cast<int>(pages.size()); ++i) {
        auto result = detailManager.accessPage(pages[i]);
        std::cout << "访问序号 " << std::setw(3) << i + 1
                  << " | 页号 " << std::setw(2) << result.page
                  << " | " << (result.hit ? "命中" : "缺页");
        if (result.replaced && result.evictedPage.has_value()) {
            std::cout << " | 淘汰页 " << std::setw(2) << *result.evictedPage;
        } else {
            std::cout << " | 淘汰页 --";
        }
        std::cout << " | 物理块：";
        printFrames(result.frames, frameCountForDetail);
        std::cout << '\n';
    }

    std::cout << "\n不同内存容量下的 LRU 命中率：" << '\n';
    std::cout << std::left << std::setw(10) << "物理块数"
              << std::setw(12) << "访问次数"
              << std::setw(12) << "缺页次数"
              << std::setw(12) << "置换次数"
              << "命中率" << '\n';
    std::cout << std::string(58, '-') << '\n';

    for (int frames = 4; frames <= 32; ++frames) {
        auto summary = LRUMemoryManager::simulateLRU(pages, frames);
        std::cout << std::left << std::setw(10) << summary.frameCount
                  << std::setw(12) << summary.totalAccesses
                  << std::setw(12) << summary.pageFaults
                  << std::setw(12) << summary.replacements
                  << std::fixed << std::setprecision(4) << summary.hitRate << '\n';
    }
}

void MiniOS::runIntegratedExperiment(bool useDefaultInput) {
    std::cout << "\n========== 综合运行：调度 + LRU 存储管理 ==========" << '\n';
    std::cout << "处理机调度：时间片轮转；页面置换：LRU。" << '\n';

    int frameCount = useDefaultInput ? 4 : readInt("请输入综合运行的物理块数 [默认 4，范围 4-32]：", 4, 4, 32);
    unsigned int seed = static_cast<unsigned int>(useDefaultInput ? 2026 : readInt("请输入随机种子 [默认 2026]：", 2026, 1, 999999));

    auto instructions = LRUMemoryManager::generateInstructionStream(320, seed);
    auto pages = LRUMemoryManager::toPageStream(instructions);
    auto processes = createProcessesWithPages(pages);

    RoundRobinScheduler scheduler;
    LRUMemoryManager memory(frameCount);
    scheduler.reset(processes);

    std::cout << "\n初始 PCB 状态：" << '\n';
    printPCBTable(processes);
    printReadyQueue(processes, scheduler.readyQueueSnapshot());

    int timeSlice = 0;
    while (scheduler.hasReadyProcess()) {
        ++timeSlice;
        int index = scheduler.dispatch();
        PCB& current = processes[index];

        int page = current.hasNextPage() ? current.currentPage() : 0;
        auto memoryResult = memory.accessPage(page);
        current.runOneTimeSlice();
        scheduler.finishTimeSlice(current, index);

        auto queueAfterFinish = scheduler.readyQueueSnapshot();
        for (auto& p : processes) {
            p.nextIndex = -1;
        }
        for (std::size_t i = 0; i < queueAfterFinish.size(); ++i) {
            int curr = queueAfterFinish[i];
            int next = queueAfterFinish[(i + 1) % queueAfterFinish.size()];
            processes[curr].nextIndex = next;
        }

        std::cout << "\n[时间片 " << timeSlice << "]" << '\n';
        std::cout << "运行进程：" << current.name << '\n';
        std::cout << "访问页号：" << page << '\n';
        std::cout << "页面结果：" << (memoryResult.hit ? "命中" : "缺页");
        if (memoryResult.replaced && memoryResult.evictedPage.has_value()) {
            std::cout << "，淘汰页 " << *memoryResult.evictedPage;
        }
        std::cout << '\n';
        std::cout << "当前物理块：";
        printFrames(memoryResult.frames, frameCount);
        std::cout << '\n';
        std::cout << "累计缺页次数：" << memoryResult.pageFaults
                  << "，累计置换次数：" << memoryResult.replacements
                  << "，当前命中率：" << std::fixed << std::setprecision(4) << memoryResult.hitRate << '\n';

        std::cout << "当前 PCB 状态：" << '\n';
        printPCBTable(processes);
        printReadyQueue(processes, scheduler.readyQueueSnapshot());
    }

    std::cout << "\n综合运行完成。" << '\n';
    std::cout << "总访问次数：" << memory.totalAccesses()
              << "，缺页次数：" << memory.pageFaults()
              << "，置换次数：" << memory.replacements()
              << "，命中率：" << std::fixed << std::setprecision(4) << memory.hitRate() << '\n';
}

void MiniOS::printPCBTable(const std::vector<PCB>& processes) const {
    std::cout << std::left << std::setw(8) << "进程名"
              << std::setw(14) << "要求运行时间"
              << std::setw(14) << "已运行时间"
              << std::setw(10) << "状态"
              << "next" << '\n';
    std::cout << std::string(58, '-') << '\n';

    for (const auto& process : processes) {
        std::string nextName = "-";
        if (process.nextIndex >= 0 && process.nextIndex < static_cast<int>(processes.size())) {
            nextName = processes[process.nextIndex].name;
        }

        std::cout << std::left << std::setw(8) << process.name
                  << std::setw(14) << process.requiredTime
                  << std::setw(14) << process.usedTime
                  << std::setw(10) << std::string(1, stateToChar(process.state))
                  << nextName << '\n';
    }
}

void MiniOS::printReadyQueue(const std::vector<PCB>& processes, const std::vector<int>& readyQueue) const {
    std::cout << "就绪队列：";
    if (readyQueue.empty()) {
        std::cout << "空" << '\n';
        return;
    }

    for (std::size_t i = 0; i < readyQueue.size(); ++i) {
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << processes[readyQueue[i]].name;
    }
    std::cout << '\n';
}

void MiniOS::printFrames(const std::vector<int>& frames, int frameCount) const {
    auto oldFlags = std::cout.flags();
    std::cout << '[';
    for (int i = 0; i < frameCount; ++i) {
        if (i > 0) {
            std::cout << ' ';
        }
        if (i < static_cast<int>(frames.size())) {
            std::cout << std::right << std::setw(2) << frames[i];
        } else {
            std::cout << " -";
        }
    }
    std::cout << ']';
    std::cout.flags(oldFlags);
}

void MiniOS::printPageStream(const std::vector<int>& pageStream, int maxCount) const {
    int count = std::min(maxCount, static_cast<int>(pageStream.size()));
    for (int i = 0; i < count; ++i) {
        std::cout << std::setw(2) << pageStream[i] << ' ';
        if ((i + 1) % 32 == 0) {
            std::cout << '\n';
        }
    }
    if (count % 32 != 0) {
        std::cout << '\n';
    }
}

int MiniOS::readInt(const std::string& prompt, int defaultValue, int minValue, int maxValue) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return defaultValue;
    }

    try {
        int value = std::stoi(line);
        if (value < minValue || value > maxValue) {
            std::cout << "输入超出范围，使用默认值 " << defaultValue << "。" << '\n';
            return defaultValue;
        }
        return value;
    } catch (...) {
        std::cout << "输入无效，使用默认值 " << defaultValue << "。" << '\n';
        return defaultValue;
    }
}
