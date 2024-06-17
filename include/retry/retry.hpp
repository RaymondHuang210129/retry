#pragma once
#include <chrono>
#include <functional>
#include <set>
#include <thread>
#include <tuple>
#include <type_traits>

namespace tsejhuang {

namespace internal {

// Helper type traits to determine if a type is a std::reference_wrapper
template <typename Type>
struct is_reference_wrapper : std::false_type {};

template <typename Type>
struct is_reference_wrapper<std::reference_wrapper<Type>> : std::true_type {};

template <typename Type>
inline constexpr bool is_reference_wrapper_v =
    is_reference_wrapper<Type>::value;

// Helper template to determine if the return value is in the retry_values
template <typename RetryValuesType, typename ReturnValueType>
bool is_in_retry_values(const RetryValuesType& retry_values,
                        const ReturnValueType& return_value) {
    return std::find_if(
               std::begin(retry_values), std::end(retry_values),
               [&return_value](const auto& retry_value) {
                   if constexpr (is_reference_wrapper_v<ReturnValueType>) {
                       return return_value.get() == retry_value.get();
                   } else {
                       return return_value == retry_value;
                   }
               }) != std::end(retry_values);
}

// Core function that retries a function until it returns a value that is
// not in the retry_values set or until the max_retry_period has elapsed
template <typename FunctionType, typename ReturnType, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType _retry_until(const FunctionType& func,
                        const ReturnType (&retry_values)[ArraySize],
                        const std::chrono::nanoseconds& max_retry_period,
                        const std::chrono::nanoseconds& retry_interval,
                        ParamTypes&&... params) {
    ReturnType return_value = retry_values[0];
    const auto start_time = std::chrono::steady_clock::now();
    auto next_wakeup_time = start_time + retry_interval;

    do {
        return_value = func(std::forward<ParamTypes>(params)...);
        if (!is_in_retry_values(retry_values, return_value)) {
            return return_value;
        }
    } while (std::chrono::steady_clock::now() - start_time < max_retry_period &&
             [&] {
                 std::this_thread::sleep_until(next_wakeup_time);
                 next_wakeup_time = next_wakeup_time + retry_interval;
                 return true;
             }());
    return return_value;
}

// Core function that retries a function until it returns a value that is
// not in the retry_values set or until the max_attempts have been exhausted
template <typename FunctionType, typename ReturnType, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType _retry_until(const FunctionType& func,
                        const ReturnType (&retry_values)[ArraySize],
                        unsigned int max_attempts,
                        const std::chrono::nanoseconds& retry_interval,
                        ParamTypes&&... params) {
    ReturnType return_value = retry_values[0];
    const auto start_time = std::chrono::steady_clock::now();
    auto next_wakeup_time = start_time + retry_interval;

    do {
        return_value = func(std::forward<ParamTypes>(params)...);
        if (!is_in_retry_values(retry_values, return_value)) {
            return return_value;
        }
    } while (--max_attempts > 0 && [&] {
        std::this_thread::sleep_for(retry_interval);
        return true;
    }());
    return return_value;
}

}  // namespace internal

/**
 * @brief Retries a given lambda function for a specified period of time until
 * it returns a value that is not in the retry_values array.
 *
 * This function retries a given lambda function until it returns a value that
 * is not in the `retry_values` array or until `max_retry_period` has elapsed.
 * With C++17 CTAD, all template arguments can be deduced. Users can pass a
 * lambda function, a contructor initializer list of retry values, the maximum
 * retry period, and the arguments that used to pass to the lambda function.
 *
 * @param lambda The lambda function to be retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param params The arguments to be passed to the lambda function.
 * @return The latest return value of the lambda function.
 */
template <typename LambdaType, typename ReturnType, unsigned long ArraySize,
          typename... ArgTypes>
ReturnType retry_until(const LambdaType&& lambda,
                       const ReturnType (&retry_values)[ArraySize],
                       const std::chrono::nanoseconds& max_retry_period,
                       ArgTypes&&... args) {
    return internal::_retry_until(lambda, retry_values, max_retry_period,
                                  std::chrono::seconds{0},
                                  std::forward<ArgTypes>(args)...);
}

/**
 * @brief Retries a given free function or a static class method for a specified
 * period of time until it returns a value that is not in the retry_values
 * array.
 *
 * This function retries a given free function or a static class method until it
 * returns a value that is not in the `retry_values` array or until
 * `max_retry_period` has elapsed. With C++17 CTAD, all template arguments can
 * be deduced. Users can pass a function pointer, a contructor initializer list
 * of retry values, the maximum retry period, and the arguments that used to
 * pass to the function.
 *
 * @param func_ptr The pointer of free function or static class method to be
 * retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param params The arguments to be passed to the function.
 * @return The latest return value of the function.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_until(ReturnType (*func_ptr)(ArgTypes...),
                       const ReturnType (&retry_values)[ArraySize],
                       const std::chrono::nanoseconds& max_retry_period,
                       ParamTypes&&... params) {
    return internal::_retry_until(func_ptr, retry_values, max_retry_period,
                                  std::chrono::seconds{0},
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given std::function or a static class method for a specified
 * period of time until it returns a value that is not in the retry_values
 * array.
 *
 * This function retries to call given std::function object until it
 * returns a value that is not in the `retry_values` array or until
 * `max_retry_period` has elapsed. With C++17 CTAD, all template arguments can
 * be deduced. Users can pass a function pointer, a contructor initializer list
 * of retry values, the maximum retry period, and the arguments that used to
 * pass to the std::function.
 *
 * @param function The std::function object to be retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param params The arguments to be passed to the std::function.
 * @return The latest return value of the std::function object.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_until(const std::function<ReturnType(ArgTypes...)>& function,
                       const ReturnType (&retry_values)[ArraySize],
                       const std::chrono::nanoseconds& max_retry_period,
                       ParamTypes&&... params) {
    return internal::_retry_until(function, retry_values, max_retry_period,
                                  std::chrono::seconds{0},
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given lambda function for specified attempts until
 * it returns a value that is not in the retry_values array.
 *
 * This function retries a given lambda function until it returns a value that
 * is not in the `retry_values` array or until `max_attempts` has reached.
 * With C++17 CTAD, all template arguments can be deduced. Users can pass a
 * lambda function, a contructor initializer list of retry values, the maximum
 * attempts to call the function, and the arguments that used to pass to the
 * lambda function.
 *
 * @param lambda The lambda function to be retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the function.
 * @param params The arguments to be passed to the lambda function.
 * @return The latest return value of the lambda function.
 */
template <typename LambdaType, typename ReturnType, unsigned long ArraySize,
          typename... ArgTypes>
ReturnType retry_until(const LambdaType&& lambda,
                       const ReturnType (&retry_values)[ArraySize],
                       unsigned int max_attempts, ArgTypes&&... args) {
    return internal::_retry_until(lambda, retry_values, max_attempts,
                                  std::chrono::seconds{0},
                                  std::forward<ArgTypes>(args)...);
}

/**
 * @brief Retries a given free function or a static class method for specified
 * attempts until it returns a value that is not in the retry_values array.
 *
 * This function retries a given free function or a static class method until it
 * returns a value that is not in the `retry_values` array or until
 * `max_attempts` has reached. With C++17 CTAD, all template arguments can
 * be deduced. Users can pass a function pointer, a contructor initializer list
 * of retry values, the maximum attempts to call the function, and the arguments
 * that used to pass to the function.
 *
 * @param func_ptr The pointer of free function or static class method to be
 * retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the function.
 * @param params The arguments to be passed to the function.
 * @return The latest return value of the function.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_until(ReturnType (*func_ptr)(ArgTypes...),
                       const ReturnType (&retry_values)[ArraySize],
                       unsigned int max_attempts, ParamTypes&&... params) {
    return internal::_retry_until(func_ptr, retry_values, max_attempts,
                                  std::chrono::seconds{0},
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given std::function or a static class method for specified
 * attempts until it returns a value that is not in the retry_values array.
 *
 * This function retries to call given std::function object until it
 * returns a value that is not in the `retry_values` array or until
 * `max_attempts` has reached. With C++17 CTAD, all template arguments can
 * be deduced. Users can pass a function pointer, a contructor initializer list
 * of retry values, the maximum attempts to call the function, and the arguments
 * that used to pass to the std::function.
 *
 * @param function The std::function object to be retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the std::function.
 * @param params The arguments to be passed to the std::function.
 * @return The latest return value of the std::function object.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_until(const std::function<ReturnType(ArgTypes...)>& function,
                       const ReturnType (&retry_values)[ArraySize],
                       unsigned int max_attempts, ParamTypes&&... params) {
    return internal::_retry_until(function, retry_values, max_attempts,
                                  std::chrono::seconds{0},
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given lambda function at intervals until it returns a value
 * that is not in the retry_values array or until the max_retry_period has
 * elapsed.
 *
 * This function retries a given lambda function at intervals until it returns a
 * value that is not in the `retry_values` array or until `max_retry_period` has
 * elapsed. With C++17 CTAD, all template arguments can be deduced. Users can
 * pass a lambda function, a contructor initializer list of retry values, the
 * maximum retry period, the interval between each retry, and the arguments that
 * used to pass to the lambda function.
 *
 * @param lambda The lambda function to be retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the lambda function.
 * @return The latest return value of the lambda function.
 */
template <typename LambdaType, typename ReturnType, unsigned long ArraySize,
          typename... ArgTypes>
ReturnType retry_at_intervals_until(
    const LambdaType&& lambda, const ReturnType (&retry_values)[ArraySize],
    const std::chrono::nanoseconds& max_retry_period,
    const std::chrono::nanoseconds interval, ArgTypes&&... args) {
    return internal::_retry_until(lambda, retry_values, max_retry_period,
                                  interval, std::forward<ArgTypes>(args)...);
}

/**
 * @brief Retries a given free function or a static class method at intervals
 * until it returns a value that is not in the retry_values array or until the
 * max_retry_period has elapsed.
 *
 * This function retries a given free function or a static class method at
 * intervals until it returns a value that is not in the `retry_values` array or
 * until `max_retry_period` has elapsed. With C++17 CTAD, all template arguments
 * can be deduced. Users can pass a function pointer, a contructor initializer
 * list of retry values, the maximum retry period, the interval between each
 * retry, and the arguments that used to pass to the function.
 *
 * @param func_ptr The pointer of free function or static class method to be
 * retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the function.
 * @return The latest return value of the function.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_at_intervals_until(
    ReturnType (*func_ptr)(ArgTypes...),
    const ReturnType (&retry_values)[ArraySize],
    const std::chrono::nanoseconds& max_retry_period,
    const std::chrono::nanoseconds& interval, ParamTypes&&... params) {
    return internal::_retry_until(func_ptr, retry_values, max_retry_period,
                                  interval,
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given std::function or a static class method at intervals
 * until it returns a value that is not in the retry_values array or until the
 * max_retry_period has elapsed.
 *
 * This function retries to call given std::function object at intervals until
 * it returns a value that is not in the `retry_values` array or until
 * `max_retry_period` has elapsed. With C++17 CTAD, all template arguments can
 * be deduced. Users can pass a function pointer, a contructor initializer list
 * of retry values, the maximum retry period, the interval between each retry,
 * and the arguments that used to pass to the std::function.
 *
 * @param function The std::function object to be retried.
 * @param retry_values The array of retry values.
 * @param max_retry_period The maximum retry period.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the std::function.
 * @return The latest return value of the std::function object.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_at_intervals_until(
    const std::function<ReturnType(ArgTypes...)>& function,
    const ReturnType (&retry_values)[ArraySize],
    const std::chrono::nanoseconds& max_retry_period,
    const std::chrono::nanoseconds& interval, ParamTypes&&... params) {
    return internal::_retry_until(function, retry_values, max_retry_period,
                                  interval,
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given lambda function at intervals until it returns a value
 * that is not in the retry_values array or until the max_attempts have been
 * exhausted.
 *
 * This function retries a given lambda function at intervals until it returns a
 * value that is not in the `retry_values` array or until `max_attempts` have
 * been exhausted. With C++17 CTAD, all template arguments can be deduced. Users
 * can pass a lambda function, a contructor initializer list of retry values,
 * the maximum attempts to call the function, the interval between each retry,
 * and the arguments that used to pass to the lambda function.
 *
 * @param lambda The lambda function to be retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the function.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the lambda function.
 * @return The latest return value of the lambda function.
 */
template <typename LambdaType, typename ReturnType, unsigned long ArraySize,
          typename... ArgTypes>
ReturnType retry_at_intervals_until(const LambdaType&& lambda,
                                    const ReturnType (&retry_values)[ArraySize],
                                    unsigned int max_attempts,
                                    const std::chrono::nanoseconds& interval,
                                    ArgTypes&&... args) {
    return internal::_retry_until(lambda, retry_values, max_attempts, interval,
                                  std::forward<ArgTypes>(args)...);
}

/**
 * @brief Retries a given free function or a static class method at intervals
 * until it returns a value that is not in the retry_values array or until the
 * max_attempts have been exhausted.
 *
 * This function retries a given free function or a static class method at
 * intervals until it returns a value that is not in the `retry_values` array or
 * until `max_attempts` have been exhausted. With C++17 CTAD, all template
 * arguments can be deduced. Users can pass a function pointer, a contructor
 * initializer list of retry values, the maximum attempts to call the function,
 * the interval between each retry, and the arguments that used to pass to the
 * function.
 *
 * @param func_ptr The pointer of free function or static class method to be
 * retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the function.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the function.
 * @return The latest return value of the function.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_at_intervals_until(ReturnType (*func_ptr)(ArgTypes...),
                                    const ReturnType (&retry_values)[ArraySize],
                                    unsigned int max_attempts,
                                    const std::chrono::nanoseconds& interval,
                                    ParamTypes&&... params) {
    return internal::_retry_until(func_ptr, retry_values, max_attempts,
                                  interval,
                                  std::forward<ParamTypes>(params)...);
}

/**
 * @brief Retries a given std::function or a static class method at intervals
 * until it returns a value that is not in the retry_values array or until the
 * max_attempts have been exhausted.
 *
 * This function retries to call given std::function object at intervals until
 * it returns a value that is not in the `retry_values` array or until
 * `max_attempts` have been exhausted. With C++17 CTAD, all template arguments
 * can be deduced. Users can pass a function pointer, a contructor initializer
 * list of retry values, the maximum attempts to call the function, the interval
 * between each retry, and the arguments that used to pass to the std::function.
 *
 * @param function The std::function object to be retried.
 * @param retry_values The array of retry values.
 * @param max_attempts The maximum attempts to call the std::function.
 * @param interval The interval between each retry.
 * @param params The arguments to be passed to the std::function.
 * @return The latest return value of the std::function object.
 */
template <typename ReturnType, typename... ArgTypes, unsigned long ArraySize,
          typename... ParamTypes>
ReturnType retry_at_intervals_until(
    const std::function<ReturnType(ArgTypes...)>& function,
    const ReturnType (&retry_values)[ArraySize], unsigned int max_attempts,
    const std::chrono::nanoseconds& interval, ParamTypes&&... params) {
    return internal::_retry_until(function, retry_values, max_attempts,
                                  interval,
                                  std::forward<ParamTypes>(params)...);
}

}  // namespace tsejhuang
