#include <SFML/Graphics.hpp>
#include <iostream>
#include<cstdlib>
#include <list>
#include <unordered_map>

#include "Envirorment.h"

#include "Entity.h"


using namespace std;

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 1500

#define WINDOW_DIM SCREEN_WIDTH, SCREEN_HEIGHT

sf::Font FONTE;

float distance(sf::Vector2f pos1, sf::Vector2f pos2) {
    float dx = pos1.x - pos2.x, dy = pos1.y - pos2.y;
    return hypot(dx, dy);
}


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

    void scrool_up() {
        this->curr_item++;
        if (curr_item >= env::Textures::LAST) {
            curr_item = 1;
        }
        this->change_item();
    }

    void scrool_down() {
        this->curr_item--;
        if (curr_item <= 0) {
            curr_item = env::Textures::LAST - 1;
        }
        this->change_item();
    }

    void add(env::Textures::block_types type) {
        this->items[type]++;
    }

    bool rem() {
        if (this->items[curr_item]) {
            this->items[curr_item]--;
            return true;
        }
        return false;
    }

    void change_item() {
        this->item.setTexture(env::Textures::textures->at(curr_item), false);
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

    sf::RectangleShape shower;

    std::vector < std::vector<env::Chunk*> > chunks_around;
    std::vector<DropItem*> entities;
    Player* player;
    sf::Vector2f presenting_pos_offset;
    sf::Vector2f window_center;

    sf::Vector2i current_u_pos;

    Inventory inventory;

    ElementsManager() {
        this->player = new Player();

        this->player->pos = sf::Vector2f(CHUNK_DIM_COLUMNS * BLOCK_X / 2.f, CHUNK_DIM_LINES * BLOCK_Y / 2.f);

        this->chunk_gen();

    }
    //~ElementsManager();

    vector<sf::FloatRect>* get_five_around(sf::Vector2f pos) {
        pos -= sf::Vector2f(BLOCK_X, BLOCK_Y) * 1.f;

        int x, y, xc, yc, lin, col;

        x = pos.x - (int)pos.x % BLOCK_X;
        if (pos.x < 0)
            x -= BLOCK_X;

        y = pos.y - (int)pos.y % BLOCK_Y;
        if (pos.y < 0)
            y -= BLOCK_Y;


        xc = x / (CHUNK_DIM_COLUMNS * BLOCK_X);
        if (pos.x < 0 && x % (CHUNK_DIM_COLUMNS * BLOCK_X))
            xc -= 1;
        col = (x - xc * (CHUNK_DIM_COLUMNS * BLOCK_X)) / (float)BLOCK_X;


        yc = y / (CHUNK_DIM_LINES * BLOCK_Y);
        if (y < 0 && y % (CHUNK_DIM_LINES * BLOCK_Y))
            yc -= 1;
        lin = (y - yc * (CHUNK_DIM_LINES * BLOCK_Y)) / (float)BLOCK_Y;

        int colc = xc - this->chunks_around[0][0]->universal_pos.x,
            linc = yc - this->chunks_around[0][0]->universal_pos.y;


        int loffset, coloffset, l = lin, c;

        vector<sf::FloatRect>* collide_blocks = new vector<sf::FloatRect>();

        for (int i = 0; i < 7; i++) {
            c = col;
            for (int j = 0; j < 7; j++) {
                loffset = l / CHUNK_DIM_LINES;
                coloffset = c / CHUNK_DIM_COLUMNS;

                if (!(abs(linc + loffset) > 2 || abs(colc + coloffset) > 2 || linc + loffset < 0 || colc + coloffset < 0)) {

                    env::Block* block = this->chunks_around[linc + loffset][colc + coloffset]->blocks[(l % CHUNK_DIM_LINES)][(c % CHUNK_DIM_COLUMNS)][1];

                    //block->change_type(env::Textures::WOOD);

                    if (block->type >= env::Textures::GRASS) {
                        collide_blocks->push_back(sf::FloatRect(block->pos, sf::Vector2f(BLOCK_DIM)));
                    }


                }

                c++;
            }
            l++;
        }

        return collide_blocks;

    }

    bool break_block(sf::Vector2f pos) {
        int x, y, xc, yc, lin, col;

        x = pos.x - (int)pos.x % BLOCK_X;
        if (pos.x < 0)
            x -= BLOCK_X;

        y = pos.y - (int)pos.y % BLOCK_Y;
        if (pos.y < 0)
            y -= BLOCK_Y;


        xc = x / (CHUNK_DIM_COLUMNS * BLOCK_X);
        if (pos.x < 0 && x % (CHUNK_DIM_COLUMNS * BLOCK_X))
            xc -= 1;
        col = (x - xc * (CHUNK_DIM_COLUMNS * BLOCK_X)) / (float)BLOCK_X;


        yc = y / (CHUNK_DIM_LINES * BLOCK_Y);
        if (y < 0 && y % (CHUNK_DIM_LINES * BLOCK_Y))
            yc -= 1;
        lin = (y - yc * (CHUNK_DIM_LINES * BLOCK_Y)) / (float)BLOCK_Y;

        int colc = xc - this->chunks_around[0][0]->universal_pos.x,
            linc = yc - this->chunks_around[0][0]->universal_pos.y;

        if (abs(linc) > 2 || abs(colc) > 2)
            return 0;

        env::Block* block = this->chunks_around[linc][colc]->blocks[lin][col][1];

        if (block->type) {
            this->entities.push_back(new DropItem(block->type, block->pos));

            block->change_type(env::Textures::AIR);
            return 0;
        }

        block = this->chunks_around[linc][colc]->blocks[lin][col][0];

        this->entities.push_back(new DropItem(block->type, block->pos));

        block->change_type(env::Textures::AIR);


        return 0;

    }

    void put_block(sf::Vector2f pos) {
        int x, y, xc, yc, lin, col;

        x = pos.x - (int)pos.x % BLOCK_X;
        if (pos.x < 0)
            x -= BLOCK_X;

        y = pos.y - (int)pos.y % BLOCK_Y;
        if (pos.y < 0)
            y -= BLOCK_Y;


        xc = x / (CHUNK_DIM_COLUMNS * BLOCK_X);
        if (pos.x < 0 && x % (CHUNK_DIM_COLUMNS * BLOCK_X))
            xc -= 1;
        col = (x - xc * (CHUNK_DIM_COLUMNS * BLOCK_X)) / (float)BLOCK_X;


        yc = y / (CHUNK_DIM_LINES * BLOCK_Y);
        if (y < 0 && y % (CHUNK_DIM_LINES * BLOCK_Y))
            yc -= 1;
        lin = (y - yc * (CHUNK_DIM_LINES * BLOCK_Y)) / (float)BLOCK_Y;

        int colc = xc - this->chunks_around[0][0]->universal_pos.x,
            linc = yc - this->chunks_around[0][0]->universal_pos.y;

        if (abs(linc) > 2 || abs(colc) > 2)
            return ;

        env::Block 
            * block = this->chunks_around[linc][colc]->blocks[lin][col][1],
            * block2 = this->chunks_around[linc][colc]->blocks[lin][col][0];

        if (block->type) {
            return;
        }

        if (block2->type){
            if(this->inventory.rem())
                block->change_type(this->inventory.curr_item);
            return;
        }

        if (this->inventory.rem())
            block2->change_type(this->inventory.curr_item);



    }

    void chunk_gen() {
        this->chunks_around = vector < vector<env::Chunk*> >(3);
        for (int i = 0; i < 3; i++) {
            this->chunks_around[i].resize(3); //three columns by three lines
            for (int j = 0; j < 3; j++) {
                sf::Vector2i offset = { j - 1, i - 1 };

                chunks_around[i][j] = new env::Chunk(current_u_pos + offset);
            }

        }
    }

    void chunk_loading(){
        float limitx_l = this->chunks_around[1][1]->pos.x, limitx_r = this->chunks_around[1][1]->pos.x + this->chunks_around[1][1]->dim.x;

        float limity_u = this->chunks_around[1][1]->pos.y, limity_d = this->chunks_around[1][1]->pos.y + this->chunks_around[1][1]->dim.y;

        if (this->player->pos.x < limitx_l) {
            for (int line = 0; line < 3; line++) {
                delete chunks_around[line][2];

                chunks_around[line][2] = chunks_around[line][1];
                chunks_around[line][1] = chunks_around[line][0];
                chunks_around[line][0] = new env::Chunk(chunks_around[line][0]->universal_pos + sf::Vector2i(-1, 0));
            }
        }
        else if (this->player->pos.x > limitx_r) {
            for (int line = 0; line < 3; line++) {
                delete chunks_around[line][0];

                chunks_around[line][0] = chunks_around[line][1];
                chunks_around[line][1] = chunks_around[line][2];
                chunks_around[line][2] = new env::Chunk(chunks_around[line][2]->universal_pos + sf::Vector2i(1, 0));
            }
        }

        /*
        0 0  0 1  0 2
        1 0  1 1  1 2
        2 0  2 1  2 2
        */

        if (this->player->pos.y < limity_u) {
            for (int column = 0; column < 3; column++) {
                delete chunks_around[2][column];

                chunks_around[2][column] = chunks_around[1][column];
                chunks_around[1][column] = chunks_around[0][column];
                chunks_around[0][column] = new env::Chunk(chunks_around[0][column]->universal_pos + sf::Vector2i(0, -1));
            }
        }
        else if (this->player->pos.y > limity_d) {
            for (int column = 0; column < 3; column++) {
                delete chunks_around[0][column];

                chunks_around[0][column] = chunks_around[1][column];
                chunks_around[1][column] = chunks_around[2][column];
                chunks_around[2][column] = new env::Chunk(chunks_around[2][column]->universal_pos + sf::Vector2i(0, 1));
            }
        }
    }

    void render_all(sf::RenderWindow * window, float interval) {
        this->chunk_loading();

        this->player->update_vel(interval);
        this->player->update_state(interval);

        for (DropItem* entity : this->entities) {
            entity->update_vel(interval);
            entity->atract(player->pos);
        }

        this->presenting_pos_offset = this->player->pos - sf::Vector2f(WINDOW_DIM) / 2.f;

        this->player->collision_algorithm(this->get_five_around(this->player->pos), interval);

        for (int i = 0; i < this->entities.size(); i++) {
            DropItem* entity = this->entities[i];

            entity->collision_algorithm(this->get_five_around(entity->pos), interval);

            if (distance(entity->pos, this->player->pos) < 40) {
                this->entities.erase(this->entities.begin() + i);
                this->inventory.add(entity->type);
                i--;
            }
        }

        for (vector<env::Chunk*> chunks_line : this->chunks_around) {
            for (env::Chunk* chunk : chunks_line) {
                for (int l = 0; l < CHUNK_DIM_LINES; l++) {
                    for (int c = 0; c < CHUNK_DIM_COLUMNS; c++) {
                        env::Block
                            * b1 = chunk->blocks[l][c][1], 
                            * b2 = chunk->blocks[l][c][0];

                        if (!b1->type) { 
                            // it render background blocks only if the surface block does not exists
                            if (!b2->type)
                                continue;

                            b2->block.setPosition(b2->pos - presenting_pos_offset);
                            window->draw(b2->block);
                            continue;
                        }
                        

                        b1->block.setPosition(b1->pos - presenting_pos_offset);
                        window->draw(b1->block);
                    }
                }
            }
        }

        this->player->shape.setPosition(this->player->pos - presenting_pos_offset);
        window->draw(this->player->shape);

        for (DropItem* entity : this->entities) {
            entity->shape.setPosition(entity->pos - presenting_pos_offset);
            window->draw(entity->shape);
        }

        this->inventory.draw(window);
    }
};

