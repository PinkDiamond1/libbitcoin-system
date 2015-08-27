/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_ALERT_FORMATTED_PAYLOAD_HPP
#define LIBBITCOIN_MESSAGE_ALERT_FORMATTED_PAYLOAD_HPP

#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API alert_formatted_payload
{
public:
    static alert_formatted_payload factory_from_data(const data_chunk& data);
    static alert_formatted_payload factory_from_data(std::istream& stream);
    static alert_formatted_payload factory_from_data(reader& source);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t satoshi_size() const;

    uint32_t version;
    uint64_t relay_until;
    uint64_t expiration;
    uint32_t id;
    uint32_t cancel;
    std::vector<uint32_t> set_cancel;
    uint32_t min_version;
    uint32_t max_version;
    std::vector<std::string> set_sub_version;
    uint32_t priority;
    std::string comment;
    std::string status_bar;
    std::string reserved;
};

BC_API bool operator==(const alert_formatted_payload& left,
    const alert_formatted_payload& right);

BC_API bool operator!=(const alert_formatted_payload& left,
    const alert_formatted_payload& right);

} // end message
} // end libbitcoin

#endif
