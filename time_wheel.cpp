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

bool TimeWheel::addTimerTask(const std::shared_ptr<TimerTask>& task){
    auto wheel_max_time = start_timestamp_+tick_count_*slot_span_ + wheel_span_;

    if (task->start_time_ < wheel_max_time){
        auto ticks_from_start_timestamp = max((task->start_time_-start_timestamp_)/slot_span_, tick_count_); // max应对任务过早的情形
        auto task_slot_index = ticks_from_start_timestamp % slots_number_;
        slots_[task_slot_index].push_back(task);

        print();
    }else{
        upper_level_wheel_->addTimerTask(task);
    }

    return true;
}


// bool TimeWheel::addTimerTask(const std::shared_ptr<TimerTask>& task){
//     auto task_slot_index = current_slot_index_;
//     auto task_delay
//     if (task->run_times_==0){
//         task_slot_index += task.delay_;
//     }else{
//         task_slot_index += task.interval_;
//     }
//     cout<<chronohelper::getTimeStamp()<<": add task "<<task.id_<<" into slot["<<task_slot_index<<"]"<<endl;  // FIXME add的位置不符合预期
//     slots_[task_slot_index].push_back(task);
//     // print();

//     return true;
// }


bool TimeWheel::tick(){
    current_slot_index_ = ++current_slot_index_ % slots_number_;
    cout <<chronohelper::getTimeStamp()<< ": tick in" << ", current_slot_index_="<<current_slot_index_<< endl;
    list<shared_ptr<TimerTask>>& task_list = slots_[current_slot_index_];
    if (!task_list.empty()){
        if (upper_level_wheel_ == nullptr){ // 如果是最底层的wheel则直接执行任务

            list<shared_ptr<TimerTask>>::iterator it = task_list.begin();
            list<shared_ptr<TimerTask>> need_reload_tasks;
            while (it != task_list.end())
            {
                auto& task = *it;
                task->runnable_();  // FIXME post到线程池处理，否则会阻塞timewheel
                task->run_counts_++;
                if (task->run_counts_ != task->repeat_times_){
                    task->start_time_ += task->interval_; 
                    need_reload_tasks.push_back(task);
                }
                it=task_list.erase(it);
            }
            for (auto task : need_reload_tasks){
                addTimerTask(task);
            }
        
        }else{ // 如果不是最底层wheel重新加载任务以让其自动分配到合适的wheel

        }
    }

    ++tick_count_;

    if (tick_count_ % slots_number_==0 && upper_level_wheel_ != nullptr){
        // 当前时间轮走完一圈后，推进上一层时间轮进行一次tick，就像时钟表盘的工作模式
        upper_level_wheel_->tick();
    }

    cout <<chronohelper::getTimeStamp()<< ": tick out." << endl;
    return true;
}


}