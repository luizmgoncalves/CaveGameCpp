#include <SFML/Graphics.hpp>
#include <iostream>
#include<cstdlib>
#include <list>
#include <unordered_map>

#include "Envirorment.h"

#include "Entity.h"


using namespace std;

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800

sf::Font FONTE;


class Inventory {
public:
    unordered_map<int, int> items;
    sf::Sprite arrow, item;
    sf::Texture arrowt;
    int curr_item = 1;

    Inventory() {
        for (int i = 1; i < env::Textures::LAST; i++) {
            items[i] = 0;
        }

        this->arrowt.loadFromFile("game_images/arrow.png");

        this->arrow = sf::Sprite();

        this->arrow.setPosition( sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.f + sf::Vector2f( - (int)this->arrowt.getSize().x / 2.f, 300 ));

        this->arrow.setTexture(this->arrowt);

        this->item.setTexture(env::Textures::textures->at(curr_item), false);
        this->item.setPosition(this->arrow.getPosition() + sf::Vector2f(this->arrowt.getSize().x, 0));
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

    std::vector < std::vector<env::Chunk*> > chunks_around;
    std::vector<Entity*> entities;
    Player * player;
    sf::Vector2f presenting_pos_offset;
    sf::Vector2f window_center;

    sf::Vector2i current_u_pos;

    Inventory inventory;

    ElementsManager() {
        this->player = new Player();

        this->chunk_gen();

    }
    //~ElementsManager();
    void chunk_gen() {
        this->chunks_around = vector < vector<env::Chunk*> >(3);
        for (int i = 0; i < 3; i++) {
            this->chunks_around[i].resize(3); //three columns by three lines
            for (int j = 0; j < 3; j++) {
                sf::Vector2i offset = {i-1, j-1};

                chunks_around[i][j] = new env::Chunk(current_u_pos + offset);
            }
            
        }
    }

    //void chunk_loading();

    void render_all(sf::RenderWindow * window, float interval) {
        this->player->update_vel(interval);

        for (vector<env::Chunk*> chunks_line : this->chunks_around) {
            for (env::Chunk* chunk : chunks_line) {
                for (env::Block* block : chunk->blocks) {
                    if(block->type)
                        window->draw(block->block);
                }
            }
        }

        window->draw(this->player->shape);
    }
};

int main()
{

    env::Textures();
    env::EnvirormentGenerator();

    FONTE.loadFromFile("arial.ttf");
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Cave Game");

    window.setFramerateLimit(45);


    sf::Clock clock;
    sf::Text fps;
    fps.setFont(FONTE);
    fps.setCharacterSize(20);


    ElementsManager el_mngr = ElementsManager();

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        fps.setString("fps: " + to_string(1.f / elapsed.asSeconds()));


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }



        //ball.update_pos(paddles);

        // draw the objects
        window.clear();

        window.draw(fps);

        el_mngr.render_all(&window, elapsed.asSeconds());
        window.display();

    }

    return 0;
}