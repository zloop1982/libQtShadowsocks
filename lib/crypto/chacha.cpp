/*
 * chacha.cpp - the source file of ChaCha class
 *
 * Copyright (C) 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "chacha.h"
#include "util/common.h"
#include <botan/loadstor.h>
#include <botan/rotate.h>
#include <stdexcept>

using namespace QSS;
using namespace Botan;

// Using anonymous namespace and static keyword to 'hide' this function
namespace {
static inline void chacha_quarter_round(uint32_t &a,
                                        uint32_t &b,
                                        uint32_t &c,
                                        uint32_t &d) {
    a += b; d ^= a; d = rotate_left(d, 16);
    c += d; b ^= c; b = rotate_left(b, 12);
    a += b; d ^= a; d = rotate_left(d, 8);
    c += d; b ^= c; b = rotate_left(b, 7);
}

}

ChaCha::ChaCha(const std::string &_key, const std::string &_iv) :
    m_position(0)
{
    const unsigned char *key =
            reinterpret_cast<const unsigned char*>(_key.data());

    m_state.resize(16);
    m_buffer.resize(64);

    m_state[0] = 0x61707865;
    m_state[1] = 0x3320646e;
    m_state[2] = 0x79622d32;
    m_state[3] = 0x6b206574;

    m_state[4] = load_le<uint32_t>(key, 0);
    m_state[5] = load_le<uint32_t>(key, 1);
    m_state[6] = load_le<uint32_t>(key, 2);
    m_state[7] = load_le<uint32_t>(key, 3);
    m_state[8] = load_le<uint32_t>(key, 4);
    m_state[9] = load_le<uint32_t>(key, 5);
    m_state[10] = load_le<uint32_t>(key, 6);
    m_state[11] = load_le<uint32_t>(key, 7);

    setIV(_iv);
}

void ChaCha::setIV(const std::string &_iv)
{
    const unsigned char *iv =
            reinterpret_cast<const unsigned char*>(_iv.data());

    m_state[12] = 0;
    m_state[13] = 0;

    if (_iv.length() == 8) {
        m_state[14] = load_le<uint32_t>(iv, 0);
        m_state[15] = load_le<uint32_t>(iv, 1);
    } else if (_iv.length() == 12) {
        m_state[13] = load_le<uint32_t>(iv, 0);
        m_state[14] = load_le<uint32_t>(iv, 1);
        m_state[15] = load_le<uint32_t>(iv, 2);
    } else {
        throw std::length_error("The IV length for ChaCha20 is invalid");
    }

    chacha();
}

void ChaCha::chacha()
{
    unsigned char *output = m_buffer.data();
    const uint32_t *input = m_state.data();
    uint32_t x00 = input[ 0], x01 = input[ 1], x02 = input[ 2], x03 = input[ 3],
             x04 = input[ 4], x05 = input[ 5], x06 = input[ 6], x07 = input[ 7],
             x08 = input[ 8], x09 = input[ 9], x10 = input[10], x11 = input[11],
             x12 = input[12], x13 = input[13], x14 = input[14], x15 = input[15];
    for (uint32_t i = 0; i != 10; ++i) {
        chacha_quarter_round(x00, x04, x08, x12);
        chacha_quarter_round(x01, x05, x09, x13);
        chacha_quarter_round(x02, x06, x10, x14);
        chacha_quarter_round(x03, x07, x11, x15);

        chacha_quarter_round(x00, x05, x10, x15);
        chacha_quarter_round(x01, x06, x11, x12);
        chacha_quarter_round(x02, x07, x08, x13);
        chacha_quarter_round(x03, x04, x09, x14);
    }

     store_le(x00 + input[ 0], output + 4 *  0);
     store_le(x01 + input[ 1], output + 4 *  1);
     store_le(x02 + input[ 2], output + 4 *  2);
     store_le(x03 + input[ 3], output + 4 *  3);
     store_le(x04 + input[ 4], output + 4 *  4);
     store_le(x05 + input[ 5], output + 4 *  5);
     store_le(x06 + input[ 6], output + 4 *  6);
     store_le(x07 + input[ 7], output + 4 *  7);
     store_le(x08 + input[ 8], output + 4 *  8);
     store_le(x09 + input[ 9], output + 4 *  9);
     store_le(x10 + input[10], output + 4 * 10);
     store_le(x11 + input[11], output + 4 * 11);
     store_le(x12 + input[12], output + 4 * 12);
     store_le(x13 + input[13], output + 4 * 13);
     store_le(x14 + input[14], output + 4 * 14);
     store_le(x15 + input[15], output + 4 * 15);

     ++m_state[12];
     m_state[13] += (m_state[12] == 0);
     m_position = 0;
}

std::string ChaCha::update(const uint8_t *in, size_t length)
{
    std::string output;
    output.resize(length);
    unsigned char *out = reinterpret_cast<unsigned char*>(&output[0]);

    uint32_t buf_size = m_buffer.size();
    for (uint32_t delta = buf_size - m_position;
         length >= delta;
         delta = buf_size - m_position) {
        Common::exclusive_or(m_buffer.data() + m_position, in, out, delta);
        length -= delta;
        in += delta;
        out += delta;
        chacha();
    }

    Common::exclusive_or(m_buffer.data() + m_position, in, out, length);
    m_position += length;
    return output;
}

std::string ChaCha::update(const std::string &input)
{
    return update(reinterpret_cast<const uint8_t*>(input.data()), input.length());
}
