#include <SFML/Graphics.hpp>
#include <vector>

#ifndef ENV_INITALIZED

#include "Envirorment.h"

#endif

class Entity {
public:
    sf::Sprite shape;
    sf::Vector2f pos;
    sf::Vector2f dim;
    sf::Vector2f vel; // pixel/s

    struct {
        bool falling = true, jumping = false, left = false, right = false, walking = false, running = false, collided_down = false;
        unsigned int counter;
    } current_state;

    int w, h;
    float gravity;
    float MAX_X_VEL;
    float MAX_Y_VEL;

    float max_vel;

    void init(int, int, sf::Texture*);

    void set_dim_text(int, int);

    void setPos(int, int);

    void update_vel(float interval);

    virtual void update_vel_x(float interval) = 0;

    virtual void update_vel_y(float interval) = 0;

    void def_max_vel();

    void collision_algorithm(std::vector<sf::FloatRect>* blocks, float interval);

    virtual void collided_down() = 0;
};

#define PLAYER_DIM_X 30

#define PLAYER_DIM_Y 34

#define PLAYER_DIM 25, 30

#define PLAYER_RUNNING_DIM_X 36

#define PLAYER_RUNNING_DIM_Y 30

#define PLAYER_RUNNING_DIM 36, 30

class Player : public Entity {
public:

    struct sprites {
        std::vector<sf::Texture*> * textures;
        float time;
    };

    std::unordered_map<std::string, struct sprites> textures;

    float frame_counter;


    Player();

    void collided_down();

    void update_state(float interval);

    void update_vel_x(float interval);

    void update_vel_y(float interval);

};

#define DROPED_ITEM_X 20

#define DROPED_ITEM_Y 20

#define DROPED_ITEM_DIM DROPED_ITEM_X, DROPED_ITEM_Y

class DropItem : public Entity {
public:
    sf::Vector2f dim_running;
    env::Textures::block_types type;

    struct {
        bool collided_down = false;
    } current_state;

    DropItem(env::Textures::block_types, sf::Vector2f);

    void atract(sf::Vector2f);

    void update_vel_x(float interval);

    void update_vel_y(float interval);

    void collided_down();

};