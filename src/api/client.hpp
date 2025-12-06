#ifndef API_CLIENT_HPP_
#define API_CLIENT_HPP_

#include <optional>
#include <string>
#include <utility>

#include "api/endpoints.hpp"
#include "models/commands.hpp"
#include "models/game_state.hpp"

namespace api {

class Client {
 public:
  explicit Client(const std::string& auth_token);

  // Send move commands and get game state
  std::optional<GameState> move(const PlayerCommand& command);

  // Get rounds information
  std::optional<nlohmann::json> get_rounds();

  // Get logs
  std::optional<nlohmann::json> get_logs();

  // Get stats
  std::optional<nlohmann::json> get_stats();

 private:
  std::string auth_token_;
  std::string base_url_;

  nlohmann::json make_request(const std::string& method,
                              const std::string& endpoint,
                              const nlohmann::json& body = nlohmann::json{});
};

}  // namespace api

#endif  // API_CLIENT_HPP_
