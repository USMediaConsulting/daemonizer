#include "../daemonizer.h"
#include <iostream>

using namespace std;

struct functor {

    void operator()(){
        while(1) sleep(10);
    }

};

int main(int argc, char* argv[]){
    cout << "starting daemonizer test" << endl;
    functor f;    
    usmc::tools::daemonizer<functor> d(f,"nemi","/");
    cout << "new daemon : " << d.daemonize() << endl;
    return 0;
}
