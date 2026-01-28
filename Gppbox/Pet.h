#pragma once
#include "Entity.hpp"

class Pet :
    public Entity
{
public:
    Pet(sf::Vector2f position, sf::Vector2f size);

    std::string get_type_name() const override;

    void go_up();
    void go_down();
};

