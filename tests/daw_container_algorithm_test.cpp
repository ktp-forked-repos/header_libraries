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

#include <algorithm>
#include <boost/iterator/counting_iterator.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost_test.h"
#include "daw_container_algorithm.h"

BOOST_AUTO_TEST_CASE( container_algorithm_accumulate ) {
	std::vector<int> test( 100, 1 );
	auto const sum = daw::container::accumulate( test, 0 );
	BOOST_REQUIRE_EQUAL( test.size( ), sum );

	auto const product = daw::container::accumulate( test, 1, []( auto lhs, auto rhs ) { return lhs * rhs; } );
	BOOST_REQUIRE_EQUAL( product, 1 );
}

BOOST_AUTO_TEST_CASE( container_algorithm_transform ) {
	std::vector<int> test_vec{ boost::counting_iterator<int>( 1 ), boost::counting_iterator<int>( 100 ) };
	std::vector<int> result{ };
	result.resize( test_vec.size( ) );

	daw::container::transform( test_vec, std::back_inserter( result ), []( int const &val ) { return 2 * val; } );

	auto const sum1 = daw::container::accumulate( test_vec, 0 );
	auto const sum2 = daw::container::accumulate( result, 0 );

	BOOST_REQUIRE_EQUAL( sum1 * 2, sum2 );
}

BOOST_AUTO_TEST_CASE( daw_container_algorithm_test_sort ) {
	std::vector<int64_t> v1{1000};
	std::iota( std::begin( v1 ), std::end( v1 ), 1 );
	daw::container::sort( v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
	BOOST_REQUIRE( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_container_algorithm_test_stable_sort ) {
	std::vector<int64_t> v1{1000};
	std::iota( std::begin( v1 ), std::end( v1 ), 1 );
	daw::container::stable_sort( v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
	BOOST_REQUIRE( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_container_algorithm_test_max_element ) {
	std::vector<int64_t> v1{1000};
	std::iota( std::begin( v1 ), std::end( v1 ), 1 );
	daw::container::max_element( v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
}

BOOST_AUTO_TEST_CASE( daw_container_algorithm_test_copy_001 ) {
	std::array<int, 100> a1{};
	std::iota( std::begin( a1 ), std::end( a1 ), 2 );
	std::array<int, 100> a2{};
	daw::container::copy( a1, a2.begin( ) );
	BOOST_REQUIRE( std::equal( std::cbegin( a1 ), std::cend( a1 ), std::cbegin( a2 ), std::cend( a2 ) ) );
}

BOOST_AUTO_TEST_CASE( daw_container_algorithm_test_copy_n_001 ) {
	std::array<int, 100> a1{};
	std::iota( std::begin( a1 ), std::end( a1 ), 2 );
	std::array<int, 100> a2{};
	daw::container::copy_n( a1, 100, a2.begin( ) );
	BOOST_REQUIRE( std::equal( std::cbegin( a1 ), std::cend( a1 ), std::cbegin( a2 ), std::cend( a2 ) ) );
}