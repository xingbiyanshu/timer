/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:18:38
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 19:15:10
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

    TimeWheel(int slot_span, int slots_number, std::unique_ptr<TimeWheel> upper_level_wheel=nullptr):
        slot_span_(slot_span), 
        slots_number_(slots_number), 
        total_span_(slot_span*slots_number),
        current_slot_index_(0), 
        slots_(slots_number), 
        upper_level_wheel_(std::move(upper_level_wheel)){
    }

    bool addTimerTask(const TimerTask& task);

    bool tick();

    void print() const {
        using namespace std;
        cout << "TimeWheel{\n"
             << "slot_span_:"<<slot_span_
             <<", slots_number_:"<<slots_number_
             <<", total_span_:"<<total_span_ 
             <<", current_slot_index_:"<<current_slot_index_ 
             << ", slots_:{\n";
        for (int i=0; i<slots_.size(); ++i){
            if (slots_[i].empty()){
                continue;
            }
            cout<<"slot["<<i<<"], ";
            for (auto task : slots_[i]){
                task.print();
            }
        }
        cout << "}";
        // cout <<", upper_level_wheel_:"<<*upper_level_wheel_ ;
        cout << "}"<< endl;
    }

    const int slot_span_; // unit:millisecond
    const int slots_number_;
    const int total_span_;
    int current_slot_index_;
    std::vector<std::list<TimerTask>> slots_;
    std::unique_ptr<TimeWheel> upper_level_wheel_;
};

}

#endif // TIME_WHEEL_H_