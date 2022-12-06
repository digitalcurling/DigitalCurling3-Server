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

#include <string>
#include <sstream>
#include <string_view>
#include <optional>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/program_options.hpp>

#include <boost/nowide/args.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/filesystem.hpp>

#include <boost/filesystem.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "digitalcurling3/digitalcurling3.hpp"

#include "log.hpp"
#include "util.hpp"
#include "config.hpp"
#include "version.hpp"


namespace digitalcurling3_server {

void Start(Config && config, std::string const& launch_time, std::string const& game_id);

} // namespace digitalcurling3_server

using namespace std::string_view_literals;
namespace dcs = digitalcurling3_server;
using dcs::Log;

int main(int argc, char* argv[])
{
    boost::nowide::args nowide_args(argc, argv);
    boost::nowide::nowide_filesystem();

    // TODO 消す
    // try {
    //     nlohmann::json j = "ほ";
    //     b << j.dump() << std::endl;
    //     // std::cout << argv[1] << std::endl;
    // } catch (std::exception & e) {
    //     std::cout << e.what() << std::endl;
    // }

    // return 0;

    constexpr auto kDefaultConfigPath = "config.json"sv;
    constexpr auto kDefaultLogPath = "log"sv;
    constexpr auto kLogFileBaseName  = "server.log"sv;
    constexpr auto kManualURL = "http://github.com/digitalcurling/DigitalCurling"sv;

    std::optional<Log> log_instance;

    try {

        // --- ゲームID，起動時刻の確定 ---

        auto const launch_time = boost::posix_time::second_clock::local_time();
        auto const game_uuid_str = boost::uuids::to_string(boost::uuids::random_generator()());


        // --- コマンドライン引数の解析 ---

        boost::program_options::options_description opt_desc("Allowed options");

        {
            std::ostringstream buf_config_desc;
            buf_config_desc << "set config json file path (default: \"" << kDefaultConfigPath << "\")";
            std::ostringstream buf_log_dir_desc;
            buf_config_desc << "set log output directory (default: \"" << kDefaultLogPath << "\")";
            opt_desc.add_options()
                ("help,h", "produce help message")
                ("config,C", boost::program_options::value<std::string>(), buf_config_desc.str().c_str())
                ("config-json", boost::program_options::value<std::string>(), "set config json text. do not set the option --config at the same time.")
                ("log-dir", boost::program_options::value<std::string>(), buf_log_dir_desc.str().c_str())
                ("version", "show version")
                ("verbose,v", "verbose command line")
                ("debug", "debug mode")
                ;
        }

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt_desc), vm);
        boost::program_options::notify(vm);

        boost::filesystem::path const log_directory = [&] {
            boost::filesystem::path path;
            if (vm.count("log-dir")) {
                path = vm["log-dir"].as<std::string>();
            } else {
                path = kDefaultLogPath.data();
            }
            return boost::filesystem::absolute(path);
        }();

        boost::filesystem::path const log_file_path = log_directory / kLogFileBaseName.data();

        boost::filesystem::path const game_log_directory = [&] {
            std::ostringstream buf;
            buf << dcs::GetISO8601String(launch_time) << '_' << game_uuid_str;
            return log_directory / buf.str();
        }();

        bool const arg_verbose = vm.count("verbose");
        bool const arg_debug = vm.count("debug");

        log_instance.emplace(log_file_path, game_log_directory, arg_verbose, arg_debug); // ログシステムの起動

        {
            std::ostringstream buf;
            buf << "Digital Curling server ver." << dcs::GetVersion();
            Log::Info(buf.str());
        }

        if (arg_debug) {
            Log::Debug("debug: on");
        } else {
            Log::Debug("debug: off");
        }

        if (arg_verbose) {
            Log::Debug("verbose: on");
        } else {
            Log::Debug("verbose: off");
        }

        if (vm.count("help")) {
            std::ostringstream buf;
            buf << "online manual: " << kManualURL << '\n' << opt_desc;
            Log::Info(buf.str());
            return 0;
        }

        if (vm.count("version")) {
            {
                std::ostringstream buf;
                buf << "library version: " << digitalcurling3::GetVersion();
                Log::Info(buf.str());
            }
            {
                std::ostringstream buf;
                buf << "protocol version: " << dcs::GetProtocolVersion();
                Log::Info(buf.str());
            }
            {
                std::ostringstream buf;
                buf << "config version: " << dcs::GetConfigVersion();
                Log::Info(buf.str());
            }
            {
                std::ostringstream buf;
                buf << "log version: " << dcs::GetLogVersion();
                Log::Info(buf.str());
            }

            return 0;
        }

        {
            std::ostringstream buf;
            buf << "log file: \"" << log_file_path.string() << "\"";
            Log::Info(buf.str());
        }

        {
            std::ostringstream buf;
            buf << "game log dir: \"" << game_log_directory.string() << "\"";
            Log::Info(buf.str());
        }

        // --- コンフィグのパース ---

        bool const arg_config = vm.count("config");
        bool const arg_config_json = vm.count("config-json");

        if (arg_config && arg_config_json) {
            throw std::runtime_error("do not set option --config and --config-json at the same time");
        }

        nlohmann::json const config_json = [&] {
            if (arg_config_json) {
                auto const config_json_str = vm["config-json"].as<std::string>();

                std::ostringstream buf;
                buf << "specified config json: " << config_json_str;
                Log::Debug(buf.str());

                return nlohmann::json::parse(config_json_str);
            } else { // config オプションが指定されているか，config config-jsonの両方が指定されていない
                boost::filesystem::path const config_path = [&] {
                    boost::filesystem::path config_path_tmp;
                    if (arg_config) {
                        auto config_path_str = vm["config"].as<std::string>();
                        config_path_tmp = config_path_str;
                        std::ostringstream buf;
                        buf << "specified config path: \"" << config_path_str << '\"';
                        Log::Debug(buf.str());
                    } else {
                        config_path_tmp = kDefaultConfigPath.data();
                        std::ostringstream buf;
                        buf << "config path was not specified. use default path \"" << kDefaultConfigPath << "\".";
                        Log::Debug(buf.str());
                    }
                    return boost::filesystem::absolute(config_path_tmp); // 絶対パスに変換
                }();

                {
                    std::ostringstream buf;
                    buf << "config file: \"" << config_path.string() << "\"";
                    Log::Info(buf.str());
                }

                boost::nowide::ifstream config_file(config_path);
                if (!config_file) {
                    throw std::runtime_error("could not open config file");
                }
                return nlohmann::json::parse(config_file, nullptr, true, true); // ignore_comment: true
            }
        }();

        dcs::Config config = config_json.get<dcs::Config>();

        // --- サーバーの起動 ---

        dcs::Start(std::move(config), dcs::GetISO8601ExtendedString(launch_time), game_uuid_str);

        Log::Info("server terminated successfully");


    } catch (std::exception & e) {
        std::ostringstream buf;
        buf << "exception: " << e.what();

        if (Log::IsValid()) {
            Log::Error(buf.str());
        } else {
            std::cerr << buf.str() << std::endl;
        }
    } catch (...) {
        constexpr auto msg = "unknown exception";
        if (Log::IsValid()) {
            Log::Error(msg);
        } else {
            std::cerr << msg << std::endl;
        }
    }


    return 0;
}
