// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#include <iostream>

#include "boost_test.h"

#include "daw_natural.h"

BOOST_AUTO_TEST_CASE( daw_natural_test_01 ) {
	daw::natural_t<int> a = 5;
	BOOST_REQUIRE_EQUAL( a, 5 );
}

BOOST_AUTO_TEST_CASE( daw_natural_test_02 ) {
	constexpr daw::natural_t<int> a = 2;
	daw::natural_t<int> b = 2;
	BOOST_REQUIRE_EQUAL( a, b );
}

BOOST_AUTO_TEST_CASE( daw_natural_op_plus_01 ) {
	constexpr daw::natural_t<int> a = 1;
	constexpr daw::natural_t<int> b = 2;
	auto result = a + b;

	BOOST_REQUIRE_EQUAL( result, 3 );
}

BOOST_AUTO_TEST_CASE( daw_natural_op_minus_01 ) {
	constexpr daw::natural_t<int> a = 1;
	constexpr daw::natural_t<int> b = 2;
	auto result = b - a;
	BOOST_REQUIRE_EQUAL( result, 1 );
}

BOOST_AUTO_TEST_CASE( daw_natural_op_minus_02 ) {
	daw::natural_t<int> a = 1;
	daw::natural_t<int> b = 2;
	BOOST_REQUIRE_THROW( a - b, daw::exception::arithmetic_exception );
}

BOOST_AUTO_TEST_CASE( daw_natural_op_mul_01 ) {
	constexpr daw::natural_t<int> a = 5;
	constexpr daw::natural_t<int> b = 6;
	auto result = a * b;

	BOOST_REQUIRE_EQUAL( result, 30 );
}

BOOST_AUTO_TEST_CASE( daw_natural_op_div_01 ) {
	constexpr daw::natural_t<int> a = 30;
	constexpr daw::natural_t<int> b = 6;
	auto result = a / b;
	BOOST_REQUIRE_EQUAL( result, 5 );
}

BOOST_AUTO_TEST_CASE( daw_natural_literal_01 ) {
	using namespace daw::literals;
	constexpr auto a = 134_N;
	constexpr auto b = 333_N;
	BOOST_REQUIRE_EQUAL( a, 134 );
	BOOST_REQUIRE_EQUAL( b, 333 );
}

