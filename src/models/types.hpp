#ifndef MODELS_TYPES_HPP_
#define MODELS_TYPES_HPP_

#include <nlohmann/json.hpp>

struct Vec3 {
  int x;
  int y;
  int z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(int x, int y, int z) : x(x), y(y), z(z) {}

  bool operator==(const Vec3& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool operator!=(const Vec3& other) const {
    return !(*this == other);
  }

  Vec3 operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }

  Vec3 operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }

  Vec3 operator-() const {
    return Vec3(-x, -y, -z);
  }

  int manhattan_distance(const Vec3& other) const {
    return std::abs(x - other.x) + std::abs(y - other.y) + std::abs(z - other.z);
  }
};

inline void to_json(nlohmann::json& j, const Vec3& v) {
  j = nlohmann::json::array({v.x, v.y, v.z});
}

inline void from_json(const nlohmann::json& j, Vec3& v) {
  if (j.is_array() && j.size() >= 3) {
    j[0].get_to(v.x);
    j[1].get_to(v.y);
    j[2].get_to(v.z);
  }
}

enum class Direction {
  NONE,
  POS_X,  // [1, 0, 0]
  NEG_X,  // [-1, 0, 0]
  POS_Y,  // [0, 1, 0]
  NEG_Y,  // [0, -1, 0]
  POS_Z,  // [0, 0, 1]
  NEG_Z   // [0, 0, -1]
};

inline Vec3 direction_to_vec3(Direction dir) {
  switch (dir) {
    case Direction::POS_X: return Vec3(1, 0, 0);
    case Direction::NEG_X: return Vec3(-1, 0, 0);
    case Direction::POS_Y: return Vec3(0, 1, 0);
    case Direction::NEG_Y: return Vec3(0, -1, 0);
    case Direction::POS_Z: return Vec3(0, 0, 1);
    case Direction::NEG_Z: return Vec3(0, 0, -1);
    default: return Vec3(0, 0, 0);
  }
}

#endif  // MODELS_TYPES_HPP_
