#include "chrono_helper.hpp"

namespace chronohelper
{

using namespace std;
using namespace std::chrono;

std::string getTimeStamp(const std::string& format, bool enable_millisecond){
    ostringstream oss;
    auto now = system_clock::now();
    auto timenow = system_clock::to_time_t(now); 
    oss << put_time(localtime(&timenow), format.c_str());
    if (enable_millisecond){
        auto cur_ms_count = duration_cast<milliseconds>(now.time_since_epoch()).count();
        oss <<"." << cur_ms_count%1000;
    }
    return oss.str();
}

int64_t getCurrentSeconds(){
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

int64_t getCurrentMilliseconds(){
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

}