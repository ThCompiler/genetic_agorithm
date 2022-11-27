#include "parallel.hpp"

namespace prll
{
    static const double resize_coef = 2;
    static const size_t min_thread_size = 1;

    ThreadsPool::ThreadsPool(size_t _max_threads)
            : _threads()
            , _main_thread(&ThreadsPool::_balance, this)
            , _exit(false)
            , _max_threads(_max_threads) {
        for (size_t i = 0; i < min_thread_size; ++i) {
            _threads.emplace_back(new Thread(_task_mutex, _tasks, _in_thread));
        }
    }

    void ThreadsPool::wait() {
        std::unique_lock<std::mutex> lck(_main_mutex);
        for (auto &thread: _threads) {
            thread->wait();
        }
        lck.unlock();
    }

    void ThreadsPool::set_max_threads(size_t max_threads) {
        std::unique_lock<std::mutex> lck(_main_mutex);
        _max_threads = max_threads;
        lck.unlock();
        _in_balance.notify_one();
    }

    ThreadsPool::~ThreadsPool() {
        if (!_exit) {
            stop();
        }
    }

    ThreadsPool::Thread::Thread(std::mutex &thread_mutex,
                                std::queue<std::function<void(void)>> &task,
                                std::condition_variable &threads)
            : _task_mutex(thread_mutex)
            , _in_thread(threads)
            , _tasks(task)
            , _end(false)
            , _main_thread(&Thread::_main, this)
            , _have_proccess(0) {}

    void ThreadsPool::Thread::join() {
        _main_thread.join();
    }

    void ThreadsPool::Thread::force_join() {
        std::unique_lock<std::mutex> lck(_task_mutex);
        _end = true;
        lck.unlock();
        _in_thread.notify_all();
        if (_main_thread.joinable()) {
            _main_thread.join();
        }
    }

    void ThreadsPool::Thread::wait() {
        std::unique_lock<std::mutex> lck(_task_mutex);
        _wait.wait(lck, [this] {
            return _end || (_tasks.empty() && _have_proccess == 0);
        });
    }

    ThreadsPool::Thread::~Thread() {
        force_join();
    }

    void ThreadsPool::Thread::_main() {
        while (true) {
            std::unique_lock<std::mutex> lck(_task_mutex);

            _in_thread.wait(lck, [this] {
                return _end || !_tasks.empty();
            });

            if (_end) {
                lck.unlock();
                break;
            }

            auto task = _tasks.front();
            _tasks.pop();

            lck.unlock();

            _have_proccess++;
            task();
            _have_proccess--;

            _wait.notify_one();
        }
    }

    void ThreadsPool::_balance() {
        while (true) {
            std::unique_lock<std::mutex> lck(_main_mutex);

            _in_balance.wait(lck, [this] {
                                 return _exit
                                        || _tasks.size() > (size_t) (resize_coef * (double) _threads.size())
                                        || _threads.size() > (size_t) (resize_coef * (double) _tasks.size())
                                        || _threads.size() > _max_threads;
                             }
            );

            if (_exit) {
                while (!_threads.empty()) {
                    _threads.back()->force_join();
                    _threads.pop_back();
                }
                break;
            }

            while (_threads.size() > _max_threads) {
                _threads.back()->wait();
                _threads.back()->force_join();
                _threads.pop_back();
            }

            auto new_size = (size_t) (resize_coef * (double) _threads.size());

            if (_tasks.size() > new_size && _threads.size() < _max_threads) {
                _threads.emplace_back(new Thread(_task_mutex, _tasks, _in_thread));
            } else {
                if (_threads.size() > new_size && _threads.size() > min_thread_size) {
                    _threads.back()->wait();
                    _threads.back()->force_join();
                    _threads.pop_back();
                }
            }
            lck.unlock();
        }
    }

    size_t ThreadsPool::get_count_threads() const {
        return _max_threads;
    }

    void ThreadsPool::stop() {
        _exit = true;
        _in_balance.notify_one();
        _main_thread.join();
    }

    void ThreadsPool::join() {
        std::unique_lock<std::mutex> lck(_main_mutex);
        for (auto &thread: _threads) {
            thread->join();
        }
    }
}