# CPU-Scheduling-Simulator
A CPU scheduling simulator implementing FCFS, SJF, SRT, and RR algorithms

## How to Run
1. Compile the program:
   ```bash
   g++ Cpu_Scheduling.cpp -o Cpu_Scheduling
## Input
Number of processes: 3
P1: Arrival Time = 0, Burst Time = 6
P2: Arrival Time = 1, Burst Time = 3
P3: Arrival Time = 2, Burst Time = 4

## Output
Gantt Chart: P1 (0–6) → P2 (6–9) → P3 (9–13)
Waiting Times: P1 = 0, P2 = 5, P3 = 7
Turnaround Times: P1 = 6, P2 = 8, P3 = 11
Average Waiting Time: 4.00
Average Turnaround Time: 8.33
