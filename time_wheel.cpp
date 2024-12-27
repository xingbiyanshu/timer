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

// bool TimeWheel::addTimerTask(const TimerTask& task){
//     auto current_ms = getCurrentMilliseconds();
//     auto task_delay = max(int(task.start_time_ - current_ms), 0);
//     cout<<"add task: task_delay: "<<task_delay<<endl; 
//     // 根据时间轮规格修正任务时间
//     if (task_delay==0){
//         // 任务至少要等下一个tick才能执行，所以即便用户期望“立即”执行，
//         // 我们也只能把它挂载到最近的slot——下一个slot，而非“当前”slot，当前slot是已经（上一次）执行过的slot。
//         task_delay = slot_span_;
//     }else if (task_delay%slot_span_!=0){
//         task_delay = (task_delay/slot_span_ + 1) * slot_span_;
//     }
//     cout<<"add task: current_ms: "<<current_ms<<", task_delay: "<<task_delay<<endl; 
//     auto task_slot_index = (current_slot_index_ + task_delay/slot_span_) % slots_number_;
//     cout<<chronohelper::getTimeStamp()<<": add task "<<task.id_<<" into slot["<<task_slot_index<<"]"<<endl;  // FIXME add的位置不符合预期
//     slots_[task_slot_index].push_back(task);
//     print();

//     return true;
// }


bool TimeWheel::addTimerTask(const TimerTask& task){
    auto task_slot_index = current_slot_index_;
    if (task.run_times_==0){
        task_slot_index += task.delay_;
    }else{
        task_slot_index += task.interval_;
    }
    cout<<chronohelper::getTimeStamp()<<": add task "<<task.id_<<" into slot["<<task_slot_index<<"]"<<endl;  // FIXME add的位置不符合预期
    slots_[task_slot_index].push_back(task);
    // print();

    return true;
}


bool TimeWheel::tick(){
    current_slot_index_ = ++current_slot_index_ % slots_number_;
    cout <<chronohelper::getTimeStamp()<< ": tick in" << ", current_slot_index_="<<current_slot_index_<< endl;
    list<TimerTask>& task_list = slots_[current_slot_index_];
    if (task_list.empty()){
        cout <<chronohelper::getTimeStamp()<< ": tick out, no task to execute." << endl;
        return false;
    }
    list<TimerTask>::iterator it = task_list.begin();
    list<TimerTask> need_reload_tasks;
    while (it != task_list.end())
    {
        it->runnable_();
        it->run_times_++;
        if (it->run_times_ != it->repeat_times_){
            // it->start_time_ += it->interval_; // update next run time of task  // FIXME task start time 对齐wheeltime没？
            need_reload_tasks.push_back(*it);
        }
        it=task_list.erase(it);
    }
    for (auto task : need_reload_tasks){
        addTimerTask(task);
    }
    
    cout <<chronohelper::getTimeStamp()<< ": tick out." << endl;
    return true;
}


}