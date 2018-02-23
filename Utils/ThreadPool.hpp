//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_THREADPOOL_HPP
#define JSONSLAVE_THREADPOOL_HPP

#include "../Config.hpp"

class ThreadPool
{
public:
	explicit ThreadPool(int count);

	~ThreadPool();

	/**
	 * Stop the thread pool
	 */
	void Stop();

	/**
	 * Add a new job to be executed by the thread pool
	 *
	 * @tparam Func
	 * @param f
	 */
	template<class Func>
	void AddJob(Func&& f);

private:
	void ProcessWorker();

	void AddTask(shared_ptr<packaged_task<void()>> task);

private:
	vector<thread> mWorkers;
	queue<function<void()>> mTasks;
	mutex mMutex;
	condition_variable mTrigger;
	atomic_bool mRunning;

};

ThreadPool::ThreadPool(int count) : mRunning(true) {
	for (int i = 0; i < count; ++i) {
		mWorkers.emplace_back(
				[this]() -> void {
					this->ProcessWorker();
				}
		);
	}
}

ThreadPool::~ThreadPool() {
	Stop();
}

template<class Func>
void ThreadPool::AddJob(Func&& f) {
	auto task = std::make_shared<std::packaged_task<void()>>(std::bind(std::forward<Func>(f)));
	auto futureResult = task->get_future();
	AddTask(task);
	mTrigger.notify_one();
}

void ThreadPool::Stop() {
	cout << "Shutting down connection pool" << endl;
	mRunning = false;
	mTrigger.notify_all();
	for (auto&& worker : mWorkers) {
		worker.join();
	}
}

void ThreadPool::ProcessWorker() {
	while (true) {
		function<void()> task;
		{
			unique_lock<mutex> l(mMutex);
			mTrigger.wait(l, [this] {
				return !mRunning || !mTasks.empty();
			});
			if (!mRunning && mTasks.empty())
				return;
			task = move(mTasks.front());
			mTasks.pop();
		}
		task();
	}
}

void ThreadPool::AddTask(shared_ptr<packaged_task<void()>> task) {
	unique_lock<mutex> l(mMutex);
	if (!mRunning)
		throw runtime_error("Cannot add tasks when the ThreadPool is not running anymore");
	mTasks.emplace([task]() { (*task)(); });
}


#endif //JSONSLAVE_THREADPOOL_HPP
