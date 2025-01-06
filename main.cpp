/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-24 13:17:25
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2025-01-06 10:35:22
 * @FilePath: \timer\main.cpp
 * @Description: 
 * 
 * Copyright (c) 2025 by sissi, All Rights Reserved. 
 */
#include <iostream>

#include "timer.hpp"
#include "chrono_helper.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

int main(int, char**){
    using namespace std;
    using namespace confsdk::infrastructure;
    using namespace std::chrono;

    cout<<chronohelper::getTimeStamp() << ": app start!\n";

    auto tm = Timer("test");
    tm.start();

    // this_thread::sleep_for(milliseconds(20));

    int task_id = tm.schedule([&task_id](){
        static int count=0;
        cout<<chronohelper::getTimeStamp()
            <<": ### task "<< task_id<< " run count: "<< ++count << endl; 
    }, 100, 200, 5); 

    this_thread::sleep_for(milliseconds(200)); 

    // tm.shutdown();

    int task3_id = tm.schedule([&task3_id](){
        static int count=0;
        cout<<chronohelper::getTimeStamp()
            <<": ### task "<< task3_id<< " run count: "<< ++count << endl; 
    }, 99, 100, 3); 

    // int task2_id = tm.schedule([&task2_id](){
    //     static int count=0;
    //     cout<<chronohelper::getTimeStamp()
    //         <<": ### task "<< task2_id<< " run count: "<< ++count << endl; 
    // }, 200); 

    tm.cancelAllTask();
    // tm.cancelTask(task_id);
    // tm.cancelTask(task3_id);
    
    this_thread::sleep_for(milliseconds(1000)); 

    cout<<chronohelper::getTimeStamp() << ": app exit!\n";
}
