#pragma once

#include "movedgameentity.hpp"

class Gun2D final : public MovedGameEntity2D
{
public:
  // Allow default constructor.
  Gun2D()
  {
    DefaultGunSetStartValue();
    logger << "created standart gun" << std::endl;
  }
  virtual std::unique_ptr<Gun2D> Create
    (Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot)
  {
    return std::unique_ptr<Gun2D>(new Gun2D(leftBottom, rightTop, health, speedShoot));
  }
  virtual std::unique_ptr<Gun2D> Create
    (Point2D const & leftBottom, Point2D const & rightTop)
  {
    return std::unique_ptr<Gun2D>(new Gun2D(leftBottom, rightTop));
  }
  virtual std::unique_ptr<Gun2D> Create
    (Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot, size_t lives)
  {
    return std::unique_ptr<Gun2D>(new Gun2D(leftBottom, rightTop, health, speedShoot, lives));
  }
  // Destructor.
  ~Gun2D() override = default;

  // Constructors with parameters.
  Gun2D(Point2D const & leftBottom, Point2D const & rightTop)
    :MovedGameEntity2D(leftBottom, rightTop)
  {
    DefaultGunSetStartValue();
  }

  Gun2D(Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot)
    :MovedGameEntity2D(leftBottom, rightTop, health, speedShoot)
  {}

  Gun2D(Point2D const & leftBottom, Point2D const & rightTop, float health, float speedShoot, size_t lives)
    :MovedGameEntity2D(leftBottom, rightTop, health, speedShoot), m_lives(lives)
  {
    if (lives <= 0) throw std::out_of_range("Lives must be more then ZERO!");
  }

  // copy constructor and assignment operator
  Gun2D(Gun2D const & obj)
    :MovedGameEntity2D(obj.GetMovedEntity()), m_lives(obj.GetLives())
  {}

  Gun2D & operator = (Gun2D const & obj)
  {
    if (this == &obj) return *this;
    SetMovedEntity(obj.GetMovedEntity());
    m_lives = obj.GetLives();
    m_gunRate = obj.GetRate();
    return *this;
  }
  void G(){std::cout << "GUN FROM FACTORY";}

  // Getters
  inline size_t const GetLives() const { return m_lives; }
  inline float const GetRate() const   { return m_gunRate; }
  // Setters
  inline void SetLives(size_t const newLives) { m_lives = newLives; }
  inline void SetRate(float const newGunRate) { m_gunRate = newGunRate; }

  // Capabilities
  void Move() override
  {
    throw std::runtime_error("Not released Gun2D::Move.");
  }  

  // Redefinition
  friend std::ostream & operator << (std::ostream & os, Gun2D const & obj)
  {
    os << "GUN{"
       << obj.GetBox().leftBottom()
       << ", " << obj.GetBox().rightTop()
       << ", HP: " << obj.GetHealth()
       << ", speed: " << obj.GetSpeed()
       << ", lives: " << obj.m_lives
       << "}";
    return os;
  }
  inline EntitiesTypes GetEntityType(){ return EntitiesTypes::GunType; }
private:
  inline void DefaultGunSetStartValue()
  {
    SetHealth(GUN_HEALTH_START);
    SetSpeed(GUN_SPEED_SHOOT_START);
  }
  size_t m_lives = GUN_LIVES_START;  // - default gun lives count
  float m_gunRate = 0;               // - game rate (from original game) (increment depends on the type of shot down alien)
};
