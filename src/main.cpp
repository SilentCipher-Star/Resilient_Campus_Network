#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include "Graph.h"

int main() {
    // =========================================================
    // 1. SOHAN'S NETWORK TOPOLOGY
    //    Node 0: ICT Center      (source)
    //    Node 1: Academic Bldg 1 (AB 1)  — fast primary link
    //    Node 2: Academic Bldg 2 (AB 2)  — slower backup link
    //    Node 3: Library
    //    Node 4: Auditorium      (destination)
    // =========================================================
    std::vector<std::string> campusNodes = {
        "ICT Center", "Academic Building 1",
        "Academic Building 2", "Library", "Auditorium"
    };

    Graph g(5, campusNodes);

    // Path 1 (Primary/Fast):   ICT Center -> AB1 -> Library -> Auditorium
    //   Total Latency = 2 + 3 + 4 = 9 ms
    g.addEdge(0, 1, 2, 20);  // ICT Center <-> AB 1       (fast link)
    g.addEdge(1, 3, 3, 15);  // AB 1       <-> Library
    // Path 2 (Backup/Slow):   ICT Center -> AB2 -> Library -> Auditorium
    //   Total Latency = 6 + 4 + 4 = 14 ms
    g.addEdge(0, 2, 6, 15);  // ICT Center <-> AB 2       (slower link)
    g.addEdge(2, 3, 4, 10);  // AB 2       <-> Library
    // Shared destination link
    g.addEdge(3, 4, 4, 25);  // Library    <-> Auditorium

    // =========================================================
    // 2. WINDOW & FONT SETUP
    // =========================================================
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "Resilient Campus Network Console",
        sf::Style::Default, settings
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        std::cerr << "Error: font.ttf not found in directory.\n";
        return -1;
    }

    // =========================================================
    // 3. UI COLOR PALETTE
    // =========================================================
    sf::Color bgDark(12, 12, 14);
    sf::Color panelDark(22, 24, 28);
    sf::Color deepPurple(157, 0, 255);
    sf::Color cyanActive(0, 230, 255);
    sf::Color errorRed(255, 65, 85);
    sf::Color successGreen(0, 220, 130);
    sf::Color textLight(220, 220, 225);
    sf::Color textMuted(120, 120, 130);
    sf::Color edgeDefault(70, 70, 80);

    // =========================================================
    // 4. UI LAYOUT REGIONS
    // =========================================================
    sf::RectangleShape titleBar(sf::Vector2f(1280, 50));
    titleBar.setFillColor(panelDark);

    sf::RectangleShape stepperBar(sf::Vector2f(1280, 40));
    stepperBar.setPosition(0, 50);
    stepperBar.setFillColor(sf::Color(18, 20, 24));

    sf::RectangleShape rightPanel(sf::Vector2f(380, 530));
    rightPanel.setPosition(870, 90);
    rightPanel.setFillColor(panelDark);

    sf::RectangleShape bottomBar(sf::Vector2f(1280, 100));
    bottomBar.setPosition(0, 620);
    bottomBar.setFillColor(panelDark);

    // =========================================================
    // 5. GRAPH NODE POSITIONS
    //    Layout (left to right):
    //      ICT Center (0) — AB1 (1) — Library (3) — Auditorium (4)
    //                     \ AB2 (2) /
    // =========================================================
    std::vector<sf::Vector2f> nodePositions = {
        {120.f, 340.f},   // 0: ICT Center
        {330.f, 210.f},   // 1: Academic Building 1 (upper path)
        {330.f, 470.f},   // 2: Academic Building 2 (lower path)
        {570.f, 340.f},   // 3: Library
        {770.f, 340.f}    // 4: Auditorium
    };

    // Edge indices map:  0=(0,1)  1=(1,3)  2=(0,2)  3=(2,3)  4=(3,4)
    std::vector<std::pair<int,int>> edgePairs = {
        {0, 1}, {1, 3}, {0, 2}, {2, 3}, {3, 4}
    };
    // Edge latency/capacity labels for display on graph
    std::vector<std::string> edgeLabels = {
        "2ms/20Mb", "3ms/15Mb", "6ms/15Mb", "4ms/10Mb", "4ms/25Mb"
    };

    // =========================================================
    // 6. SIMULATION STATE (Sohan's 3-event narrative)
    //    Stage 0: Initial — primary path via AB1
    //    Stage 1: AB1 link cut — reroute via AB2
    //    Stage 2: Shared Library->Auditorium link cut — total disconnect
    //    Stage 3: Reset to initial
    // =========================================================
    int simStage = 0;
    bool ab1LinkDown = false;
    bool sharedLinkDown = false;

    // Helper: compute live results for display
    auto computeResults = [&](std::string& reachStr, std::string& pathStr,
                               std::string& latencyStr, std::string& flowStr,
                               std::string& bottleneckStr,
                               std::vector<int>& dijkPath) {
        int src = 0, dst = 4;
        bool reachable = g.isReachable(src, dst);
        reachStr = reachable ? "YES" : "NO";
        dijkPath.clear();
        if (reachable) {
            int lat = g.dijkstra(src, dst, dijkPath);
            // Build path string using Sohan's node names
            pathStr = "";
            for (size_t i = 0; i < dijkPath.size(); ++i) {
                pathStr += g.getNodeName(dijkPath[i]);
                if (i + 1 < dijkPath.size()) pathStr += " -> ";
            }
            latencyStr = std::to_string(lat) + " ms";

            // Bottleneck
            int u_bn, v_bn;
            int bnCap = g.getBottleneckCapacity(dijkPath, u_bn, v_bn);
            bottleneckStr = std::to_string(bnCap) + " Mbps";

            // Max flow
            int mf = g.edmondsKarp(src, dst);
            flowStr = std::to_string(mf) + " Mbps";
        } else {
            pathStr = "Disconnected";
            latencyStr = "N/A";
            bottleneckStr = "N/A";
            flowStr = "0 Mbps";
        }
    };

    // =========================================================
    // 7. MAIN LOOP
    // =========================================================
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                // Stage 0: Reset / Initial state
                if (event.key.code == sf::Keyboard::Num0 ||
                    event.key.code == sf::Keyboard::R) {
                    simStage = 0;
                    ab1LinkDown = false;
                    sharedLinkDown = false;
                    g.toggleLink(0, 1, true);
                    g.toggleLink(3, 4, true);
                }
                // Stage 1: Cut AB1 primary link (Sohan's Event 1)
                if (event.key.code == sf::Keyboard::Num1) {
                    simStage = 1;
                    if (!ab1LinkDown) {
                        ab1LinkDown = true;
                        g.toggleLink(0, 1, false);
                    }
                }
                // Stage 2: Cut shared Library<->Auditorium link (Sohan's Event 2)
                if (event.key.code == sf::Keyboard::Num2) {
                    simStage = 2;
                    if (!ab1LinkDown) {
                        ab1LinkDown = true;
                        g.toggleLink(0, 1, false);
                    }
                    if (!sharedLinkDown) {
                        sharedLinkDown = true;
                        g.toggleLink(3, 4, false);
                    }
                }
                // Stage 3: Full analysis view (all links active — show algorithms)
                if (event.key.code == sf::Keyboard::Num3) {
                    simStage = 3;
                    ab1LinkDown = false;
                    sharedLinkDown = false;
                    g.toggleLink(0, 1, true);
                    g.toggleLink(3, 4, true);
                }
                // SPACE: quick-toggle the shared Library<->Auditorium fiber
                if (event.key.code == sf::Keyboard::Space) {
                    sharedLinkDown = !sharedLinkDown;
                    g.toggleLink(3, 4, !sharedLinkDown);
                }
            }
        }

        // ---------- Compute live results ----------
        std::string reachStr, pathStr, latencyStr, flowStr, bottleneckStr;
        std::vector<int> dijkPath;
        computeResults(reachStr, pathStr, latencyStr, flowStr, bottleneckStr, dijkPath);
        bool reachable = (reachStr == "YES");

        // Build set of edges on the shortest path for highlighting
        std::set<std::pair<int,int>> pathEdgeSet;
        for (size_t i = 0; i + 1 < dijkPath.size(); ++i) {
            pathEdgeSet.insert({dijkPath[i], dijkPath[i+1]});
            pathEdgeSet.insert({dijkPath[i+1], dijkPath[i]});
        }

        // ---------- Draw ----------
        window.clear(bgDark);

        // -- Panels --
        window.draw(titleBar);
        window.draw(stepperBar);
        window.draw(rightPanel);
        window.draw(bottomBar);

        // -- Helper lambda --
        auto drawText = [&](const std::string& str, float x, float y,
                            int size, sf::Color color) {
            sf::Text t(str, font, size);
            t.setPosition(x, y);
            t.setFillColor(color);
            window.draw(t);
        };

        // -- Title & Stepper --
        drawText("RESILIENT CAMPUS NETWORK CONSOLE", 20, 14, 16, cyanActive);

        // Stage labels in stepper bar
        auto stageColor = [&](int s) {
            return simStage == s ? cyanActive : textMuted;
        };
        drawText("[0] Initial State",         20,  60, 13, stageColor(0));
        drawText("[1] AB1 Link Cut",          200, 60, 13, stageColor(1));
        drawText("[2] Total Disconnect",       370, 60, 13, stageColor(2));
        drawText("[3] Full Analysis",          560, 60, 13, stageColor(3));

        // -- Right Panel: LIVE FEED --
        float px = 885.f, py = 100.f, lineH = 22.f;
        drawText("LIVE FEED & RESULTS", px, py, 15, deepPurple);
        py += 30.f;

        // Status header
        std::string stageLabel;
        switch (simStage) {
            case 0: stageLabel = ">>> Initial State: Primary via AB1"; break;
            case 1: stageLabel = ">>> EVENT: AB1 Cut — Rerouting"; break;
            case 2: stageLabel = ">>> EVENT: Total Disconnection"; break;
            case 3: stageLabel = ">>> Algorithm Analysis (All Links)"; break;
        }
        drawText(stageLabel, px, py, 12, simStage == 2 ? errorRed : cyanActive);
        py += lineH + 5.f;

        // Separator line
        sf::RectangleShape sep(sf::Vector2f(340.f, 1.f));
        sep.setPosition(px, py);
        sep.setFillColor(sf::Color(60, 60, 70));
        window.draw(sep);
        py += 10.f;

        // Reachability
        drawText("BFS Reachability:", px, py, 13, textMuted);
        py += lineH;
        drawText(reachStr, px + 12, py, 14,
                 reachable ? successGreen : errorRed);
        if (ab1LinkDown)
            drawText("  [AB1 link: OFFLINE]", px + 55, py, 12, errorRed);
        if (sharedLinkDown)
            drawText("  [Fiber to Aud: CUT]", px + 55, py + lineH, 12, errorRed);
        py += lineH * (sharedLinkDown ? 2.f : 1.f) + 8.f;

        // Dijkstra
        drawText("Dijkstra (Min-Latency Path):", px, py, 13, textMuted);
        py += lineH;
        if (reachable) {
            // Word-wrap the path string across two lines if needed
            std::string p1 = pathStr, p2 = "";
            if (pathStr.size() > 28) {
                size_t mid = pathStr.rfind("->", 28);
                if (mid != std::string::npos) {
                    p1 = pathStr.substr(0, mid + 2);
                    p2 = "  " + pathStr.substr(mid + 2);
                }
            }
            drawText(p1, px + 12, py, 12, textLight);
            if (!p2.empty()) { py += 16; drawText(p2, px + 12, py, 12, textLight); }
            py += lineH;
            drawText("Latency: " + latencyStr, px + 12, py, 13, cyanActive);
        } else {
            drawText("Path: Disconnected", px + 12, py, 13, errorRed);
            py += lineH;
            drawText("Latency: N/A", px + 12, py, 13, textMuted);
        }
        py += lineH + 8.f;

        // Max Flow
        drawText("Edmonds-Karp (Max Flow):", px, py, 13, textMuted);
        py += lineH;
        drawText("Max Flow: " + flowStr,         px + 12, py, 13, deepPurple);
        py += lineH;
        drawText("Bottleneck: " + bottleneckStr,  px + 12, py, 13,
                 reachable ? cyanActive : textMuted);
        py += lineH + 15.f;

        // -- Bottom Bar --
        drawText("DISRUPTION CONTROLS", 20, 632, 13, textMuted);
        drawText("[0] Initial  [1] Cut AB1  [2] Total Disconnect  [3] Analysis  "
                 "[SPACE] Toggle Fiber  [R] Reset",
                 20, 655, 12, textLight);

        // =========================================================
        // 8. DRAW GRAPH MAP
        // =========================================================

        // -- Edges --
        for (size_t i = 0; i < edgePairs.size(); ++i) {
            int u = edgePairs[i].first;
            int v = edgePairs[i].second;

            bool isDown = false;
            if (i == 0 && ab1LinkDown)   isDown = true;  // ICT <-> AB1
            if (i == 4 && sharedLinkDown) isDown = true; // Library <-> Auditorium

            sf::Color eCol = edgeDefault;
            if (isDown) {
                eCol = errorRed;
            } else if (reachable && pathEdgeSet.count({u, v})) {
                // Highlight shortest path
                eCol = cyanActive;
            } else if (simStage == 3 && reachable) {
                eCol = deepPurple; // All active edges in analysis mode
            }

            // Thicker line (3 lines offset by 1px for thickness illusion)
            for (int off = -1; off <= 1; ++off) {
                sf::Vertex line[2] = {
                    sf::Vertex(sf::Vector2f(nodePositions[u].x, nodePositions[u].y + off), eCol),
                    sf::Vertex(sf::Vector2f(nodePositions[v].x, nodePositions[v].y + off), eCol)
                };
                window.draw(line, 2, sf::Lines);
            }

            // Edge weight label at midpoint
            sf::Vector2f mid = {
                (nodePositions[u].x + nodePositions[v].x) / 2.f - 25.f,
                (nodePositions[u].y + nodePositions[v].y) / 2.f - 18.f
            };
            drawText(edgeLabels[i], mid.x, mid.y, 10,
                     isDown ? errorRed : sf::Color(160, 160, 170));
        }

        // -- Nodes --
        for (size_t i = 0; i < nodePositions.size(); ++i) {
            bool isDown = (i == 1 && ab1LinkDown) ||
                          (i == 4 && sharedLinkDown);

            // Glow ring
            sf::CircleShape glow(18.f);
            glow.setOrigin(18.f, 18.f);
            glow.setPosition(nodePositions[i]);
            sf::Color glowCol = isDown ? sf::Color(80, 20, 20, 100) :
                                (reachable && (std::find(dijkPath.begin(), dijkPath.end(), (int)i) != dijkPath.end())
                                    ? sf::Color(0, 180, 200, 80)
                                    : sf::Color(100, 0, 200, 60));
            glow.setFillColor(glowCol);
            window.draw(glow);

            // Main circle
            sf::CircleShape node(12.f);
            node.setOrigin(12.f, 12.f);
            node.setPosition(nodePositions[i]);

            sf::Color nodeCol = deepPurple;
            if (isDown) {
                nodeCol = sf::Color(50, 50, 55);
            } else if (reachable && std::find(dijkPath.begin(), dijkPath.end(), (int)i) != dijkPath.end()) {
                nodeCol = cyanActive;
            } else if (simStage == 3) {
                nodeCol = deepPurple;
            }
            node.setFillColor(nodeCol);
            node.setOutlineThickness(2.f);
            node.setOutlineColor(isDown ? errorRed : sf::Color(200, 200, 220, 180));
            window.draw(node);

            // Node label (Sohan's names)
            float lx = nodePositions[i].x - 55.f;
            float ly = nodePositions[i].y + 16.f;
            // For AB1 and AB2, label above to avoid overlap
            if (i == 1 || i == 2) {
                ly = nodePositions[i].y - 32.f;
            }
            drawText(campusNodes[i], lx, ly, 11,
                     isDown ? errorRed : textLight);

            // Node index badge
            drawText(std::to_string(i),
                     nodePositions[i].x - 4.f,
                     nodePositions[i].y - 8.f,
                     11, isDown ? errorRed : bgDark);
        }

        window.display();
    }

    return 0;
}