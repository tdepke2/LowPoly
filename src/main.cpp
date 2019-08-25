#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace sf;

const unsigned int FRAMERATE_LIMIT = 60;
const Vector2u INITIAL_WINDOW_SIZE(600, 600);
mt19937 mainRNG;
vector<Vector2f> points;
vector<Color> colors;

float randomFloat(float min, float max) {
    uniform_real_distribution<float> minMaxRange(min, max);
    return minMaxRange(mainRNG);
}

void buildTriangles() {
    const Vector2u GRID_SIZE(10, 10);
    const Vector2f CANVAS_SIZE(500.0f, 500.0f);
    const Vector2f SQUARE_SIZE(CANVAS_SIZE.x / GRID_SIZE.x, CANVAS_SIZE.y / GRID_SIZE.y);
    
    points.clear();
    points.reserve((GRID_SIZE.x + 1) * (GRID_SIZE.y + 1));
    
    points.push_back(Vector2f(0.0f, 0.0f));    // Top row.
    for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
        points.push_back(Vector2f(SQUARE_SIZE.x * x, 0.0f));
    }
    points.push_back(Vector2f(CANVAS_SIZE.x, 0.0f));
    
    for (unsigned int y = 1; y < GRID_SIZE.y; ++y) {    // Middle rows.
        float yPos = SQUARE_SIZE.y * y;
        points.push_back(Vector2f(0.0f, yPos));
        for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
            points.push_back(Vector2f(SQUARE_SIZE.x * x, yPos));
        }
        points.push_back(Vector2f(CANVAS_SIZE.x, yPos));
    }
    
    points.push_back(Vector2f(0.0f, CANVAS_SIZE.y));    // Bottom row.
    for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
        points.push_back(Vector2f(SQUARE_SIZE.x * x, CANVAS_SIZE.y));
    }
    points.push_back(CANVAS_SIZE);
}

int main() {
    mainRNG.seed(static_cast<unsigned long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    RenderWindow window(VideoMode(INITIAL_WINDOW_SIZE.x, INITIAL_WINDOW_SIZE.y), "LowPoly Loading...");
    window.setFramerateLimit(FRAMERATE_LIMIT);
    /*ConvexShape triangle(3);
    triangle.setPoint(0, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setPoint(1, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setPoint(2, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setFillColor(Color::Green);*/
    Vertex point;
    point.color = Color::Green;
    buildTriangles();
    int fpsCounter = 0;
    Clock perSecondClock;
    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {    // Process events.
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        
        if (perSecondClock.getElapsedTime().asSeconds() >= 1.0f) {    // Calculate FPS.
            window.setTitle("LowPoly [FPS: " + to_string(fpsCounter) + ", Points: " + to_string(points.size()) + "]");
            perSecondClock.restart();
            fpsCounter = 0;
        }
        
        ++fpsCounter;
        window.clear();
        for (const Vector2f& pos : points) {
            point.position = pos;
            window.draw(&point, 1, Points);
        }
        window.display();
    }
}