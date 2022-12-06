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

#ifndef DIGITALCURLING3_SERVER_LOG_HPP
#define DIGITALCURLING3_SERVER_LOG_HPP

#include <cstdint>
#include <string_view>
#include <fstream>
#include <mutex>
#include <variant>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/filesystem.hpp>

#include "nlohmann/json.hpp"

namespace digitalcurling3_server {

/// \brief サーバーのログはこのクラスの関数を介して出力される．
///
/// シングルトン
class Log {
public:

    Log(boost::filesystem::path const& log_file, boost::filesystem::path const& game_log_directory, bool verbose, bool debug);
    Log(Log const&) = delete;
    Log & operator = (Log const&) = delete;
    ~Log();

    struct Server {};
    static constexpr Server kServer{};
    struct Client {
        size_t id;
        Client(size_t id) : id(id) {}
        Client(Client const&) = default;
        Client & operator = (Client const&) = default;
    };
    using Target = std::variant<Server, Client>;

    /// \brief 通信内容の表示に用いる
    /// 
    /// \param from 送信元
    /// \param to 受信先
    /// \param message 通信内容
    static void Trace(Target const& from, Target const& to, std::string_view message);

    /// \brief デバッグ用のログ
    ///
    /// \param message ログ
    static void Debug(std::string_view message);

    /// \brief 試合ログを出す
    ///
    /// GUIで試合ログを表示するためのデータ
    ///
    /// \param json ログデータ
    static void Game(nlohmann::json const& json);

    /// \brief ショットのログを出す
    ///
    /// GUIで試合ログを表示するためのデータ
    ///
    /// \param json ログデータ
    /// \param end エンド番号
    /// \param shot ショット番号
    static void Shot(nlohmann::json const& json, std::uint8_t end, std::uint8_t shot);

    /// \brief CUIに表示するログを出す
    ///
    /// \param message 表示するログ
    static void Info(std::string_view message);

    /// \brief 警告ログを出す
    ///
    /// サーバーを終了する程では無い，異常な動作を検出した場合に使う．
    ///
    /// \param message 警告メッセージ
    static void Warning(std::string_view message);

    /// \brief エラーメッセージを出す
    ///
    /// 通信などにエラーが発生した場合用いる．
    /// このメッセージを出したらサーバーは速やかに終了する．
    ///
    /// \param message エラーメッセージ
    static void Error(std::string_view message);

    /// \brief ログを出せる状態にあるか？
    /// 
    /// \return ログが出せるなら \c true
    static bool IsValid();

private:
    static inline Log * instance_ = nullptr;
    boost::filesystem::path const game_log_directory_;
    bool const verbose_;
    bool const debug_;

    std::mutex mutex_;
    uint64_t next_id_;  // ログのID値生成用
    bool directory_created_;
    boost::nowide::ofstream file_all_;
    boost::nowide::ofstream file_game_;
    nlohmann::ordered_json CreateDetailedLog(std::string_view tag, nlohmann::json const& json,
        boost::posix_time::ptime time);
    void CheckGameLogFileOpen();
    void CheckGameLogDirectoryCreated();
};

} // namespace digitalcurling3_server

#endif
