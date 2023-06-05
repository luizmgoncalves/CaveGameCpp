#include <SFML/Graphics.hpp>
#include <iostream>
#include<cstdlib>
#include <list>
#include <Entity.h>
#include <Envirorment.h>

using namespace std;

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800


class Inventory {
public:
    unordered_map<int, int> items;
    sf::Sprite arrow, item;
    sf::Texture arrowt;
    int curr_item = 1;

    Inventory() {
        for (int i = 1; i < env::Textures::LAST; i++) {
            items.at(i) = 0;
        }

        this->arrowt.loadFromFile("game_images/arrow.png");

        this->arrow = sf::Sprite();

        this->arrow.setPosition({ SCREEN_WIDTH, SCREEN_HEIGHT } / 2.f + {-this->arrowt.getSize().x / 2.f, 300});

        this->arrow.setTexture(this->arrowt);

        this->item = env::Textures.textures->at(curr_item);
        this->item.setPosition(this->arrow.getPosition() + {this->arrowt.getSize().x, 0});

    }

    void draw(sf::RenderWindow* window) {
        window->draw(this->arrow);
        window->draw(this->item);

        sf::Text number(to_string(this->items[this->curr_item]), FONTE);
        number.setPosition(this->item.getPosition());

        window->draw(number);
    }
};


class ElementsManager {
public:
    struct {
        bool breaking;
        int button;
        sf::Vector2f pos;
    } breaking_h;

    std::vector<env::Chunk*> chunks_around;
    std::vector<Entity*> entities;
    Player player;
    sf::Vector2f presenting_pos_offset;
    sf::Vector2f window_center;

    ElementsManager();
    ~ElementsManager();
    void chunk_gen();
    void chunk_loading();
    void render_all(sf::RenderWindow, float);
};

sf::Font FONTE;

int main()
{
    FONTE.loadFromFile("arial.ttf");
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pong");

    window.setFramerateLimit(45);


    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }



        ball.update_pos(paddles);

        // draw the objects
        window.clear();



        window.display();

    }

    return 0;
}