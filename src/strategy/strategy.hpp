#ifndef STRATEGY_STRATEGY_HPP_
#define STRATEGY_STRATEGY_HPP_

#include "models/types.hpp"
#include "models/snake.hpp"
#include "models/game_state.hpp"

namespace strategy {

class Strategy {
 public:
  virtual ~Strategy() = default;
  virtual Vec3 compute_move(const Snake& snake, const GameState& state) = 0;
};

}  // namespace strategy

#endif  // STRATEGY_STRATEGY_HPP_
