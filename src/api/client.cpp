#include "api/client.hpp"

#include <cpr/cpr.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

namespace api {

Client::Client(const std::string& auth_token)
    : auth_token_(auth_token), base_url_(BASE_URL) {}

nlohmann::json Client::make_request(const std::string& method,
                                     const std::string& endpoint,
                                     const nlohmann::json& body) {
  cpr::Header headers = {
    {"X-Auth-Token", auth_token_},
    {"Content-Type", "application/json"}
  };

  std::string url = base_url_ + endpoint;
  cpr::Response response;

  if (method == "GET") {
    response = cpr::Get(cpr::Url{url}, headers);
  } else if (method == "POST") {
    std::string body_str = body.empty() ? "{}" : body.dump();
    response = cpr::Post(cpr::Url{url}, headers, cpr::Body{body_str});
  } else {
    throw std::runtime_error("Unsupported HTTP method: " + method);
  }

  if (response.status_code != 200) {
    std::cerr << "API request failed: " << response.status_code
              << " - " << response.text << std::endl;
    return nlohmann::json{};
  }

  try {
    return nlohmann::json::parse(response.text);
  } catch (const nlohmann::json::exception& e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
    return nlohmann::json{};
  }
}

std::optional<GameState> Client::move(const PlayerCommand& command) {
  nlohmann::json body;
  to_json(body, command);

  auto json_response = make_request("POST", MOVE_ENDPOINT, body);
  if (json_response.empty()) {
    return std::nullopt;
  }

  try {
    GameState state;
    from_json(json_response, state);
    return state;
  } catch (const nlohmann::json::exception& e) {
    std::cerr << "Failed to parse MoveResponse: " << e.what() << std::endl;
    return std::nullopt;
  }
}

std::optional<nlohmann::json> Client::get_rounds() {
  auto json_response = make_request("GET", ROUNDS_ENDPOINT);
  return json_response.empty() ? std::nullopt : std::make_optional(json_response);
}

std::optional<nlohmann::json> Client::get_logs() {
  auto json_response = make_request("GET", LOGS_ENDPOINT);
  return json_response.empty() ? std::nullopt : std::make_optional(json_response);
}

std::optional<nlohmann::json> Client::get_stats() {
  auto json_response = make_request("GET", STATS_ENDPOINT);
  return json_response.empty() ? std::nullopt : std::make_optional(json_response);
}

}  // namespace api
