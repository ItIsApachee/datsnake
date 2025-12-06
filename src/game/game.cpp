#include "game/game.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "models/commands.hpp"

namespace game {

GameManager::GameManager(
    std::unique_ptr<api::Client> client, std::unique_ptr<strategy::Strategy> strategy)
    : client_(std::move(client)), strategy_(std::move(strategy)), running_(false) {}

void GameManager::run() {
  running_ = true;
  std::cout << "Game loop started" << std::endl;

  while (running_) {
    tick();
  }

  std::cout << "Game loop stopped" << std::endl;
}

void GameManager::stop() {
  running_ = false;
}

void GameManager::tick() {
  // First, get current game state with empty command
  PlayerCommand empty_cmd;
  auto state_opt = client_->move(empty_cmd);

  if (!state_opt.has_value()) {
    std::cerr << "Failed to get game state, retrying in 1 second..."
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
  }

  GameState state = state_opt.value();

  // Log any errors from the server
  for (const auto& error : state.errors) {
    std::cerr << "Server error: " << error << std::endl;
  }

  // Build commands for all alive snakes
  std::vector<SnakeCommand> snake_commands;
  for (const auto& snake : state.snakes) {
    if (snake.is_alive()) {
      Vec3 direction = strategy_->compute_move(snake, state);
      snake_commands.push_back({snake.id, direction});
    }
  }

  // Send move commands if we have any
  if (!snake_commands.empty()) {
    PlayerCommand cmd;
    cmd.snakes = snake_commands;
    auto result = client_->move(cmd);

    if (result.has_value()) {
      std::cout << "Turn: " << result->turn
                << " | Points: " << result->points
                << " | Snakes alive: " << snake_commands.size()
                << std::endl;
    }
  }

  // Sleep until next tick
  auto sleep_duration = calculate_sleep_duration(state.tick_remain_ms);
  std::this_thread::sleep_for(sleep_duration);
}

std::chrono::milliseconds GameManager::calculate_sleep_duration(
    int tick_remain_ms) {
  // Use most of the remaining tick time, leaving some buffer
  int sleep_ms = std::max(tick_remain_ms - 50, 100);
  return std::chrono::milliseconds(sleep_ms);
}

}  // namespace game
