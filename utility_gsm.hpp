#ifndef UTILITY_GSM_H_
#define UTILITY_GSM_H_

struct SIM900{};
struct SIM900A{};
struct SIM800L{};

// a polyfill of meta-function
template<typename T, T v>
struct integral_constant {
    static const T value = v;
};

typedef integral_constant<bool, true>   true_type;
typedef integral_constant<bool, false>  false_type;

// simple implemenation of enable if 
template<bool Condition, typename T = void>
struct enable_if{};

template<typename T>
struct enable_if<true, T>{
    typedef T type;
};

// SIM900A
template<typename>
struct is_sim900a : false_type {};

template<>
struct is_sim900a<SIM900A> : true_type {};

// SIM900
template<typename>
struct is_sim900 : false_type {};

template<>
struct is_sim900<SIM900> : true_type {};

// SIM800L
template<typename>
struct is_sim800l : false_type {};

template<>
struct is_sim800l<SIM800L> : true_type {};

#endif