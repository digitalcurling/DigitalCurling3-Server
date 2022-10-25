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

#ifndef DIGITALCURLING3_SERVER_GAME_HPP
#define DIGITALCURLING3_SERVER_GAME_HPP

#include <memory>
#include <array>
#include "digitalcurling3/digitalcurling3.hpp"
#include "config.hpp"
#include "trajectory_compressor.hpp"

namespace digitalcurling3_server {

class Server;

class Game {
public:
    Game(Server & server, Config && config, std::string const& date_time, std::string const& game_id);

    void OnSessionStart(size_t client_id);
    void OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output);
    void OnSessionTimeout(size_t client_id);
    void OnSessionStop(size_t client_id);

    Config const& GetConfig() const { return config_; }

private:
    struct Client {
        enum class State {
            kBeforeSessionStart, ///< ~ dc
            kDC,       ///< dc ~ dc_ok
            kReady,    ///< is_ready ~ ready_ok
            kNewGame,  ///< ready_ok ~ new_game
            kMyTurn,
            kOpponentTurn,
            kGameOver,
        };
        State state = State::kBeforeSessionStart;
        std::string name;
        std::vector<std::unique_ptr<digitalcurling3::IPlayer>> players;
        std::vector<size_t> player_order;
    };

    Server & server_;
    Config config_;
    std::string const date_time_;
    std::string const game_id_;

    nlohmann::json const json_dc_;
    nlohmann::json json_is_ready_;

    std::array<Client, 2> clients_;

    std::unique_ptr<digitalcurling3::ISimulator> const simulator_;
    digitalcurling3::GameState game_state_;

    TrajectoryCompressor compressor_;
    bool last_move_has_value_;
    bool last_move_free_guard_zone_foul_;
    nlohmann::json json_last_move_actual_move_;
    nlohmann::json json_last_move_trajectory_;

    void DoApplyMove(size_t moving_client_id, digitalcurling3::Move && move, std::chrono::milliseconds const& elapsed);
    void DeliverUpdateMessage();
};



} // namespace digitalcurling3_server

#endif // DIGITALCURLING3_SERVER_GAME_HPP
