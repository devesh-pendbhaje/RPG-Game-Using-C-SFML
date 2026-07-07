#include <SFML/Graphics.hpp>
#include <iostream>
int main() {
    std::cout << "Starting Test..." << std::endl;
    sf::RenderWindow window(sf::VideoMode(200, 200), "Test");
    std::cout << "Window created." << std::endl;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        window.clear();
        window.display();
    }
    std::cout << "Exiting Test." << std::endl;
    return 0;
}
