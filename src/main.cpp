#include "Graph.h"

int main() {
    // 5 Nodes: 0 (Source/Server), 1(A), 2(B), 3(C), 4(D/Target)
    Graph g(5);
    
    g.addEdge(0, 1, 4, 20); 
    g.addEdge(0, 2, 2, 15); 
    g.addEdge(1, 3, 5, 12); 
    g.addEdge(2, 3, 3, 8);  
    g.addEdge(3, 4, 4, 10); 
    
    int source = 0, destination = 4;

    std::cout << "--- Initial Network State ---\n";
    std::cout << "Reachable: " << (g.isReachable(source, destination) ? "Yes\n" : "No\n");
    g.dijkstra(source, destination);
    std::cout << "Maximum Bandwidth (Max Flow): " << g.edmondsKarp(source, destination) << " Mbps\n\n";

    // Simulate link failure: Router C to Target D goes down
    std::cout << "--- Simulating Failure (Link 3 -> 4) ---\n";
    g.toggleLink(3, 4, false);

    bool reachable = g.isReachable(source, destination);
    std::cout << "Reachable: " << (reachable ? "Yes\n" : "No\n");
    
    if (reachable) {
        g.dijkstra(source, destination);
        std::cout << "Maximum Bandwidth (Max Flow): " << g.edmondsKarp(source, destination) << " Mbps\n";
    } else {
        std::cout << "Target is completely disconnected. Routing halted.\n";
    }

    return 0;
}