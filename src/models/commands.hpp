#ifndef MODELS_COMMANDS_HPP_
#define MODELS_COMMANDS_HPP_

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "models/types.hpp"

struct SnakeCommand {
  std::string id;
  Vec3 direction;
};

inline void to_json(nlohmann::json& j, const SnakeCommand& cmd) {
  j = nlohmann::json{
    {"id", cmd.id},
    {"direction", cmd.direction}
  };
}

struct PlayerCommand {
  std::vector<SnakeCommand> snakes;
};

inline void to_json(nlohmann::json& j, const PlayerCommand& cmd) {
  j = nlohmann::json{
    {"snakes", cmd.snakes}
  };
}

#endif  // MODELS_COMMANDS_HPP_
