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

#include <array>
#include <functional>
#include <type_traits>
#include <utility>

namespace daw {
	namespace detail {
		template<class T, std::size_t N, std::size_t... I>
		constexpr std::array<std::remove_cv_t<T>, N> to_array_impl( T ( &a )[N], std::index_sequence<I...> ) {
			return {{a[I]...}};
		}
	} // namespace detail

	template<class T, std::size_t N>
	constexpr std::array<std::remove_cv_t<T>, N> to_array( T ( &a )[N] ) {
		return detail::to_array_impl( a, std::make_index_sequence<N>{} );
	}

	template<class B>
	struct negation : std::integral_constant<bool, !bool( B::value )> {};

	template<class...>
	struct conjunction : std::true_type {};

	template<class B1>
	struct conjunction<B1> : B1 {};

	template<class B1, class... Bn>
	struct conjunction<B1, Bn...> : std::conditional_t<bool( B1::value ), conjunction<Bn...>, B1> {};

	template<typename... T>
	using conjunction_t = typename conjunction<T...>::type;

	template<typename... T>
	constexpr auto const conjunction_v = conjunction<T...>::value;

	namespace details {
		template<class>
		struct is_ref_wrapper : std::false_type {};

		template<class T>
		struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

		template<class T>
		using not_ref_wrapper = negation<is_ref_wrapper<std::decay_t<T>>>;

		template<class D, class...>
		struct return_type_helper {
			using type = D;
		};
		template<class... Types>
		struct return_type_helper<void, Types...> : std::common_type<Types...> {
			static_assert( conjunction_v<not_ref_wrapper<Types>...>,
			               "Types cannot contain reference_wrappers when D is void" );
		};

		template<class D, class... Types>
		using return_type = std::array<typename return_type_helper<D, Types...>::type, sizeof...( Types )>;
	} // namespace details

	template<class D = void, class... Types>
	constexpr details::return_type<D, Types...> make_array( Types &&... t ) {
		return {std::forward<Types>( t )...};
	}
} // namespace daw
