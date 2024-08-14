#include "TaskQueue.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace Yo::Thread {
    TaskQueue::TaskQueue() {
        mRunning = true;
        mThread  = std::thread(&TaskQueue::run, this);
        std::unique_lock lock(mTaskQueueMutex);
        mQueueReady.wait(lock);
    }

    TaskQueue::~TaskQueue() {
        mRunning = false;
        mCVNotify.notify_one();
        mThread.join();
    }

    void TaskQueue::AddTask(std::function<void(void)> func, std::string id, bool rm_same_id) {
        std::unique_lock lock(mTaskQueueMutex);
        bool             find_same = false;
        for (int i = 0; i < std::ssize(mTaskQueue); i++) {
            auto &[_task, _id] = mTaskQueue.front();
            if (rm_same_id && _id == id) {
                _task     = func;
                find_same = true;
            }
            mTaskQueue.push({_task, _id});
            mTaskQueue.pop();
        }
        if (!rm_same_id || !find_same) {
            mTaskQueue.push({func, id});
        }
        mCVNotify.notify_one();
    }

    void TaskQueue::AddTaskToGlobalQueue(std::function<void(void)> func, std::string id, bool rm_same_id) {
        static TaskQueue gTaskQueue;
        gTaskQueue.AddTask(func, id, rm_same_id);
    }

    void TaskQueue::run(void) {
        mQueueReady.notify_one();
        while (mRunning) {
            std::unique_lock lock(mTaskQueueMutex);
            mCVNotify.wait(lock);
            while (!mTaskQueue.empty()) {
                auto &[task, id] = mTaskQueue.front();
                task();
                mTaskQueue.pop();
            }
        }
    }

} // namespace Yo::Thread
