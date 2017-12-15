#ifndef HAVE_CXX11

/*
 * homebrew "nullptr" for C++03.
 * see "Effective C++" ed. 2.
 */
const class nullptr_t {
public:
	template <typename T> operator T*() const { return 0; }
	template <typename C, typename T> operator T C::*() const { return 0; }
 
private:
	void operator&()  const;
} nullptr = {};

#endif
