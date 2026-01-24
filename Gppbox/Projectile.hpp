#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Entity.hpp"

class Projectile :
    public Entity
{
public:
    int static constexpr MAX_DISTANCE{ 150 };
    Projectile(sf::Vector2f position, sf::Vector2f size, bool facesLeft);
    std::string get_type_name() const override  { return "Projectile"; }

    void update(double deltaTime) override;

    bool check_right_collision() override;
    bool check_left_collision() override;
    bool check_bottom_collision() override;
    void go_left() override;
    void go_right() override;

    int initialCx;
};

#endif
