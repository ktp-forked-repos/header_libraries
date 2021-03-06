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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "daw/daw_benchmark.h"
#include "daw/daw_container_algorithm.h"
#include "daw/daw_string_split_range.h"

constexpr bool splt_test_000( ) {
	daw::string_view const str = "This is a test of the split";
	std::array<daw::string_view, 7> const expected_tst = {
	  "This", "is", "a", "test", "of", "the", "split"};

	auto rng = daw::split_string( str, " " );

	bool const ans = daw::algorithm::equal(
	  rng.begin( ), rng.end( ), expected_tst.cbegin( ), expected_tst.cend( ) );

	return ans;
}
static_assert( splt_test_000( ) );

void string_split_range_001( ) {
	std::string const str = "This is a test of the split";
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};
	std::string delem = " ";
	daw::string_split_range<char> rng = daw::split_string( str, delem );

	daw::algorithm::transform( rng.begin( ), rng.end( ),
	                           std::back_inserter( tst ),
	                           []( auto sv ) { return sv.to_string( ); } );

	bool const ans = daw::algorithm::equal(
	  tst.cbegin( ), tst.cend( ), expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

void string_split_range_002( ) {
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};

	for( auto sv : daw::split_string( "This is a test of the split", " " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal( tst.cbegin( ), tst.cend( ),
	                             expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

void string_split_range_003( ) {
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};

	for( auto sv :
	     daw::split_string( "This  is  a  test  of  the  split", "  " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal( tst.cbegin( ), tst.cend( ),
	                             expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

int main( ) {
	string_split_range_001( );
	string_split_range_002( );
	string_split_range_003( );
}
