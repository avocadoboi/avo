template<typename, typename>
struct IsSameStruct {
	static constexpr auto value = false;
};
template<typename T>
struct IsSameStruct<T, T> {
	static constexpr auto value = true;
};

template<typename T, typename U>
concept IsSame = IsSameStruct<T, U>::value;

template<typename T>
struct Degrees {};

template<typename T, template<typename> typename U>
concept IsInstantiationOf = requires(T object) {
	{ U{object} } -> IsSame<T>;
};

int main() {
	IsInstantiationOf<Degrees<int>, Degrees>;
}