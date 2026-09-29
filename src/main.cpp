#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

// ball would take 4 seconds to go from left to right
const int FRAMES_PER_ANIMATION = 120;

// 100 sample points for the curve to be drawn
const int SAMPLE_COUNT = 100;

// NOTES:

// a std::function is a variable that can hold a function
// A lambda is basically a function that you write without giving it a normal function name

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

            // linear
            if (keyPressed->code == sf::Keyboard::Key::Num1) {
                tween = [](float a, float b, float t) {
                    return (1 - t) * a + t * b;
                };
            }

            // quadratic ease in
            else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                tween = [](float a, float b, float t) {
                    float t2 = t * t;
                    return (1 - t2) * a + t2 * b;
                };
            }

            // sin ease in out
            else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                tween = [](float a, float b, float t) {
                    float newT = (std::sin((t - 0.5) * 3.14159265f) + 1) / 2;

                    return (1 - newT) * a + newT * b;
                };
            }

            // cos ease in out
            else if (keyPressed->code == sf::Keyboard::Key::Num4) {
                tween = [](float a, float b, float t) {
                    float newT = t * t * t * t;

                    return (1 - newT) * a + newT * b;
                };
            }

            // smooth
            else if (keyPressed->code == sf::Keyboard::Key::Num5) {
                tween = [](float a, float b, float t) {
                    float tmp1 = t * t * t;
                    float tmp2 = (1-t) * (1-t) * (1-t);

                    t = (1 - t) * tmp1 + t * (1 - tmp2);
                    return (1 - t) * a + t * b;                
                };
            }

            // ease-out quadratic. Starts fast and slows near the end
            else if (keyPressed->code == sf::Keyboard::Key::Num6) {
                tween = [](float a, float b, float t) {
                    float newT = 1 - (1 - t) * (1 - t);

                    return (1 - newT) * a + newT * b;
                };
            }

            // cubic ease-in
            else if(keyPressed->code == sf::Keyboard::Key::Num7) {
                tween = [](float a, float b, float t) {
                    float newT = t * t * t;

                    return (1 - newT) * a + newT * b;
                };
            }

            // cubic ease out
            else if(keyPressed->code == sf::Keyboard::Key::Num8) {
                tween = [](float a, float b, float t) {
                    float newT = 1 - (1 - t) * (1 - t) * (1 - t);

                    return (1 - newT) * a + newT * b;
                };
            }

            // cubic ease in out
            // slow start, faster middle, slow end
            else if(keyPressed->code == sf::Keyboard::Key::Num9) {
                tween = [](float a, float b, float t) {
                    float newT;

                    if (t < 0.5f) {
                        newT = 4.f * t * t * t;
                    }
                    else {
                        float x = -2.f * t + 2.f;
                        newT = 1.f - (x * x * x) / 2.f;
                    }

                    return (1.f - newT) * a + newT * b;
                };
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======

    // need to remember which frame we're on. 
    // static remembers the value between function calls so it doesnt reset
    static std::uint64_t frame = 0;

    // converts the frame number into a value from 0 to 1
    const float animationTime =
        // frame % FRAMES_PER_ANIMATION makes the frame number repeat
        static_cast<float>(frame % FRAMES_PER_ANIMATION) /
        static_cast<float>(FRAMES_PER_ANIMATION);

    const float startX = 100.f;
    const float endX = 700.f;

    // moves between startX and endX using the tween
    // based on how far through the animation we are
    const float circleX = tween(startX, endX, animationTime);

    // circle should be one third down from the top
    const float circleY = static_cast<float>(WINDOW_HEIGHT) / 3.f;

    sf::CircleShape circle(20.f);

    circle.setPosition({circleX, circleY});

    window.draw(circle);

    // moves to the next frame so the animation changes next time render runs
    ++frame;

    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======

    // where we want the graph to be in the window
    const float graphLeft = 100.f; // x=100
    const float graphTop = 550.f; // y=550
    const float graphWidth = 600.f; // 600px wide
    const float graphHeight = 200.f; // 200px tall

    const float graphBottom = graphTop + graphHeight;

    // create two points and interpret it as a line
    sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);

    // set the posoitions of the two points and make it a line
    xAxis[0].position = {graphLeft, graphBottom};
    xAxis[1].position = {graphLeft + graphWidth, graphBottom};

    window.draw(xAxis); // draw x

    // create 2 points on the y axis to make a vertical line
    sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);

    // the two points we need
    yAxis[0].position = {graphLeft, graphBottom};
    yAxis[1].position = {graphLeft, graphTop};

    window.draw(yAxis); // draw y

    // creates an array of vertices called curve 
    // connect eveyr point to each other
    sf::VertexArray curve(
    sf::PrimitiveType::LineStrip,
    SAMPLE_COUNT + 1
    );

    for (int i = 0; i <= SAMPLE_COUNT; ++i) {
        float t =
            // convert i into a number between 0 and 1
            static_cast<float>(i) /
            static_cast<float>(SAMPLE_COUNT);

        float tweenValue = tween(0, 1, t);

        // turns the graph's x-value t into a screen x coordinate
        float screenX = graphLeft + t * graphWidth;

        // turns the tween's value into a screen y coordinate
        float screenY = graphBottom - tweenValue * graphHeight;

        // stores the point inside the curve array
        curve[i].position = {screenX, screenY};
    }

    window.draw(curve);

    // get the current y-value of the tween curve
    // using the same animation time that controls the big circle
    float currentTweenValue = tween(0.f, 1.f, animationTime);

    // convert animationTime (0 to 1) into an x-position
    float small_cirlceX = graphLeft + animationTime * graphWidth;

    // convert the current tween value (0 to 1) into a y-position
    float small_cirlceY = graphBottom - currentTweenValue * graphHeight;

    // create a small circle
    sf::CircleShape small_cirlce(6.f);

    // put the small circle at the current point on the curve
    small_cirlce.setPosition({small_cirlceX, small_cirlceY});

    window.draw(small_cirlce);
    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
