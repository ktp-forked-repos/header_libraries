// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_optional.h"
#include "daw_traits.h"

namespace daw {
	struct bad_variant_t_access: public std::runtime_error {
		template<typename... Args>
			bad_variant_t_access( Args&&... args ):
				std::runtime_error{ std::forward<Args>( args )... } { }

		~bad_variant_t_access( ) = default;
	};

	template<typename T>
		static auto get_type_index( ) {
			using value_type = std::remove_cv_t<T>;
			return std::type_index( typeid( value_type ) );
		}

	template<typename Type, typename... Types> struct variant_t;
}	// namespace daw


namespace daw {
	template<typename T, typename... Types>  const T & get( daw::variant_t<Types...> const & value );

	namespace tostrings {
		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
			auto to_string( ::std::basic_string<CharT, Traits, Allocator> const & s ) {
				return s;
			}

		template<typename T>
			std::string to_string( T const * ptr ) {
				if( nullptr == ptr ) {
					return "";
				}
				using std::to_string;
				using daw::tostrings::to_string;
				return to_string( *ptr );
			}
	}	// namespace tostrings

	template<typename... Types>
	class generate_to_strings_t {
		template<typename T>
		struct generated_t {
			std::string operator( )( variant_t<Types...> const & value ) const {
				using std::to_string;
				using daw::tostrings::to_string;
				static_assert( sizeof( decltype( to_string( std::declval<T>( ) ) ) ) != 0, "to_string must be defined for type" );
				return to_string( get<T>( value ) );
			}
		}; 	// generated_t
	public:
		template<typename T>
		auto generate( ) const {
			return generated_t<T>{ };
		}
	};	// generate_to_strings_t

	template<typename... Types>
	class generate_compare_t {
		template<typename T>
		static auto constexpr op_compare( variant_t<Types...> const & lhs, variant_t<Types...> const & rhs ) -> std::enable_if_t<daw::traits::operators::has_op_eq<T, T>::value && daw::traits::operators::has_op_lt<T, T>::value, int> {
			auto const & a = lhs.template get<T>( );
			auto const & b = rhs.template get<T>( );
			if( a == b ) {
				return 0;
			} else if( a < b ) {
				return -1;
			}
			return 1;
		}

		template<typename T>
		static auto op_compare( variant_t<Types...> const & lhs, variant_t<Types...> const & rhs ) -> std::enable_if_t<!(daw::traits::operators::has_op_eq<T, T>::value && daw::traits::operators::has_op_lt<T, T>::value), int> {
			return lhs.to_string( ).compare( rhs.to_string( ) );
		}

	public:
		template<typename T>
		auto const & generate( ) const {
			static auto result = []( variant_t<Types...> const & lhs, variant_t<Types...> const & rhs ) {
				if( lhs.type_index( ) == rhs.type_index( ) ) {
					return op_compare<T>( lhs, rhs );
				}
				return lhs.to_string( ).compare( rhs.to_string( ) );

			};
			return result;
		}
	};	// generate_compare_t

	template<typename... Types>
	struct generate_destruct_t {
		template<typename T>
		auto generate( ) const {
			return []( variant_t<Types...> & value ) {
				value.template get<T>( ).~T( );
			};
		}
	};	// generate_destruct_t

	template<typename... Types>
		struct variant_helper_funcs_t {
			using to_string_t = std::function<std::string( variant_t<Types...> const & )>;
			using compare_t = std::function<int( variant_t<Types...> const &, variant_t<Types...> const & )>;
			using destruct_t = std::function<void( variant_t<Types...> & )>;
			to_string_t to_string;	
			compare_t compare;	
			destruct_t destruct;

			variant_helper_funcs_t( ):
				to_string{ },
				compare{ },
				destruct{ } { }

			template<typename ToString, typename Compare, typename Destruct>
				variant_helper_funcs_t( ToString a, Compare b, Destruct c ):
					to_string{ a },
				compare{ b },
				destruct{ c } { }
		};	// variant_helper_funcs_t

	template<typename... Types>
		struct generate_variant_helper_funcs_t {
			generate_to_strings_t<Types...> generate_to_strings;
			generate_compare_t<Types...> generate_compares;
			generate_destruct_t<Types...> generate_destructs;

			template<typename T>
				auto generate( ) const {
					return variant_helper_funcs_t<Types...>{ generate_to_strings. template generate<T>( ), generate_compares. template generate<T>( ), generate_destructs. template generate<T>( ) };
				}
		};	// generate_variant_helper_funcs_t

	template<typename... Stuff>
		void do_nothing(Stuff...) { }

	template<typename... Types>
		auto get_variant_helpers( ) {
			static_assert( sizeof...(Types) > 0, "Must supply at least one type" );

			std::unordered_map<std::type_index, variant_helper_funcs_t<Types...>> results;
			generate_variant_helper_funcs_t<Types...> generate_variant_helper_funcs;

			auto list = { ((void)(results[get_type_index<Types>( )] = generate_variant_helper_funcs.template generate<Types>( )), 0)... };

			do_nothing( list );

			return results;
		}

	template<typename Type, typename... Types>
		struct variant_t {
			static constexpr size_t BUFFER_SIZE = daw::traits::max_sizeof_v<Type,Types...>;
			template<typename T>
				static constexpr bool is_valid_type = daw::traits::is_one_of_v<std::remove_cv_t<T>, std::remove_cv_t<Type>, std::remove_cv_t<Types>...>;
			private:
			alignas(daw::traits::max_sizeof_t<Type,Types...>) std::array<uint8_t, BUFFER_SIZE> m_buffer;
			daw::optional<std::type_index> m_stored_type;

