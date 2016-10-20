#include <iostream>
#include <exception>
#include <limits>

struct unsigned_tag {
};

struct signed_tag {
};

template<typename T>
struct number_traits;

template<>
struct number_traits<int> {
    typedef signed_tag tag;
};

template<>
struct number_traits<unsigned int> {
    typedef unsigned_tag tag;
};

template<>
struct number_traits<long long> {
    typedef signed_tag tag;
};

template<>
struct number_traits<unsigned long long> {
    typedef unsigned_tag tag;
};

struct overflow_exception : std::exception {
    virtual const char *what() const throw() {
        return "OverFlow";
    }
};

template<typename T>
class checked {
    T value;
public:
    checked() {
    }

    checked(T value) : value(value) {
    };

    T get_value() const {
        return value;
    }
};

template<typename T>
checked<T> sum(const checked<T>& a, const checked<T>& b, unsigned_tag) {
    T max = std::numeric_limits<T>::max();
    if (max - b.get_value() < a.get_value()) {
        throw overflow_exception();
    } else {
        return a.get_value() + b.get_value();
    }
}

template<typename T>
checked<T> sum(const checked<T>& a, const checked<T>& b, signed_tag) {
    T max = std::numeric_limits<T>::max();
    T min = std::numeric_limits<T>::min();
    T bMax = (a.get_value() > 0 ? max - a.get_value() : max);
    T bMin = (a.get_value() < 0 ? max - a.get_value() : min);
    if (!(bMin <= b.get_value() && b.get_value() <= bMax)) {
        throw overflow_exception();
    } else {
        return a.get_value() + b.get_value();
    }
}

template<typename T>
checked<T> operator+(const checked<T>& a, const checked<T>& b) {
    return sum(a, b, typename number_traits<T>::tag());
}

template<typename T>
checked<T> subtract(const checked<T>& a, const checked<T>& b, unsigned_tag) {
    if (a.get_value() < b.get_value()) {
        throw overflow_exception();
    } else {
        return a.get_value() - b.get_value();
    }
}

template<typename T>
checked<T> subtract(const checked<T>& a, const checked<T>& b, signed_tag) {
    T max = std::numeric_limits<T>::max();
    T min = std::numeric_limits<T>::min();
    T aMin = (b.get_value() > 0 ? (min + b.get_value()) : min);
    T aMax = (b.get_value() < 0 ? (max + b.get_value()) : max);
    if (!(aMin <= a.get_value() && a.get_value() <= aMax)) {
        throw overflow_exception();
    } else {
        return a.get_value() - b.get_value();
    }
}

template<typename T>
checked<T> operator-(const checked<T>& a, const checked<T>& b) {
    return subtract(a, b, typename number_traits<T>::tag());
}

template<typename T>
checked<T> multiply(const checked<T>& a, const checked<T>& b, unsigned_tag) {
    if (a.get_value() == 0) {
        return 0;
    }
    T max = std::numeric_limits<T>::max();
    if (max / a.get_value() < b.get_value()) {
        throw overflow_exception();
    } else {
        return a.get_value() * b.get_value();
    }
}

template<typename T>
checked<T> multiply(const checked<T>& a, const checked<T>& b, signed_tag) {
    if (a.get_value() == 0) {
        return a.get_value() * b.get_value();
    }
    T max = std::numeric_limits<T>::max();
    T min = std::numeric_limits<T>::min();
    if (a.get_value() == -1) {
        if (b.get_value() == min) {
            throw overflow_exception();
        } else {
            return a.get_value() * b.get_value();
        }
    }
    T bMax = max / a.get_value();
    T bMin = min / a.get_value();
    if (bMax < bMin) {
        T w = bMin;
        bMin = bMax;
        bMax = w;
    }
    //std::cout << "multiply: " << a.get_value() << ' ' << b.get_value() << "| " << bMin << " " << bMax << std::endl;
    if (!(bMin <= b.get_value() && b.get_value() <= bMax)) {
        throw overflow_exception();
    } else {
        return a.get_value() * b.get_value();
    }
}

template<typename T>
checked<T> operator*(const checked<T>& a, const checked<T>& b) {
    return multiply(a, b, typename number_traits<T>::tag());
}

template<typename T>
checked<T> divide(const checked<T>& a, const checked<T>& b, unsigned_tag) {
    return a.get_value() / b.get_value();
}

template<typename T>
checked<T> divide(const checked<T>& a, const checked<T>& b, signed_tag) {
    if (b.get_value() == -1 && a.get_value() == std::numeric_limits<T>::min()) {
        throw overflow_exception();
    }
    return a.get_value() / b.get_value();
}

template<typename T>
checked<T> operator/(const checked<T>& a, const checked<T>& b) {
    if (b.get_value() == 0) {
        throw overflow_exception();
    }
    return divide(a, b, typename number_traits<T>::tag());
}

template<typename T>
checked<T> negation(const checked<T>& value, unsigned_tag) {
    if (value.get_value() == 0) {
        return value;
    } else {
        throw overflow_exception();
    }
}

template<typename T>
checked<T> negation(const checked<T>& value, signed_tag) {
    if (value.get_value() == std::numeric_limits<T>::min()) {
        throw overflow_exception();
    }
    return -value.get_value();
}

template<typename T>
checked<T> operator-(const checked<T>& value) {
    return negation(value, typename number_traits<T>::tag());
}

int main() {
    int signed_const[5] = {std::numeric_limits<int>::min(), -1, 0, 1, std::numeric_limits<int>::max()};
    unsigned int unsigned_const[5] = {0, 1, std::numeric_limits<unsigned int>::max()};

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            checked<int> a(signed_const[i]), b(signed_const[j]);

            std::cout << a.get_value() << " + " << b.get_value() << ": ";
            try {
                std::cout << (a + b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " - " << b.get_value() << ": ";
            try {
                std::cout << (a - b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " * " << b.get_value() << ": ";
            try {
                std::cout << (a * b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " / " << b.get_value() << ": ";
            try {
                std::cout << (a / b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << "-" << a.get_value() << ": ";
            try {
                std::cout << (-a).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;
            std::cout << "______________________________________________________________" << std::endl;
        }
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "----------------------------------------------------------------------\n";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            checked<unsigned int> a(unsigned_const[i]), b(unsigned_const[j]);

            std::cout << a.get_value() << " + " << b.get_value() << ": ";
            try {
                std::cout << (a + b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " - " << b.get_value() << ": ";
            try {
                std::cout << (a - b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " * " << b.get_value() << ": ";
            try {
                std::cout << (a * b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << a.get_value() << " / " << b.get_value() << ": ";
            try {
                std::cout << (a / b).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;

            std::cout << "-" << a.get_value() << ": ";
            try {
                std::cout << (-a).get_value();
            } catch (overflow_exception &e) {
                std::cout << "Exception: " << e.what();
            }
            std::cout << std::endl;
            std::cout << "______________________________________________________________" << std::endl;
        }
    }

    return 0;
}
