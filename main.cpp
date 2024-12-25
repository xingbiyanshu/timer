#include <iostream>

#include "timer.hpp"
#include <iostream>

int main(int, char**){
    using namespace std;
    using namespace confsdk::infrastructure;
    int count=0;
    auto tm = Timer("test");
    tm.start();
    tm.schedule([&count](){
        cout << "timer task run: "<< ++count << endl;
    }, 1000, 2000);
    std::cout << "Hello, from timer!\n";
}
