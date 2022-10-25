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

#include "util.hpp"

#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace digitalcurling3_server {

std::string GetTimeOfDay(boost::posix_time::ptime t)
{
    return boost::posix_time::to_simple_string(t.time_of_day());
}

std::string GetISO8601ExtendedString(boost::posix_time::ptime t)
{
    const auto utc_timestamp = boost::posix_time::second_clock::universal_time();
    const auto local_timestamp = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc_timestamp);
    const auto diff_from_utc = local_timestamp - utc_timestamp;

    std::ostringstream oss;
    oss << boost::posix_time::to_iso_extended_string(t);
    oss << (diff_from_utc.is_negative() ? '-' : '+');
    oss << std::setw(2) << std::setfill('0');
    oss << boost::date_time::absolute_value(diff_from_utc.hours());
    oss << ':';
    oss << std::setw(2) << std::setfill('0');
    oss << boost::date_time::absolute_value(diff_from_utc.minutes());
    return oss.str();

}

std::string GetISO8601String(boost::posix_time::ptime t)
{
    return boost::posix_time::to_iso_string(t);
}

} // namespace digitalcurling3_server
