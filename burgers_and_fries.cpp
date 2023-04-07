#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;
sem_t s_fries;
sem_t s_burger;
std::mutex m;
int current_processing;
int waiting_fries;
int waiting_burger;
// Do not change
void process_order() {
    sleep(2);
}

void place_order(int type) {
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process; 
     *     if true -> wait (print waiting)
     *     otherwise place this order (print order)
     *  Use type_names[type] to print the order type
     */
    if(current_processing==k)
    {
        m.lock();
        if(type==0)
        {
            waiting_burger++;
        }
        else
        {
            waiting_fries++;
        }
        m.unlock();
        std::cout<<"Waiting: "<<type_names[type]<<endl;
        if(type==0)
        {
            
            sem_wait(&s_burger);
            m.lock();
            waiting_burger--;
            m.unlock();
        }
        else
        {
            
            sem_wait(&s_fries);
            m.lock();
            waiting_fries--;
            m.unlock();
        }
    }
    
    m.lock();
    current_processing++;
    m.unlock();
    std::cout<<"Order: "<<type_names[type]<<endl;

    

    process_order();        // Do not remove, simulates preparation
    m.lock();
    current_processing--;
    m.unlock();
    if(type==0)
    {
        if(waiting_burger>0)
        {
            sem_post(&s_burger);
            return;
        }
        if(waiting_fries>0)
        {
            sem_post(&s_fries);
            return;
        }
    }
    else
    {
        if(waiting_fries>0)
        {
            sem_post(&s_fries);
            return;
        }
        if(waiting_burger>0)
        {
            sem_post(&s_burger);
            return;
        }
    }

    /**
     *  Add logic for synchronization after order processed
     *  Allow next order of the same type to proceed if there is any waiting; if not, allow the other type to proceed.
     */
}

int main() {
    // Initialize necessary variables, semaphores etc.
    current_processing=0;
    waiting_fries=0;
    waiting_burger=0;
    sem_init(&s_fries, 0, 0);
    sem_init(&s_burger, 0, 0);
    // Read data: done for you, do not change
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    int t;
    cin >> k;
    cin >> t;
    for (int i = 0; i < t; ++i) {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }

    // Create threads: done for you, do not change
    thread* threads[MAX_THREADS];
    for (int i = 0; i < t; ++i) {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1) {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    return 0;
}