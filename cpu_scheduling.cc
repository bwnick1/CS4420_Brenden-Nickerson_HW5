#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include <cstdlib>


using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int end_time;
    int waiting_time;
    bool completed;

    Process (int id, int arrival, int burst){
        pid = id;
        arrival_time = arrival;
        burst_time = burst;
        remaining_time = burst;
        start_time = -1;
        end_time = -1;
        waiting_time = 0;
        completed = false;
    }
};


void print_Usage() {
    cout << "Usage: ./cpu_scheduling input_file [FCFS|RR|SJF] [time_quantum]" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << " ./cpu_scheduling input.txt FCFS" << endl;
    cout << " ./cpu_scheduling input.txt RR 5"<< endl;
    cout << " ./cpu_scheduling input.txt SJF" << endl;


}

bool read_Input(const string& filename, vector<Process>& processes) {
    ifstream inputfile(filename);

    if(!inputfile){
        cerr << "Error: Cannot open file:" << filename << endl;
        return false;
    }

    int number_of_processes;
    inputfile >> number_of_processes;
    
    for(int i = 0; i < number_of_processes; ++i) {
        int pid;
        int arrival;
        int burst;

        inputfile >> pid >> arrival >> burst;
        processes.push_back(Process(pid, arrival, burst));
    }

    inputfile.close();
    return true;
}

bool allCompleted(const vector<Process>& processes) {
    for (const Process& process : processes) {
        if(!process.completed) {
            return false;
        }
    }
    return true;
}

void addArrivedProcesses(const vector<Process>& processes, int current_time, queue<int>& ready_queue, vector<bool>& in_queue, int running_process) {
    for(size_t i = 0; i < processes.size(); ++i) {
        if(processes[i].arrival_time <= current_time && !processes[i].completed && !in_queue[i] && static_cast<int>(i) != running_process){
            ready_queue.push(static_cast<int>(i));
            in_queue[i] = true;
        }
    }
}

void simulateFCFS(vector<Process>& processes) 
{
    cout << endl;
    cout << "========================================" << endl;
    cout << " FCFS Scheduling" << endl;
    cout << "========================================" << endl;

    queue<int> ready_queue;

    vector<bool> in_queue(processes.size(), false);

    int current_time = 0;
    int running_process = -1;

    while(!allCompleted(processes)) {
        addArrivedProcesses(processes, current_time, ready_queue, in_queue, running_process);
        
        if(running_process == -1){
            if(!ready_queue.empty()){
                running_process = ready_queue.front();
                ready_queue.pop();
                in_queue[running_process] = false;

                if(processes[running_process].start_time == -1){
                    processes[running_process].start_time = current_time;
                }

                cout << "Time " << current_time << ": PID " << processes[running_process].pid << " selected" << endl;
            }
            else {
                cout << "Time " << current_time << ": CPU is idle" << endl;
                current_time++;
                continue;
            }
        }

        processes[running_process].remaining_time--;
        current_time++;

        if(processes[running_process].remaining_time == 0) {
            processes[running_process].completed = true;
            processes[running_process].end_time = current_time;

            cout << "Time " << current_time << ": PID " << processes[running_process].pid << " completed" << endl;
            running_process = -1;
        }
    }
}


void simulateSJF(vector<Process>& processes) {
    cout << endl;
    cout << "========================================" << endl;
    cout << " SJF Scheduling" << endl;
    cout << "========================================" << endl;

    int current_time = 0;

    while (!allCompleted(processes)) {
        int selected = -1;

        for (size_t i = 0; i < processes.size(); ++i) {
            if(!processes[i].completed && processes[i].arrival_time <= current_time) {
                if(selected == -1) {
                    selected = static_cast<int>(i);
                }
                else if(processes[i].burst_time < processes[selected].burst_time) {
                    selected = static_cast<int>(i);
                }
                
                else if(processes[i].burst_time == processes[selected].burst_time) {
                    if(processes[i].arrival_time < processes[selected].arrival_time) {
                        selected = static_cast<int>(i);
                    }

                    else if (processes[i].arrival_time == processes[selected].arrival_time && processes[i].pid < processes[selected].pid) {
                        selected = static_cast<int>(i);
                    }
                }
            }
        }

        if(selected == -1) {
            cout << "Time " << current_time << ": CPU is idle" << endl;
            current_time++;
            continue;
        }

        processes[selected].start_time = current_time;

        cout << "Time " << current_time << ": PID " << processes[selected].pid << " selected" << endl;

        while(processes[selected].remaining_time > 0) {
            processes[selected]. remaining_time--;
            current_time++;
        }

        processes[selected].completed = true;
        processes[selected].end_time = current_time;

        cout << "Time " << current_time << ": PID " << processes[selected].pid << " completed" << endl;
    }
}


