// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_function.h"

constexpr int func( int ) noexcept {
	return 1;
}
constexpr void func2( int ) noexcept {
	return;
}

constexpr bool test_001( ) {
	auto call = []( int ) noexcept {
		return 0;
	};

	auto c0 = daw::make_callable( func );
	auto c1 = daw::make_callable( call );

	return c0( 5 ) + c1( 10 ) == 1;
}
static_assert( test_001( ) );

constexpr bool test_002( ) {
	auto call = []( int ) noexcept {
		return;
	};

	daw::make_callable( func2 );
	daw::make_callable( call );

	return true;
}
static_assert( test_002( ) );

int main( ) {}
