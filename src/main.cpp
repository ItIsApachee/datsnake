#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "api/client.hpp"
#include "game/game.hpp"
#include "strategy/greedy.hpp"

// Global game manager for signal handling
std::unique_ptr<game::GameManager> g_game_manager;

void signal_handler(int signal) {
  std::cout << "\nReceived signal " << signal << ", stopping game..."
            << std::endl;
  if (g_game_manager) {
    g_game_manager->stop();
  }
}

// Parse .env file for TOKEN
std::string load_token_from_env_file(const std::string& filepath = ".env") {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    return "";
  }

  std::string line;
  while (std::getline(file, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Find the = delimiter
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    // Trim whitespace from key
    while (!key.empty() && std::isspace(key.back())) {
      key.pop_back();
    }
    while (!key.empty() && std::isspace(key.front())) {
      key.erase(0, 1);
    }

    // Trim whitespace and quotes from value
    while (!value.empty() && std::isspace(value.back())) {
      value.pop_back();
    }
    while (!value.empty() && std::isspace(value.front())) {
      value.erase(0, 1);
    }
    // Remove surrounding quotes if present
    if (value.size() >= 2 &&
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
      value = value.substr(1, value.size() - 2);
    }

    if (key == "TOKEN") {
      return value;
    }
  }

  return "";
}

void print_usage(const char* program_name) {
  std::cout << "Usage: " << program_name << " [OPTIONS]\n"
            << "\n"
            << "Options:\n"
            << "  -t, --token TOKEN    Authentication token (overrides .env)\n"
            << "  -h, --help           Show this help message\n"
            << "\n"
            << "The auth token can be provided via:\n"
            << "  1. Command line argument (-t or --token)\n"
            << "  2. TOKEN environment variable\n"
            << "  3. .env file in current directory\n";
}

int main(int argc, char* argv[]) {
  std::string token;

  // Parse command line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_usage(argv[0]);
      return 0;
    } else if ((arg == "-t" || arg == "--token") && i + 1 < argc) {
      token = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      print_usage(argv[0]);
      return 1;
    }
  }

  // If token not provided via CLI, try environment variable
  if (token.empty()) {
    const char* env_token = std::getenv("TOKEN");
    if (env_token != nullptr) {
      token = env_token;
    }
  }

  // If still no token, try .env file
  if (token.empty()) {
    token = load_token_from_env_file();
  }

  // Verify we have a token
  if (token.empty()) {
    std::cerr << "Error: No authentication token provided.\n"
              << "Please provide a token via -t flag, TOKEN environment "
              << "variable, or .env file.\n";
    return 1;
  }

  std::cout << "DatsSnake Bot starting..." << std::endl;
  std::cout << "Token: " << token.substr(0, 8) << "..." << std::endl;

  // Set up signal handlers
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create components
  auto client = std::make_unique<api::Client>(token);
  auto strategy = std::make_unique<strategy::GreedyStrategy>();

  // Check rounds info
  auto rounds = client->get_rounds();
  if (rounds.has_value()) {
    std::cout << "Connected to server successfully" << std::endl;
    if (rounds->contains("rounds")) {
      auto& round_list = (*rounds)["rounds"];
      for (const auto& round : round_list) {
        std::cout << "Round: " << round.value("name", "unknown")
                  << " - Status: " << round.value("status", "unknown")
                  << std::endl;
      }
    }
  } else {
    std::cerr << "Warning: Could not fetch rounds info" << std::endl;
  }

  // Create and run game manager
  g_game_manager = std::make_unique<game::GameManager>(
      std::move(client), std::move(strategy));

  g_game_manager->run();

  return 0;
}
