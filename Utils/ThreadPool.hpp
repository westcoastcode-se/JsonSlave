#ifndef JSONSLAVE_THREADPOOL_HPP
#define JSONSLAVE_THREADPOOL_HPP

#include "../Config.hpp"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

/**
 * Based on https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
 */
class ThreadPool
{
public:
	explicit ThreadPool(size_t threads);

	~ThreadPool();

	template<class F>
	future<void> Execute(F&& f);

private:
	vector<thread> mWorkers;
	queue<function<void()>> mTasks;
	mutex mMutex;
	condition_variable mCondition;
	bool mStopped;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
		: mStopped(false) {
	for (size_t i = 0; i < threads; ++i)
		mWorkers.emplace_back(
				[this] {
					while (true) {
						function<void()> task;

						{
							unique_lock<std::mutex> lock(this->mMutex);
							this->mCondition.wait(lock,
							                      [this] { return this->mStopped || !this->mTasks.empty(); });
							if (this->mStopped && this->mTasks.empty())
								return;
							task = std::move(this->mTasks.front());
							this->mTasks.pop();
						}

						task();
					}
				}
		);
}

template<class F>
future<void> ThreadPool::Execute(F&& f) {
	auto task = make_shared<std::packaged_task<void()>>(bind(f));

	future<void> res = task->get_future();
	{
		unique_lock<std::mutex> lock(mMutex);

		// don't allow enqueueing after stopping the pool
		if (mStopped)
			throw runtime_error("Cannot add tasks on a stopped ThreadPool");

		mTasks.emplace([task]() { (*task)(); });
	}
	mCondition.notify_one();
	return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
	{
		unique_lock<std::mutex> lock(mMutex);
		mStopped = true;
	}
	mCondition.notify_all();
	for (thread& worker: mWorkers)
		worker.join();
}

#endif //JSONSLAVE_THREADPOOL_HPP
