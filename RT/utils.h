#pragma once

#include <random>
#include <type_traits>
#include <optional>

namespace utils {
	inline thread_local std::mt19937 rng;
	inline thread_local std::uniform_real_distribution default_distribution{ 0.0, 1.0 };

	inline double random_double() noexcept {
		return default_distribution(rng);
	}

	inline double random_double(double min, double max) noexcept {
		return default_distribution(rng, std::uniform_real_distribution<double>::param_type{ min, max });
	}

	template <typename T>
	T clamp(T x, T min, T max) {
		return x < min ? min
			 : x > max ? max
			 :           x;
	}

	template <typename T, typename... Ts>
	using is_in_pack = std::disjunction<std::is_same<T, Ts>...>;
	template <typename T, typename... Ts>
	using is_in_pack_t = typename is_in_pack<T, Ts...>::type;
	template <typename T, typename... Ts>
	constexpr auto is_in_pack_v = is_in_pack<T, Ts...>::value;

	template <typename T, typename... Ts>
	using count_in_pack = std::integral_constant < std::size_t, (size_t{ 0 } + ... + (std::is_same_v<T, Ts> ? 1 : 0)) > ;
	template <typename T, typename... Ts>
	using count_in_pack_t = typename count_in_pack<T, Ts...>::type;
	template <typename T, typename... Ts>
	constexpr auto count_in_pack_v = count_in_pack<T, Ts...>::value;

	template <typename... Ts>
	constexpr bool are_distinct_impl = (... && (count_in_pack_v<Ts, Ts...> == 1));

	template <typename... Ts>
	using are_distinct = std::bool_constant<are_distinct_impl<Ts...>>;
	template <typename... Ts>
	using are_distinct_t = typename are_distinct<Ts...>::type;
	template <typename... Ts>
	constexpr auto are_distinct_v = are_distinct<Ts...>::value;

	template <typename T, typename... Ts>
	struct are_same : std::bool_constant<count_in_pack_v<T, Ts...> == sizeof...(Ts)> {};

	template <typename T, typename... Ts>
	struct first_in_pack { using type = T; };

	template <typename T>
	struct is_optional : std::false_type {};

	template <typename T>
	struct is_optional<std::optional<T>> : std::true_type {};

	template <typename T, typename U, typename... Ts>
	struct first_occurance : std::integral_constant<size_t, std::is_same_v<T, U> ? 0 : 1 + first_occurance<T, Ts...>::value> {};

	template <typename T, typename U>
	struct first_occurance<T, U> : std::integral_constant<size_t, std::is_same_v<T, U> ? 0 : 1> {};

	template <typename T, typename U>
	struct tuple_index {
		static_assert(!std::is_void_v<std::void_t<T>>, "Wrong use of tuple_index");
	};

	template <typename T, typename... Ts>
	struct tuple_index<T, std::tuple<Ts...>> : first_occurance<T, Ts...> {};

	template <typename... Tuple_types, typename Callable>
	auto visit_tuple(const std::tuple<Tuple_types...>& tuple, Callable&& callable, size_t index) {
		static_assert(sizeof...(Tuple_types) > 0, "Cannot visit empty tuple");
		static_assert((std::is_invocable_v<Callable, Tuple_types> && ...), "Cannot invoke callable with some of types in tuple");
		static_assert(are_same<decltype((std::forward<Callable>(callable))(std::declval<Tuple_types>()))...>::value, "Return types differ");

		using return_type = decltype(std::forward<Callable>(callable)(std::declval<typename first_in_pack<Tuple_types...>::type>()));

		auto impl = [&]<size_t idx>(std::integral_constant<size_t, idx>, auto& self) ->
			std::conditional_t<std::is_void_v<return_type>,     void,
			std::conditional_t<is_optional<return_type>::value, return_type,
			                                                    std::optional<return_type>
			>>
		{
			if constexpr (idx == sizeof...(Tuple_types)) {
				if constexpr (std::is_void_v<return_type>) {
					return;
				}
				else {
					return {};
				}
			}
			else {
				if (index == idx) {
					return std::forward<Callable>(callable)(std::get<idx>(tuple));
				}
				else {
					return self(std::integral_constant<size_t, idx + 1>{}, self); //recursive lambda
				}
			}
		};

		return impl(std::integral_constant<size_t, 0>{}, impl);
	}
}