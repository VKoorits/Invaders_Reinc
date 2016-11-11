#pragma once

#include "Obstacle.hpp"
#include "bullet.hpp"
#include <vector>
using ObstacleVector = std::vector<Obstacle2D*>;     // Alias
class Obstacle2DManager
{
public:
  // constructor.
  Obstacle2DManager()
  {
    CreateObstacleVector(4); // initialize default matrix
  }

  // Destructor.
  ~Obstacle2DManager() = default;

  // Constructors with parameters.
  Obstacle2DManager(size_t const countObstacle)
  {
    CreateObstacleVector(countObstacle);
  }

  // no copy constructor
  Obstacle2DManager(Obstacle2DManager const & obj) = delete;

  // assignment operator (for next level reinit)
  Obstacle2DManager & operator = (Obstacle2DManager const & obj)
  {
    if (this == &obj) return *this;
    m_obstacles = obj.GetObstacleVector();
    return *this;
  }

  // Getters
  inline ObstacleVector const & GetObstacleVector() const { return m_obstacles; }
  inline ObstacleVector & GetObstacleVector() { return m_obstacles; }
  inline size_t const GetCountOObstacle() const    { return m_obstacles.size(); }


  // Capabilities
  bool CheckIntersection(Bullet2D& bul)
  {
    for(auto it = m_obstacles.begin(); it != m_obstacles.end(); ++it)
      if((*it)->CheckIntersection(bul))
        break;
  }




private:
  void CreateObstacleVector(size_t const countObstacle)
  {
    m_obstacles.clear();
    m_obstacles.reserve(countObstacle);
    for (size_t i = 0; i < countObstacle; ++i)
    {
      m_obstacles.push_back
        (
          new Obstacle2D
          (
            Point2D
            {
              OBSTACLE_LEFT_LIMIT + (i+1) * OBSTACLE_DISTANCE + i * OBSTACLE_WIDTH
              , OBSTACLE_BOTTOM_LIMIT
            },
            Point2D
            {
              OBSTACLE_LEFT_LIMIT + (i+1) * ( OBSTACLE_DISTANCE + OBSTACLE_WIDTH )
              , OBSTACLE_BOTTOM_LIMIT + OBSTACLE_HEIGHT
            }
          )
        );
    }
  }

  ObstacleVector m_obstacles;          // matrix of Obstacles
};
