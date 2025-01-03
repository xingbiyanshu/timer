/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:38
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2025-01-03 16:43:50
 * @FilePath: \timer\time_wheel.hpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#ifndef TIME_WHEEL_H_
#define TIME_WHEEL_H_

#include "timer_task.hpp"
#include <vector>
#include <list>
#include <optional>
#include <memory>
#include <iostream>

namespace confsdk::infrastructure{

class TimeWheel{
public:

    TimeWheel(int64_t start_timestamp, int slot_span, int slots_number):
            id_(count++),
            start_timestamp_(start_timestamp),
            slot_span_(slot_span), 
            slots_number_(slots_number), 
            wheel_span_(slot_span*slots_number),
            tick_counts_(0),
            slots_(slots_number){
    }

    ~TimeWheel(){
        // TODO 合理释放
    }


    bool addTimerTask(const std::shared_ptr<TimerTask>& task);


    bool tick();


    void setUpperLevelWheel(const std::shared_ptr<TimeWheel>& upper_level_wheel){
        upper_level_wheel_ = upper_level_wheel;
    }
    

    void setLowerLevelWheel(const std::shared_ptr<TimeWheel>& lower_level_wheel){
        lower_level_wheel_ = lower_level_wheel;
    }


    void print() const {
        using namespace std;
        cout << "TimeWheel{\n"
             << "id_:"<<id_
             << ", slot_span_:"<<slot_span_
             <<", slots_number_:"<<slots_number_
             <<", wheel_span_:"<<wheel_span_ 
             <<", tick_count_:"<<tick_counts_ 
             << ", slots_:{\n";
        for (int i=0; i<slots_.size(); ++i){
            if (slots_[i].empty()){
                continue;
            }
            cout<<"slot["<<i<<"], ";
            for (auto task : slots_[i]){
                task->print();
            }
        }
        cout << "}";
        cout << "}"<< endl;
    }

    static int count;
    const int id_;
    const int slot_span_; // unit:millisecond
    const int slots_number_;
    const int wheel_span_;
    const int64_t start_timestamp_; // first tick start timestamp
    int64_t tick_counts_;
    std::vector<std::list<std::shared_ptr<TimerTask>>> slots_;
    std::shared_ptr<TimeWheel> upper_level_wheel_;
    std::shared_ptr<TimeWheel> lower_level_wheel_;
};

}

#endif // TIME_WHEEL_H_