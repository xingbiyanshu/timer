/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:25
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-31 17:29:28
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

    /**
     * 获取Timer的精度。单位：毫秒。
     * NOTE: 定时任务的时间参数需要以该精度为基准，设置为其整数倍，
     * 若非整数倍则timer内部会强制对齐到整数倍。
     */
    int getAccuracy();

    bool start();

    // const Timer& obtain(); // TODO 做成单例

    // void shutdown();

    // int schedule(TimerTask task, int delay);

    /**
     * @description: 计划定时任务
     * @param task {function<void ()>}
     * @param delay {int} 任务首次执行延迟时间。单位：毫秒
     * @param period {int} 任务重复执行间隔时间。单位：毫秒
     * @param repeat_times {int} 任务执行次数。0表示无限次，1表示执行一次。
     * @return {*} 任务id。可用于控制任务生命周期
     */    
    int schedule(std::function<void ()> task, int delay, int period=0, int repeat_times=1);

    // void cancelTask(int taskid);

    // void cancelAllTask();


private:

    void build_time_wheels();

    void printAllWheels();

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