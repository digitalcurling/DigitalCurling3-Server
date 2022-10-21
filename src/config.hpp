#ifndef DIGITALCURLING3_SERVER_CONFIG_HPP
#define DIGITALCURLING3_SERVER_CONFIG_HPP

#include <array>
#include <vector>
#include <memory>
#include <chrono>
#include "nlohmann/json.hpp"
#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_server {

struct Config {
    struct Server {
        std::array<unsigned short, 2> port;
        std::chrono::milliseconds timeout_dc_ok;
        bool send_trajectory;
        size_t steps_per_trajectory_frame;
    } server;

    struct Game {
        enum class Rule {
            kNormal
        } rule;
        digitalcurling3::GameSetting setting;
        std::unique_ptr<digitalcurling3::ISimulatorFactory> simulator;
        std::array<
            std::vector<std::unique_ptr<digitalcurling3::IPlayerFactory>>,
            2> players;  // normalルールではプレイヤーは各チーム4人
    } game;

    // is_ready 時に"game"として送信するjson
    nlohmann::json game_is_ready;
};


// JSONへの変換
// こちらの機能はデバッグ用
void to_json(nlohmann::json &, Config const&);

// JSONからの変換
void from_json(nlohmann::json const&, Config &);


} // namespace digitalcurling3_server

#endif
