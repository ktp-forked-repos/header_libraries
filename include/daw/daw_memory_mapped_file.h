// The MIT License (MIT)
//
// Copyright (c) 2013-2019 Darrell Wright
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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <cstddef>
#include <memory>

#include "daw_string_view.h"
#include "daw_swap.h"
#include "daw_traits.h"

namespace daw {
	namespace filesystem {
		template<typename T>
		class memory_mapped_file_t {
			// TODO remove size_t const m_max_buff_size = 1048576;
			boost::filesystem::path m_file_path;
			boost::iostreams::mapped_file_params m_mf_params;
			boost::iostreams::mapped_file m_mf_file;

		public:
			using value_type = std::decay_t<T>;
			using pointer = value_type *;
			using const_pointer = value_type const *;
			using iterator = value_type *;
			using const_iterator = value_type const *;
			using reference = value_type &;
			using const_reference = value_type const &;

			memory_mapped_file_t( boost::filesystem::path file_path,
			                      bool const readonly = true )
			  : m_file_path{file_path}
			  , m_mf_params{file_path.string( )} {

				m_mf_params.flags = boost::iostreams::mapped_file::mapmode::readwrite;
				if( readonly ) {
					// FIXME: seems to crash	m_mf_params.flags =
					// boost::iostreams::mapped_file::mapmode::readonly;
				}
				m_mf_params.offset = 0;
				m_mf_file.open( m_mf_params );
			}

			template<typename charT, typename traits>
			memory_mapped_file_t( daw::basic_string_view<charT, traits> filename,
			                      bool const readonly = true )
			  : memory_mapped_file_t{boost::filesystem::path{filename.data( )},
			                         readonly} {}

			memory_mapped_file_t( ) = delete;

			memory_mapped_file_t( memory_mapped_file_t && ) = default;
			memory_mapped_file_t &operator=( memory_mapped_file_t && ) = default;

			memory_mapped_file_t( memory_mapped_file_t const & ) = delete;
			memory_mapped_file_t &operator=( memory_mapped_file_t const & ) = delete;

			void close( ) {
				if( m_mf_file.is_open( ) ) {
					m_mf_file.close( );
				}
			}

			virtual ~memory_mapped_file_t( ) noexcept {
				try {
					close( );
				} catch( ... ) {}
			}

			bool is_open( ) const {
				return m_mf_file.is_open( );
			}

			explicit operator bool( ) const {
				return m_mf_file.is_open( );
			}

			reference operator[]( size_t position ) {
				return m_mf_file.data( )[position];
			}

			const_reference operator[]( size_t position ) const {
				return static_cast<const_reference>( m_mf_file.data( )[position] );
			}

			pointer data( size_t position = 0 ) {
				return reinterpret_cast<pointer>(
				  m_mf_file.data( ) +
				  static_cast<boost::iostreams::stream_offset>( position ) );
			}

			const_pointer data( size_t position = 0 ) const {
				return reinterpret_cast<const_pointer>(
				  m_mf_file.data( ) +
				  static_cast<boost::iostreams::stream_offset>( position ) );
			}

			void swap( memory_mapped_file_t &rhs ) noexcept {
				daw::cswap( m_file_path, rhs.m_file_path );
				daw::cswap( m_mf_params, rhs.m_mf_params );
				daw::cswap( m_mf_file, rhs.m_mf_file );
			}

			size_t size( ) const {
				return m_mf_file.size( );
			}

			iterator begin( ) {
				return m_mf_file.data( );
			}

			const_iterator begin( ) const {
				return reinterpret_cast<const_iterator>( m_mf_file.data( ) );
			}

			iterator end( ) {
				return std::next( m_mf_file.data( ), size( ) );
			}

			const_iterator end( ) const {
				return reinterpret_cast<const_iterator>(
				  std::next( m_mf_file.data( ), size( ) ) );
			}

			const_iterator cbegin( ) const {
				return reinterpret_cast<const_iterator>( m_mf_file.data( ) );
			}

			const_iterator cend( ) const {
				return reinterpret_cast<const_iterator>(
				  std::next( m_mf_file.data( ), size( ) ) );
			}
		}; // memory_mapped_file_t

		template<typename T>
		void swap( memory_mapped_file_t<T> &lhs,
		           memory_mapped_file_t<T> &rhs ) noexcept {
			lhs.swap( rhs );
		}

		template<typename T>
		using MemoryMappedFile = memory_mapped_file_t<T>;

		template<typename OStream, typename T,
		         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, memory_mapped_file_t<T> const &mmf ) {

			for( auto const &value : mmf ) {
				os << mmf;
			}
			return os;
		}

		template<typename OStream, typename T,
		         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, memory_mapped_file_t<T> const &&mmf ) {

			for( auto const &value : mmf ) {
				os << mmf;
			}
			return os;
		}
	} // namespace filesystem
} // namespace daw
