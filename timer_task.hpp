/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 15:11:39
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 19:23:40
 * @FilePath: \timer\timer_task.hpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

// #include "chrono_helper.hpp"
#include <functional>
#include <chrono>
#include <iostream>
#include <type_traits>

namespace confsdk::infrastructure{

using Runnable = std::function<void()>;

class TimerTask{
public:
    TimerTask(const Runnable& runnable, int delay=0, int interval=0, int repeat_times=1)
        :id_(count++),
        runnable_(runnable), 
        delay_(delay),
        // start_time_(std::max(delay,0) + chronohelper::getCurrentMilliseconds()), 
        interval_(interval), 
        repeat_times_(repeat_times),
        run_times_(0){
    }

    void print() const{
        using namespace std;
        cout << "TimerTask{" 
             << "id_:"<<id_
            //  << ", start_time_:"<<start_time_
             << ", delay_:"<<delay_
             <<", interval_:"<<interval_
             <<", repeat_times_:"<<repeat_times_ 
             <<", run_times_:"<<run_times_ 
             <<"}"<<endl;
    }

    static int count;
    const int id_;
    Runnable runnable_;
    // int64_t start_time_;    // start time of next run.
    const int delay_;
    const int interval_;    // interval between two runs. unit: TimeWheel::slot_span_        // TODO 都转成时间轮的最小刻度的整数倍，以时间轮tick_span为单位？
    const int repeat_times_; // 1 means run once, 2 run twice, 0 means run infinitely
    int run_times_;         // how many times has run.
};

}

#endif // TIMER_TASK_H_