void simulateRR(vector<Process>& processes, int time_quantum) {
    cout << endl;
    cout << "========================================" << endl;
    cout << " RR Scheduling" << endl;
    cout << "========================================" << endl;

    queue<int> ready_queue;

    vector<bool> in_queue(processes.size(), false);
    int current_time = 0;
    int running_process = -1;
    int quantum_used = 0;

    while(!allCompleted(processes)) {
        addArrivedProcesses(processes, current_time, ready_queue, in_queue, running_process);

        if(running_process == -1){
            if(!ready_queue.empty()) {
                running_process = ready_queue.front();
                ready_queue.pop();
                in_queue[running_process] = false;
                quantum_used = 0;

                if(processes[running_process].start_time == -1){
                    processes[running_process].start_time = current_time;
                }

                cout << "Time " << current_time << ": PID " << processes[running_process].pid << " selected" << endl;
            }
            else {
                cout << "Time " << current_time << ": CPU is idle" << endl;
                current_time++;
                continue;
            }
        }

        processes[running_process].remaining_time--;
        current_time++;
        quantum_used++;

        addArrivedProcesses(processes, current_time, ready_queue, in_queue, running_process);

        if(processes[running_process].remaining_time == 0) {
            processes[running_process].completed = true;
            processes[running_process].end_time = current_time;

            cout << "Time " << current_time << ": PID " << processes[running_process].pid << " completed" << endl;

            running_process = -1;
            quantum_used = 0;
        }

        else if(quantum_used == time_quantum) {
            cout << "Time " << current_time << ": PID " << processes[running_process].pid << " quantum expired" << endl;

            ready_queue.push(running_process);
            in_queue[running_process] = true;

            running_process = -1;
            quantum_used = 0;
        }
    }
}

void calculateWaitingTime(vector<Process>& processes) {
    for (Process& process : processes) {
        process.waiting_time = process.end_time - process.arrival_time - process.burst_time;
    }
}

void printStatistics(const vector<Process>& processes, const string& algorithm) {
    cout << endl;
    cout << "========================================" << endl;
    cout << algorithm << " Statistics" << endl;
    cout << "========================================" << endl;

    cout << left
         << setw(8)  << "PID"
         << setw(15) << "Arrival Time"
         << setw(12) << "Start"
         << setw(12) << "End"
         << setw(14) << "Running"
         << setw(14) << "Waiting"
         << endl;
    
    cout << "-------------------------------------------------------------------" << endl;

    double total_waiting_time = 0.0;

    for(const Process& process : processes) {
        cout << left
             << setw(8)  << process.pid
             << setw(15) << process.arrival_time
             << setw(12) << process.start_time
             << setw(12) << process.end_time
             << setw(14) << process.burst_time
             << setw(14) << process.waiting_time
             << endl;

        total_waiting_time += process.waiting_time;
    }

    double average_waiting_time = total_waiting_time / processes.size();

    cout << endl;
    cout << "Average Waiting Time: " << fixed << setprecision(2) << average_waiting_time << endl;
}


int main (int argc, char* argv[]) {
    if(argc < 3 || argc > 4) {
        print_Usage();
        return 1;
    }

    string input_file = argv[1];
    string algorithm = argv[2];

    if(algorithm != "FCFS" && algorithm != "RR" && algorithm != "SJF") {
        cerr << "Error: Invalid scheduling algorithm." << endl;
        print_Usage();
        return 1;
    }

    int time_quantum = 0;
    if(algorithm == "RR") {
        if(argc != 4) {
            cerr << "Error: Time Quantum is required" << endl;
            print_Usage();
            return 1;
        }

        time_quantum = atoi(argv[3]);
        if(time_quantum <= 0) {
            cerr << "Error: Time Quantum must be greater than 0" << endl;
            return 1;
        }
    }
    else {
        if(argc != 3) {
            cerr << "Error: Time Quantum is only for RR algorithm" << endl;
            print_Usage();
            return 1;
        }
    }

    vector<Process> processes;

    if(!read_Input(input_file, processes)) {
        return 1;
    }

    if(processes.empty()) {
        cerr << "Error: No processes found" << endl;
        return 1;
    }

    if(algorithm =="FCFS") {
        simulateFCFS(processes);
    }
    else if(algorithm == "SJF") {
        simulateSJF(processes);
    }
    else if(algorithm == "RR") {
        simulateRR(processes, time_quantum);
    }

    calculateWaitingTime(processes);

    printStatistics(processes, algorithm);
    
    return 0;
}