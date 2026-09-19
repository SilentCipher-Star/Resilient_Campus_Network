#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>

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
    std::vector<std::string> nodeNames;

public:
    Graph(int V, const std::vector<std::string>& names = {});
    void setNodeNames(const std::vector<std::string>& names);
    std::string getNodeName(int u) const;
    
    void addEdge(int u, int v, int latency, int capacity);
    void toggleLink(int u, int v, bool state);
    
    bool isReachable(int s, int t);
    int dijkstra(int s, int t, std::vector<int>& path);
    int edmondsKarp(int s, int t);
    int getBottleneckCapacity(const std::vector<int>& path, int& u_out, int& v_out);
    
    void analyzeNetwork(int s, int t);
};

#endif