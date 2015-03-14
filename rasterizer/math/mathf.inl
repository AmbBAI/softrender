
template<class T>
T Mathf::Min(T a, T b)
{
	return std::min(a, b);
}

template<class T>
T Mathf::Min(T a, T b, T c)
{
	return Mathf::Min(a, Mathf::Min(b, c));
}

template<class T>
T Mathf::MinElement(const T* values, int count)
{
	assert(values != nullptr);
	assert(count > 0);

	return *std::min_element(values, values + count);
}

template<class T>
T Mathf::MinElement(const std::vector<T>& values)
{
	return *std::min_element(values.begin(), values.end());
}

template<class T>
T Mathf::Max(T a, T b)
{
	return std::max(a, b);
}

template<class T>
T Mathf::Max(T a, T b, T c)
{
	return Mathf::Max(a, Mathf::Max(b, c));
}

template<class T>
T Mathf::MaxElement(const T* values, int count)
{
	assert(values != nullptr);
	assert(count > 0);

	return *std::max_element(values, values + count);
}

template<class T>
T Mathf::MaxElement(const std::vector<T>& values)
{
	return *std::max_element(values.begin(), values.end());
}

template<class T>
T Mathf::Clamp(T value, T min, T max)
{
	return value > max ? max : Mathf::Max(value, min);
}
