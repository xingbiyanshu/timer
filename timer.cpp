/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 19:16:28
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2025-01-03 17:28:03
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
#include <numeric>


namespace confsdk::infrastructure
{
    using namespace std;
    using namespace chronohelper;


    bool Timer::start(){
        cout<< getTimeStamp() << ": starting timer(accuracy="<<tick_span_<<"ms)..." << endl;
        if (running_){
            cout<< getTimeStamp() << ": warning: timer started already!" << endl;
            return false;
        }

        work_thread_ = thread([this](){
            start_timestamp_ = getCurrentMilliseconds();
            buildTimeWheels();
            tick_counts = 0;
            running_ = true;
            while (running_){
                auto sleep_time = this->start_timestamp_ + this->tick_span_*(this->tick_counts+1) - getCurrentMilliseconds();
                this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
                // cout << getTimeStamp() << ": timer running" << endl;

                // 卸载任务
                if (this->has_canceled_task_){
                    this->unloadTasks();
                }

                // 加载任务
                if (this->has_new_task_){
                    this->loadTasks();
                }

                // 驱动时间轮
                auto task_executed=time_wheel_->tick();

                // if (task_executed){
                //     cout << "all wheels=========" << endl;
                //     this->printAllWheels();
                // }

                this->tick_counts++;
                // cout << getTimeStamp() << ": timer sleeping" << endl;
            }

            this->time_wheel_ = nullptr;
            this->new_tasks_.clear();
        });
        work_thread_.detach();

        while (!running_){ // 等工作线程起来
            // do nothing
        }
        
        cout<< getTimeStamp() << ": start timer success!" << endl;

        return true;
    }


    void Timer::shutdown(){
        cout<< getTimeStamp() << ": timer shutdown!" << endl;
        running_ = false;
    }


    int Timer::schedule(function<void ()> task, int delay, int period, int repeat_times){
        lock_guard<mutex> lock(mutex_);

        cout<< getTimeStamp() 
            << ": try schedule task(delay="<< delay<<", period="<<period<<", repeat_times="<<repeat_times<<")"<< endl;

        // 对齐时间参数到定时器最小精度的整数倍
        auto time_corrector = [this](int time){
            time = max(time, tick_span_);
            if (time%tick_span_!=0){
                time = (time/tick_span_)*tick_span_ + tick_span_;
            }
            return time;
        };
        delay = time_corrector(delay); 
        period = time_corrector(period); 

        auto timer_task = make_shared<TimerTask>(task, delay, period, repeat_times);

        cout<< getTimeStamp() 
            << ": schedule task(delay="<< delay<<", period="<<period<<", repeat_times="<<repeat_times<<")"<< endl;

        new_tasks_.push_back(timer_task);

        has_new_task_ = true;

        return timer_task->id_;
    }
    

    void Timer::cancelTask(int taskid){
        canceled_tasks_.push_back(taskid);
        has_canceled_task_=true;
    }


    void Timer::loadTasks(){
        lock_guard<mutex> lock(mutex_);
        for (auto task:new_tasks_){
            time_wheel_->addTimerTask(task);
        }
        // cout << "wheels=========" << endl;
        // this->printAllWheels();
        new_tasks_.clear();
        has_new_task_ = false;
    }


    void Timer::unloadTasks(){
        lock_guard<mutex> lock(mutex_);
        if (has_new_task_){
            for (auto canceled_task:canceled_tasks_){
                for (auto task : new_tasks_){
                    if (task->id_ == canceled_task){
                        new_tasks_.remove(task);
                        cout << "remove new task "<< task->id_ << endl;
                        break;
                    }
                }
            }
        }
        for (auto task:canceled_tasks_){
            time_wheel_->removeTimerTask(task);
            cout << "remove task "<< task << endl;
        }
        // cout << "wheels=========" << endl;
        // this->printAllWheels();
        canceled_tasks_.clear();
        has_canceled_task_ = false;
    }


    void Timer::buildTimeWheels(){
        using namespace std;
        auto hundred_millisecond_wheel = make_shared<TimeWheel>(
            start_timestamp_, 
            tick_span_,  // 注意最小时间轮精度和timer的精度保持一致。
            10); 
        auto second_wheel = make_shared<TimeWheel>(start_timestamp_, 1000, 60);
        auto minute_wheel = make_shared<TimeWheel>(start_timestamp_, 60*1000, 60);
        // auto second_wheel = make_shared<TimeWheel>(start_timestamp_, 1000, 10);
        // auto minute_wheel = make_shared<TimeWheel>(start_timestamp_, 10*1000, 10);
        minute_wheel->setUpperLevelWheel(nullptr);
        minute_wheel->setLowerLevelWheel(second_wheel);
        second_wheel->setUpperLevelWheel(minute_wheel);
        second_wheel->setLowerLevelWheel(hundred_millisecond_wheel);
        hundred_millisecond_wheel->setUpperLevelWheel(second_wheel);
        hundred_millisecond_wheel->setLowerLevelWheel(nullptr);

        time_wheel_ = hundred_millisecond_wheel;  // timer持有最底层时间轮
    }


    void Timer::printAllWheels(){
        time_wheel_->print();
        auto upper_wheel = time_wheel_->upper_level_wheel_;
        while(upper_wheel!=nullptr){
            upper_wheel->print();
            upper_wheel = upper_wheel->upper_level_wheel_;
        }
    }

}