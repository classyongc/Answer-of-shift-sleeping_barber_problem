//
//  Sleeping Barber Problem.cpp
//
//

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cmath>
#include <condition_variable>
#include <queue>


using namespace std;
int chairs = 5;
int empty_chairs = chairs;
int customers = 0;
vector<int> waiting_room;
condition_variable cv_barber;
mutex chairs_mtx, cus_mtx, bar_mtx;


//creat the barber object
class Barbers {
private:
    int num;
    int grade;
    
public:
    Barbers(){
        this->num = 1;
        this->grade = 5;
    }
    void office (int num){
        while (true)
        {
            unique_lock<mutex> lck(bar_mtx);
            cv_barber.wait(lck, []
                            {
                                if (customers > 0)
                                {
                                    return true;
                                }
                                else
                                {
                                    cout << "Barber sleep" << endl;
                                    return false;
                                }
                            });
            unique_lock<mutex> lck2(chairs_mtx);
            customers--;
            empty_chairs++;
            cout << "Barber cuting the hair of customer " <<waiting_room[0]<<endl;
            waiting_room.erase(waiting_room.begin()+0);
            cout << "Waiting room: ";
            copy (waiting_room.begin(), waiting_room.end(), ostream_iterator<int> (cout, " "));
            cout<<endl;
            this_thread::sleep_for(std::chrono::milliseconds((1+rand()%6)*1000));
            lck2.unlock();
        }
    }
};

class Customer{
public:
    int _id;
    Customer(int id){
        _id = id;
    }
    
    void visit(){
        unique_lock<mutex> lck(chairs_mtx);
        if (empty_chairs > 0)
        {
            empty_chairs--;
            customers++;
            waiting_room.push_back(this->_id);
            cv_barber.notify_one();
            lck.unlock();
        }
        else
        {
            cout << "Customer " << _id << " leaving" << endl;
            lck.unlock();
        }
    }
};

int main(){
    Barbers *b = new Barbers();
    thread t1{&Barbers::office,b,1};
    vector<thread> v;
    int id = 1;
    while (id<100){
        Customer *c = new Customer(id);
        v.push_back(thread (&Customer::visit,c));
        id++;
        this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
    t1.join();
    for (int i = 0; i < v.size(); i++) {
        v[i].join();
    }
    return 0;
}

