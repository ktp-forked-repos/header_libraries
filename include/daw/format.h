// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>

#include "daw_string_view.h"
#include "daw_traits.h"

/// formatter will directly substitute variables into a string
/// Uses a format like "{0} {1} {0}" which is a zero index array
/// of the arguments

namespace daw {
	namespace impl {
		class brace_splitter_t {
			enum class states_t : uint8_t { in_escape = 1, in_brace = 2; };
			size_t m_size;
			uint8_t m_state;

			constexpr bool get_state( states_t s ) noexcept {
				return ( m_state & static_cast<uint8_t>( s ) ) != 0;
			}

			constexpr void set_state( states_t s ) noexcept {
				m_state |= static_cast<uint8_t>( s );
			}

			constexpr void unset_state( states_t s ) noexcept {
				m_state &= ~static_cast<uint8_t>( s );
			}

			constexpr void switch_state( states_t s ) noexcept {
				if( get_state( s ) ) {
					set_state( s );
				} else {
					unset_state( s );
				}
			}

			constexpr void count_if( ) noexcept {
				if( get_state( states_t::in_brace ) ) {
					++m_size;
				}
			}

		public:
			constexpr brace_splitter_t( )
			  : m_size{0}
			  , m_state{0} {}

			constexpr bool operator( )( char c ) {
				if( m_state == 0 ) {
					m_size = 0;
				}
				switch( c ) {
				case '{':
					if( get_state( states_t::in_escape ) ) {
						count_if( );
						unset_set( states_t::in_escape );
						return true;
					}
					daw::exception::daw_throw_on_true( get_state( states_t::in_brace ),
					                                   "Invalid brace within brace" );
					set_state( states_t::in_brace );
					return true;
				case '}':
					if( get_state( states_t::in_escape ) ) {
						count_if( );
						unset_set( states_t::in_escape );
						return true;
					}
					daw::exception::daw_throw_on_false(
					  get_state( states_t::in_brace ),
					  "Invalid close brace without opening brace" );
					m_state = 0;
					return false;
				case '\\':
					switch_state( states_t::in_escape );
					count_if( );
					return true;
				}
			}

			constexpr size_t size( ) const noexcept {
				return m_size( );
			}
		};

		constexpr size_t find_variable_count( char const ( &str )[N] ) {

			for( size_t n = 0; n < N - 1; ++n ) {}
		}
	} // namespace impl

	template<size_t N>
	constexpr auto make_formatter( char const ( &str )[N] ) {
		size_t num_vars = find_variable_count( str );
	}
} // namespace daw
