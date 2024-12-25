/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:25
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-24 18:47:09
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

namespace confsdk::infrastructure{

class Timer{
public:
    Timer()=default;
    Timer(std::string name){

    }


    bool start();

    // void shutdown();

    // int schedule(TimerTask task, int delay);

    int schedule(std::function<void ()> task, int delay, int period);

    // void cancelTask(int taskid);

    // void cancelAllTask();


    std::thread work_thread_;
    std::mutex mutex_;
    std::vector<std::shared_ptr<TimeWheel>> time_wheels_;
};

}

#endif // TIMER_H_