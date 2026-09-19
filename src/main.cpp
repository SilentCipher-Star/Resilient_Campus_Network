#include "Graph.h"

int main() {
  // 5 Nodes representing campus locations:
  // 0: ICT Center
  // 1: Academic Building 1
  // 2: Academic Building 2
  // 3: Library
  // 4: Auditorium
  std::vector<std::string> campusNodes = {"ICT Center", "Academic Building 1",
                                          "Academic Building 2", "Library",
                                          "Auditorium"};

  Graph g(5, campusNodes);

  // addEdge(u, v, latency_ms, capacity_Mbps)
  g.addEdge(0, 1, 4, 20); // ICT Center to AB 1
  g.addEdge(0, 2, 2, 15); // ICT Center to AB 2
  g.addEdge(1, 3, 5, 12); // AB 1 to Library
  g.addEdge(2, 3, 3, 8);  // AB 2 to Library
  g.addEdge(3, 4, 4, 10); // Library to Auditorium

  int source = 0, destination = 4;

  std::cout << ">>> INITIAL CAMPUS NETWORK STATE <<<\n\n";
  g.analyzeNetwork(source, destination);

  // Simulate link failure: Library (3) to Auditorium (4) goes down
  std::cout << ">>> SIMULATING LINK FAILURE: Library <-> Auditorium "
               "<<<\n\n";
  g.toggleLink(3, 4, false);

  g.analyzeNetwork(source, destination);

  // Restore link 3-4 and cut link 0-1 ( ICT Center <-> AB 1) to show dynamic
  // rerouting
  std::cout << ">>> RESTORING LINK 3-4 AND CUTTING LINK 0-1 (ICT Center <-> "
               "AB 1) <<<\n\n";
  g.toggleLink(3, 4, true);
  g.toggleLink(0, 1, false);

  g.analyzeNetwork(source, destination);

  return 0;
}
