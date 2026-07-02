#include "Process.h"

#include <stdexcept>

char stateToChar(ProcessState state) {
    return state == ProcessState::Ready ? 'R' : 'E';
}

std::string stateToText(ProcessState state) {
    return state == ProcessState::Ready ? "就绪" : "结束";
}

bool PCB::isEnded() const {
    return state == ProcessState::Ended;
}

bool PCB::hasNextPage() const {
    return nextPageOffset < pageStream.size();
}

int PCB::currentPage() const {
    if (!hasNextPage()) {
        throw std::out_of_range("进程已经没有可访问的页号");
    }
    return pageStream[nextPageOffset];
}

void PCB::consumeOnePage() {
    if (hasNextPage()) {
        ++nextPageOffset;
    }
}

void PCB::runOneTimeSlice() {
    if (state == ProcessState::Ended) {
        return;
    }

    ++usedTime;
    consumeOnePage();

    if (usedTime >= requiredTime) {
        state = ProcessState::Ended;
        nextIndex = -1;
    }
}
