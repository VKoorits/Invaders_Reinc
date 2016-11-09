#pragma once

#include "movedgameentity.hpp"

class Alien2D final : public MovedGameEntity2D
{
public:
  // Allow default constructor.
  Alien2D()
  {
    DefaultAlienSetStartValue();
  }
  std::unique_ptr<GameEntity2D> Create
    (Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot)
  {
    return std::unique_ptr<GameEntity2D>(new Alien2D(leftBottom, rightTop, health, speedShoot));
  }
  std::unique_ptr<GameEntity2D> Create
    (Point2D const & leftBottom, Point2D const & rightTop)
  {
    return std::unique_ptr<GameEntity2D>(new Alien2D(leftBottom, rightTop));
  }
  // Destructor.
  ~Alien2D() override = default;

  // Constructors with parameters.
  Alien2D(Point2D const & leftBottom, Point2D const & rightTop)
    :MovedGameEntity2D(leftBottom, rightTop)
  {
    DefaultAlienSetStartValue();
  }

  Alien2D(Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot)
    :MovedGameEntity2D(leftBottom, rightTop, health, speedShoot)
  {}

  // copy constructor and assignment operator
  Alien2D(Alien2D const & obj)
    :MovedGameEntity2D(obj.GetMovedEntity())
  {}

  Alien2D & operator = (Alien2D const & obj)
  {
    if (this == &obj) return *this;
    SetMovedEntity(obj.GetMovedEntity());
    return *this;
  }

  // Capabilities
  void Move() override
  {
    throw std::runtime_error("Not released Alien2D::Move.");
  }

  // Redefinition
  friend std::ostream & operator << (std::ostream & os, Alien2D const & obj)
  {
    os << "ALIEN{"
       << obj.GetBox().leftBottom()
       << ", " << obj.GetBox().rightTop()
       << ", HP: " << obj.GetHealth()
       << ", speed: " << obj.GetSpeed()
       << "}";
    return os;
  }

  inline void DefaultAlienSetStartValue()
  {
    SetHealth(ALIEN_HEALTH_START);
    SetSpeed(ALIEN_SPEED_SHOOT_START);
  }
  inline EntitiesTypes GetEntityType(){ return EntitiesTypes::AlienType; }private:
};
