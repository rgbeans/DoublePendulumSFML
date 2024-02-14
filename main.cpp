#include <SFML/Graphics.hpp>
#include <iostream>
#include <omp.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

//stuff
const double g = 9.80665;
const int screenWidth = 2560;
const int screenHeight = 1440;

class Pendulum {
public:
    double L1, L2; // lengths
    double m1, m2; // masses
    double theta1, theta2; //angles
    double omega1, omega2; //angluar velocity
    sf::CircleShape bob1, bob2; //bob the builder
    sf::Vertex arm1[2], arm2[2]; //arms

    //ctor
    Pendulum(double L1, double L2, double m1, double m2, double theta1, double theta2)
        : L1(L1), L2(L2), m1(m1), m2(m2), theta1(theta1), theta2(theta2), omega1(0), omega2(0) {
        bob1.setRadius(1);
        bob2.setRadius(2);
        bob1.setOrigin(1, 1);
        bob2.setOrigin(2, 2);
    }

    //set color in rgb and predefined colors
    void setColor(const sf::Color& color) {
        bob1.setFillColor(color);
        bob2.setFillColor(color);
        arm1[0].color = color;
        arm1[1].color = color;
        arm2[0].color = color;
        arm2[1].color = color;
    }

    //set color, but in HSV and only H
    void setColorHue(float hue) {
        // i love hardcoding!!!!
        float r, g, b;
        int i = floor(hue * 6);
        float f = hue * 6 - i;
        float q = 1 - f;

        //i love hardcoding hsv!!
        switch (i % 6) {
        case 0: r = 1, g = f, b = 0; break;
        case 1: r = q, g = 1, b = 0; break;
        case 2: r = 0, g = 1, b = f; break;
        case 3: r = 0, g = q, b = 1; break;
        case 4: r = f, g = 0, b = 1; break;
        case 5: r = 1, g = 0, b = q; break;
        }

        //ok now that the hardcoding is done we can fill the colors
        sf::Color color(r * 255, g * 255, b * 255, 255);
        bob1.setFillColor(color);
        bob2.setFillColor(color);
        arm1[0].color = color;
        arm1[1].color = color;
        arm2[0].color = color;
        arm2[1].color = color;
    }

    //Runge-Kutta step
    void step(double dt) {
        double dtheta1 = omega1;
        double dtheta2 = omega2;
        double domega1 = (m2 * g * std::sin(theta2) * std::cos(theta1 - theta2) - m2 * std::sin(theta1 - theta2) * (L1 * omega1 * omega1 * std::cos(theta1 - theta2) + L2 * omega2 * omega2) - (m1 + m2) * g * std::sin(theta1)) / (L1 * (m1 + m2 * std::sin(theta1 - theta2) * std::sin(theta1 - theta2)));
        double domega2 = ((m1 + m2) * (L1 * omega1 * omega1 * std::sin(theta1 - theta2) - g * std::sin(theta2) + g * std::sin(theta1) * std::cos(theta1 - theta2)) + m2 * L2 * omega2 * omega2 * std::sin(theta1 - theta2) * std::cos(theta1 - theta2)) / (L2 * (m1 + m2 * std::sin(theta1 - theta2) * std::sin(theta1 - theta2)));
        theta1 += dtheta1 * dt;
        theta2 += dtheta2 * dt;
        omega1 += domega1 * dt;
        omega2 += domega2 * dt;
    }

    //drawing stuff
    void draw(sf::RenderWindow& window) {
        //set the arm positions
        arm1[0].position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
        arm1[1].position = sf::Vector2f((screenWidth / 2) + L1 * std::sin(theta1), (screenHeight / 2) + L1 * std::cos(theta1));
        arm2[0].position = arm1[1].position;
        arm2[1].position = sf::Vector2f(arm2[0].position.x + L2 * std::sin(theta2), arm2[0].position.y + L2 * std::cos(theta2));
        //i dont like bob so i got rid of him
        /*bob1.setPosition(arm1[1].position);
        bob2.setPosition(arm2[1].position);*/

        window.draw(arm1, 2, sf::Lines);
        window.draw(arm2, 2, sf::Lines);
        //dont draw bob
        /*window.draw(bob1);
        window.draw(bob2);*/

    }
};

//function i made to console writeline because i hate c++
void cw(const std::string& message) {
    std::cout << message << std::endl;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Double Pendulum", sf::Style::Fullscreen);

    int numPendulums = 10000; //num of pendulum
    double minAngle = 135.0; //min angle in degrees (0 is hanging downwards vertically)
    double maxAngle = 145.0; //max angle in degrees
    std::vector<Pendulum> pendulums;
    bool isPaused = true; //is the sim paused initially

    //if its just one pendulum do these (because dividing by 0 is fun)
    if (numPendulums == 1)
    {
        //different angle for this one because funny
        double theta = 110 * (M_PI / 180);
        Pendulum pendulum(300, 300, 1, 2, theta, theta);
        pendulum.setColor(sf::Color::Red);
        pendulums.push_back(pendulum);
    }
    else
    {
        double diffTheta = (maxAngle - minAngle) / (numPendulums - 1); //calculate difference in degrees between each pendulum
        cw("Difference in degrees between each pendulum: " + std::to_string(diffTheta)); //so that i can output it to console
        for (int i = 0; i < numPendulums; ++i) {
            //dynamically set the angle proportional to the amount of pendulums
            double theta = (minAngle + i * (maxAngle - minAngle) / (numPendulums - 1)) * (M_PI / 180.0);
            Pendulum pendulum(300, 300, 1, 2, theta, theta);
            //make them colors on the hsv scale dynamically
            pendulum.setColorHue((float)i / numPendulums);
            pendulums.push_back(pendulum);
        }
    }

    //main loop
    while (window.isOpen()) {
        sf::Event event;
        //space bar pauses the sim and esc closes it
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                isPaused = !isPaused;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        //if sim is unpaused step over all the pendulums
        if (!isPaused) {
            //i love parallel processing!!!
            #pragma omp parallel for
            for (int i = 0; i < pendulums.size(); ++i) {
                pendulums[i].step(0.05);
            }
        }

        //then clear the window and draw all the pendulums seperately from the calculation because
        //sfml don't like multithreading like that (or so i heard)
        window.clear();
        for (Pendulum& pendulum : pendulums) {
            pendulum.draw(window);
        }
        window.display();
    }
    return 0;
}