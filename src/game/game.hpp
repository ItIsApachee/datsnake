#ifndef GAME_GAME_HPP_
#define GAME_GAME_HPP_

#include <memory>
#include <chrono>
#include <atomic>
#include "api/client.hpp"
#include "strategy/strategy.hpp"

namespace game {

class GameManager {
 public:
  GameManager(std::unique_ptr<api::Client> client,
              std::unique_ptr<strategy::Strategy> strategy);

  // Run the main game loop
  void run();

  // Stop the game loop
  void stop();

 private:
  std::unique_ptr<api::Client> client_;
  std::unique_ptr<strategy::Strategy> strategy_;
  std::atomic<bool> running_;

  // Process one game tick
  void tick();

  // Calculate sleep duration until next tick
  std::chrono::milliseconds calculate_sleep_duration(int tick_remain_ms);
};

}  // namespace game

#endif  // GAME_GAME_HPP_
