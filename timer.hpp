/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:25
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 17:05:00
 * @FilePath: \timer\timer.hpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#ifndef TIMER_H_
#define TIMER_H_

#include "time_wheel.hpp"
#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <deque>
#include <atomic>


namespace confsdk::infrastructure{

class Timer{
public:
    Timer()=default;
    Timer(std::string name):
        tick_span_(100),
        running_(false){
    }


    bool start();

    // void shutdown();

    // int schedule(TimerTask task, int delay);

    int schedule(std::function<void ()> task, int delay, int period=0, int repeat_times=1);

    // void cancelTask(int taskid);

    // void cancelAllTask();


private:

    void build_time_wheels();

    std::thread work_thread_;
    std::mutex mutex_;
    std::shared_ptr<TimeWheel> time_wheel_;
    int tick_span_; // unit: millisecond      // TODO 允许用户定义精度
    int64_t tick_counts;
    std::atomic<bool> running_;
    int64_t start_timestamp_;
    int64_t last_tick_timestamp_;
    // std::deque<int> precision_correct_factors_;
};

}

#endif // TIMER_H_