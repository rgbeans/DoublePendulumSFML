#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <omp.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

//stuff
const long double g = 9.80665;
const int screenWidth = 2560;
const int screenHeight = 1440;

class Fortnite {};

class Pendulum {
public:
    long double L1, L2; // lengths
    long double m1, m2; // masses
    long double theta1, theta2; //angles
    long double omega1, omega2; //angluar velocity
    long double time;
    long double step;
    sf::CircleShape bob1, bob2; //bob the builder
    sf::Vertex arm1[2], arm2[2]; //arms

    //ctor
    Pendulum(long double L1, long double L2, long double m1, long double m2, long double theta1, long double theta2, long double step)
        : L1(L1), L2(L2), m1(m1), m2(m2), theta1(theta1), theta2(theta2), omega1(0), omega2(0), step(step) {
        bob1.setRadius(10);
        bob2.setRadius(20);
        bob1.setOrigin(10, 10);
        bob2.setOrigin(20, 20);
    }

    // Get the x position of the first mass
    long double getX1() const {
        return L1 * std::sin(theta1);
    }

    // Get the y position of the first mass
    long double getY1() const {
        return L1 * std::cos(theta1);
    }

    // Get the x position of the second mass
    long double getX2() const {
        return getX1() + L2 * std::sin(theta2);
    }

    // Get the y position of the second mass
    long double getY2() const {
        return getY1() + L2 * std::cos(theta2);
    }


    void getPos(const std::string& filePath) const {
        std::ofstream outFile(filePath); // Open file for writing
        if (outFile.is_open()) { // Check if the file is opened successfully
            outFile << "bob1 x: " << getX1() << "  y: " << getY1()
                << "    bob2 x: " << getX2() << " y: " << getY2() << std::endl;
            outFile.close(); // Close the file
        }
        else {
            std::cerr << "Unable to open file for writing!" << std::endl;
        }
    }

