#include "Entity.h" 

#include <SFML/Graphics.hpp>

#include <unordered_map>

#include <vector>

void Entity::init(int w, int h, sf::Texture* text) {
    this->shape.setTexture(*text, true);


    this->dim = sf::Vector2f( w, h );

    this->shape.setScale(sf::Vector2f( w / (float)text->getSize().x, h / (float)text->getSize().y ));
    this->gravity = 1000;
    def_max_vel();
}


void Entity::update_vel(float interval) {
    this->update_vel_x(interval);
    this->update_vel_y(interval);

    if (abs(this->vel.x) && abs(this->vel.x) * interval > this->MAX_X_VEL) {
        float sign = this->vel.x > 0 ? 1 : -1;
        this->vel.x = this->MAX_X_VEL * sign / interval;
    }

    if (abs(this->vel.y) && abs(this->vel.y) * interval > this->MAX_Y_VEL) {
        float sign = this->vel.y > 0 ? 1 : -1;
        this->vel.y = this->MAX_Y_VEL * sign / interval;
    }
}

void Entity::def_max_vel() {
    this->MAX_X_VEL = BLOCK_X;
    this->MAX_Y_VEL = BLOCK_Y;
}

void Entity::collision_algorithm(std::vector<sf::FloatRect>* blocks, float interval) {
    sf::FloatRect new_rect = sf::FloatRect(this->pos, this->dim);

    new_rect.top += this->vel.y * interval;

    bool up = this->vel.y < 0;

    for (sf::FloatRect block : (*blocks)) {
        if (block.intersects(new_rect)) {
            if (up) {
                new_rect.top = block.top + block.height;

            }
            else {
                new_rect.top = block.top - new_rect.height;
                this->collided_down();
            }
            this->vel.y = 0;
        }
    }

    bool left = this->vel.x < 0;
    new_rect.left += this->vel.x * interval;

    for (sf::FloatRect block : (*blocks)) {
        if (block.intersects(new_rect)) {
            if (left) {
                new_rect.left = block.left + block.width;

            }
            else {
                new_rect.left = block.left - new_rect.width;
            }
            this->vel.x = 0;
        }
    }

    this->pos = { new_rect.left, new_rect.top };

    delete blocks;
}


Player::Player() {

    struct path_sprites {
        std::vector<std::string> textures;
        float time;
    };

    std::unordered_map<std::string, struct path_sprites> image_types = {
        {"stopped", {{"p1", "p2", "p3"}, 0.2f}},
        {"walking", {{"r1", "r2", "r3", "r4", "r5", "r6"}, 0.1f}},
        {"running", {{"rr1", "rr2", "rr3", "rr4", "rr5", "rr6"}, 0.05f}},
        {"jumping", {{"j1", "j2"}, 3.f}}
    };
    for (auto it = image_types.begin(); it != image_types.end(); it++) {
        std::string key = (*it).first; 

        this->textures[key + "r"].textures = new std::vector<sf::Texture *>();//right textures
        this->textures[key + "r"].time = image_types[key].time;

        this->textures[key + "l"].textures = new std::vector<sf::Texture *>(); //left texture
        this->textures[key + "l"].time = image_types[key].time;

        for (std::string path : image_types[key].textures) {
            sf::Image im;
            sf::Texture * texr, * texl;

            texr = new sf::Texture();
            texl = new sf::Texture();

            im.loadFromFile("game_images/" + path + ".png");
            texr->loadFromImage(im);
            this->textures[key + "r"].textures->push_back(texr);

            im.flipHorizontally();
            texl->loadFromImage(im);


            this->textures[key + "l"].textures->push_back(texl);
        }
    }

    this->init(PLAYER_DIM, this->textures.at("stoppedr").textures->at(0));
}


void Player::collided_down() {
    this->current_state.falling = false;
    this->current_state.collided_down = true;

    this->current_state.jumping = false;
}

