// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw/cpp_17.h"
#include "daw/iterator/daw_indexed_iterator.h"

constexpr bool array_test_001( ) {
		std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		return *f == 1;
}
static_assert( array_test_001, "" );

constexpr bool array_test_002( ) {
		std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
}
static_assert( array_test_002, "" );

constexpr bool array_test_003( ) {
		std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		return *f == 1;
}
static_assert( array_test_003, "" );

constexpr bool array_test_004( ) {
		std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
}
static_assert( array_test_004, "" );

constexpr bool carray_test_001( ) {
		int numbers[7] = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		return *f == 1;
}
static_assert( carray_test_001, "" );

constexpr bool carray_test_002( ) {
	int numbers[7] = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
}
static_assert( carray_test_002, "" );

constexpr bool carray_test_003( ) {
		int const numbers[7] = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		return *f == 1;
}
static_assert( carray_test_003, "" );

constexpr bool carray_test_004( ) {
	int const numbers[7] = {1, 2, 3, 4, 5, 6, 7};
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
}
static_assert( carray_test_004, "" );