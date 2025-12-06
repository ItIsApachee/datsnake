#ifndef MODELS_SNAKE_HPP_
#define MODELS_SNAKE_HPP_

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "models/types.hpp"

struct Snake {
  std::string id;
  std::vector<Vec3> geometry;  // First element is head
  Vec3 direction;
  Vec3 old_direction;
  std::string status;  // "alive" | "dead"
  int death_count;
  int revive_remain_ms;

  bool is_alive() const {
    return status == "alive";
  }

  Vec3 get_head() const {
    return geometry.empty() ? Vec3() : geometry[0];
  }

  bool can_reverse() const {
    return geometry.size() < 2;
  }

  bool is_valid_direction(const Vec3& new_dir) const {
    if (can_reverse()) {
      return true;
    }
    // Cannot reverse direction if length >= 2
    return new_dir != -direction;
  }
};

inline void to_json(nlohmann::json& j, const Snake& s) {
  j = nlohmann::json{
    {"id", s.id},
    {"geometry", s.geometry},
    {"direction", s.direction},
    {"oldDirection", s.old_direction},
    {"status", s.status},
    {"deathCount", s.death_count},
    {"reviveRemainMs", s.revive_remain_ms}
  };
}

inline void from_json(const nlohmann::json& j, Snake& s) {
  j.at("id").get_to(s.id);
  j.at("geometry").get_to(s.geometry);
  j.at("direction").get_to(s.direction);
  j.at("oldDirection").get_to(s.old_direction);
  j.at("status").get_to(s.status);
  j.at("deathCount").get_to(s.death_count);
  j.at("reviveRemainMs").get_to(s.revive_remain_ms);
}

#endif  // MODELS_SNAKE_HPP_
