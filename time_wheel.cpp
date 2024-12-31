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

int TimeWheel::count = 0;

using namespace std;
using namespace chronohelper;

bool TimeWheel::addTimerTask(const std::shared_ptr<TimerTask>& task){
    auto wheel_max_time = start_timestamp_+tick_counts_*slot_span_ + wheel_span_;
    cout<<chronohelper::getTimeStamp()
        <<": wheel "<<id_<<" try add task "<<task->id_
        <<", task_start_time="<<task->start_time_
        <<", wheel_start_timestamp_="<<start_timestamp_ 
        <<", wheel_max_time="<<wheel_max_time  
        <<"  tick_counts_="<<tick_counts_<<endl;   
    if (task->start_time_ <= wheel_max_time){  // 当前时间轮时间范围覆盖到了任务的开始时间，则挂载到当前时间轮
        auto span = task->start_time_-start_timestamp_;
        auto task_ticks = span/slot_span_; 
        if (lower_level_wheel_==nullptr){ // 针对最底层wheel，我们需额外的处理
            /**我们无法处理低于wheel最小精度的情形。
             * task的start_time可能小于wheel的start_time（task创建早于wheel），
             * 又或者task的delay or interval小于slot_span_，针对这些情况，我们补齐
             * task延迟至我们的最小精度slot_span_。特别注意，当task延迟=0时，
             * 即用户期望task“立即执行”，我们仍然需要等到下一个tick，即slot_span_的时长才能执行该task。
             * timer应该向用户声明这点，用户也应了解不能期望低于timer时间精度的时间控制生效*/ 
            span = max((int)span, slot_span_);
            /**task的延迟执行时间(delay or interval)可能不是slot_span的整数倍，此种情形我们补足为整数倍，
             * 因为我们也无法处理精度小于slot_span的情形。
             * timer应该向用户声明这点，用户也应了解不能期望与timer时间精度不匹配的时间控制生效
             * */
            task_ticks = span/slot_span_ + (span%slot_span_!=0 ? 1 : 0); 
        }

        auto task_slot_index = task_ticks % slots_number_;    
        cout<<chronohelper::getTimeStamp() <<": added task "<<task->id_<<" into slot "<<task_slot_index<<endl;   
        slots_[task_slot_index].push_back(task);

        // print();
    }else{ // 当前时间轮不能覆盖任务的开始时间，则尝试将任务投递给上层时间轮处理
        if (upper_level_wheel_!=nullptr){
            upper_level_wheel_->addTimerTask(task);
        }
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
    // tick应该是先执行再前进？这样对于即时执行的任务更自然
    auto current_slot_index_ = ++tick_counts_ % slots_number_;  // XXX 考虑插入一个delay小于tick的任务是否能立即被执行，以及delay等于tick的任务能否预期到下一个tick执行
    cout <<chronohelper::getTimeStamp()
        << ": >>> wheel "<<id_<<" tick in" << ", current_slot_index_="<<current_slot_index_<< endl;
    list<shared_ptr<TimerTask>>& task_list = slots_[current_slot_index_];
    // if (tick_counts_ == 1){
    //     /* wheel的运行机制是每次tick前进一个slot，然后再取出该slot的task_list运行，即“先前进再取task” 。
    //     所以对于wheel首次运行的场景，slot[0]的task是会被跳过的，要等wheel转动一周才会轮到运行，
    //     而实际上它们是要求“立即执行”的task。所以如果wheel是首次运行，我们将index=0处的tasklist也一并取出来运行。
    //     这种情况发生在timer start后立即schedule 一个delay<tick_span_的task */
    //     list<shared_ptr<TimerTask>>& immediate_tasks = slots_[0];
    //     if (!immediate_tasks.empty()){
    //         cout<<"immediate_tasks.size="<<immediate_tasks.size()<<endl;
    //         list<shared_ptr<TimerTask>>::iterator it = immediate_tasks.begin();
    //         int count=0;
    //         while (it != immediate_tasks.end()){
    //             task_list.push_back(*it);
    //             it=immediate_tasks.erase(it);
    //             cout<<"count"<<++count<<endl;
    //         }
    //     }
    // }
    auto has_task = !task_list.empty();
    if (has_task){
        if (lower_level_wheel_ == nullptr){ // 如果是最底层的wheel则直接执行任务

            list<shared_ptr<TimerTask>>::iterator it = task_list.begin();
            list<shared_ptr<TimerTask>> repeat_tasks;
            while (it != task_list.end())
            {
                auto& task = *it;
                task->runnable_();  // FIXME post到线程池处理，否则会阻塞timewheel
                task->run_counts_++;
                if (task->run_counts_ != task->repeat_times_){
                    /**需要重复执行的任务刷新下次执行时间，并准备重新加载。
                     * 对于精度小于slot_span_的时间，我们补齐到slot_span_*/ 
                    task->start_time_ += max(task->interval_, slot_span_); 
                    repeat_tasks.push_back(task);
                }
                it=task_list.erase(it);
            }
            for (auto task : repeat_tasks){
                // 重新加载需要重复执行的任务
                addTimerTask(task);
            }
        
        }else{ // 如果不是最底层wheel，尝试重新加载任务以让其自动分配到合适的wheel。（任务只会在最底层wheel执行）
            if (lower_level_wheel_ != nullptr){
                auto lowest_wheel = lower_level_wheel_;
                while (true){
                    if (lowest_wheel->lower_level_wheel_ == nullptr){
                        break;
                    }
                    lowest_wheel = lowest_wheel->lower_level_wheel_;
                }
                auto need_reload = task_list;
                task_list.clear();
                cout<<"reload tasks"<<endl;
                // 从最底层wheel重新加载task
                for (auto tsk : need_reload){
                    lowest_wheel->addTimerTask(tsk);
                }
            }
            // print();
        }
    }

    if (tick_counts_ % slots_number_==0 && upper_level_wheel_ != nullptr){
        // 当前时间轮走完一圈后，推进上一层时间轮进行一次tick，就像时钟表盘的工作模式
        cout<<"upper level wheel go"<<endl;
        upper_level_wheel_->tick();
    }
    cout <<chronohelper::getTimeStamp() << ": >>> wheel "<<id_<<" tick out" << endl;
    return has_task;
}


}