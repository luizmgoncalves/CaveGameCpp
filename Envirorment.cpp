#include <SFML/Graphics.hpp>

#include <vector>

#include "Perlin.h"

#include "Envirorment.h" 

namespace env {
    std::vector<sf::Texture>* Textures::textures;

    Textures::Textures() {
        textures = new std::vector<sf::Texture>(LAST);

        textures->at(LEAVE).loadFromFile("game_images/leave.png");
        textures->at(WOOD).loadFromFile("game_images/wood.png");

        //COLLIDABLE

        textures->at(GRASS).loadFromFile("game_images/grass.png");
        textures->at(DIRT).loadFromFile("game_images/dirt.png");
        textures->at(STONE).loadFromFile("game_images/stone.png");
    }



    Block::Block(Chunk* chunk, int type, sf::Vector2f pos, int line, int col, int layer) {
        this->c = col;
        this->l = line;
        this->layer = layer;

        this->pos = pos;

        if (type) {
            this->block.setTexture(Textures::textures->at(type), false);

            this->block.setScale(sf::Vector2f(BLOCK_X / (float)Textures::textures->at(type).getSize().x, BLOCK_Y / (float)Textures::textures->at(type).getSize().y));
        }

        this->block.setPosition(pos);

        this->type = (Textures::block_types)type;

        this->owner = chunk;
    }

    void Block::change_type(int type) {
        this->type = (Textures::block_types)type;
        this->owner->v_blocks->blocks[this->l][this->c][this->layer] = (Textures::block_types)type;
    }

    void Block::hit() {
        //pass
    }


    std::unordered_map<size_t, struct v_blocks*> EnvirormentGenerator::all_blocks = {};

    Perlin* EnvirormentGenerator::noise = 0;


    EnvirormentGenerator::EnvirormentGenerator() {
        noise = new Perlin(1, 3, 1.f, 0);

    }

    void EnvirormentGenerator::gen_tree(int line, int col, struct v_blocks*) {
        //pass
    }

    struct v_blocks* EnvirormentGenerator::gen(sf::Vector2i pos_a) {
        Point pos;
        pos.point.x = pos_a.x;
        pos.point.y = pos_a.y;

        if (EnvirormentGenerator::all_blocks.count(pos.hashable)) { // check if these blocks already exists
            return EnvirormentGenerator::all_blocks.at(pos.hashable);
        }

        struct v_blocks* new_blocks = new struct v_blocks;

        for (int i = 0; i < CHUNK_DIM_LINES; i++) {
            for (int j = 0; j < CHUNK_DIM_COLUMNS; j++) {
                int type = Textures::AIR;

                float y = pos_a.y * CHUNK_DIM_LINES  + i ,
                    x = pos_a.x * CHUNK_DIM_COLUMNS  + j;

                float y_i = EnvirormentGenerator::noise->Get(x/100.f, 0.f) * 100.f + 3; //0 a 100

                float index = abs(EnvirormentGenerator::noise->Get(x/100.f, y/100.f)); //0 a 1

                std::cout << "y_i: " << (int)y_i  << " y: " << y << std::endl;

                if (y == (int)y_i) {
                    type = Textures::GRASS;
                }
                else if (y > y_i) {
                    if (index < 0.30) {
                        type = Textures::DIRT;
                    }
                    else {
                        type = Textures::STONE;
                    }
                }

                for (int layer = 0; layer < 2; layer++) {
                    new_blocks->blocks[i][j][layer] = (Textures::block_types)type;
                }
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
                    delete this->blocks[i * CHUNK_DIM_COLUMNS * 2 + j * 2 + layer];
                }
            }
        }
    }

    void Chunk::virtual_to_real_blocks() {
        for (int i = 0; i < CHUNK_DIM_LINES; i++) {
            for (int j = 0; j < CHUNK_DIM_COLUMNS; j++) {
                for (int layer = 0; layer < 2; layer++) {
                    sf::Vector2f upos = this->pos + sf::Vector2f(j * BLOCK_X, i * BLOCK_Y);

                    this->blocks[i * CHUNK_DIM_COLUMNS * 2 + j * 2 + layer] = new Block(this, this->v_blocks->blocks[i][j][layer], upos, i, j, layer);
                }
            }
        }
    }


}