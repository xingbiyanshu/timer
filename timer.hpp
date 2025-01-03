/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:25
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2025-01-03 16:30:40
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

/**
 * 定时器
 * 基于层级时间轮算法实现。
 * NOTE: 该定时器非高精度定时器，实测默认精度100ms时，误差在0~20ms；
 * 添加任务时需谨记，任务的时间设置需对齐定时器的精度的整数倍，否则定时器
 * 内部会强制对齐为整数倍，最终导致定时任务的执行时间可能不符合用户期望。
 * */
class Timer{ 
public:
    Timer()=default;

    /**
     * @description: 定时器构造函数
     * @param name {string} 定时器名称
     * @param accuracy {int} 定时器精度。取值范围[100,)。单位：毫秒。建议为100整数倍
     * @return {*}
     */    
    Timer(std::string name, int accuracy=100):
        tick_span_(std::max(accuracy, 100)),
        running_(false){
    }


    /**
     * @description: 获取定时器精度。单位：毫秒。默认精度为100ms
     * @return {*} 定时器精度
     */
    int getAccuracy(){
        return tick_span_;
    }


    /**
     * @description: 启动定时器
     * @return {*}
     * NOTE: 必须先调用该接口启动定时器，而后才能执行定时任务。
     */
    bool start();


    // const Timer& obtain(); // TODO 做成单例

    // void shutdown();

    // int schedule(TimerTask task, int delay);


    /**
     * @description: 添加定时任务
     * @param task {function<void ()>} 任务执行体
     * @param delay {int} 任务首次执行延迟时间。单位：毫秒
     * @param period {int} 任务重复执行间隔时间。单位：毫秒
     * @param repeat_times {int} 任务执行次数。0表示无限次，1表示执行一次。
     * @return {*} 任务id。可用于控制任务生命周期
     * 
     * NOTE: 谨记，时间参数(delay, period)需要以定时器的精度为基准，设置为其整数倍，
     * 若非整数倍则定时器内部会强制对齐到整数倍，(默认的定时器精度为100ms)，
     * 此行为可能产生与用户期望不符的结果。例如：
     * （假设定时器精度为100ms）
     * delay取值[0,100)时，任务总是会在100ms后执行，而非用户期望的当delay=0或很小的值时“立即”执行；
     * delay取值为精度的非整数倍时，如delay=150，会强制向上提升对齐到整数倍即delay=200；
     * period亦同理；
     * 若有如下task“task1.delay=10; task2.delay=20; task3.delay=30;”则task的执行顺序取决于其添加顺序
     * 而非用户期望的delay小的先执行，因为它们都对齐为了100.
     */    
    int schedule(std::function<void ()> task, int delay, int period=0, int repeat_times=1);

    // void cancelTask(int taskid);

    // void cancelAllTask();


private:

    void buildTimeWheels();

    void loadTasks();

    void printAllWheels();

    std::thread work_thread_; // 定时器工作线程
    std::mutex mutex_;
    std::shared_ptr<TimeWheel> time_wheel_; // 时间轮（最低层级的）
    int tick_span_;     // 定时器跳动一下的刻度，即定时器的精度。单位：毫秒
    int64_t tick_counts; // 定时器自启动开始的跳动次数
    std::atomic<bool> running_; // 定时器是否在运行
    int64_t start_timestamp_; // 定时器启动时间戳
    std::atomic<bool> has_new_task_; 
    std::list<std::shared_ptr<TimerTask>> new_tasks_;
};

}

#endif // TIMER_H_