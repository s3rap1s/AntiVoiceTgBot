#include "task_manager.hpp"

#include "utils/types.hpp"

#include <mutex>
#include <queue>
#include <string>

TaskManager::TaskManager() : cleanupThread(&TaskManager::cleanupLoop, this) {}

TaskManager::~TaskManager() noexcept {
    cleanupThread.request_stop();
    queueCV.notify_one();
}

void TaskManager::startTask(const InlineMessageId& inlineMessageId, std::jthread&& thread) {
    std::unique_lock lock(tasksMutex);
    activeTasks[inlineMessageId] = std::move(thread);
}

void TaskManager::notifyFinished(const InlineMessageId& inlineMessageId) noexcept {
    {
        std::unique_lock lock(queueMutex);
        finishedTasksQueue.push(inlineMessageId);
    }
    queueCV.notify_one();
}

void TaskManager::cleanupLoop() {
    std::stop_token stop_token = cleanupThread.get_stop_token();
    while (!stop_token.stop_requested()) {
        std::unique_lock lock(queueMutex);

        queueCV.wait(lock, [this, &stop_token] { return !finishedTasksQueue.empty() || stop_token.stop_requested(); });

        if (stop_token.stop_requested())
            break;

        std::queue<std::string> localQueue;
        std::swap(finishedTasksQueue, localQueue);

        std::unique_lock tasksLock(tasksMutex);
        while (!localQueue.empty()) {
            std::string id = localQueue.front();
            localQueue.pop();
            activeTasks.erase(id);
        }
    }
}

