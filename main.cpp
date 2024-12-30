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

    int count=0;
    auto tm = Timer("test");
    tm.start();
    tm.schedule([&count](){
        cout<<chronohelper::getTimeStamp()
            <<": ### task run count: "<< ++count << endl;  // FIXME 两次回调的间隔和预期差距较大
    }, 100, 200, 10); // 测试边缘场景
    this_thread::sleep_for(milliseconds(2000));  // TODO 时间长点 让wheel转几轮

    cout<<chronohelper::getTimeStamp() << ": app exit!\n";
}
