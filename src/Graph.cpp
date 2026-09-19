#include "Graph.h"
#include <algorithm>
#include <iomanip>

const int INF = std::numeric_limits<int>::max();

Graph::Graph(int V, const std::vector<std::string>& names) {
    this->V = V;
    adj.resize(V);
    setNodeNames(names);
}

void Graph::setNodeNames(const std::vector<std::string>& names) {
    nodeNames = names;
    if ((int)nodeNames.size() < V) {
        for (int i = nodeNames.size(); i < V; ++i) {
            nodeNames.push_back("Node " + std::to_string(i));
        }
    }
}

std::string Graph::getNodeName(int u) const {
    if (u >= 0 && u < (int)nodeNames.size()) {
        return nodeNames[u];
    }
    return "Node " + std::to_string(u);
}

void Graph::addEdge(int u, int v, int latency, int capacity) {
    adj[u].push_back({v, latency, capacity, 0, (int)adj[v].size(), true});
    adj[v].push_back({u, latency, capacity, 0, (int)adj[u].size() - 1, true});
}

void Graph::toggleLink(int u, int v, bool state) {
    for (auto& edge : adj[u]) {
        if (edge.to == v) edge.isActive = state;
    }
    for (auto& edge : adj[v]) {
        if (edge.to == u) edge.isActive = state;
    }
}

bool Graph::isReachable(int s, int t) {
    std::vector<bool> visited(V, false);
    std::queue<int> q;
    
    q.push(s);
    visited[s] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        if (u == t) return true;
        
        for (const auto& edge : adj[u]) {
            if (edge.isActive && !visited[edge.to]) {
                visited[edge.to] = true;
                q.push(edge.to);
            }
        }
    }
    return false;
}

int Graph::dijkstra(int s, int t, std::vector<int>& path) {
    path.clear();
    std::vector<int> dist(V, INF);
    std::vector<int> parent(V, -1);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    
    dist[s] = 0;
    pq.push({0, s});
    
    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        
        if (d > dist[u]) continue;
        if (u == t) break;
        
        for (const auto& edge : adj[u]) {
            if (edge.isActive && dist[u] + edge.latency < dist[edge.to]) {
                dist[edge.to] = dist[u] + edge.latency;
                parent[edge.to] = u;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }
    
    if (dist[t] == INF) return -1;

    for (int curr = t; curr != -1; curr = parent[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    
    return dist[t];
}

int Graph::getBottleneckCapacity(const std::vector<int>& path, int& u_out, int& v_out) {
    if (path.size() < 2) return 0;
    
    int minCap = INF;
    u_out = -1;
    v_out = -1;
    
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i];
        int v = path[i+1];
        
        for (const auto& edge : adj[u]) {
            if (edge.to == v && edge.isActive) {
                if (edge.capacity < minCap) {
                    minCap = edge.capacity;
                    u_out = u;
                    v_out = v;
                }
                break;
            }
        }
    }
    return minCap == INF ? 0 : minCap;
}

int Graph::edmondsKarp(int s, int t) {
    // Reset flows
    for (int i = 0; i < V; ++i) {
        for (auto& edge : adj[i]) {
            edge.flow = 0;
        }
    }

    int maxFlow = 0;
    std::vector<int> parent(V);
    std::vector<int> parentEdge(V);

    while (true) {
        std::fill(parent.begin(), parent.end(), -1);
        std::queue<std::pair<int, int>> q;
        q.push({s, INF});
        parent[s] = s;

        int newFlow = 0;
        while (!q.empty()) {
            int u = q.front().first;
            int flow = q.front().second;
            q.pop();

            for (size_t i = 0; i < adj[u].size(); ++i) {
                const auto& edge = adj[u][i];
                if (edge.isActive && parent[edge.to] == -1 && edge.capacity - edge.flow > 0) {
                    parent[edge.to] = u;
                    parentEdge[edge.to] = i;
                    int curFlow = std::min(flow, edge.capacity - edge.flow);
                    
                    if (edge.to == t) {
                        newFlow = curFlow;
                        break;
                    }
                    q.push({edge.to, curFlow});
                }
            }
            if (newFlow > 0) break;
        }

        if (newFlow == 0) break;

        maxFlow += newFlow;
        int curr = t;
        while (curr != s) {
            int p = parent[curr];
            int idx = parentEdge[curr];
            int rev_idx = adj[p][idx].rev_index;

            adj[p][idx].flow += newFlow;
            adj[curr][rev_idx].flow -= newFlow;
            curr = p;
        }
    }
    return maxFlow;
}

void Graph::analyzeNetwork(int s, int t) {
    bool reachable = isReachable(s, t);
    std::cout << "Reachable   : " << (reachable ? "Yes" : "No") << "\n";

    if (!reachable) {
        std::cout << "Path        : Disconnected (Routing & Flow halted)\n\n";
        return;
    }

    std::vector<int> path;
    int minLatency = dijkstra(s, t, path);
    int u_bn, v_bn;
    int bnCap = getBottleneckCapacity(path, u_bn, v_bn);
    int maxThroughput = edmondsKarp(s, t);

    std::cout << "Fastest Path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << getNodeName(path[i]);
        if (i + 1 < path.size()) std::cout << " -> ";
    }
    std::cout << "\n";
    std::cout << "Latency     : " << minLatency << " ms\n";
    std::cout << "Max Flow    : " << maxThroughput << " Mbps (Path Bottleneck: " << bnCap << " Mbps)\n\n";
}
