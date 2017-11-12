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

#include <iterator>
#include <string>
#include <tuple>
#include <utility>

#include "daw_function.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		struct parser_exception {};
		struct invalid_input_exception : parser_exception {};
		struct empty_input_exception : invalid_input_exception {};
		struct numeric_overflow_exception : invalid_input_exception {};

		namespace converters {
			constexpr char parse_to_value( daw::string_view str, char ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return str.front( );
			}

			namespace helpers {
				template<typename Result>
				constexpr Result parse_int( daw::string_view &str ) {
					intmax_t count = std::numeric_limits<Result>::digits10;
					Result result = 0;
					bool is_neg = false;
					if( '-' == str.front( ) ) {
						if( !std::numeric_limits<Result>::is_signed ) {
							throw invalid_input_exception{};
						}
						is_neg = true;
						str.remove_prefix( );
					}
					while( count > 0 && !str.empty( ) && is_number( str.front( ) ) ) {
						result *= static_cast<Result>( 10 );
						result += static_cast<Result>( str.front( ) - '0' );
						--count;
						str.remove_prefix( );
					}
					if( !str.empty( ) ) {
						throw numeric_overflow_exception{};
					}
					if( is_neg ) {
						result *= static_cast<Result>( -1 );
					}
					return result;
				}

				template<typename Result>
				constexpr Result parse_unsigned_int( daw::string_view &str ) {
					if( '-' == str.front( ) ) {
						throw invalid_input_exception{};
					}
					return parse_int<Result>( str );
				}

			} // namespace helpers

			template<typename T,
			         std::enable_if_t<!is_same_v<T, char> && is_integral_v<T> && is_signed_v<T>, std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, T ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return helpers::parse_int<T>( str );
			}

			template<typename T, std::enable_if_t<is_integral_v<T> && is_unsigned_v<T>, std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, T ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return helpers::parse_unsigned_int<T>( str );
			}

			namespace impl {
				constexpr bool is_quote( char const last_char, char const c ) noexcept {
					return last_char != '\\' && c == '"';
				}
			} // namespace impl

			struct unquoted_string {};
			struct unquoted_string_view{};

			constexpr daw::string_view parse_to_value( daw::string_view str, unquoted_string_view ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return str;
			}

			constexpr daw::string_view parse_to_value( daw::string_view str, daw::string_view ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				if( str.size( ) < 2 ) {
					struct input_too_small_exception {};
					throw input_too_small_exception{};
				}
				if( str.front( ) != '"' ) {
					struct missing_expected_quotes_exception {};
					throw missing_expected_quotes_exception{};
				}
				str.remove_prefix( );
				auto const first = str.cbegin( );
				char last_char = str.pop_front( );
				while( !str.empty( ) && !impl::is_quote( last_char, str.front( ) ) ) {
					last_char = str.pop_front( );
				}
				if( str.front( ) != '"' ) {
					struct missing_expected_quotes_exception {};
					throw missing_expected_quotes_exception{};
				}
				return daw::make_string_view_it( first, str.cbegin( ) );
			}

			std::string parse_to_value( daw::string_view str, std::string ) {
				return parse_to_value( str, daw::string_view{} ).to_string( );
			}

			std::string parse_to_value( daw::string_view str, unquoted_string ) {
				return parse_to_value( str, unquoted_string_view{} ).to_string( );
			}

			float parse_to_value( daw::string_view str, float ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtof( s.c_str( ), end );
			}

			double parse_to_value( daw::string_view str, double ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtod( s.c_str( ), end );
			}

			long double parse_to_value( daw::string_view str, long double ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtold( s.c_str( ), end );
			}
		} // namespace converters

		namespace impl {
			template<size_t N, typename... Args, std::enable_if_t<( N == 0 ), std::nullptr_t> = nullptr, typename Tuple,
			         typename Splitter>
			constexpr void set_value_from_string_view( Tuple &, daw::string_view, Splitter ) {}

			template<size_t N, typename... Args, std::enable_if_t<( N > 0 ), std::nullptr_t> = nullptr, typename Tuple,
			         typename Splitter>
			constexpr void set_value_from_string_view( Tuple &tp, daw::string_view str, Splitter splitter ) {
				static_assert( N <= sizeof...( Args ), "Invalud value of N" );
				auto const end_pos = splitter( str );
				if( N > 1 && end_pos.first == str.npos ) {
					throw invalid_input_exception{};
				}
				using namespace ::daw::parser::converters;
				using pos_t = std::integral_constant<size_t, sizeof...( Args ) - N>;
				using value_t = std::decay_t<decltype( std::get<pos_t::value>( std::declval<std::tuple<Args...>>( ) ) )>;

				std::get<pos_t::value>( tp ) = parse_to_value( str.substr( 0, end_pos.first ), value_t{} );

				str.remove_prefix( end_pos.last );
				daw::parser::impl::set_value_from_string_view<N - 1, Args...>( tp, std::move( str ), std::move( splitter ) );
			}
		} // namespace impl
		class default_splitter {
			daw::string_view m_delemiter;

		public:
			constexpr default_splitter( daw::string_view delemiter ) : m_delemiter{std::move( delemiter )} {}
			constexpr default_splitter( ) noexcept = delete;
			~default_splitter( ) noexcept = default;
			constexpr default_splitter( default_splitter const & ) noexcept = default;
			constexpr default_splitter( default_splitter && ) noexcept = default;
			constexpr default_splitter &operator=( default_splitter const & ) noexcept = default;
			constexpr default_splitter &operator=( default_splitter && ) noexcept = default;

			constexpr auto operator( )( daw::string_view str ) const noexcept {
				struct result_t {
					typename daw::string_view::size_type first;
					typename daw::string_view::size_type last;
				};
				auto const pos = str.find( m_delemiter );
				if( pos < str.size( ) ) {
					return result_t{pos, pos + m_delemiter.size( )};
				}
				return result_t{str.npos, str.npos};
			}
		};

		template<bool skip_multiple>
		struct basic_whitespace_splitter {
			constexpr auto operator( )( daw::string_view str ) const {
				struct result_t {
					typename daw::string_view::size_type first;
					typename daw::string_view::size_type last;
				};
				typename daw::string_view::size_type n = 0;
				size_t const sz = str.size( );
				while( n < sz && not_unicode_whitespace( str[n] ) ) {
					++n;
				}
				if( n == sz ) {
					return result_t{str.npos, str.npos};
				}
				if( !skip_multiple ) {
					return result_t{n, n + 1};
				}
				auto const f = n;
				while( n < sz && is_unicode_whitespace( str[n] ) ) {
					++n;
				}
				return result_t{f, n};
			}
		};

		using whitespace_splitter = basic_whitespace_splitter<true>;
		using single_whitespace_splitter = basic_whitespace_splitter<false>;

		namespace impl {
			template<typename T>
			class parse_result_of {
				static auto get_type( ) noexcept {
					using namespace ::daw::parser::converters;
					return parse_to_value( daw::string_view{}, T{} );
				}

			public:
				using type = std::decay_t<decltype( get_type( ) )>;
			};

			template<typename T>
			using parse_result_of_t = typename parse_result_of<T>::type;
		} // namespace impl

		template<typename... Args, typename Splitter,
		         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
		constexpr decltype( auto ) parse_to( daw::string_view str, Splitter splitter ) {
			static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );
			std::tuple<impl::parse_result_of_t<Args>...> result{};
			impl::set_value_from_string_view<sizeof...( Args ), Args...>( result, std::move( str ), std::move( splitter ) );
			return result;
		}

		template<typename... Args>
		constexpr decltype( auto ) parse_to( daw::string_view str, daw::string_view delemiter ) {
			return parse_to<Args...>( std::move( str ), default_splitter{std::move( delemiter )} );
		}

		template<typename... Args>
		constexpr decltype( auto ) parse_to( daw::string_view str ) {
			return parse_to<Args...>( std::move( str ), daw::string_view{" "} );
		}
	} // namespace parser

	namespace impl {
		template<typename T>
		struct make_a {
			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&... args ) const {
				return T{std::forward<Args>( args )...};
			}
		};
	} // namespace impl

	template<typename Destination, typename... ExpectedArgs, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) construct_from( daw::string_view str, Splitter splitter ) {
		static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );

		return daw::apply( impl::make_a<Destination>{},
		                   parser::parse_to<ExpectedArgs...>( std::move( str ), std::move( splitter ) ) );
	}

	template<typename Destination, typename... ExpectedArgs>
	constexpr decltype( auto ) construct_from( daw::string_view str, daw::string_view delemiter ) {
		return construct_from<Destination, ExpectedArgs...>( std::move( str ),
		                                                     parser::default_splitter{std::move( delemiter )} );
	}

	namespace impl {
		template<typename... Args, typename Callable, typename Splitter>
		constexpr decltype( auto ) apply_string_impl( std::tuple<Args...>, Callable callable, daw::string_view str,
		                                              Splitter splitter ) {
			return daw::apply( std::move( callable ), parser::parse_to<Args...>( std::move( str ), std::move( splitter ) ) );
		}
	} // namespace impl
	template<typename Callable, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string( Callable callable, daw::string_view str, Splitter splitter ) {
		static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );
		using ftraits = typename daw::function_info<decltype( callable )>::decayed_args_tuple;
		return impl::apply_string_impl( ftraits{}, std::move( callable ), std::move( str ), std::move( splitter ) );
	}

	template<typename Callable>
	constexpr decltype( auto ) apply_string( Callable callable, daw::string_view str, daw::string_view delemiter ) {
		return apply_string<Callable>( std::move( callable ), std::move( str ),
		                               parser::default_splitter{std::move( delemiter )} );
	}
	namespace impl {
		template<size_t CallableArgsArity, size_t ParsedArgsArity>
		class ArityCheckEqual {
			static_assert( CallableArgsArity == ParsedArgsArity, "Callable args arity does not match number of arguments" );
		};
	} // namespace impl

	template<typename... Args, typename Callable, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string2( Callable callable, daw::string_view str, Splitter splitter ) {
		static_cast<void>( impl::ArityCheckEqual<daw::function_info<Callable>::arity, sizeof...( Args )>{} );
		return daw::apply( std::move( callable ), parser::parse_to<Args...>( std::move( str ), std::move( splitter ) ) );
	}

	template<typename... Args, typename Callable>
	constexpr decltype( auto ) apply_string2( Callable callable, daw::string_view str, daw::string_view delemiter ) {
		return apply_string2<Args...>( std::move( callable ), std::move( str ),
		                               parser::default_splitter{std::move( delemiter )} );
	}

	namespace detectors {
		template<typename Stream>
		using has_str = decltype( Stream{}.str( ) );
	}

	template<
	  typename... Args, typename Stream, typename Splitter,
	  std::enable_if_t<(!is_detected_v<detectors::has_str, Stream> && !is_convertible_v<Splitter, daw::string_view>),
	                   std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&s, Splitter splitter ) {

		std::string const str{std::istreambuf_iterator<char>{s}, {}};
		return parser::parse_to<Args...>( daw::string_view{str}, std::move( splitter ) );
	}

	template<
	  typename... Args, typename Stream, typename Splitter,
	  std::enable_if_t<(is_detected_v<detectors::has_str, Stream> && !is_convertible_v<Splitter, daw::string_view>),
	                   std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&s, Splitter splitter ) {

		return parser::parse_to<Args...>( daw::string_view{s.str( )}, std::move( splitter ) );
	}

	template<typename... Args, typename Stream>
	decltype( auto ) values_from_stream( Stream &&s, daw::string_view delemiter ) {
		return values_from_stream<Args...>( std::forward<Stream>( s ), parser::default_splitter{std::move( delemiter )} );
	}
} // namespace daw
