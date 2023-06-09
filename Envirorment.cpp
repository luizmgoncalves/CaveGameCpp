#include <SFML/Graphics.hpp>

#include <vector>

#include "Perlin.h"

#include "Envirorment.h" 

namespace env {
    std::vector<sf::Texture>* Textures::textures;
    std::vector<sf::Texture>* Textures::textures_b;

    Textures::Textures() {
        textures = new std::vector<sf::Texture>(LAST);
        textures_b = new std::vector<sf::Texture>(LAST);


        textures->at(LEAVE).loadFromFile("game_images/leave.png");
        textures->at(WOOD).loadFromFile("game_images/wood.png");
        //COLLIDABLE
        textures->at(GRASS).loadFromFile("game_images/grass.png");
        textures->at(DIRT).loadFromFile("game_images/dirt.png");
        textures->at(STONE).loadFromFile("game_images/stone.png");


        //BACKGROUND TEXTURES
        // |
        // V
        textures_b->at(LEAVE).loadFromFile("game_images/leave_b.png");
        textures_b->at(WOOD).loadFromFile("game_images/wood_b.png");
        textures_b->at(GRASS).loadFromFile("game_images/grass_b.png");
        textures_b->at(DIRT).loadFromFile("game_images/dirt_b.png");
        textures_b->at(STONE).loadFromFile("game_images/stone_b.png");
    }



    Block::Block(Chunk* chunk, int type, sf::Vector2f pos, int line, int col, int layer) {
        this->c = col;
        this->l = line;
        this->layer = layer;

        this->pos = pos;

        if (type) {
            if (this->layer)
                this->block.setTexture(Textures::textures->at(type), false);
            else
                this->block.setTexture(Textures::textures_b->at(type), false);

            this->block.setScale(sf::Vector2f(BLOCK_X / (float)Textures::textures->at(type).getSize().x, BLOCK_Y / (float)Textures::textures->at(type).getSize().y));
        }

        this->block.setPosition(pos);

        this->type = (Textures::block_types)type;

        this->owner = chunk;
    }

    void Block::change_type(int type) {
        this->type = (Textures::block_types)type;
        this->owner->v_blocks->blocks[this->l][this->c][this->layer] = (Textures::block_types)type;

        if (type) {
            if(this->layer)
                this->block.setTexture(Textures::textures->at(type), false);
            else
                this->block.setTexture(Textures::textures_b->at(type), false);

            this->block.setScale(sf::Vector2f(BLOCK_X / (float)Textures::textures->at(type).getSize().x, BLOCK_Y / (float)Textures::textures->at(type).getSize().y));
        }
    }

    void Block::hit() {
        //pass
    }


    std::unordered_map<size_t, struct v_blocks*> EnvirormentGenerator::all_blocks = {};

    Perlin* EnvirormentGenerator::noise = 0;


    EnvirormentGenerator::EnvirormentGenerator() {
        noise = new Perlin(1, 3, 1.f, 0);

    }

    void EnvirormentGenerator::gen_tree(int line, int col, struct v_blocks* blocks) {
        int tree_form[5][5] = {
            0, 1, 1, 1, 0,
            1, 1, 1, 1, 1,
            1, 1, 2, 1, 1,
            0, 0, 2, 0, 0,
            0, 0, 2, 0, 0,
        };

        int height = rand() % 5;

        int l, c;

        if (line - 5 - height < 0)
            return;
        if (col - 2 < 0 || col + 2 >= CHUNK_DIM_COLUMNS)
            return;

        
        for (int i = 0; i < 5 + height; i++) {
            for (int j = 0; j < 5; j++) {
                l = line - 5 - height + i;
                c = col - 2 + j;

                if (i >= 5) {
                    if(c == col)
                        blocks->blocks[l][c][1] = Textures::WOOD;

                }else if (tree_form[i][j]) {
                    blocks->blocks[l][c][1] = (Textures::block_types) tree_form[i][j];
                }
                
                    
            }
        }


        //pass
    }

    struct v_blocks* EnvirormentGenerator::gen(sf::Vector2i pos_a) {
        Point pos;
        pos.point.x = pos_a.x;
        pos.point.y = pos_a.y;

