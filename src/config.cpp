// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "config.hpp"
#include <cmath>
#include "util.hpp"

namespace digitalcurling3_server {

NLOHMANN_JSON_SERIALIZE_ENUM(Config::Game::Rule, {
    {Config::Game::Rule::kNormal, "normal"},
})


// --- Config ---

void to_json(nlohmann::json& j, Config const& config)
{
    namespace dc = digitalcurling3;

    {
        auto& j_server = j["server"];
        {
            auto& j_server_port = j_server["port"];
            for (size_t i = 0; i < 2; ++i) {
                j_server_port[dc::ToString(static_cast<dc::Team>(i))] = config.server.port[i];
            }
        }
        j_server["timeout_dc_ok"] = config.server.timeout_dc_ok;
        j_server["update_interval"] = config.server.update_interval;
        j_server["send_trajectory"] = config.server.send_trajectory;
        j_server["steps_per_trajectory_frame"] = config.server.steps_per_trajectory_frame;
    }

    {
        auto& j_game = j["game"];
        j_game["rule"] = config.game.rule;
        j_game["setting"] = config.game.setting;
        j_game["simulator"] = *config.game.simulator;
        {
            auto& j_game_players = j_game["players"];
            for (size_t i = 0; i < 2; ++i) {
                auto& j_game_players_teamx = j_game_players[dc::ToString(static_cast<dc::Team>(i))];
                for (auto const& player : config.game.players[i]) {
                    j_game_players_teamx.push_back(*player);
                }
            }
        }
    }

    j["game_is_ready"] = config.game_is_ready;
}



void from_json(nlohmann::json const& j, Config & config)
{
    namespace dc = digitalcurling3;

    {
        auto const& j_server = j.at("server");
        {
            auto const& j_server_port = j_server.at("port");
            for (size_t i = 0; i < 2; ++i) {
                j_server_port.at(dc::ToString(static_cast<dc::Team>(i))).get_to(config.server.port[i]);
            }
        }
        j_server.at("timeout_dc_ok").get_to(config.server.timeout_dc_ok);
        if (auto it = j_server.find("update_interval"); it != j_server.end()) {
            it.value().get_to(config.server.update_interval);
        } else {
            config.server.update_interval = std::chrono::milliseconds(0);
        }
        j_server.at("send_trajectory").get_to(config.server.send_trajectory);
        j_server.at("steps_per_trajectory_frame").get_to(config.server.steps_per_trajectory_frame);
    }

    {
        auto const& j_game = j.at("game");
        j_game.at("rule").get_to(config.game.rule);
        j_game.at("setting").get_to(config.game.setting);
        config.game.simulator = j_game.at("simulator").get<std::unique_ptr<digitalcurling3::ISimulatorFactory>>();
        {
            auto const& j_game_players = j_game.at("players");
            for (size_t i = 0; i < 2; ++i) {
                auto const& j_game_players_teamx = j_game_players.at(dc::ToString(static_cast<dc::Team>(i)));
                for (auto const& j_game_players_teamx_x : j_game_players_teamx) {
                    config.game.players[i].emplace_back(j_game_players_teamx_x.get<std::unique_ptr<digitalcurling3::IPlayerFactory>>());
                }
            }
            switch (config.game.rule) {
                case Config::Game::Rule::kNormal:
                    for (auto const& player : config.game.players) {
                        if (player.size() != 4) {
                            throw std::runtime_error("config.game.player.size() != 4");
                        }
                    }
                    break;
                default:
                    assert(false);
            }
        }
    }

    // game_is_ready の構築

    // game_is_readyとgame_is_ready_patchの両方を指定した場合エラーとする
    if (j.contains("game_is_ready") && j.contains("game_is_ready_patch")) {
        throw std::runtime_error("specify only one of \"game_is_ready\" or \"game_is_ready_patch\"");
    }

    if (auto it = j.find("game_is_ready"); it != j.end()) {
        config.game_is_ready = *it;
    } else if (auto it = j.find("game_is_ready_patch"); it != j.end()) {
        config.game_is_ready = j.at("game").patch(*it);
    } else {
        config.game_is_ready = j.at("game");
    }
}


} // namespace digitalcurling3_server
