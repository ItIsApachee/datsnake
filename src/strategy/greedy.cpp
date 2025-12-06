#include "greedy.hpp"
#include <algorithm>
#include <limits>
#include <cmath>

namespace strategy {

Vec3 GreedyStrategy::compute_move(const Snake& snake, const GameState& state) {
  if (!snake.is_alive() || snake.geometry.empty()) {
    return snake.direction;  // Return current direction if dead or empty
  }

  Vec3 head = snake.get_head();

  // Find nearest food (prioritize golden > normal > suspicious)
  std::optional<Vec3> target = find_nearest_food(head, state);
  if (!target.has_value()) {
    // No food available, continue in current direction if valid
    if (is_valid_move(head, snake.direction, state, snake)) {
      return snake.direction;
    }
    // Try alternative directions
    std::vector<Vec3> directions = {
      Vec3(1, 0, 0), Vec3(-1, 0, 0),
      Vec3(0, 1, 0), Vec3(0, -1, 0),
      Vec3(0, 0, 1), Vec3(0, 0, -1)
    };
    for (const auto& dir : directions) {
      if (snake.is_valid_direction(dir) && is_valid_move(head, dir, state, snake)) {
        return dir;
      }
    }
    return snake.direction;  // Fallback
  }

  // Calculate direction towards target
  Vec3 desired_dir = get_direction(head, target.value());

  // Check if desired direction is valid
  if (snake.is_valid_direction(desired_dir) &&
      is_valid_move(head, desired_dir, state, snake)) {
    return desired_dir;
  }

  // Try alternative directions if desired is invalid
  std::vector<Vec3> alternatives = {
    Vec3(desired_dir.x != 0 ? 0 : 1, desired_dir.y != 0 ? 0 : 1, 0),
    Vec3(desired_dir.x != 0 ? 0 : -1, desired_dir.y != 0 ? 0 : -1, 0),
    Vec3(0, 0, desired_dir.z != 0 ? 0 : 1),
    Vec3(0, 0, desired_dir.z != 0 ? 0 : -1)
  };

  for (const auto& alt : alternatives) {
    if (snake.is_valid_direction(alt) && is_valid_move(head, alt, state, snake)) {
      return alt;
    }
  }

  // Fallback: continue current direction if valid
  if (is_valid_move(head, snake.direction, state, snake)) {
    return snake.direction;
  }

  return snake.direction;
}

Vec3 GreedyStrategy::get_direction(const Vec3& head, const Vec3& target) const {
  Vec3 diff = target - head;
  if (diff.x != 0) {
    return Vec3(diff.x > 0 ? 1 : -1, 0, 0);
  }
  if (diff.y != 0) {
    return Vec3(0, diff.y > 0 ? 1 : -1, 0);
  }
  if (diff.z != 0) {
    return Vec3(0, 0, diff.z > 0 ? 1 : -1);
  }
  return Vec3(0, 0, 0);
}

int GreedyStrategy::manhattan_distance(const Vec3& a, const Vec3& b) const {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

bool GreedyStrategy::is_valid_move(const Vec3& head, const Vec3& direction,
                                    const GameState& state, const Snake& snake) const {
  Vec3 next_pos = head + direction;

  // Check bounds
  if (next_pos.x < 0 || next_pos.x >= state.map_size.x ||
      next_pos.y < 0 || next_pos.y >= state.map_size.y ||
      next_pos.z < 0 || next_pos.z >= state.map_size.z) {
    return false;
  }

  // Check collision
  return !is_collision(next_pos, state, snake);
}

bool GreedyStrategy::is_collision(const Vec3& pos, const GameState& state,
                                   const Snake& snake) const {
  // Check fences
  for (const auto& fence : state.fences) {
    if (pos == fence) {
      return true;
    }
  }

  // Check own snake body (excluding head)
  for (size_t i = 1; i < snake.geometry.size(); ++i) {
    if (pos == snake.geometry[i]) {
      return true;
    }
  }

  // Check enemy snakes
  for (const auto& enemy : state.enemies) {
    if (!enemy.is_alive()) continue;
    for (const auto& segment : enemy.geometry) {
      if (pos == segment) {
        return true;
      }
    }
  }

  return false;
}

std::optional<Vec3> GreedyStrategy::find_nearest_food(const Vec3& head,
                                                       const GameState& state) const {
  std::optional<Vec3> best_target;
  int best_distance = std::numeric_limits<int>::max();

  // Priority 1: Golden food
  for (const auto& golden : state.special_food.golden) {
    int dist = manhattan_distance(head, golden);
    if (dist < best_distance) {
      best_distance = dist;
      best_target = golden;
    }
  }

  // Priority 2: Normal food
  for (const auto& food : state.food) {
    int dist = manhattan_distance(head, food.c);
    if (dist < best_distance) {
      best_distance = dist;
      best_target = food.c;
    }
  }

  // Priority 3: Suspicious food (only if nothing else available)
  if (!best_target.has_value()) {
    for (const auto& suspicious : state.special_food.suspicious) {
      int dist = manhattan_distance(head, suspicious);
      if (dist < best_distance) {
        best_distance = dist;
        best_target = suspicious;
      }
    }
  }

  return best_target;
}

}  // namespace strategy
