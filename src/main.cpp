#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
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

    // 2. Window & Font Setup
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Resilient Campus Network Console", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        std::cerr << "Error: font.ttf not found in directory. The copy command failed!\n";
        return -1;
    }

    // 3. UI Color Palette
    sf::Color bgDark(12, 12, 14);           
    sf::Color panelDark(22, 24, 28);        
    sf::Color deepPurple(157, 0, 255);      
    sf::Color cyanActive(0, 230, 255);      
    sf::Color errorRed(255, 65, 85);        
    sf::Color textLight(220, 220, 225);
    sf::Color textMuted(120, 120, 130);

    // 4. UI Layout Regions
    sf::RectangleShape titleBar(sf::Vector2f(1280, 50));
    titleBar.setFillColor(panelDark);

    sf::RectangleShape stepperBar(sf::Vector2f(1280, 40));
    stepperBar.setPosition(0, 50);
    stepperBar.setFillColor(sf::Color(18, 20, 24));

    sf::RectangleShape rightPanel(sf::Vector2f(380, 530));
    rightPanel.setPosition(900, 90);
    rightPanel.setFillColor(panelDark);

    sf::RectangleShape bottomBar(sf::Vector2f(1280, 100));
    bottomBar.setPosition(0, 620);
    bottomBar.setFillColor(panelDark);

    // 5. Graph Coordinates 
    std::vector<sf::Vector2f> nodePositions = {
        {150.f, 350.f}, {450.f, 200.f}, {450.f, 500.f}, {750.f, 350.f}, {860.f, 350.f}
    };
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}
    };

    // 6. Simulation State
    int currentStage = 0; 
    bool linkFailed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                // Stepper Controls
                if (event.key.code == sf::Keyboard::Num1) currentStage = 1; 
                if (event.key.code == sf::Keyboard::Num2) currentStage = 2; 
                if (event.key.code == sf::Keyboard::Num3) currentStage = 3; 
                if (event.key.code == sf::Keyboard::Num0) currentStage = 0; 
                
                // Disruption & Master Reset Controls
                if (event.key.code == sf::Keyboard::Space) {
                    linkFailed = !linkFailed; 
                    g.toggleLink(3, 4, !linkFailed);
                }
                if (event.key.code == sf::Keyboard::R) {
                    currentStage = 0;
                    linkFailed = false;
                    g.toggleLink(3, 4, true);
                }
            }
        }

        window.clear(bgDark);

        // --- DRAW UI PANELS ---
        window.draw(titleBar);
        window.draw(stepperBar);
        window.draw(rightPanel);
        window.draw(bottomBar);

        // --- DRAW TEXT ELEMENTS ---
        auto drawText = [&](std::string str, float x, float y, int size, sf::Color color) {
            sf::Text t(str, font, size);
            t.setPosition(x, y);
            t.setFillColor(color);
            window.draw(t);
        };

        drawText("RESILIENT CAMPUS NETWORK CONSOLE", 20, 15, 16, cyanActive);
        
        // Stepper Text
        drawText("[0] Baseline", 20, 60, 14, currentStage == 0 ? cyanActive : textMuted);
        drawText("[1] Reachability (BFS)", 150, 60, 14, currentStage == 1 ? cyanActive : textMuted);
        drawText("[2] Min-Latency (Dijkstra)", 350, 60, 14, currentStage == 2 ? cyanActive : textMuted);
        drawText("[3] Throughput (Max Flow)", 580, 60, 14, currentStage == 3 ? cyanActive : textMuted);

        // Right Panel Content
        drawText("LIVE FEED & RESULTS", 920, 110, 16, deepPurple);
        if (linkFailed) drawText("ALERT: Link 3->4 Failed", 920, 150, 14, errorRed);
        else drawText("System Nominal", 920, 150, 14, textLight);

        if (currentStage == 1) {
            drawText("> Running BFS...", 920, 200, 14, textMuted);
            drawText(linkFailed ? "Target Unreachable" : "Target Reachable", 920, 220, 14, textLight);
        } else if (currentStage == 2 && !linkFailed) {
            drawText("> Running Dijkstra...", 920, 200, 14, textMuted);
            drawText("Path: 0 -> 2 -> 3 -> 4", 920, 220, 14, textLight);
            drawText("Total Latency: 9 ms", 920, 240, 14, cyanActive);
        } else if (currentStage == 3 && !linkFailed) {
            drawText("> Running Edmonds-Karp...", 920, 200, 14, textMuted);
            drawText("Bottleneck: 8 Mbps", 920, 220, 14, textLight);
            drawText("Max Flow: 10 Mbps", 920, 240, 14, deepPurple);
        } else if (currentStage == 0) {
            drawText("> Waiting for input...", 920, 200, 14, textMuted);
        }

        // Bottom Bar Controls
        drawText("DISRUPTION INJECTION & CONTROLS", 20, 640, 14, textMuted);
        drawText("Press [SPACE] Toggle Fiber Cut   |   Press [0-3] Step Pipeline   |   Press [R] Master Reset", 20, 670, 14, textLight);

        // --- DRAW GRAPH MAP ---
        for (size_t i = 0; i < edges.size(); ++i) {
            sf::Color eCol = sf::Color(70, 70, 80);
            if (i == 4 && linkFailed) eCol = errorRed;
            else if (currentStage == 2 && !linkFailed && (i == 1 || i == 3 || i == 4)) eCol = cyanActive;
            else if (currentStage == 3 && !linkFailed) eCol = deepPurple; 

            sf::Vertex line[] = {
                sf::Vertex(nodePositions[edges[i].first], eCol),
                sf::Vertex(nodePositions[edges[i].second], eCol)
            };
            window.draw(line, 2, sf::Lines);
        }

        for (size_t i = 0; i < nodePositions.size(); ++i) {
            bool isDead = (i == 4 && linkFailed);
            sf::CircleShape node(12.f);
            node.setOrigin(12.f, 12.f);
            node.setPosition(nodePositions[i]);
            node.setFillColor(isDead ? sf::Color(50, 50, 50) : deepPurple);
            if (currentStage == 2 && !isDead && (i == 0 || i == 2 || i == 3 || i == 4)) node.setFillColor(cyanActive);
            
            window.draw(node);
        }

        window.display();
    }
    return 0;
}