#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>

using namespace std;

struct Process {
    string pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;

    Process(string pid, int arrival_time, int burst_time)
        : pid(pid), arrival_time(arrival_time), burst_time(burst_time),
          remaining_time(burst_time), waiting_time(0), turnaround_time(0) {}
};

void printResults(const vector<Process>& processes, const vector<pair<string, pair<int, int>>>& gantt_chart) {
    cout << "Gantt Chart:\n";
    for (const auto& entry : gantt_chart) {
        cout << entry.first << " (" << entry.second.first << "–" << entry.second.second << ") → ";
    }
    cout << "\n\n";

    cout << "Waiting Times:\n";
    int total_waiting_time = 0;
    for (const auto& process : processes) {
        cout << process.pid << " = " << process.waiting_time << "\n";
        total_waiting_time += process.waiting_time;
    }
    cout << "Average Waiting Time: " << fixed << setprecision(2) << (double)total_waiting_time / processes.size() << "\n\n";

    cout << "Turnaround Times:\n";
    int total_turnaround_time = 0;
    for (const auto& process : processes) {
        cout << process.pid << " = " << process.turnaround_time << "\n";
        total_turnaround_time += process.turnaround_time;
    }
    cout << "Average Turnaround Time: " << fixed << setprecision(2) << (double)total_turnaround_time / processes.size() << "\n\n";
}

vector<pair<string, pair<int, int>>> fcfs(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<pair<string, pair<int, int>>> gantt_chart;
    int current_time = 0;

    for (auto& process : processes) {
        if (current_time < process.arrival_time) {
            current_time = process.arrival_time;
        }
        process.waiting_time = current_time - process.arrival_time;
        process.turnaround_time = process.waiting_time + process.burst_time;
        gantt_chart.push_back({process.pid, {current_time, current_time + process.burst_time}});
        current_time += process.burst_time;
    }

    return gantt_chart;
}

vector<pair<string, pair<int, int>>> sjf(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<pair<string, pair<int, int>>> gantt_chart;
    int current_time = 0;
    vector<Process> remaining_processes = processes;

    while (!remaining_processes.empty()) {
        auto it = min_element(remaining_processes.begin(), remaining_processes.end(), [current_time](const Process& a, const Process& b) {
            bool a_available = a.arrival_time <= current_time;
            bool b_available = b.arrival_time <= current_time;
            if (a_available && b_available) return a.burst_time < b.burst_time;
            return a_available;
        });

        if (it == remaining_processes.end() || it->arrival_time > current_time) {
            current_time++;
            continue;
        }

        Process& shortest_process = *it;
        shortest_process.waiting_time = current_time - shortest_process.arrival_time;
        shortest_process.turnaround_time = shortest_process.waiting_time + shortest_process.burst_time;
        gantt_chart.push_back({shortest_process.pid, {current_time, current_time + shortest_process.burst_time}});
        current_time += shortest_process.burst_time;
        remaining_processes.erase(it);
    }

    return gantt_chart;
}

vector<pair<string, pair<int, int>>> srt(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<pair<string, pair<int, int>>> gantt_chart;
    int current_time = 0;
    vector<Process> remaining_processes = processes;

    while (!remaining_processes.empty()) {
        auto it = min_element(remaining_processes.begin(), remaining_processes.end(), [current_time](const Process& a, const Process& b) {
            bool a_available = a.arrival_time <= current_time;
            bool b_available = b.arrival_time <= current_time;
            if (a_available && b_available) return a.remaining_time < b.remaining_time;
            return a_available;
        });

        if (it == remaining_processes.end() || it->arrival_time > current_time) {
            current_time++;
            continue;
        }

        Process& shortest_process = *it;
        if (!gantt_chart.empty() && gantt_chart.back().first == shortest_process.pid) {
            gantt_chart.back().second.second++;
        } else {
            gantt_chart.push_back({shortest_process.pid, {current_time, current_time + 1}});
        }
        shortest_process.remaining_time--;
        if (shortest_process.remaining_time == 0) {
            shortest_process.turnaround_time = current_time + 1 - shortest_process.arrival_time;
            shortest_process.waiting_time = shortest_process.turnaround_time - shortest_process.burst_time;
            remaining_processes.erase(it);
        }
        current_time++;
    }

    return gantt_chart;
}

vector<pair<string, pair<int, int>>> roundRobin(vector<Process>& processes, int quantum) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<pair<string, pair<int, int>>> gantt_chart;
    int current_time = 0;
    queue<Process*> ready_queue;
    vector<Process> remaining_processes = processes;
    size_t index = 0;

    while (!remaining_processes.empty() || !ready_queue.empty()) {
        while (index < processes.size() && processes[index].arrival_time <= current_time) {
            ready_queue.push(&processes[index]);
            index++;
        }

        if (ready_queue.empty()) {
            current_time++;
            continue;
        }

        Process* current_process = ready_queue.front();
        ready_queue.pop();

        int time_slice = min(quantum, current_process->remaining_time);
        gantt_chart.push_back({current_process->pid, {current_time, current_time + time_slice}});
        current_process->remaining_time -= time_slice;
        current_time += time_slice;

        while (index < processes.size() && processes[index].arrival_time <= current_time) {
            ready_queue.push(&processes[index]);
            index++;
        }

        if (current_process->remaining_time > 0) {
            ready_queue.push(current_process);
        } else {
            current_process->turnaround_time = current_time - current_process->arrival_time;
            current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
            remaining_processes.erase(
                remove_if(remaining_processes.begin(), remaining_processes.end(),
                          [current_process](const Process& p) { return p.pid == current_process->pid; }),
                remaining_processes.end());
        }
    }

    return gantt_chart;
}

int main() {
    while (true) {
        cout << "1. First-Come, First-Served (FCFS)\n";
        cout << "2. Shortest-Job-First (SJF)\n";
        cout << "3. Shortest-Remaining-Time (SRT)\n";
        cout << "4. Round Robin (RR)\n";
        cout << "5. Exit\n";
        cout << "Select an option: ";
        int choice;
        cin >> choice;

        if (choice == 5) break;

        int num_processes;
        cout << "Enter the number of processes: ";
        cin >> num_processes;

        vector<Process> processes;
        for (int i = 0; i < num_processes; i++) {
            string pid = "P" + to_string(i + 1);
            int arrival_time, burst_time;
            cout << "Enter arrival time for " << pid << ": ";
            cin >> arrival_time;
            cout << "Enter burst time for " << pid << ": ";
            cin >> burst_time;
            if (arrival_time < 0 || burst_time <= 0) {
                cout << "Error: Arrival time must be non-negative and burst time must be positive.\n";
                return 1;
            }
            processes.push_back(Process(pid, arrival_time, burst_time));
        }

        vector<pair<string, pair<int, int>>> gantt_chart;
        if (choice == 1) {
            gantt_chart = fcfs(processes);
        } else if (choice == 2) {
            gantt_chart = sjf(processes);
        } else if (choice == 3) {
            gantt_chart = srt(processes);
        } else if (choice == 4) {
            int quantum;
            cout << "Enter time quantum for Round Robin: ";
            cin >> quantum;
            if (quantum <= 0) {
                cout << "Error: Time quantum must be positive.\n";
                return 1;
            }
            gantt_chart = roundRobin(processes, quantum);
        } else {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        printResults(processes, gantt_chart);
    }

    return 0;
}