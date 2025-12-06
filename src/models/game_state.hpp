#ifndef MODELS_GAME_STATE_HPP_
#define MODELS_GAME_STATE_HPP_

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "models/food.hpp"
#include "models/snake.hpp"
#include "models/types.hpp"

struct Enemy {
  std::string status;  // "alive" | "dead"
  std::vector<Vec3> geometry;
  int kills;

  bool is_alive() const {
    return status == "alive";
  }
};

inline void from_json(const nlohmann::json& j, Enemy& e) {
  j.at("status").get_to(e.status);
  j.at("geometry").get_to(e.geometry);
  j.at("kills").get_to(e.kills);
}

struct SpecialFood {
  std::vector<Vec3> golden;
  std::vector<Vec3> suspicious;
};

inline void from_json(const nlohmann::json& j, SpecialFood& sf) {
  if (j.contains("golden")) {
    j.at("golden").get_to(sf.golden);
  }
  if (j.contains("suspicious")) {
    j.at("suspicious").get_to(sf.suspicious);
  }
}

struct GameState {
  std::string name;
  int points;
  int turn;
  int tick_remain_ms;
  int revive_timeout_sec;
  Vec3 map_size;
  std::vector<Snake> snakes;  // Player's snakes
  std::vector<Enemy> enemies;
  std::vector<Food> food;
  std::vector<Vec3> fences;  // Obstacles
  SpecialFood special_food;
  std::vector<std::string> errors;
};

inline void from_json(const nlohmann::json& j, GameState& gs) {
  if (j.contains("name")) {
    j.at("name").get_to(gs.name);
  }
  if (j.contains("points")) {
    j.at("points").get_to(gs.points);
  }
  if (j.contains("turn")) {
    j.at("turn").get_to(gs.turn);
  }
  if (j.contains("tickRemainMs")) {
    j.at("tickRemainMs").get_to(gs.tick_remain_ms);
  }
  if (j.contains("reviveTimeoutSec")) {
    j.at("reviveTimeoutSec").get_to(gs.revive_timeout_sec);
  }
  if (j.contains("mapSize")) {
    j.at("mapSize").get_to(gs.map_size);
  }
  if (j.contains("snakes")) {
    j.at("snakes").get_to(gs.snakes);
  }
  if (j.contains("enemies")) {
    j.at("enemies").get_to(gs.enemies);
  }
  if (j.contains("food")) {
    j.at("food").get_to(gs.food);
  }
  if (j.contains("fences")) {
    j.at("fences").get_to(gs.fences);
  }
  if (j.contains("specialFood")) {
    j.at("specialFood").get_to(gs.special_food);
  }
  if (j.contains("errors")) {
    j.at("errors").get_to(gs.errors);
  }
}

#endif  // MODELS_GAME_STATE_HPP_
