//
//  Sleeping Barber Problem.cpp
//  
//
//  Created by chen on 3/28/18.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cmath>
#include <condition_variable>

using namespace std;

int m;
int chairs = m;
int empty_chairs = chairs;
int customers = 0;
condition_variable cvisit_barber;
mutex chairs_mtx, cus_mtx, bar_mtx;

//creat the barber object
class Barber {
private:
    int num;
    int grade;
    
public:
    Barber(int num, int grade){
        this.num = num;
        this.grade = grade;
    }
    void office (int num){
        while(true){
            unique_lock<mutex> lck(bar_mtx);
            cvisit_barber.wait(lck, [] {
                if (customers > 0){
                    cout << "customer coming, wake barber up" << endl;
                    return true;
                }
                else{
                    cout << "no customer, barber takes break or sleeps" << endl;
                    return false;
                }
            });
            unique_lock<mutex> lck2(chairs_mtx);
            customers--;
            empty_chairs++;
            cout << "Barber is doing hair cut" << endl;
            lck2.unlock();
            /*The haircut is to be simulated with a thread sleep of N seconds, where N is a random number between 1 and 5, inclusive (i.e. [1,5])*/
            this_thread::sleep_for(std::chrono::microseconds(rand() % 6));
            lck.unlock();
        }
    }
};


class Customer{
private:
    int _id;

public:
    Customer(int id){
        _id = id;
    }
    
    void visit(){
        unique_lock<mutex> lck(chairs_mtx);
        if(empty_chairs > 0){
            empty_chairs--;
            customers++;
            cvisit_barber.notify_one();
            cout<<"customer ID "<<_id<<"waiting for hair cut"<<endl;
        }
        else{
            cout<<"customer ID "<<_id<<"has no seat and leaving"<<endl;
            lck.unlock();
        }
    }
};
                               
int main(){
    thread t1(Barber(1).office());
    vector<thread> v;
    int id = 0;
    while (id<100){
        v.push_back(thread (Customer(id++).visit()));
        this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
    t1.join();
    for (int i = 0; i < v.size(); i++) {
        v[i].join();
    }
    return 0;
}
