#include <iostream>

struct unsigned_tag {
};

struct signed_tag {
};

struct size16_tag {
};

struct size32_tag {
};

struct size64_tag {
};

template <typename T>
struct get_size_tag<int> {

};

template <>
struct get_size_tag<16> {
    typedef size16_tag tag;
};

template <>
struct get_size_tag<32> {
    typedef size32_tag tag;
};

template <typename T>
struct number_traits;

template <>
struct number_traits<int32_t> {
    typedef size32_tag size_tag;
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}