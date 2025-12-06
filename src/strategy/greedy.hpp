#ifndef STRATEGY_GREEDY_HPP_
#define STRATEGY_GREEDY_HPP_

#include "strategy.hpp"
#include <vector>
#include <optional>

namespace strategy {

class GreedyStrategy : public Strategy {
 public:
  Vec3 compute_move(const Snake& snake, const GameState& state) override;

 private:
  Vec3 get_direction(const Vec3& head, const Vec3& target) const;
  int manhattan_distance(const Vec3& a, const Vec3& b) const;
  bool is_valid_move(const Vec3& head, const Vec3& direction,
                     const GameState& state, const Snake& snake) const;
  bool is_collision(const Vec3& pos, const GameState& state,
                    const Snake& snake) const;
  std::optional<Vec3> find_nearest_food(const Vec3& head,
                                         const GameState& state) const;
};

}  // namespace strategy

#endif  // STRATEGY_GREEDY_HPP_