        if (EnvirormentGenerator::all_blocks.count(pos.hashable) && EnvirormentGenerator::all_blocks.at(pos.hashable)->tottaly_generated) { 
            // check if these blocks already exists and if it is tottaly generated
            return EnvirormentGenerator::all_blocks.at(pos.hashable);
        }

        struct v_blocks* new_blocks = new struct v_blocks;

        new_blocks->pos = pos;

        for (int i = 0; i < CHUNK_DIM_LINES; i++) {
            for (int j = 0; j < CHUNK_DIM_COLUMNS; j++) {
                int type1 = Textures::AIR;
                

                float y = pos_a.y * CHUNK_DIM_LINES  + i ,
                    x = pos_a.x * CHUNK_DIM_COLUMNS  + j;

                float y_i = EnvirormentGenerator::noise->Get(x/300.f, 0.f) * 100.f + 3; //0 a 100

                float index = abs(EnvirormentGenerator::noise->Get(x/100.f, y/100.f)); //0 a 1

                index += (rand() % 100)/100.f / 50.f;

                int type2 = y >= (int)y_i ? Textures::DIRT : Textures::AIR;

                

                if (y == (int)y_i) {
                    bool tree_chance = rand() % 100 > 90; // chance of a tree spawn

                    if (tree_chance)
                        EnvirormentGenerator::gen_tree(i , j, new_blocks);

                    if(index > 0.02) 
                        type1 = Textures::GRASS;

                }
                else if (y > y_i) {
                    if (y - y_i < 10) {
                        if (index < 0.04) {
                            type1 = Textures::AIR;
                            type2 = index < 0.003 ? Textures::AIR : Textures::DIRT;
                        }
                        else if (index < 0.5) {
                            type1 = Textures::DIRT;
                        }
                        else {
                            type1 = Textures::STONE;
                            type2 = Textures::STONE;
                        }
                    }
                    else {
                        if (index < 0.04) {
                            type1 = Textures::AIR;
                            type2 = index < 0.001 ? Textures::AIR : Textures::DIRT;
                        }
                        else if (index < 0.35) {
                            type1 = Textures::DIRT;
                        }
                        else {
                            type1 = Textures::STONE;
                            type2 = Textures::STONE;
                        }
                    }
                }

                new_blocks->blocks[i][j][0] = (Textures::block_types)type2;
                new_blocks->blocks[i][j][1] = (Textures::block_types)type1; // surface
                
            }
        }

        EnvirormentGenerator::all_blocks[pos.hashable] = new_blocks;
        // adding generated v_blocks to all map

        new_blocks->tottaly_generated = true;


        return new_blocks;
    }

    Chunk::Chunk(sf::Vector2i pos) {
        
        this->universal_pos = pos;

        this->dim = { CHUNK_DIM_COLUMNS * BLOCK_X, CHUNK_DIM_LINES * BLOCK_Y };

        this->pos = { this->universal_pos.x * this->dim.x, this->universal_pos.y * this->dim.y };
        

        this->v_blocks = EnvirormentGenerator::gen(pos);
        
        this->virtual_to_real_blocks();
        
    }

    Chunk::~Chunk() {
        for (int i = 0; i < CHUNK_DIM_LINES; i++) {
            for (int j = 0; j < CHUNK_DIM_COLUMNS; j++) {
                for (int layer = 0; layer < 2; layer++) {
                    delete this->blocks[i][j][layer];
                }
            }
        }
    }

    void Chunk::virtual_to_real_blocks() {
        for (int i = 0; i < CHUNK_DIM_LINES; i++) {
            for (int j = 0; j < CHUNK_DIM_COLUMNS; j++) {
                for (int layer = 0; layer < 2; layer++) {
                    sf::Vector2f upos = this->pos + sf::Vector2f(j * BLOCK_X, i * BLOCK_Y);

                    this->blocks[i][j][layer] = new Block(this, this->v_blocks->blocks[i][j][layer], upos, i, j, layer);
                }
            }
        }
    }


}