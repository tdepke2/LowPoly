#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace sf;

mt19937 mainRNG;
vector<Vector2f> points;
vector<Color> colors;
RenderWindow* windowPtr = nullptr;
Image* imagePtr = nullptr;

Color findAverageColor(const Image& image, const Color& altColor, const Vector2f& a, const Vector2f& b, const Vector2f& c) {    // Find the average color in the image within the triangle between a, b, and c. If no color could be found, then altColor is picked.
    const Vector2u TOP_LEFT(max(0, static_cast<int>(min(a.x, min(b.x, c.x)))), max(0, static_cast<int>(min(a.y, min(b.y, c.y)))));
    const Vector2u BOTTOM_RIGHT(min(static_cast<int>(image.getSize().x - 1), static_cast<int>(max(a.x, max(b.x, c.x)))), min(static_cast<int>(image.getSize().y - 1), static_cast<int>(max(a.y, max(b.y, c.y)))));
    int numColors = 0;
    double redSum = 0.0, greenSum = 0.0, blueSum = 0.0;
    for (unsigned int y = TOP_LEFT.y; y < BOTTOM_RIGHT.y; ++y) {
        for (unsigned int x = TOP_LEFT.x; x < BOTTOM_RIGHT.x; ++x) {
            bool signBACrossPA = (a.x - b.x) * (a.y - y) < (a.y - b.y) * (a.x - x);
            bool signCBCrossPB = (b.x - c.x) * (b.y - y) < (b.y - c.y) * (b.x - x);
            bool signACCrossPC = (c.x - a.x) * (c.y - y) < (c.y - a.y) * (c.x - x);
            if (signBACrossPA == signCBCrossPB && signCBCrossPB == signACCrossPC) {    // If point lies within the triangle, then add it to the average.
                Color color = image.getPixel(x, y);
                redSum += color.r * color.r;    // Correct way to average colors is to take the mean of the squares of each component, then take square root.
                greenSum += color.g * color.g;
                blueSum += color.b * color.b;
                ++numColors;
            }
        }
    }
    if (numColors > 0) {
        return Color(static_cast<Uint8>(sqrt(redSum / numColors)), static_cast<Uint8>(sqrt(greenSum / numColors)), static_cast<Uint8>(sqrt(blueSum / numColors)));
    } else {
        return altColor;
    }
}

void buildTriangles(const Image& image, const Vector2u& GRID_SIZE) {
    const Vector2f CANVAS_SIZE(image.getSize());
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
    
    /*const int numColors = GRID_SIZE.x * GRID_SIZE.y * 2;
    uniform_int_distribution<int> colorRange(0, 255);
    for (int i = 0; i < numColors; ++i) {
        colors.push_back(Color(colorRange(mainRNG), colorRange(mainRNG), colorRange(mainRNG)));
    }*/
    Color lastColor = Color::Black;
    for (unsigned int y = 0; y < GRID_SIZE.y; ++y) {
        for (unsigned int x = 0; x < GRID_SIZE.x; ++x) {
            unsigned int pointIndex = y * (GRID_SIZE.x + 1) + x;
            colors.push_back(findAverageColor(image, lastColor, points[pointIndex], points[pointIndex + 1], points[pointIndex + 1 + GRID_SIZE.x]));
            lastColor = colors.back();
            colors.push_back(findAverageColor(image, lastColor, points[pointIndex + 1], points[pointIndex + 1 + GRID_SIZE.x], points[pointIndex + 2 + GRID_SIZE.x]));
            lastColor = colors.back();
        }
    }
}

void drawTriangles(RenderWindow& window, const View& view, const Vector2u& GRID_SIZE) {
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
    window.setTitle("LowPoly [Points: " + to_string(points.size()) + ", Triangles: " + to_string(colors.size()) + "]");
    window.display();
}

int main() {
    string imageFilename;
    cout << "Enter filename of image to load: ";
    getline(cin, imageFilename);
    Image image;
    if (!image.loadFromFile(imageFilename)) {
        cout << "Error: Unable to open file \"" << imageFilename << "\"" << endl;
        cout << "(Press enter)" << endl;
        cin.get();
        return -1;
    }
    float x, y;
    cout << "Enter desired grid square width and height in pixels (will be approximated): ";
    cin >> x >> y;
    const Vector2u GRID_SIZE(static_cast<unsigned int>(image.getSize().x / x + 0.5f), static_cast<unsigned int>(image.getSize().y / y + 0.5f));
    cout << "Grid size is " << GRID_SIZE.x << " x " << GRID_SIZE.y << endl;
    
    mainRNG.seed(static_cast<unsigned long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    RenderWindow window(VideoMode(image.getSize().x, image.getSize().y), "LowPoly Loading...");
    windowPtr = &window;
    imagePtr = &image;
    View view(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
    
    buildTriangles(image, GRID_SIZE);
    drawTriangles(window, view, GRID_SIZE);
    cout << "Render complete, press Enter to run it again." << endl;
    cout << "Press Ctrl + S to save the points and triangle colors to a file." << endl;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {    // Process events.
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Return) {
                    buildTriangles(image, GRID_SIZE);
                    window.setTitle("LowPoly Loading...");
                    drawTriangles(window, view, GRID_SIZE);
                } else if (event.key.code == Keyboard::S && event.key.control) {
                    cout << "Saving data... ";
                    ofstream outputFile("pointsAndColors.txt");
                    if (!outputFile.is_open()) {
                        cout << "Error: Unable to open file \"pointsAndColors.txt\" for writing." << endl;
                        cout << "(Press enter)" << endl;
                        cin.get();
                        return -1;
                    }
                    
                    outputFile << GRID_SIZE.x << " " << GRID_SIZE.y << endl;
                    outputFile << points.size() << endl;
                    for (const Vector2f& p : points) {
                        outputFile << p.x << " " << p.y << endl;
                    }
                    outputFile << colors.size() << endl;
                    for (const Color& c : colors) {
                        outputFile << static_cast<int>(c.r) << " " << static_cast<int>(c.g) << " " << static_cast<int>(c.b) << endl;
                    }
                    outputFile.close();
                    cout << "Done" << endl;
                }
            } else if (event.type == Event::Resized) {
                view.reset(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
                drawTriangles(window, view, GRID_SIZE);
            } else if (event.type == Event::Closed) {
                window.close();
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return 0;
}