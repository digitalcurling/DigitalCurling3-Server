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