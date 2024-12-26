/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 19:16:28
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 17:00:01
 * @FilePath: \timer\timer.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 19:16:28
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-25 17:01:09
 * @FilePath: \timer\timer.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#include "timer.hpp"
#include "chrono_helper.hpp"
#include <functional>
#include <iostream>
#include <mutex>

namespace confsdk::infrastructure
{
    using namespace std;

    bool Timer::start(){
        work_thread_ = thread([this](){
            while (!shutdown_){
                this_thread::sleep_for(std::chrono::milliseconds(tick_span_));  //FIXME tick_span_要减去执行耗时
                std::lock_guard<std::mutex> lock(mutex_);
                cout << chronohelper::getTimeStamp()<< ": timer tick start" << endl;
                time_wheels_[0].tick();
                cout << chronohelper::getTimeStamp() << ": timer tick finish" << endl;
            }
        });
        work_thread_.detach();
        return true;
    }


    int Timer::schedule(function<void ()> task, int delay, int period, int repeat_times){
        std::lock_guard<std::mutex> lock(mutex_);
        time_wheels_[0].addTimerTask(TimerTask(task, delay, period, repeat_times));
        return 0;
    }

}