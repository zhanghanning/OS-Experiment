#include "MemoryManager.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

LRUMemoryManager::LRUMemoryManager(int frameCount) {
    reset(frameCount);
}

void LRUMemoryManager::reset(int frameCount) {
    if (frameCount <= 0) {
        throw std::invalid_argument("物理块数量必须大于 0");
    }

    frameCount_ = frameCount;
    clock_ = 0;
    pageFaults_ = 0;
    replacements_ = 0;
    totalAccesses_ = 0;
    frames_.clear();
    lastUsedTime_.clear();
}

MemoryAccessResult LRUMemoryManager::accessPage(int page) {
    if (page < 0) {
        throw std::invalid_argument("页号不能为负数");
    }

    ++clock_;
    ++totalAccesses_;

    MemoryAccessResult result;
    result.page = page;

    if (containsPage(page)) {
        result.hit = true;
        lastUsedTime_[page] = clock_;
    } else {
        result.hit = false;
        ++pageFaults_;

        if (static_cast<int>(frames_.size()) < frameCount_) {
            frames_.push_back(page);
        } else {
            int victimIndex = findLeastRecentlyUsedFrameIndex();
            int victimPage = frames_[victimIndex];
            lastUsedTime_.erase(victimPage);
            frames_[victimIndex] = page;
            ++replacements_;
            result.replaced = true;
            result.evictedPage = victimPage;
        }

        lastUsedTime_[page] = clock_;
    }

    result.frames = frames_;
    result.pageFaults = pageFaults_;
    result.replacements = replacements_;
    result.totalAccesses = totalAccesses_;
    result.hitRate = hitRate();
    return result;
}

int LRUMemoryManager::frameCount() const {
    return frameCount_;
}

int LRUMemoryManager::pageFaults() const {
    return pageFaults_;
}

int LRUMemoryManager::replacements() const {
    return replacements_;
}

int LRUMemoryManager::totalAccesses() const {
    return totalAccesses_;
}

double LRUMemoryManager::hitRate() const {
    if (totalAccesses_ == 0) {
        return 0.0;
    }
    return 1.0 - static_cast<double>(pageFaults_) / static_cast<double>(totalAccesses_);
}

const std::vector<int>& LRUMemoryManager::frames() const {
    return frames_;
}

std::vector<int> LRUMemoryManager::generateInstructionStream(int length, unsigned int seed) {
    if (length <= 0) {
        return {};
    }

    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> startDist(0, 318);
    std::vector<int> instructions;
    instructions.reserve(length);

    auto pushAddress = [&](int address) {
        if (static_cast<int>(instructions.size()) < length) {
            address = std::max(0, std::min(319, address));
            instructions.push_back(address);
        }
    };

    while (static_cast<int>(instructions.size()) < length) {
<<<<<<< HEAD
        // 每轮生成 4 次访问：顺序、低地址、顺序、高地址，近似满足 50%/25%/25%。
=======
        // 起点 m 用来产生第一条顺序执行地址 m + 1。
        // 每轮产生 4 次访问：顺序、低地址、顺序、高地址，近似满足 50%/25%/25%。
>>>>>>> d43ace2f57b7d226bfea79c0fbde8a92a3578f75
        int m = startDist(rng);
        pushAddress(m + 1);

        std::uniform_int_distribution<int> lowDist(0, std::min(319, m + 1));
        int m1 = lowDist(rng);
        pushAddress(m1);
        pushAddress(m1 + 1);

        int highBegin = std::min(319, m1 + 2);
        std::uniform_int_distribution<int> highDist(highBegin, 319);
        int m2 = highDist(rng);
        pushAddress(m2);
    }

    return instructions;
}

std::vector<int> LRUMemoryManager::toPageStream(const std::vector<int>& instructionStream, int instructionsPerPage) {
    if (instructionsPerPage <= 0) {
        throw std::invalid_argument("每页指令数必须大于 0");
    }

    std::vector<int> pages;
    pages.reserve(instructionStream.size());
    for (int instruction : instructionStream) {
        pages.push_back(instruction / instructionsPerPage);
    }
    return pages;
}

SimulationSummary LRUMemoryManager::simulateLRU(const std::vector<int>& pageStream, int frameCount) {
    LRUMemoryManager manager(frameCount);
    for (int page : pageStream) {
        manager.accessPage(page);
    }

    SimulationSummary summary;
    summary.frameCount = frameCount;
    summary.totalAccesses = manager.totalAccesses();
    summary.pageFaults = manager.pageFaults();
    summary.replacements = manager.replacements();
    summary.hitRate = manager.hitRate();
    return summary;
}

bool LRUMemoryManager::containsPage(int page) const {
    return std::find(frames_.begin(), frames_.end(), page) != frames_.end();
}

int LRUMemoryManager::findLeastRecentlyUsedFrameIndex() const {
    if (frames_.empty()) {
        throw std::runtime_error("物理块为空，无法执行页面置换");
    }

    int victimIndex = 0;
    int earliestTime = std::numeric_limits<int>::max();

    for (int i = 0; i < static_cast<int>(frames_.size()); ++i) {
        int page = frames_[i];
        auto it = lastUsedTime_.find(page);
        int usedTime = (it == lastUsedTime_.end()) ? -1 : it->second;
        if (usedTime < earliestTime) {
            earliestTime = usedTime;
            victimIndex = i;
        }
    }

    return victimIndex;
}
