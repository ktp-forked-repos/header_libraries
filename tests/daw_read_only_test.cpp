// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/daw_read_only.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

namespace test_01 {
	template<typename T>
	using rw_type = decltype( daw::read_only<T>{4} = 5 );

	static_assert( !daw::is_detected_v<rw_type, int>,
	               "Read only type is assignable, it should not be" );
} // namespace test_01

namespace test_02 {
	struct A {
		void b( ) {}
		void c( ) const {}
	};

	void daw_read_only_test_02( ) {
		std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
		std::cout << " sizeof( int ) -> " << sizeof( int );
		std::cout << " sizeof( daw::read_only<int> ) -> "
		          << sizeof( daw::read_only<int> );
		std::cout << " sizeof( daw::read_only<size_t> ) -> "
		          << sizeof( daw::read_only<size_t> ) << '\n';
		daw::read_only<int> a{};
		daw::read_only<int> b{1};
		daw::read_only<int> c{2};
		daw::read_only<int> d{1};
		daw::read_only<int> e = 5;
		::Unused( e );

		daw::read_only<A> x{};
		x->c( );

		// a & b
		daw::expecting( !( a == b ) );
		daw::expecting( !( b == a ) );
		daw::expecting( a != b );
		daw::expecting( b != a );
		daw::expecting( a < b );
		daw::expecting( !( b < a ) );
		daw::expecting( a <= b );
		daw::expecting( !( b <= a ) );
		daw::expecting( !( a >= b ) );
		daw::expecting( b >= a );
		daw::expecting( b == d );
		daw::expecting( b != c );
		daw::expecting( b < c );
		daw::expecting( b <= c );
		daw::expecting( c > b );
		daw::expecting( c >= b );
	}
} // namespace test_02

constexpr bool daw_read_only_test_03( ) {
	daw::read_only<int> a{5};
	int b = a;
	daw::expecting( a, b );
	return true;
}
static_assert( daw_read_only_test_03( ) );

int main( ) {
	test_02::daw_read_only_test_02( );
}
