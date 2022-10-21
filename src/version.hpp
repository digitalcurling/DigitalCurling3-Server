#ifndef DIGITALCURLING3_SERVER_VERSION_HPP
#define DIGITALCURLING3_SERVER_VERSION_HPP

#include <cstdint>
#include <string>

namespace digitalcurling3_server {

std::string GetVersion();
std::uint32_t GetVersionMajor();
std::uint32_t GetVersionMinor();
std::uint32_t GetVersionPatch();

std::string GetProtocolVersion();
std::uint32_t GetProtocolVersionMajor();
std::uint32_t GetProtocolVersionMinor();

std::string GetConfigVersion();
std::uint32_t GetConfigVersionMajor();
std::uint32_t GetConfigVersionMinor();

std::string GetLogVersion();
std::uint32_t GetLogVersionMajor();
std::uint32_t GetLogVersionMinor();

} // namespace digitalcurling3_server

#endif // DIGITALCURLING3_SERVER_VERSION_HPP