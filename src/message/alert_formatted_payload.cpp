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
#include <bitcoin/bitcoin/message/alert_formatted_payload.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

alert_formatted_payload alert_formatted_payload::factory_from_data(
    const data_chunk& data)
{
    alert_formatted_payload instance;
    instance.from_data(data);
    return instance;
}

alert_formatted_payload alert_formatted_payload::factory_from_data(
    std::istream& stream)
{
    alert_formatted_payload instance;
    instance.from_data(stream);
    return instance;
}

alert_formatted_payload alert_formatted_payload::factory_from_data(
    reader& source)
{
    alert_formatted_payload instance;
    instance.from_data(source);
    return instance;
}

bool alert_formatted_payload::is_valid() const
{
    return (version != 0)
        || (relay_until != 0)
        || (expiration != 0)
        || (id != 0)
        || (cancel != 0)
        || !set_cancel.empty()
        || (min_version != 0)
        || (max_version != 0)
        || !set_sub_version.empty()
        || (priority != 0)
        || !comment.empty()
        || !status_bar.empty()
        || !reserved.empty();
}

void alert_formatted_payload::reset()
{
    version = 0;
    relay_until = 0;
    expiration = 0;
    id = 0;
    cancel = 0;
    set_cancel.clear();
    min_version = 0;
    max_version = 0;
    set_sub_version.clear();
    priority = 0;
    comment.clear();
    status_bar.clear();
    reserved.clear();
}

bool alert_formatted_payload::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool alert_formatted_payload::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool alert_formatted_payload::from_data(reader& source)
{
    reset();

    version = source.read_4_bytes_little_endian();
    relay_until = source.read_8_bytes_little_endian();
    expiration = source.read_8_bytes_little_endian();
    id = source.read_4_bytes_little_endian();
    cancel = source.read_4_bytes_little_endian();

    uint64_t set_cancel_size = source.read_variable_uint_little_endian();
    for (uint64_t i = 0; i < set_cancel_size && source; i++)
        set_cancel.push_back(source.read_4_bytes_little_endian());

    min_version = source.read_4_bytes_little_endian();
    max_version = source.read_4_bytes_little_endian();

    uint64_t set_sub_version_size = source.read_variable_uint_little_endian();
    for (uint64_t i = 0; i < set_sub_version_size && source; i++)
        set_sub_version.push_back(source.read_string());

    priority = source.read_4_bytes_little_endian();
    comment = source.read_string();
    status_bar = source.read_string();
    reserved = source.read_string();

    if (!source)
        reset();

    return source;
}

data_chunk alert_formatted_payload::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void alert_formatted_payload::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void alert_formatted_payload::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(version);
    sink.write_8_bytes_little_endian(relay_until);
    sink.write_8_bytes_little_endian(expiration);
    sink.write_4_bytes_little_endian(id);
    sink.write_4_bytes_little_endian(cancel);

    sink.write_variable_uint_little_endian(set_cancel.size());
    for (const auto& entry : set_cancel)
        sink.write_4_bytes_little_endian(entry);

    sink.write_4_bytes_little_endian(min_version);
    sink.write_4_bytes_little_endian(max_version);

    sink.write_variable_uint_little_endian(set_sub_version.size());
    for (const auto& entry : set_sub_version)
        sink.write_string(entry);

    sink.write_4_bytes_little_endian(priority);
    sink.write_string(comment);
    sink.write_string(status_bar);
    sink.write_string(reserved);
}

uint64_t alert_formatted_payload::satoshi_size() const
{
    uint64_t size = 40 + variable_uint_size(comment.size()) + comment.size() +
        variable_uint_size(status_bar.size()) + status_bar.size() +
        variable_uint_size(reserved.size()) + reserved.size() +
        variable_uint_size(set_cancel.size()) + (4 * set_cancel.size()) +
        variable_uint_size(set_sub_version.size());

    for (const auto& sub_version : set_sub_version)
        size += variable_uint_size(sub_version.size()) + sub_version.size();

    return size;
}

bool operator==(const alert_formatted_payload& left,
    const alert_formatted_payload& right)
{
    bool result = (left.version == right.version) &&
        (left.relay_until == right.relay_until) &&
        (left.expiration == right.expiration) &&
        (left.id == right.id) &&
        (left.cancel == right.cancel) &&
        (left.set_cancel.size() == right.set_cancel.size()) &&
        (left.min_version == right.min_version) &&
        (left.max_version == right.max_version) &&
        (left.set_sub_version.size() == right.set_sub_version.size()) &&
        (left.priority == right.priority) &&
        (left.comment == right.comment) &&
        (left.status_bar == right.status_bar) &&
        (left.reserved == right.reserved);

    for (std::vector<uint32_t>::size_type i = 0; i < left.set_cancel.size() && result; i++)
        result = (left.set_cancel[i] == right.set_cancel[i]);

    for (std::vector<std::string>::size_type i = 0; i < left.set_sub_version.size() && result; i++)
        result = (left.set_sub_version[i] == right.set_sub_version[i]);

    return result;
}

bool operator!=(const alert_formatted_payload& left,
    const alert_formatted_payload& right)
{
    return !(left == right);
}

} // end message
} // end libbitcoin
