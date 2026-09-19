#include "Graph.h"

int main() {
  // 5 Nodes representing campus locations:
  // 0: ICT Center
  // 1: Academic Building 1 (AB 1)
  // 2: Academic Building 2 (AB 2)
  // 3: Library
  // 4: Auditorium
  std::vector<std::string> campusNodes = {"ICT Center", "Academic Building 1",
                                          "Academic Building 2", "Library",
                                          "Auditorium"};

  Graph g(5, campusNodes);

  // addEdge(u, v, latency_ms, capacity_Mbps)
  // Path 1 via AB 1: Total Latency = 2 + 3 + 4 = 9 ms (Primary/Fastest)
  g.addEdge(0, 1, 2, 20); // ICT Center <-> AB 1 (Fast Link)
  g.addEdge(1, 3, 3, 15); // AB 1 <-> Library

  // Path 2 via AB 2: Total Latency = 6 + 4 + 4 = 14 ms (Secondary/Backup)
  g.addEdge(0, 2, 6, 15); // ICT Center <-> AB 2 (Slower Link)
  g.addEdge(2, 3, 4, 10); // AB 2 <-> Library

  // Shared Link to Destination
  g.addEdge(3, 4, 4, 25); // Library <-> Auditorium

  int source = 0, destination = 4;

  std::cout << "========================================================\n";
  std::cout << ">>> 1. INITIAL STATE: Primary Path via AB 1 Selected <<<\n";
  std::cout << "========================================================\n";
  g.analyzeNetwork(source, destination);

  // Event 1: Disconnect AB 1 (Cut Link 0 <-> 1)
  // Automatically reroutes via AB 2!
  std::cout << "========================================================\n";
  std::cout << ">>> 2. EVENT: AB 1 Disconnected! (Dynamic Rerouting) <<<\n";
  std::cout << "========================================================\n";
  g.toggleLink(0, 1, false);
  g.analyzeNetwork(source, destination);

  // Event 2: Cut Shared Link to Auditorium (3 <-> 4)
  // ALL paths destroyed -> Program Halts at Stage 1!
  std::cout << "========================================================\n";
  std::cout << ">>> 3. EVENT: Total Disconnection (All Paths Down)   <<<\n";
  std::cout << "========================================================\n";
  g.toggleLink(3, 4, false);
  g.analyzeNetwork(source, destination);

  return 0;
}
