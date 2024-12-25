/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 15:11:39
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-25 15:36:19
 * @FilePath: \timer\timer_task.hpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

#include <functional>

namespace confsdk::infrastructure{

using Runnable = std::function<void()>;

class TimerTask{
public:
    TimerTask(const Runnable& runnable, int start_time, int interval, int repeat_times)
    :runnable_(runnable), start_time_(start_time), interval_(interval), repeat_times_(repeat_times){

    }

    Runnable runnable_;
    int start_time_; 
    int interval_;  // unit: millisecond
    int repeat_times_;
};

}

#endif // TIMER_TASK_H_