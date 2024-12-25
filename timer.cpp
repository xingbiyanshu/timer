/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 19:16:28
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-25 17:24:48
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
#include <functional>
#include <iostream>

namespace confsdk::infrastructure
{
    using namespace std;

    bool Timer::start(){
        work_thread_ = thread([this](){
            while (!shutdown_){
                cout << "timer thread..." << endl;


                
                this_thread::sleep_for(std::chrono::milliseconds(tick_));
            }
        });
        work_thread_.detach();
        return true;
    }

    int Timer::schedule(function<void ()> task, int delay, int period){
        return 0;
    }
}