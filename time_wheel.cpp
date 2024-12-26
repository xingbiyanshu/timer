/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-25 15:39:01
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 19:02:15
 * @FilePath: \timer\time_wheel.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#include "time_wheel.hpp"
#include "chrono_helper.hpp"
#include "timer_task.hpp"
#include <iostream>

namespace confsdk::infrastructure{

using namespace std;
using namespace chronohelper;

bool TimeWheel::addTimerTask(const TimerTask& task){
    auto current_ms = getCurrentMilliseconds();
    auto task_delay = task.start_time_ - current_ms;
    if (task_delay < slot_span_ || total_span_ <= task_delay){
        cout<<chronohelper::getTimeStamp()<<"task_delay invalid:"<<task_delay<<endl;
        return false;
    }
    auto current_wheel_time = current_slot_index_ * slot_span_;
    auto task_slot_index = (current_slot_index_ + task_delay/slot_span_) % slots_number_;
    cout<<"add task "<<task.id_<<" into slot["<<task_slot_index<<"]"<<endl;  // FIXME add的位置不符合预期
    slots_[task_slot_index].push_back(task);
    print();

    return true;
}


bool TimeWheel::tick(){
    current_slot_index_ = ++current_slot_index_ % slots_number_;
    cout << "tick in" << ", current_slot_index_="<<current_slot_index_<< endl;
    list<TimerTask>& task_list = slots_[current_slot_index_];
    if (task_list.empty()){
        return false;
    }
    list<TimerTask>::iterator it = task_list.begin();
    list<TimerTask> need_reload_tasks;
    while (it != task_list.end())
    {
        it->runnable_();
        it->run_times_++;
        // cout << "task_list size " <<task_list.size()<< endl;
        if (it->run_times_ < it->repeat_times_){
            it->start_time_ += it->interval_; // update next run time of task
            need_reload_tasks.push_back(*it);
        }
        it=task_list.erase(it);
    }
    for (auto task : need_reload_tasks){
        addTimerTask(task);
    }
    
    cout << "tick out" << endl;
    return true;
}


}