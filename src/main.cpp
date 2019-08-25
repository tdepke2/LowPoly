#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

const unsigned int FRAMERATE_LIMIT = 60;
const Vector2u INITIAL_WINDOW_SIZE(900, 900);
mt19937 mainRNG;

float randomFloat(float min, float max) {
    uniform_real_distribution<float> minMaxRange(min, max);
    return minMaxRange(mainRNG);
}

int main() {
    mainRNG.seed(static_cast<unsigned long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    RenderWindow window(VideoMode(INITIAL_WINDOW_SIZE.x, INITIAL_WINDOW_SIZE.y), "LowPoly Loading...");
    window.setFramerateLimit(FRAMERATE_LIMIT);
    ConvexShape triangle(3);
    triangle.setPoint(0, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setPoint(1, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setPoint(2, Vector2f(randomFloat(0.0f, 900.0f), randomFloat(0.0f, 900.0f)));
    triangle.setFillColor(Color::Green);
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
            window.setTitle("LowPoly [FPS: " + to_string(fpsCounter) + "]");
            perSecondClock.restart();
            fpsCounter = 0;
        }
        
        ++fpsCounter;
        window.clear();
        window.draw(triangle);
        window.display();
    }
}