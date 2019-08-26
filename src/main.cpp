#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace sf;

const Vector2u INITIAL_WINDOW_SIZE(600, 600);
mt19937 mainRNG;
vector<Vector2f> points;
vector<Color> colors;

const Vector2u GRID_SIZE(10, 10);

void buildTriangles() {
    const Vector2f CANVAS_SIZE(500.0f, 500.0f);
    const Vector2f SQUARE_SIZE(CANVAS_SIZE.x / GRID_SIZE.x, CANVAS_SIZE.y / GRID_SIZE.y);
    
    points.clear();
    points.reserve((GRID_SIZE.x + 1) * (GRID_SIZE.y + 1));
    colors.clear();
    colors.reserve(GRID_SIZE.x * GRID_SIZE.y * 2);
    uniform_real_distribution<float> xMaxOffset(-SQUARE_SIZE.x / 2.0f, SQUARE_SIZE.x / 2.0f);
    uniform_real_distribution<float> yMaxOffset(-SQUARE_SIZE.y / 2.0f, SQUARE_SIZE.y / 2.0f);
    
    points.push_back(Vector2f(0.0f, 0.0f));    // Top row.
    for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
        points.push_back(Vector2f(SQUARE_SIZE.x * x + xMaxOffset(mainRNG), 0.0f));
    }
    points.push_back(Vector2f(CANVAS_SIZE.x, 0.0f));
    
    for (unsigned int y = 1; y < GRID_SIZE.y; ++y) {    // Middle rows.
        float yPos = SQUARE_SIZE.y * y;
        points.push_back(Vector2f(0.0f, yPos + yMaxOffset(mainRNG)));
        for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
            points.push_back(Vector2f(SQUARE_SIZE.x * x + xMaxOffset(mainRNG), yPos + yMaxOffset(mainRNG)));
        }
        points.push_back(Vector2f(CANVAS_SIZE.x, yPos + yMaxOffset(mainRNG)));
    }
    
    points.push_back(Vector2f(0.0f, CANVAS_SIZE.y));    // Bottom row.
    for (unsigned int x = 1; x < GRID_SIZE.x; ++x) {
        points.push_back(Vector2f(SQUARE_SIZE.x * x + xMaxOffset(mainRNG), CANVAS_SIZE.y));
    }
    points.push_back(CANVAS_SIZE);
    
    const int numColors = GRID_SIZE.x * GRID_SIZE.y * 2;
    for (int i = 0; i < numColors; ++i) {
        int alpha = static_cast<double>(i) / numColors * INT_MAX;
        colors.push_back(Color(alpha, alpha, alpha));
    }
}

void drawTriangles(RenderWindow& window, const View& view) {
    ConvexShape triangle(3);
    window.clear();
    window.setView(view);
    unsigned int colorIndex = 0;
    for (unsigned int y = 0; y < GRID_SIZE.y; ++y) {
        for (unsigned int x = 0; x < GRID_SIZE.x; ++x) {
            unsigned int pointIndex = y * (GRID_SIZE.x + 1) + x;
            triangle.setPoint(0, points[pointIndex]);
            triangle.setPoint(1, points[pointIndex + 1]);
            triangle.setPoint(2, points[pointIndex + 1 + GRID_SIZE.x]);
            triangle.setFillColor(colors[colorIndex]);
            window.draw(triangle);
            ++colorIndex;
            triangle.setPoint(0, points[pointIndex + 1]);
            triangle.setPoint(1, points[pointIndex + 1 + GRID_SIZE.x]);
            triangle.setPoint(2, points[pointIndex + 2 + GRID_SIZE.x]);
            triangle.setFillColor(colors[colorIndex]);
            window.draw(triangle);
            ++colorIndex;
        }
    }
    window.display();
}

int main() {
    mainRNG.seed(static_cast<unsigned long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    RenderWindow window(VideoMode(INITIAL_WINDOW_SIZE.x, INITIAL_WINDOW_SIZE.y), "LowPoly Loading...");
    View view(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
    buildTriangles();
    window.setTitle("LowPoly [Points: " + to_string(points.size()) + "]");
    
    drawTriangles(window, view);
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {    // Process events.
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Enter) {
                    buildTriangles();
                    drawTriangles(window, view);
                }
            } else if (event.type == Event::Resized) {
                view.reset(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
                drawTriangles(window, view);
            } else if (event.type == Event::Closed) {
                window.close();
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}