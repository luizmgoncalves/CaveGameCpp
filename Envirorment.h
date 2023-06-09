#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <unordered_map>

#include "Perlin.h"

#define ENV_INITALIZED

namespace env {
#define CHUNK_DIM_LINES 20
#define CHUNK_DIM_COLUMNS 38

#define BLOCK_X 20
#define BLOCK_Y 20
#define BLOCK_DIM BLOCK_X, BLOCK_Y

    class Textures {
    public:
        static enum block_types { // update textures vector lenght 
            AIR,
            LEAVE,
            WOOD,
            GRASS, // collidable
            DIRT,
            STONE,
            LAST
        };
        static std::vector<sf::Texture>* textures;
        static std::vector<sf::Texture>* textures_b;
        Textures();
    };

    union Point {
        size_t hashable;
        struct {
            int x, y;
        } point;
    };

    struct v_blocks {
        Point pos;
        bool tottaly_generated;
        Textures::block_types blocks[CHUNK_DIM_LINES][CHUNK_DIM_COLUMNS][2];
    };

    class Chunk;

    class Block {
    public:
        Textures::block_types type;
        int c, l, layer;

        float integrity;

        Chunk* owner;

        sf::Sprite block;
        sf::Vector2f pos;
        Block(Chunk*, int, sf::Vector2f, int, int, int);
        void change_type(int);
        void hit();
    };

    class EnvirormentGenerator {
    public:
        static std::unordered_map<size_t, struct v_blocks*> all_blocks;

         static Perlin *  noise;

        EnvirormentGenerator();

        static void gen_tree(int, int, struct v_blocks*);

        static struct v_blocks* gen(sf::Vector2i);
    };

    class Chunk {
    public:
        struct v_blocks* v_blocks;
        Block* blocks[CHUNK_DIM_LINES][CHUNK_DIM_COLUMNS][2];
        sf::Vector2i universal_pos;
        sf::Vector2f pos;
        sf::Vector2f dim;
        Chunk(sf::Vector2i);
        ~Chunk();
        void virtual_to_real_blocks();
    };
}

