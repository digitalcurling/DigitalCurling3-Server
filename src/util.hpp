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

#ifndef DIGITALCURLING3_SERVER_UTIL_HPP
#define DIGITALCURLING3_SERVER_UTIL_HPP

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace digitalcurling3_server {

/// \brief mm:ss+xx:yy 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return hh:mm:ss 形式の時刻
std::string GetTimeOfDay(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

/// \brief YYYY-MM-DDThh:mm:ss+xx:yy 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return YYYY-MM-DDThh:mm:ss+xx:yy 形式の時刻
std::string GetISO8601ExtendedString(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

/// \brief YYYYMMDDThhmmss 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return YYYYMMDDThhmmss 形式の時刻
std::string GetISO8601String(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

} // namespace digitalcurling3_server

#endif
