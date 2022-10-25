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

#ifndef DIGITALCURLING3_SERVER_TCP_SESSION_HPP
#define DIGITALCURLING3_SERVER_TCP_SESSION_HPP

#include <deque>
#include <chrono>
#include <optional>
#include <memory>
#include <string>
#include <exception>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

namespace digitalcurling3_server {

class Server;

class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(boost::asio::ip::tcp::socket && socket, Server & server, size_t client_id);
    void Open();

    /// <summary>
    /// メッセージを送信する．
    /// </summary>
    /// <param name="message">送信するメッセージ．</param>
    /// <param name="input_timeout">次回の入力までのタイムアウト．nulloptの場合タイムアウトは発生しない．</param>
    void Deliver(std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout);
    void Close();
    bool IsClosed() const;

private:
    struct Message {
        std::string message;
        std::optional<std::chrono::milliseconds> input_timeout;

        Message(std::string const& m, std::optional<std::chrono::milliseconds> const& t)
            : message(m)
            , input_timeout(t) {}
    };

    void ReadLine();
    void AwaitOutput();
    void WriteLine();
    void CheckInputDeadline();

    Server & server_;
    boost::asio::ip::tcp::socket socket_;
    size_t const client_id_;
    std::string input_buffer_;
    boost::asio::steady_timer input_deadline_;
    std::deque<Message> output_queue_;
    boost::asio::steady_timer non_empty_output_queue_;
    boost::asio::steady_timer::time_point last_output_time_;
};

} // namespace digitalcurling3_server



#endif // DIGITALCURLING3_SERVER_TCP_SESSION_HPP