void Player::update_state(float interval) {
    this->frame_counter += interval;

    if (this->current_state.left) {
        if (this->current_state.jumping) {
            int index = this->frame_counter / this->textures.at("jumpingl").time;

            this->shape.setTexture(*this->textures.at("jumpingl").textures->at(index % this->textures.at("jumpingl").textures->size()), true);
        }
        else if (this->current_state.running) {
            int index = this->frame_counter / this->textures.at("runningl").time;

            this->shape.setTexture(*this->textures.at("runningl").textures->at(index % this->textures.at("runningl").textures->size()), true);
        }
        else if (this->current_state.walking) {
            int index = this->frame_counter / this->textures.at("walkingl").time;

            this->shape.setTexture(*this->textures.at("walkingl").textures->at(index % this->textures.at("walkingl").textures->size()), true);
        }
        else {
            int index = this->frame_counter / this->textures.at("stoppedl").time;

            this->shape.setTexture(*this->textures.at("stoppedl").textures->at(index % this->textures.at("stoppedl").textures->size()), true);
        }
    }
    else {
        if (this->current_state.jumping) {
            int index = this->frame_counter / this->textures.at("jumpingr").time;

            this->shape.setTexture(*this->textures.at("jumpingr").textures->at(index % this->textures.at("jumpingr").textures->size()), true);
        }
        else if (this->current_state.running) {
            int index = this->frame_counter / this->textures.at("runningr").time;

            this->shape.setTexture(*this->textures.at("runningr").textures->at(index % this->textures.at("runningr").textures->size()), true);
        }
        else if (this->current_state.walking) {
            int index = this->frame_counter / this->textures.at("walkingr").time;

            this->shape.setTexture(*this->textures.at("walkingr").textures->at(index % this->textures.at("walkingr").textures->size()), true);
        }
        else {
            int index = this->frame_counter / this->textures.at("stoppedr").time;

            this->shape.setTexture(*this->textures.at("stoppedr").textures->at(index % this->textures.at("stoppedr").textures->size()), true);
        }
    }
}


void Player::update_vel_x(float interval) {

    this->vel.x *= 0.8;
    this->current_state.walking = false;
    this->current_state.running = false;


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->current_state.left = false;
        this->current_state.right = true;
        this->current_state.walking = true;

        this->vel.x = 400;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->current_state.left = true;
        this->current_state.right = false;
        this->current_state.walking = true;

        this->vel.x = -400;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && this->current_state.walking) {
        this->vel.x *= 2.f;
        this->current_state.walking = false;
        this->current_state.running = true;
    }
    
}

void Player::update_vel_y(float interval) {
    this->vel.y += this->gravity * interval;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->vel.y = -400;
        this->current_state.jumping = true;
        this->current_state.running = false;
        this->current_state.walking = false;
    }
}


DropItem::DropItem(env::Textures::block_types type, sf::Vector2f pos) {
    this->pos = pos;

    this->type = type;

    //srand(time(0));

    this->vel.x = rand() % 200 - 100;
    this->vel.y = - rand() % 100;

    this->vel *= 6.f;

    this->init(DROPED_ITEM_DIM, &env::Textures::textures->at(type));
}

void DropItem::atract(sf::Vector2f point) {
    float lenght = hypot(point.x - this->pos.x, point.y - this->pos.y),
        difx = point.x - this->pos.x,
        dify = point.y - this->pos.y;


    sf::Vector2f force = { 1, 1 };

    if (difx) {
        force.x = difx > 0 ? 1 : -1;
    }

    if (dify) {
        force.y = dify > 0 ? 1 : -1;
    }

    force *= lenght * 0.15f;

    this->vel += force;
}

void DropItem::update_vel_x(float interval) {
    this->vel.x *= 0.9 + (rand()%10 / 100.f);
}

void DropItem::update_vel_y(float interval) {
    this->vel.y += this->gravity * interval;
}

void DropItem::collided_down() {

}