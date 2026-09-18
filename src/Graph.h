#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <limits>

struct Edge {
    int to;
    int latency;
    int capacity;
    int flow;
    int rev_index;
    bool isActive;
};

class Graph {
private:
    int V;
    std::vector<std::vector<Edge>> adj;

public:
    Graph(int V);
    void addEdge(int u, int v, int latency, int capacity);
    void toggleLink(int u, int v, bool state);
    bool isReachable(int s, int t);
    int dijkstra(int s, int t);
    int edmondsKarp(int s, int t);
};

#endif