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

    static constexpr float SHOOT_RATE_PER_SECONDS{2.0f};
    static constexpr float ENEMY_RANGE{8.0f};

	Entity* scan_for_enemies();
};

