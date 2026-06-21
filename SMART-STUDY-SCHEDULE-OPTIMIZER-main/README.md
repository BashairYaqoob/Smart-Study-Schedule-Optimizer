# SMART-STUDY-SCHEDULE-OPTIMIZER
A C++ program that generates an **optimized daily task schedule** based on:

- Task dependencies
- Difficulty level
- Deadlines
- Duration
- Limited working hours per day (default: 8 hours)

The scheduler ensures that tasks are completed in a valid order and prioritizes urgent or difficult tasks.

---

## 📌 Features

- ✅ User-defined tasks
- ✅ Dependency handling
- ✅ Topological sorting (Kahn's Algorithm)
- ✅ Custom Max Heap priority queue
- ✅ Daily scheduling (8 hours/day)
- ✅ Cycle detection in dependencies

---

## 📂 Task Structure

Each task includes:

| Field | Description |
|--------|-------------|
| Name | Task identifier |
| Deadline | Days remaining |
| Difficulty | 1–10 scale |
| Duration | Hours needed |
| Dependencies | Other tasks required before this |

Priority formula:

priority = (1 / deadline) + difficulty

---

## 🧠 How It Works

1. User inputs tasks
2. Program assigns priority scores
3. Builds a dependency graph
4. Performs topological sorting
5. Uses a Max Heap to select the highest-priority available task
6. Schedules tasks into daily plans (8 hours max)
7. Prints the final schedule

---

## 🛠️ Build & Run

```sh
g++ main.cpp -o scheduler
./scheduler
✅ Example Output
sql
Copy code
-----FINAL DAILY SCHEDULE-----
Day 1: Design Research
Day 2: Prototype Testing
Day 3: Report

```
🧱 Technologies Used
C++

Custom Max Heap

Linked List Queue

Graph Theory

Greedy Scheduling

🚧 Future Improvements
Better priority formula

Split long tasks across days

File-based task input

GUI or web interface

Dependency graph visualization

📜 License
This project is open-source and free to use.
