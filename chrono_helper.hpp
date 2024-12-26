/*
 * @Author: sissi xingbiyanshu@gmail.com
 * @Date: 2024-12-26 16:00:59
 * @LastEditors: sissi xingbiyanshu@gmail.com
 * @LastEditTime: 2024-12-26 19:03:13
 * @FilePath: \timer\chrono_helper.hpp
 * @Description: 
 * 
 * Copyright (c) 2024 by sissi, All Rights Reserved. 
 */
#ifndef CHRONO_HELPER_H_
#define CHRONO_HELPER_H_

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>


namespace chronohelper{

std::string getTimeStamp(const std::string& format="%Y-%m-%d %H:%M:%S", bool show_millisecond=true);

int64_t getCurrentSeconds();

int64_t getCurrentMilliseconds();

}


#endif // CHRONO_HELPER_H_