    void getPos2() const {
        std::cout << "bob1 x: " << getX1() << "  y: " << getY1()
            << "    bob2 x: " << getX2() << " y: " << getY2() << std::endl;
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
    void setColorHue(float hue, float opacity) {
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
        sf::Color color(r * 255, g * 255, b * 255, opacity);
        bob1.setFillColor(color);
        bob2.setFillColor(color);
        arm1[0].color = color;
        arm1[1].color = color;
        arm2[0].color = color;
        arm2[1].color = color;
    }

    //Runge-Kutta step
    void RKstep(long double dt) {
        time += step;
        long double dtheta1 = omega1;
        long double dtheta2 = omega2;
        long double domega1 = (m2 * g * std::sin(theta2) * std::cos(theta1 - theta2) - m2 * std::sin(theta1 - theta2) * (L1 * omega1 * omega1 * std::cos(theta1 - theta2) + L2 * omega2 * omega2) - (m1 + m2) * g * std::sin(theta1)) / (L1 * (m1 + m2 * std::sin(theta1 - theta2) * std::sin(theta1 - theta2)));
        long double domega2 = ((m1 + m2) * (L1 * omega1 * omega1 * std::sin(theta1 - theta2) - g * std::sin(theta2) + g * std::sin(theta1) * std::cos(theta1 - theta2)) + m2 * L2 * omega2 * omega2 * std::sin(theta1 - theta2) * std::cos(theta1 - theta2)) / (L2 * (m1 + m2 * std::sin(theta1 - theta2) * std::sin(theta1 - theta2)));
        theta1 += dtheta1 * dt;
        theta2 += dtheta2 * dt;
        omega1 += domega1 * dt;
        omega2 += domega2 * dt;
    }

    //drawing stuff
    void draw(sf::RenderWindow& window) {
        //set the arm positions
        arm1[0].position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
        arm1[1].position = sf::Vector2f((screenWidth / 2) + 300 * std::sin(theta1), (screenHeight / 2) + 300 * std::cos(theta1));
        arm2[0].position = arm1[1].position;
        arm2[1].position = sf::Vector2f(arm2[0].position.x + 300 * std::sin(theta2), arm2[0].position.y + 300 * std::cos(theta2));
        //i dont like bob so i got rid of him
        bob1.setPosition(arm1[1].position);
        bob2.setPosition(arm2[1].position);

        window.draw(arm1, 2, sf::Lines);
        window.draw(arm2, 2, sf::Lines);
        //dont draw bob
        window.draw(bob1);
        window.draw(bob2);

    }
};

//function i made to console writeline because i hate c++
void cw(const std::string& message) {
    std::cout << message << std::endl;
}

inline std::string to_string_precise(long double _Val, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << _Val;
    return out.str();
}

int main() {
    //loading the font gave me a ton of problems :/
    sf::Font font;
    if (!font.loadFromFile("ComicSansMS3.ttf"))
    {
        // Error handling
        std::cout << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Double Pendulum"/*, sf::Style::Fullscreen*/);


    //watermark
    sf::Text watermark("@greenbeans9814 on instagram", font, 50);
    sf::Vector2f watermarkVelocity(200.f, 150.f);
    watermark.setFillColor(sf::Color(255, 255, 255, 64));
    watermark.setPosition(screenWidth / 2 - watermark.getGlobalBounds().width / 2, screenHeight / 2 - watermark.getGlobalBounds().height / 2);

    int numPendulums = 1; //num of pendulum
    long double minAngle = 177.0; //min angle in degrees (0 is hanging downwards vertically)
    long double maxAngle = 179.9; //max angle in degrees
    std::vector<Pendulum> pendulums;
    bool isPaused = true; //is the sim paused initially
    bool watermarkMovin = false; //does the watermark move

    //if its just one pendulum do these (because dividing by 0 is fun)
    if (numPendulums == 1)
    {
        //different angle for this one because funny
        long double theta = 110 * (M_PI / 180);
        long double theta2 = theta + 0.00000000000001;
        Pendulum pendulum(1, 1, 1, 2, theta, theta, 0.000000001); //0.000000001
        Pendulum pendulum2(1, 1, 1, 2, theta2, theta2, 0.000000001);
        pendulum.setColor(sf::Color::Green);
        pendulum2.setColor(sf::Color::Red);
        pendulums.push_back(pendulum);
        pendulums.push_back(pendulum2);
    }
    else
    {
        double diffTheta = (maxAngle - minAngle) / (numPendulums - 1); //calculate difference in degrees between each pendulum
        cw("Difference in degrees between each pendulum: " + std::to_string(diffTheta)); //so that i can output it to console
        for (int i = 0; i < numPendulums; ++i) {
            //dynamically set the angle proportional to the amount of pendulums
            double theta = (minAngle + i * (maxAngle - minAngle) / (numPendulums - 1)) * (M_PI / 180.0);
            Pendulum pendulum(1, 1, 1, 2, theta, theta, 0.000000001);
            //make them colors on the hsv scale dynamically
            pendulum.setColorHue((float)i / numPendulums, 128);
            pendulums.push_back(pendulum);
        }
    }

    sf::Clock clock;

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

        sf::Time deltaTime = clock.restart();

        long double timeIntervals[] = { 10.0, 10.0, 10.0 };
        
        

        //if sim is unpaused step over all the pendulums
        if (!isPaused) {
            //i love parallel processing!!!
            //#pragma omp parallel for
            for (int i = 0; i < pendulums.size(); ++i) {
                for (int j = 0; j <= 100000; j++)
                {
                    pendulums[i].RKstep(pendulums[i].step);
                    // Check if the totalTime has exceeded the logInterval
                    if (pendulums[0].time >= timeIntervals[i]) {
                        isPaused = true;

                        sf::Texture texture;
                        texture.create(window.getSize().x, window.getSize().y);
                        texture.update(window);

                        // Convert the texture to an image
                        sf::Image screenshot = texture.copyToImage();

                        // Save the image to a file
                        /*std::string screenshotFilename = "screenshot" + std::to_string(i) + std::to_string(timeIntervals[i]) + ".png";
                        screenshot.saveToFile(screenshotFilename);

                        std::string outputFilename = "output" + std::to_string(i) + std::to_string(timeIntervals[i]) + ".txt";
                        pendulums[i].getPos(outputFilename);*/
                        timeIntervals[i] = 100000.0;
                    }
                }
            }
        }

        watermark.setString("x1:" + to_string_precise(pendulums[0].getX1(), 33) + "\nx2:" +
            to_string_precise(pendulums[0].getX2(),33) + "\ny1:" +
            to_string_precise(pendulums[0].getY1(),33) + "\ny2:" +
            to_string_precise(pendulums[0].getY2(),33) + "\ntime:" + 
            to_string_precise(pendulums[0].time, 33) + "\nx1:" +
            to_string_precise(pendulums[1].getX1(), 33) + "\nx2:" +
            to_string_precise(pendulums[1].getX2(), 33) + "\ny1:" +
            to_string_precise(pendulums[1].getY1(), 33) + "\ny2:" +
            to_string_precise(pendulums[1].getY2(), 33) + "\ntime:" +
            to_string_precise(pendulums[1].time, 33) + "");

        


        //then clear the window and draw all the pendulums seperately from the calculation because
        //sfml don't like multithreading like that (or so i heard)
        window.clear();

        if (watermarkMovin)
        {
            //draw the watermark
            float dt = deltaTime.asSeconds();
            watermark.move(watermarkVelocity.x * dt, watermarkVelocity.y * dt);

            //check for collision
            if (watermark.getPosition().x < 0 || watermark.getPosition().x + watermark.getGlobalBounds().width > screenWidth)
                watermarkVelocity.x = -watermarkVelocity.x;
            if (watermark.getPosition().y < 0 || watermark.getPosition().y + watermark.getGlobalBounds().height > screenHeight)
                watermarkVelocity.y = -watermarkVelocity.y;
        }

        window.draw(watermark);

        for (Pendulum& pendulum : pendulums) {
            pendulum.draw(window);
        }
        window.display();
    }
    return 0;
}
