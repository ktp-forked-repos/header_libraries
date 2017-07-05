// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

#include "daw_exception.h"

namespace daw {
	// Adapted from GSL
	//
	// not_null
	//
	// Restricts a pointer or smart pointer to only hold non-null values.
	//
	// Has zero size overhead over T.
	//
	// If T is a pointer ( i.e. T == U* ) then
	// - allow construction from U*
	// - disallow construction from null_ptr
	// - disallow default construction
	// - ensure construction from null U* fails
	// - allow implicit conversion to U*
	//
	template<class T>
	class not_null {
		T m_ptr;

	  public:
		static_assert( std::is_assignable<T &, std::nullptr_t>::value, "T cannot be assigned nullptr." );

		template<typename U, typename Dummy = std::enable_if_t<std::is_convertible<U, T>::value>>
		constexpr not_null( U &&u ) : m_ptr{std::forward<U>( u )} {

			daw::exception::daw_throw_on_null( m_ptr, "Cannot be assigned nullptr" );
		}

		template<typename U, typename Dummy = std::enable_if_t<std::is_convertible<U, T>::value>>
		constexpr not_null( not_null<U> const &other ) : not_null{other.get( )} {}

		not_null( not_null const &other ) = default;
		not_null &operator=( not_null const &other ) = default;

		constexpr T get( ) const {
			daw::exception::daw_throw_on_null( m_ptr, "Cannot be nullptr" );
			return m_ptr;
		}

		constexpr operator T( ) const {
			return get( );
		}

		constexpr T operator->( ) const {
			return get( );
		}

		// prevents compilation when someone attempts to assign a null pointer constant
		not_null( std::nullptr_t ) = delete;
		not_null &operator=( std::nullptr_t ) = delete;

		// unwanted operators...pointers only point to single objects!
		not_null &operator++( ) = delete;
		not_null &operator--( ) = delete;
		not_null operator++( int ) = delete;
		not_null operator--( int ) = delete;
		not_null &operator+=( std::ptrdiff_t ) = delete;
		not_null &operator-=( std::ptrdiff_t ) = delete;
		void operator[]( std::ptrdiff_t ) const = delete;
	};

	// more unwanted operators
	template<class T, class U>
	std::ptrdiff_t operator-( const not_null<T> &, const not_null<U> & ) = delete;

	template<class T>
	not_null<T> operator-( const not_null<T> &, std::ptrdiff_t ) = delete;

	template<class T>
	not_null<T> operator+( const not_null<T> &, std::ptrdiff_t ) = delete;

	template<class T>
	not_null<T> operator+( std::ptrdiff_t, const not_null<T> & ) = delete;
} // namespace daw

namespace std {
	template<class T>
	struct hash<daw::not_null<T>> {
		std::size_t operator( )( daw::not_null<T> const &value ) const {
			return hash<T>{}( value );
		}
	};
} // namespace std