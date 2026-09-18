#include "Graph.h"

int main() {
    // 5 Nodes: 0 (Source/Server), 1(A), 2(B), 3(C), 4(D/Target)
    Graph g(5);
    
    // addEdge(u, v, latency, bandwidth)
    g.addEdge(0, 1, 4, 20); // S to A
    g.addEdge(0, 2, 2, 15); // S to B
    g.addEdge(1, 3, 5, 12); // A to C
    g.addEdge(2, 3, 3, 8);  // B to C
    g.addEdge(3, 4, 4, 10); // C to D
    
    int source = 0, destination = 4;

    std::cout << "--- Initial Network State ---\n";
    std::cout << "Reachable: " << (g.isReachable(source, destination) ? "Yes" : "No") << "\n";
    std::cout << "Minimum Latency: " << g.dijkstra(source, destination) << " ms\n";
    std::cout << "Maximum Bandwidth: " << g.edmondsKarp(source, destination) << " Mbps\n\n";

    // Simulate link failure: Router C to Target D goes down
    std::cout << "--- Simulating Failure (Link 3 -> 4) ---\n";
    g.toggleLink(3, 4, false);

    std::cout << "Reachable: " << (g.isReachable(source, destination) ? "Yes" : "No") << "\n";
    int lat = g.dijkstra(source, destination);
    if (lat != -1) {
        std::cout << "Minimum Latency: " << lat << " ms\n";
        std::cout << "Maximum Bandwidth: " << g.edmondsKarp(source, destination) << " Mbps\n";
    } else {
        std::cout << "Target is completely disconnected. Routing halted.\n";
    }

    return 0;
}