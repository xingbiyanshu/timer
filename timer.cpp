/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 19:16:28
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 17:00:01
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


    inline int Timer::getAccuracy(){
        return tick_span_;
    }


    bool Timer::start(){
        if (running_){
            return true;
        }

        work_thread_ = thread([this](){
            start_timestamp_ = getCurrentMilliseconds();
            build_time_wheels();
            last_tick_timestamp_ = start_timestamp_;
            tick_counts = 0;
            running_ = true;
            while (running_){
                // 理想情况下"last_run_timestamp_-start_timestamp_"应该是整数倍tick_span_,
                // 但代码执行也会耗时，我们做适当修正避免累计误差以使timer按固定周期（tick_span_）运行。
                // auto curtimestamp = getCurrentMilliseconds();
                // cout << getTimeStamp()<< ": curtimestamp="<<curtimestamp<< ": last_run_timestamp_="<<last_run_timestamp_ << endl;
                // int64_t average_precision_correct_factor = 0;
                // if (!precision_correct_factors_.empty()){
                //     average_precision_correct_factor
                //         = accumulate(precision_correct_factors_.begin(), precision_correct_factors_.end(), 0) / precision_correct_factors_.size();
                // }
                // auto sleep_time = max(last_tick_timestamp_+tick_span_ - getCurrentMilliseconds() /* - average_precision_correct_factor */, 0LL); 
                auto sleep_time = this->start_timestamp_ + this->tick_span_*(this->tick_counts+1) - getCurrentMilliseconds();
                // auto sleep_time = (getCurrentMilliseconds() - start_timestamp_)%tick_span_;
                // cout << getCurrentMilliseconds()<< ": timer sleep:"<<sleep_time << endl;
                // auto timestamp_before_sleep = getCurrentMilliseconds();
                this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

                // auto timestamp_after_sleep = getCurrentMilliseconds();
                // last_tick_timestamp_ = timestamp_after_sleep;
                /** sleep_for不精确（由于CPU调度，线程从睡眠到唤醒运行也要耗时，实际sleep比预期长），
                 * 经观测误差值比较稳定，我们记录误差在下次sleep时尝试利用该值修正sleep时长 */
                // auto precision_correct_factor = (timestamp_after_sleep - timestamp_before_sleep) - sleep_time;
                // precision_correct_factors_.push_back(precision_correct_factor);
                // if (precision_correct_factors_.size()>1){
                //     precision_correct_factors_.pop_front();
                // }
                // cout << timestamp_after_sleep<< ": timer start" << endl;
                // cout << "timestamp_before_sleep:"<<timestamp_before_sleep<< ", timestamp_after_sleep:"
                //      <<timestamp_after_sleep<< ", precision_correct_factor_:"<<precision_correct_factor << endl;
                // this_thread::sleep_for(std::chrono::milliseconds(tick_span_));
                std::lock_guard<std::mutex> lock(mutex_);  // XXX 加锁耗时，尽量避免（绝大多数时候无意义锁定）

                auto task_executed=time_wheel_->tick();
                if (task_executed){
                    cout << "all wheels=========" << endl;
                    this->printAllWheels();
                }

                this->tick_counts++;
                // cout << getTimeStamp() << ": timer finish" << endl;
            }
        });
        work_thread_.detach();
        while (!running_){ // 等工作线程起来
            // do nothing
        }
        
        return true;
    }


    int Timer::schedule(function<void ()> task, int delay, int period, int repeat_times){
        // auto& wheel = time_wheel_;
        // if ((delay < 0 || wheel.total_span_<delay || delay%wheel.slot_span_!=0) 
        //     || (period<0 || wheel.total_span_ < period || period%wheel.slot_span_!=0)){
        //     cout << "invalid task! wheel cannot handle it: wheel{total_span="
        //          << wheel.total_span_<<", slot_span="<<wheel.slot_span_<<"}"
        //          << ", delay="<<delay<< ", period="<<period
        //          << "(delay and period must be less than total_span and integral multiple of slot_span)"<<endl;
        //     return -1;
        // }

        // // 根据时间轮规格修正任务时间
        // auto time_corrector = [&wheel](int time)->int{
        //     // 如果有延迟不能小于时间轮最小刻度
        //     time = time!=0 ? max(time, wheel.slot_span_) : 0; 
        //     auto remainder = time%wheel.slot_span_;
        //     if (remainder!=0){
        //         // 任务时间需对齐到时间轮刻度整数倍（四舍五入）
        //         auto quotient = time/wheel.slot_span_;
        //         time = quotient*wheel.slot_span_ + remainder*2>=wheel.slot_span_ ? wheel.slot_span_ : 0; 
        //     }
        //     return time;
        // };

        lock_guard<mutex> lock(mutex_);
        time_wheel_->addTimerTask(make_shared<TimerTask>(task, delay, period, repeat_times));
        return 0;
    }


    void Timer::build_time_wheels(){
        using namespace std;
        auto hundred_millisecond_wheel = make_shared<TimeWheel>(
            start_timestamp_, 
            tick_span_,  // 注意最小时间轮精度和timer的精度保持一致。
            10); 
        auto second_wheel = make_shared<TimeWheel>(start_timestamp_, 1000, 60);
        auto minute_wheel = make_shared<TimeWheel>(start_timestamp_, 60*1000, 60);
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