#ifndef MODELS_FOOD_HPP_
#define MODELS_FOOD_HPP_

#include "types.hpp"
#include <nlohmann/json.hpp>

struct Food {
  Vec3 c;  // Coordinate
  int points;
  int type;  // 0=normal, distinguish golden/suspicious via specialFood
};

inline void to_json(nlohmann::json& j, const Food& f) {
  j = nlohmann::json{
    {"c", f.c},
    {"points", f.points},
    {"type", f.type}
  };
}

inline void from_json(const nlohmann::json& j, Food& f) {
  j.at("c").get_to(f.c);
  j.at("points").get_to(f.points);
  j.at("type").get_to(f.type);
}

#endif  // MODELS_FOOD_HPP_
