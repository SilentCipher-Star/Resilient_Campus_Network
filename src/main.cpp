#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Graph.h"

int main() {
    // 1. Initialize Network Math
    Graph g(5);
    g.addEdge(0, 1, 4, 20); 
    g.addEdge(0, 2, 2, 15); 
    g.addEdge(1, 3, 5, 12); 
    g.addEdge(2, 3, 3, 8);  
    g.addEdge(3, 4, 4, 10); 

    // 2. Initialize SFML Window with Anti-Aliasing for smooth, clean rendering
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Resilient Campus Network Dashboard", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    // 3. Sophisticated Color Palette (Inspired by reference dashboard)
    sf::Color bgDarkSlate(15, 20, 31);       // Deep navy/slate background
    sf::Color edgeBase(60, 75, 100);         // Muted blue-gray for inactive links
    sf::Color edgeActive(0, 230, 255);       // Bright cyan for shortest path
    sf::Color edgeError(255, 65, 85);        // Alert red/pink for failed links
    
    sf::Color nodeCore(0, 230, 255);         // Solid cyan core
    sf::Color nodeHalo(0, 230, 255, 30);     // Transparent cyan glow (alpha = 30)
    sf::Color nodeDisconnected(70, 70, 80);  // Dimmed gray for unreachable nodes

    std::vector<sf::Vector2f> nodePositions = {
        {250.f, 360.f}, {600.f, 200.f}, {600.f, 520.f}, {950.f, 360.f}, {1150.f, 360.f}
    };

    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}
    };

    // Simulation States
    bool linkFailed = false;
    bool showShortestPath = false;

    std::cout << "--- DASHBOARD CONTROLS ---\n";
    std::cout << "[ENTER] : Calculate and Highlight Minimum-Latency Route\n";
    std::cout << "[SPACE] : Simulate Link Failure (Node 3 -> Node 4)\n";
    std::cout << "[R]     : Reset Network\n\n";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Keyboard Controls
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    showShortestPath = true;
                    std::cout << "> Running Dijkstra...\n";
                    g.dijkstra(0, 4);
                }
                if (event.key.code == sf::Keyboard::Space) {
                    linkFailed = true;
                    showShortestPath = false; 
                    g.toggleLink(3, 4, false);
                    std::cout << "> ALERT: Fiber cut detected between Node 3 and Node 4!\n";
                }
                if (event.key.code == sf::Keyboard::R) {
                    linkFailed = false;
                    showShortestPath = false;
                    g.toggleLink(3, 4, true);
                    std::cout << "> System Reset.\n";
                }
            }
        }

        window.clear(bgDarkSlate);

        // Draw Edges (Lines)
        for (size_t i = 0; i < edges.size(); ++i) {
            sf::Color currentColor = edgeBase;
            
            if (i == 4 && linkFailed) {
                currentColor = edgeError;
            } else if (showShortestPath && !linkFailed && (i == 1 || i == 3 || i == 4)) {
                currentColor = edgeActive;
            }

            sf::Vertex line[] = {
                sf::Vertex(nodePositions[edges[i].first], currentColor),
                sf::Vertex(nodePositions[edges[i].second], currentColor)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Draw Nodes (Two-layer design for a clean, modern look)
        for (size_t i = 0; i < nodePositions.size(); ++i) {
            bool isDisconnected = (i == 4 && linkFailed);
            
            // 1. Draw the outer semi-transparent halo
            sf::CircleShape halo(22.f);
            halo.setOrigin(22.f, 22.f);
            halo.setPosition(nodePositions[i]);
            halo.setFillColor(isDisconnected ? sf::Color::Transparent : nodeHalo);
            
            // 2. Draw the solid inner core
            sf::CircleShape core(8.f);
            core.setOrigin(8.f, 8.f);
            core.setPosition(nodePositions[i]);
            core.setFillColor(isDisconnected ? nodeDisconnected : nodeCore);

            window.draw(halo);
            window.draw(core);
        }

        window.display();
    }

    return 0;
}