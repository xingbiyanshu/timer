/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-25 15:39:01
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-25 16:41:17
 * @FilePath: \timer\time_wheel.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#include "time_wheel.hpp"
#include "timer_task.hpp"
#include <chrono>

namespace confsdk::infrastructure{

bool TimeWheel::addTimerTask(const TimerTask& task){
    using namespace std::chrono;
  
    auto current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    auto task_delay = task.start_time_ - current_time;
    if (task_delay < slot_span_ || total_span_ <= task_delay){
        return false;
    }
    auto current_wheel_time = current_slot_index_ * slot_span_;
    auto task_slot_index = (current_slot_index_ + task_delay/slot_span_) % slots_number_;
    slots_[task_slot_index].push_back(task);

    return true;
}




}