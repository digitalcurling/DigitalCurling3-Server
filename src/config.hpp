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
