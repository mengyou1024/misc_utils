#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace Yo::Thread {
    class TaskQueue {
    public:
        using FUNC_PAIR  = std::pair<std::function<void(void)>, std::string>;
        using FUNC_QUEUE = std::queue<FUNC_PAIR>;
        using CV_ANY     = std::condition_variable_any;
        using MUT_RE     = std::recursive_mutex;
        explicit TaskQueue();
        ~TaskQueue();

        /**
         * @brief 添加任务到队列
         * @param func 可调用对象
         * @param id 对象ID
         * @param rm_same_id 是否覆盖相同ID
         */
        void AddTask(std::function<void(void)> func, const std::string& id = {}, bool rm_same_id = false);

        /**
         * @brief 添加任务到全局队列
         * @param func 可调用对象
         * @param id 对象ID
         * @param rm_same_id 是否覆盖相同ID
         */
        static void AddTaskToGlobalQueue(std::function<void(void)> func, const std::string& id = {}, bool rm_same_id = false);

    private:
        CV_ANY      mCVNotify       = {};
        CV_ANY      mQueueReady     = {};
        bool        mRunning        = {};
        MUT_RE      mTaskQueueMutex = {};
        FUNC_QUEUE  mTaskQueue      = {};
        std::thread mThread         = {};

        void run(void);
    };

} // namespace Yo::Thread