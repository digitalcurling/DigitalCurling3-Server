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

#include "log.hpp"
#include <cassert>
#include <exception>
#include <thread>
#include <string_view>
#include <charconv>
#include <boost/nowide/iostream.hpp>
#include "util.hpp"
#include "version.hpp"

namespace digitalcurling3_server {

namespace {

using namespace std::string_view_literals;

constexpr auto kGameLogFile = "game.dcl2"sv;
inline std::string GetShotLogFile(uint8_t end, uint8_t shot)
{
    std::ostringstream buf;
    buf << "shot_e" << std::setw(3) << std::setfill('0') << static_cast<unsigned int>(end)
        << "s" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(shot) << ".json";
    return buf.str();
}

constexpr auto kTagTrace   = "trc"sv;
constexpr auto kTagDebug   = "dbg"sv;
constexpr auto kTagInfo    = "inf"sv;
constexpr auto kTagGame    = "gam"sv;
constexpr auto kTagShot    = "sht"sv;
constexpr auto kTagWarning = "wrn"sv;
constexpr auto kTagError   = "err"sv;

constexpr auto kTargetServer = "server"sv;
constexpr auto kTargetClient = "client"sv;


// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

std::string ToString(Log::Target const& target)
{
    std::string str;
    std::visit(
        Overloaded{
            [&](Log::Server const&) {
                str = kTargetServer;
            },
            [&](Log::Client const& c) {
                std::ostringstream buf;
                buf << kTargetClient << c.id;
                str = buf.str();
            }
        },
        target);
    return str;
}



void PutLineHeader(std::ostream & o, std::string_view header, std::string_view message)
{
    size_t line_end = message.find_first_of('\n');
    while (true) {
        o << header;
        if (line_end == std::string_view::npos) {
            o << message;
            break;
        }
        o << message.substr(0, line_end + 1);
        message.remove_prefix(line_end + 1);
        line_end = message.find_first_of('\n');
    }
}

void PutMessage(std::ostream & o, boost::posix_time::ptime time, std::string_view header, std::string_view message)
{
    std::ostringstream buf_header;
    buf_header << '[' << GetTimeOfDay(time) << "] " << header;
    PutLineHeader(boost::nowide::cout, buf_header.str(), message);
    boost::nowide::cout << std::endl;
}



} // unnamed namespace


Log::Log(boost::filesystem::path const& log_file, boost::filesystem::path const& game_log_directory, bool verbose, bool debug)
    : game_log_directory_(game_log_directory)
    , verbose_(verbose)
    , debug_(debug)
    , mutex_()
    , next_id_(0)
    , directory_created_(false)
    , file_all_()
    , file_game_()
{
    assert(instance_ == nullptr);
    instance_ = this;

    boost::filesystem::create_directories(log_file.parent_path());
    file_all_.open(log_file, std::ios_base::out);

    // check game_log_directory
    if (boost::filesystem::exists(game_log_directory)) {
        throw std::runtime_error("log directory already exists");
    }
}

Log::~Log()
{
    instance_ = nullptr;
}

void Log::Trace(Target const& from, Target const& to, std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();

    nlohmann::ordered_json const json{
        { "from", ToString(from) },
        { "to", ToString(to) },
        { "msg", message }
    };

    auto const detailed = instance_->CreateDetailedLog(kTagTrace, json, t);

    instance_->file_all_ << detailed << std::endl;
}

void Log::Debug(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagDebug, message, t);

    if (instance_->debug_) {
        if (instance_->verbose_) {
            boost::nowide::cout << detailed << std::endl;
        } else {
            PutMessage(boost::nowide::cout, t, "[debug] ", message);
        }
    }

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Info(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagInfo, message, t);
    
    // stdout
    if (instance_->verbose_) {
        boost::nowide::cout << detailed << std::endl;
    } else {
        PutMessage(boost::nowide::cout, t, "", message);
    }

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Game(nlohmann::json const& json)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagGame, json, t);

    instance_->CheckGameLogFileOpen();

    if (instance_->verbose_) {
        boost::nowide::cout << detailed << std::endl;
    }

    instance_->file_game_ << detailed << std::endl;
    instance_->file_all_ << detailed << std::endl;
}

void Log::Shot(nlohmann::json const& json, std::uint8_t end, std::uint8_t shot)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagShot, json, t);

    instance_->CheckGameLogDirectoryCreated();

    boost::nowide::ofstream file(instance_->game_log_directory_ / GetShotLogFile(end, shot));
    file << detailed.dump(2) << std::endl;

    instance_->file_all_ << detailed << std::endl;
}

void Log::Warning(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagWarning, message, t);

    PutMessage(boost::nowide::cerr, t, "[warning] ", message);

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Error(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagError, message, t);

    PutMessage(boost::nowide::cerr, t, "[error] ", message);

    // all
    instance_->file_all_ << detailed << std::endl;

    // gameログファイルが開かれているなら，エラーメッセージを出す
    if (instance_->file_game_) {
        instance_->file_game_ << detailed << std::endl;
    }
}

bool Log::IsValid()
{
    return instance_ != nullptr;
}

nlohmann::ordered_json Log::CreateDetailedLog(std::string_view tag, nlohmann::json const& json, boost::posix_time::ptime time)
{
    std::ostringstream buf_thread_id;
    buf_thread_id << std::this_thread::get_id();

    nlohmann::ordered_json j{
        { "ver", { GetLogVersionMajor(), GetLogVersionMinor() } },
        { "tag", tag },
        { "id", next_id_ },
        { "date_time", GetISO8601ExtendedString(time) },
        { "thread", buf_thread_id.str() },
        { "log", json }
    };

    ++next_id_;

    return j;
}

void Log::CheckGameLogFileOpen()
{
    CheckGameLogDirectoryCreated();

    if (!file_game_.is_open()) {  // operator bool で判定すると初回にスキップされるため is_open() で判定する
        file_game_.open(game_log_directory_ / kGameLogFile.data());
    }
}

void Log::CheckGameLogDirectoryCreated()
{
    if (directory_created_) return;

    boost::filesystem::create_directories(game_log_directory_);

    directory_created_ = true;
}


} // namespace digitalcurling3_server
