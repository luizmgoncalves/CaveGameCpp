#include "Entity.h" 

#include <SFML/Graphics.hpp>

#include <unordered_map>

#include <vector>

void Entity::init(int w, int h, sf::Texture* text) {
    this->shape.setTexture(*text, true);


    this->dim = sf::Vector2f( w, h );

    this->shape.setScale(sf::Vector2f( w / text->getSize().x, h / text->getSize().y ));
    this->gravity = 1000;
    def_max_vel();
}


void Entity::update_vel(float interval) {
    this->update_vel_x(interval);
    this->update_vel_y(interval);

    if (abs(this->vel.x) && abs(this->vel.x) * interval > this->MAX_X_VEL) {
        float sign = this->vel.x > 0 ? 1 : -1;
        this->vel.x = this->MAX_X_VEL * sign;
    }

    if (abs(this->vel.y) && abs(this->vel.y) * interval > this->MAX_Y_VEL) {
        float sign = this->vel.y > 0 ? 1 : -1;
        this->vel.y = this->MAX_Y_VEL * sign;
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
            }
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
        }
    }

    this->pos = { new_rect.left, new_rect.top };

    delete blocks;
}


Player::Player() {
    std::unordered_map<std::string, std::vector<std::string>> image_types = {
        {"stopped", {"p1", "p2", "p3"}},
        {"walking", {"r1", "r2", "r3", "r4", "r5", "r6"}},
        {"running", {"rr1", "rr2", "rr3", "rr4", "rr5", "rr6"}},
        {"jumping", {"j1", "j2"}}
    };
    for (auto it = image_types.begin(); it != image_types.end(); it++) {
        std::string key = (*it).first;

        this->textures[key + "r"] = new std::vector<sf::Texture *>();//right textures
        this->textures[key + "l"] = new std::vector<sf::Texture *>(); //left texture

        for (std::string path : image_types[key]) {
            sf::Image im;
            sf::Texture * texr, * texl;

            texr = new sf::Texture();
            texl = new sf::Texture();

            im.loadFromFile("game_images/" + path + ".png");
            texr->loadFromImage(im);
            this->textures[key + "r"]->push_back(texr);

            im.flipHorizontally();
            texl->loadFromImage(im);


            this->textures[key + "l"]->push_back(texl);
        }
    }

    this->init(PLAYER_DIM, this->textures.at("stoppedr")->at(0));
}


void Player::collided_down() {
    this->current_state.falling = false;
    this->current_state.collided_down = true;

    this->current_state.jumping = false;
}

void Player::update_state() {
    if (this->current_state.left) {
        if (this->current_state.walking) {
            this->shape.setTexture(*this->textures.at("walkingl")->at(0), true);
        }
        else if (this->current_state.running) {
            this->shape.setTexture(*this->textures.at("runningl")->at(0), true);
        }
        else if (this->current_state.jumping) {
            this->shape.setTexture(*this->textures.at("jumpingl")->at(0), true);
        }
        else {
            this->shape.setTexture(*this->textures.at("stoppedl")->at(0), true);
        }
    }
    else if (this->current_state.right) {
        if (this->current_state.walking) {
            this->shape.setTexture(*this->textures.at("walkingr")->at(0), true);
        }
        else if (this->current_state.running) {
            this->shape.setTexture(*this->textures.at("runningr")->at(0), true);
        }
        else if (this->current_state.jumping) {
            this->shape.setTexture(*this->textures.at("jumpingr")->at(0), true);
        }
        else {
            this->shape.setTexture(*this->textures.at("stoppedr")->at(0), true);
        }
    }
}


void Player::update_vel_x(float interval) {

    this->vel.x *= 0.8;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->current_state.left = true;
        this->current_state.right = false;

        this->vel.x = 1000;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->current_state.left = false;
        this->current_state.right = true;

        this->vel.x = -1000;
    }
}

void Player::update_vel_y(float interval) {
    this->vel.y += this->gravity * interval;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->pos.y = -1000;
    }
}


DropItem::DropItem(env::Textures::block_types type, sf::Vector2f pos) {
    this->pos = pos;
    this->init(DROPED_ITEM_DIM, &env::Textures::textures->at(type));
}

void DropItem::atract(sf::Vector2f point) {
    float lenght = hypot(point.x - this->pos.x, point.y - this->pos.y),
        difx = point.x - this->pos.x,
        dify = point.y - this->pos.y;


    sf::Vector2f force = { 1, 1 };

    force *= 10.f; //factor

    if (difx) {
        force.x = difx > 0 ? 1 : -1;
    }

    if (dify) {
        force.y = dify > 0 ? 1 : -1;
    }

    force *= lenght;

    this->vel += force;
}

void DropItem::update_vel_x(float interval) {
    this->vel.x *= 0.8;
}

void DropItem::update_vel_y(float interval) {
    this->vel.y += this->gravity * interval;
}

void DropItem::collided_down() {

}