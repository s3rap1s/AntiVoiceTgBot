#pragma once

#include "utils/types.hpp"

#include <condition_variable>
#include <queue>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

class TaskManager {
  public:
    TaskManager();
    ~TaskManager() noexcept;
    void startTask(const InlineMessageId& inlineMessageId, std::jthread&& thread);
    void notifyFinished(const InlineMessageId& inlineMessageId) noexcept;

  private:
    void cleanupLoop();

    std::shared_mutex tasksMutex;
    std::unordered_map<std::string, std::jthread> activeTasks;

    std::mutex queueMutex;
    std::queue<std::string> finishedTasksQueue;
    std::condition_variable queueCV;

    std::jthread cleanupThread;
};
