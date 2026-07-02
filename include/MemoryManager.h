#ifndef OS_EXPERIMENT_MEMORY_MANAGER_H
#define OS_EXPERIMENT_MEMORY_MANAGER_H

#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

struct MemoryAccessResult {
    int page = -1;
    bool hit = false;
    bool replaced = false;
    std::optional<int> evictedPage;
    std::vector<int> frames;
    int pageFaults = 0;
    int replacements = 0;
    int totalAccesses = 0;
    double hitRate = 0.0;
};

struct SimulationSummary {
    int frameCount = 0;
    int totalAccesses = 0;
    int pageFaults = 0;
    int replacements = 0;
    double hitRate = 0.0;
};

class LRUMemoryManager {
public:
    explicit LRUMemoryManager(int frameCount = 4);

    void reset(int frameCount);
    MemoryAccessResult accessPage(int page);

    int frameCount() const;
    int pageFaults() const;
    int replacements() const;
    int totalAccesses() const;
    double hitRate() const;
    const std::vector<int>& frames() const;

    static std::vector<int> generateInstructionStream(int length = 320, unsigned int seed = std::random_device{}());
    static std::vector<int> toPageStream(const std::vector<int>& instructionStream, int instructionsPerPage = 10);
    static SimulationSummary simulateLRU(const std::vector<int>& pageStream, int frameCount);

private:
    int frameCount_ = 4;
    int clock_ = 0;
    int pageFaults_ = 0;
    int replacements_ = 0;
    int totalAccesses_ = 0;
    std::vector<int> frames_;
    std::unordered_map<int, int> lastUsedTime_;

    bool containsPage(int page) const;
    int findLeastRecentlyUsedFrameIndex() const;
};

#endif // OS_EXPERIMENT_MEMORY_MANAGER_H
