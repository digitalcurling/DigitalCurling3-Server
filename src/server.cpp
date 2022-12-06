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

#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include "log.hpp"
#include "util.hpp"

namespace digitalcurling3_server {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context & io_context, Config && config, std::string const& date_time, std::string const& game_id)
    : listen_endpoints_()
    , acceptors_()
    , sessions_()
    , game_(*this, std::move(config), date_time, game_id)
{
    for (size_t i = 0; i < 2; ++i) {
        listen_endpoints_[i].emplace(tcp::v4(), game_.GetConfig().server.port[i]);
        acceptors_[i].emplace(io_context, *listen_endpoints_[i]);

        // accept
        acceptors_.at(i)->async_accept(
            [this, i](boost::system::error_code const& error, tcp::socket && socket)
            {
                if (!error) {
                    sessions_[i] = std::make_shared<TCPSession>(std::move(socket), *this, i);
                    sessions_[i]->Open();
                }
            });
    }
}

void Server::Stop()
{
    // stop accept
    for (auto & acceptor : acceptors_) {
        acceptor->cancel();
    }

    for (auto & session : sessions_) {
        if (session) {
            session->Close();
            session.reset();
        }
    }

    Log::Debug("server stopped");
}

void Server::OnSessionStop(size_t client_id)
{
    sessions_[client_id].reset();

    try {
        game_.OnSessionStop(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionStart(size_t client_id)
{
    try {
        game_.OnSessionStart(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output)
{
    try {
        game_.OnSessionRead(client_id, input_message, elapsed_from_output);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionTimeout(size_t client_id)
{
    try {
        game_.OnSessionTimeout(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::DeliverMessage(size_t client_id, std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout)
{
    if (sessions_[client_id] && !sessions_[client_id]->IsClosed()) {
        sessions_[client_id]->Deliver(std::move(message), input_timeout);
    } else {
        std::ostringstream buf;
        buf << "client " << client_id << " deliver message failed";
        throw std::runtime_error(buf.str());
    }
}


void Server::HandleError(std::exception & e)
{
    Log::Error(e.what());
    Stop();
}


void Start(Config && config, std::string const& launch_time, std::string const& game_id)
{
    {
        std::ostringstream buf;
        buf << "launch time: " << launch_time;
        Log::Info(buf.str());
    }

    {
        std::ostringstream buf;
        buf << "game id    : " << game_id;
        Log::Info(buf.str());
    }

    for (size_t i = 0; i < config.server.port.size(); ++i) {
        std::ostringstream buf;
        buf << "team " << i << " port: " << config.server.port[i];
        Log::Info(buf.str());
    }
    Log::Info("Note: Team 1 has the last stone in the first end.");

    boost::asio::io_context io_context;
    Server s(io_context, std::move(config), launch_time, game_id);

    Log::Info("server started");

    io_context.run();
}

} // namespace digitalcurling3_server

