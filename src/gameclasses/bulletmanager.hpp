#pragma once

#include "bullet.hpp"
#include "gun.hpp"
#include "alien.hpp"
#include <list>

using BulletList = std::list<Bullet2D>; // Alias

class Bullet2DManager
{
public:
  // constructor.
  Bullet2DManager() = default;

  // Destructor.
  ~Bullet2DManager() = default;

  // no copy constructor
  Bullet2DManager(Bullet2DManager const & obj) = delete;

  // assignment operator (for next level reinit)
  Bullet2DManager & operator = (Bullet2DManager const & obj)
  {
    if (this == &obj) return *this;
    m_fromAlien = obj.GetBulletsFromAliensList();
    m_fromGun = obj.GetBulletsFromGunList();
    return *this;
  }

  // Getters
  inline BulletList const GetBulletsFromAliensList() const { return m_fromAlien; }
  inline BulletList const GetBulletsFromGunList() const    { return m_fromGun; }
  inline size_t const GetCountOfAlienBullets() const       { return m_fromAlien.size(); }
  inline size_t const GetCountOfGunBullets() const         { return m_fromGun.size(); }

  // Capabilities
  bool CheckAllInterections()
  {
    throw std::runtime_error("Not released Bullet2DManager::CheckAllInterections.");
    return false;
  }

  void BulletsMove(Box2D const & border)
  {
    throw std::runtime_error("Not released Bullet2DManager::BulletsMove.");
  }

  bool NewBullet(Bullet2D const & bullet, EntitiesTypes Type)
  {
    switch (Type)
    {
      case GunType: // gun bullet add
      {
        m_fromGun.push_back(bullet);
        break;
      }
      case AlienType: // alien bullet add
      {
        m_fromAlien.push_back(bullet);
        break;
      }
      default:
      {
        return false; // error undefined type
      }
    }
    return true; // allright
  }

private:
  BulletList m_fromAlien; // not need to return
  BulletList m_fromGun;   // not need to return
};