			static auto get_helper_funcs( std::type_index idx ) {
				static auto func_map = get_variant_helpers<Type, Types...>( );
				return func_map[idx];
			}

			template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, std::remove_cv_t<T>>>
				Result * ptr( ) {
					using namespace daw::exception;
					using value_type = std::remove_cv_t<T>;
					daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
					daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ), "Attempt to access a value of another type" );
					static_assert( sizeof(value_type) <= BUFFER_SIZE, "This should never happen.  sizeof(T) does not fit into m_buffer" );
					return reinterpret_cast<value_type *>(static_cast<void*>(m_buffer.data( )));
				}

			template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, std::remove_cv_t<T>>>
			Result const * ptr( ) const {
				using namespace daw::exception;
				using value_type = std::remove_cv_t<T>;
				daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
				daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ), "Attempt to access a value of another type" );
				static_assert( sizeof(value_type) <= BUFFER_SIZE, "This should never happen.  sizeof(T) does not fit into m_buffer" );
				return reinterpret_cast<value_type const *>(static_cast<void const *>(m_buffer.data( )));
			}

			void * raw_ptr( ) {
				return static_cast<void *>(m_buffer.data( ));
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			void set_type( ) {
				m_stored_type = get_type_index<T>( );
				new(raw_ptr( )) T{ }; 
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			void set_type( T value ) {
				if( !is_same_type<T>() ) {
					reset( );
				}
				m_stored_type = get_type_index<T>( );
				void * p = raw_ptr( );
				new (p) T{std::move( value )}; 
			}

			public:
			variant_t( ):
					m_buffer{ },
					m_stored_type{ } { }

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			variant_t( T value ):
					variant_t{ } {

				store( std::move( value ) );
			}

			~variant_t( ) {
				if( m_stored_type ) {
					get_helper_funcs( *m_stored_type ).destruct( *this );
				}
			}

			variant_t( variant_t const & ) = default;
			variant_t & operator=( variant_t const & ) = default;
			variant_t( variant_t && ) = default;
			variant_t & operator=( variant_t && ) = default;

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			variant_t & operator=( T value ) {
				store( std::move( value ) );
				return *this;
			}

			friend void swap( variant_t & lhs, variant_t & rhs ) noexcept {
				lhs.m_buffer.swap( rhs.m_buffer );
				lhs.m_stored_type.swap( rhs.m_stored_type );
			}

			auto type_index( ) const {
				return m_stored_type;
			}

			bool empty( ) const {
				return !static_cast<bool>( m_stored_type );
			}

			explicit operator bool( ) const {
				return static_cast<bool>( m_stored_type );
			}

			void reset( ) {
				if( m_stored_type ) {
					get_helper_funcs( *m_stored_type ).destruct( *this );
				}
				m_stored_type.reset( ); 
			}

			template<typename T>
			bool is_same_type( ) const {
				return m_stored_type == get_type_index<T>( );
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			variant_t & store( T const & value ) {
				set_type<T>( );
				get<T>( ) = value;
				return *this;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
				T & get( ) {
					return *ptr<T>( );
				}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
				T const & get( ) const {
					return *ptr<T>( );
				}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
				explicit operator T const &( ) const {
					return get<T>( );
				}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
				explicit operator T &( ) {
					return get<T>( );
				}

			std::string to_string( ) const {
				if( empty( ) ) {
					return "";
				}
				return get_helper_funcs( *m_stored_type ).to_string( *this );
			}

			std::string operator*( ) const {
				return to_string( );
			}

			auto compare( variant_t const & rhs ) const {
				return get_helper_funcs( *m_stored_type ).compare( *this, rhs );
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			auto compare( T const & rhs ) const {
				return compare( variant_t( rhs ) );
			}

			friend bool operator==( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) == 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator==( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) == 0;
			}

			friend bool operator!=( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) != 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator!=( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) != 0;
			}

			friend bool operator<( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) < 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator<( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) < 0;
			}
			
			friend bool operator>( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) > 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator>( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) > 0;
			}

			friend bool operator<=( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) <= 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator<=( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) <= 0;
			}
			
			friend bool operator>=( variant_t const & lhs, variant_t const & rhs ) {
				return lhs.compare( rhs ) >= 0;
			}

			template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
			friend bool operator>=( variant_t const & lhs, T const & rhs ) {
				return lhs.compare( rhs ) >= 0;
			}


		};	// variant_t

	template<typename... Args>
		std::string to_string( variant_t<Args...> const & value ) {
			return value.to_string( );
		}

	template<typename... Args>
		std::ostream & operator>>( std::ostream & os, variant_t<Args...> const & value ) {
			using std::to_string;
			os << to_string( value );
			return os;
		}

	template<typename Type, typename... Types>
		auto as_variant_t( Type const & value ) {
			return variant_t<Type, Types...>{ }.store( value );
		};

	template<typename T, typename... Types>
		T const & get( daw::variant_t<Types...> const & value ) {
			return value.template get<T>( );
		}

	template<typename T, typename... Types>
		T & get( daw::variant_t<Types...> & value ) {
			return value.template get<T>( );
		}

}	// namespace daw

