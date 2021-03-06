// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <cstddef>
#include <type_traits>
#include <utility>

#include "daw_move.h"
#include "daw_traits.h"
#include "daw_utility.h"

/// Provide a class to validate a type T after construction
/// Validator must return true if the value is valid, false otherwise
namespace daw {
	template<typename T, typename Validator>
	struct validated {
		static_assert( is_same_v<T, remove_cvref_t<T>>,
		               "T cannot be cv or ref qualified" );
		// This is really true for ref qualified T as it allows the value to change
		// without validation

		using value_t = T;
		using const_reference = value_t const &;
		using const_pointer = value_t const *;

	private:
		value_t m_value;

		template<typename U>
		constexpr decltype( auto ) validate( U &&value ) {
			if( !Validator{}( value ) ) {
				throw std::out_of_range( "Argument did not pass validation" );
			}
			return std::forward<U>( value );
		}

	public:
		template<
		  typename... Args,
		  std::enable_if_t<( !is_enum_v<value_t> &&
		                     !(sizeof...( Args ) == 1 &&
		                       daw::traits::is_first_type_v<validated, Args...>)),
		                   std::nullptr_t> = nullptr>
		constexpr validated( Args &&... args )
		  : m_value( validate(
		      construct_a<value_t>{}( std::forward<Args>( args )... ) ) ) {}

		// Handle enum's differently as it is UB to go out of range on a c enum
		template<
		  typename... Args,
		  std::enable_if_t<all_true_v<is_enum_v<value_t>, sizeof...( Args ) < 2>,
		                   std::nullptr_t> = nullptr>
		constexpr validated( Args &&... args )
		  : m_value(
		      static_cast<value_t>( validate( std::underlying_type_t<value_t>(
		        std::forward<Args>( args )... ) ) ) ) {}

		constexpr validated &operator=( const_reference rhs ) {
			m_value = validate( rhs );
			return *this;
		}

		constexpr validated &operator=( value_t &&rhs ) {
			m_value = validate( daw::move( rhs ) );
			return *this;
		}

		constexpr const_reference get( ) const &noexcept {
			return m_value;
		}

		constexpr value_t &&get( ) && noexcept {
			return daw::move( m_value );
		}

		constexpr operator const_reference( ) const noexcept {
			return m_value;
		}

		constexpr const_reference operator*( ) const noexcept {
			return m_value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return &m_value;
		}
	};
} // namespace daw
