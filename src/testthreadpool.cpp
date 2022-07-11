#include "ctpl_stl.h"
#include <iostream>
#include <string>
#include "logging.h"


void first(int id) {
    std::cout << "hello from " << id << ", function\n";
}

void aga(int id, int par) {
    std::cout << "hello from " << id << ", function with parameter " << par <<'\n';
}

struct Third {
    Third(int v) { this->v = v; std::cout << "Third ctor " << this->v << '\n'; }
    Third(Third && c) { this->v = c.v; std::cout<<"Third move ctor\n"; }
    Third(const Third & c) { this->v = c.v; std::cout<<"Third copy ctor\n"; }
    ~Third() { std::cout << "Third dtor\n"; }
    int v;
};

void mmm(int id, const std::string & s) {
    std::cout << "mmm function " << id << ' ' << s << '\n';
}

void ugu(int id, Third & t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "hello from " << id << ", function with parameter Third " << t.v <<'\n';
}

int main(int argc, char **argv) {
    ctpl::thread_pool p(4 /* two threads in the pool */);

    // testing logging as well

    LogByThread::initializeservice(4);

    std::cout << "print build log";

    for (int j = 0; j<50; j++)
        p.push([](int id, int j){  // lambda
        bool bind = LogByThread::bundleidwithID(std::this_thread::get_id(), id);
        if (!bind)
            std::cout << "thread " << std::this_thread::get_id() << " already bundled " << "\n";
        LOG(DEBUG, TestThreadPoolandLog, "this is the taks of " <<  j << "in the log" << "\n");


        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "hello from " << std::this_thread::get_id() << ' ' << "with arg" << '\n';
        }, j);

    p.stop(true);
    ; LogByThread::endservice(); ;
    std::cout<< "current size" << p.size();
    p.reset();

    p.push(first);  // function
    p.push(aga, 7);  // function

    {
        struct Second {
            Second(const std::string & s) { std::cout << "Second ctor\n"; this->s = s; }
            Second(Second && c) { std::cout << "Second move ctor\n"; s = std::move(c.s); }
            Second(const Second & c) { std::cout << "Second copy ctor\n"; this->s = c.s; };
            ~Second() { std::cout << "Second dtor\n"; }
            void operator()(int id) const {
                std::cout << "hello from " << id << ' ' << this->s << '\n';
            }
        private:
            std::string s;
        } second(", functor");

        p.push(std::ref(second));  // functor, reference
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        p.push(const_cast<const Second &>(second));  // functor, copy ctor
        p.push(std::move(second));  // functor, move ctor
        p.push(second);  // functor, move ctor
        p.push(Second(", functor"));  // functor, move ctor
    }
        {
            Third t(100);

            p.push(ugu, std::ref(t));  // function. reference
            p.push(ugu, t);  // function. copy ctor, move ctor
            p.push(ugu, std::move(t));  // function. move ctor, move ctor

        }
        p.push(ugu, Third(200));  // function



    std::string s = ", lambda";
    p.push([s](int id){  // lambda
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "hello from " << id << ' ' << s << '\n';
    });

    p.push([s](int id){  // lambda
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "hello from " << id << ' ' << s << '\n';
    });

    p.push(mmm, "worked");



    std::string s2 = "result";
     p.push([s2](int){
        std::cout<< "run sth after stop";

        return s2;
    });
    // other code here
    //...



    p.push([](int){
        throw std::exception();
    });
    // other code here
    //...

    p.stop(true);


    // change the number of treads in the pool
        std::cout<< "\n why I not stuck here" << std::endl;


    //                                  p.resize(5);

    std::cout<< "\n why I stuck here";
    return 0;
}