int main()
{

    env::Textures();
    env::EnvirormentGenerator();

    FONTE.loadFromFile("arial.ttf");
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Cave Game");

    window.setFramerateLimit(30);


    sf::Clock clock;
    sf::Text fps;
    fps.setFont(FONTE);
    fps.setCharacterSize(20);

    sf::Texture BG_IMAGET;

    sf::Sprite BG_IMAGE;

    BG_IMAGET.loadFromFile("game_images/bg.jpg");
    BG_IMAGE.setPosition({ 0, 0 });
    BG_IMAGE.setTexture(BG_IMAGET, true);


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

            if (event.type == sf::Event::MouseWheelMoved) {
                if (event.mouseWheel.delta > 0)
                    el_mngr.inventory.scrool_up();
                else
                    el_mngr.inventory.scrool_down();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                float 
                    wx = event.mouseButton.x / (float)window.getSize().x * SCREEN_WIDTH, 
                    wy = event.mouseButton.y / (float)window.getSize().y * SCREEN_HEIGHT;

                if(event.mouseButton.button == 0)
                    el_mngr.break_block(sf::Vector2f(wx, wy) + el_mngr.presenting_pos_offset);
                else if (event.mouseButton.button == 1)
                    el_mngr.put_block(sf::Vector2f(wx, wy) + el_mngr.presenting_pos_offset);

            }
        }



        //ball.update_pos(paddles);

        // draw the objects
        window.clear();

        window.draw(BG_IMAGE);

        el_mngr.render_all(&window, elapsed.asSeconds());

        window.draw(fps);
        window.display();

    }

    return 0;
}