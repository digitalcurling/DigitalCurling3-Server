﻿// MIT License
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

#ifndef DIGITALCURLING3_SERVER_SERVER_HPP
#define DIGITALCURLING3_SERVER_SERVER_HPP

#include <string>
#include <string_view>
#include <array>
#include <optional>
#include <chrono>
#include <memory>
#include <exception>
#include "config.hpp"
#include "game.hpp"
#include "tcp_session.hpp"

namespace digitalcurling3_server {

class Server {
public:
    // Start() から呼び出す関数 ---

    Server(boost::asio::io_context & io_context, Config && config, std::string const& date_time, std::string const& game_id);

    // TCPSessionから呼び出す関数 ---

    /// \brief サーバーを停止する
    void Stop();
    void OnSessionStart(size_t client_id);
    void OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output);
    void OnSessionTimeout(size_t client_id);
    void OnSessionStop(size_t client_id);

    // Game から呼び出す関数 ---

    /// \brief メッセージを送信する
    /// 
    /// \param client_id 送信先クライアントID
    /// \param message 送信するメッセージ(末尾の改行文字を含まない)
    /// \param input_timeout タイムアウトまでの時間( \c std::nullopt で制限時間無し)
    void DeliverMessage(size_t client_id, std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout = std::nullopt);

private:
    std::array<std::optional<boost::asio::ip::tcp::endpoint>, 2> listen_endpoints_;
    std::array<std::optional<boost::asio::ip::tcp::acceptor>, 2> acceptors_;
    std::array<std::shared_ptr<TCPSession>, 2> sessions_;
    Game game_;

    void HandleError(std::exception & e);
};

} // namespace digitalcurling3_server

#endif
