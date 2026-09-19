<h1 align="center" style="color: #9D00FF;">Resilient Campus Network</h1>

<div align="center">
  <i>An algorithmic simulation of a resilient campus network that dynamically handles link failures.</i>
</div>

---

## 📌 Problem Overview
A university campus network connects academic buildings, residential halls, laboratories, libraries, administrative offices, and the central data center through routers, switches, fiber links, and wireless backup links. 

The system analyzes the surviving network graph after simulated link failures to determine endpoint reachability, calculate minimum-latency routes, and optimize maximum data throughput.

## ⚙️ Core Algorithms
*   **Breadth-First Search (BFS):** Determines baseline reachability to verify if the target destination remains connected after link failures.
*   **Dijkstra's Algorithm:** Computes the minimum-latency route using a priority queue ($O((V+E)\log V)$) among surviving non-negative edges.
*   **Edmonds-Karp (Max Flow):** Calculates the maximum simultaneous data transfer capacity (bottleneck bandwidth) from the source server to the destination.

## 🚀 Compilation & Execution
This project requires **SFML** (Simple and Fast Multimedia Library) for graphical rendering. It can be compiled using `g++` on any major operating system.

**1. Install SFML**
*   **Linux (Fedora/RHEL):** 
    ```bash
    sudo dnf install SFML-devel
    ```
*   **macOS (Homebrew):** 
    ```bash
    brew install sfml
    ```
*   **Windows (MSYS2 / MinGW64):** 
    ```bash
    pacman -S mingw-w64-x86_64-sfml
    ```

**2. Compile and Run**
Navigate to the source directory and compile the code, ensuring the SFML hardware libraries are linked.

```bash
# Navigate to the source directory
cd src

# Compile the source code
g++ main.cpp Graph.cpp -o NetworkSim -lsfml-graphics -lsfml-window -lsfml-system

# Execute the dashboard (Linux/macOS)
./NetworkSim

# Execute the dashboard (Windows)
.\NetworkSim.exe