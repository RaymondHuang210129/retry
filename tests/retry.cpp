#include "retry/retry.hpp"

#include <gtest/gtest.h>

#include <set>

#include "testutil.hpp"

TEST(BasicCompileTest, ParamTypes) {
    // Test function and callable with no parameters
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_no_params, {false},
                                       std::chrono::seconds{1}));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_no_params, {false},
                                       std::chrono::seconds{1}));
    ASSERT_TRUE(tsejhuang::retry_until([]() { return true; }, {false},
                                       std::chrono::seconds{1}));

    // Test function and callable with single primitive type parameter
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_primitive_param,
                                       {false}, std::chrono::seconds{1}, 1));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_primitive_param,
                                       {false}, std::chrono::seconds{1}, 1));
    ASSERT_TRUE(tsejhuang::retry_until([](int i) { return true; }, {false},
                                       std::chrono::seconds{1}, 1));

    // Test function and callable with single primitive type reference parameter
    int i = 1;
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_primitive_ref_param,
                                       {false}, std::chrono::seconds{1}, i));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_primitive_ref_param,
                                       {false}, std::chrono::seconds{1}, i));
    ASSERT_TRUE(tsejhuang::retry_until([](int& i) { return true; }, {false},
                                       std::chrono::seconds{1}, i));

    // Test function and callable with single primitive type const reference
    // parameter
    const int& ci = i;
    ASSERT_TRUE(
        tsejhuang::retry_until(test_callable_single_primitive_cref_param,
                               {false}, std::chrono::seconds{1}, ci));
    ASSERT_TRUE(
        tsejhuang::retry_until(test_function_single_primitive_cref_param,
                               {false}, std::chrono::seconds{1}, ci));
    ASSERT_TRUE(tsejhuang::retry_until([](const int& i) { return true; },
                                       {false}, std::chrono::seconds{1}, ci));

    // Test function and callable with single pointer parameter
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_pointer_param,
                                       {false}, std::chrono::seconds{1}, &i));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_pointer_param,
                                       {false}, std::chrono::seconds{1}, &i));
    ASSERT_TRUE(tsejhuang::retry_until([](int* i) { return true; }, {false},
                                       std::chrono::seconds{1}, &i));

    // Test function and callable with single struct parameter
    TestClass tc{1};
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_struct_param,
                                       {false}, std::chrono::seconds{1}, tc));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_struct_param,
                                       {false}, std::chrono::seconds{1}, tc));
    ASSERT_TRUE(tsejhuang::retry_until([](TestClass tc) { return true; },
                                       {false}, std::chrono::seconds{1}, tc));

    // Test function and callable with single struct reference parameter
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_struct_ref_param,
                                       {false}, std::chrono::seconds{1}, tc));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_struct_ref_param,
                                       {false}, std::chrono::seconds{1}, tc));
    ASSERT_TRUE(tsejhuang::retry_until([](TestClass& tc) { return true; },
                                       {false}, std::chrono::seconds{1}, tc));

    // Test function and callable with single struct const reference parameter
    const TestClass& ctc = tc;
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_struct_cref_param,
                                       {false}, std::chrono::seconds{1}, ctc));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_struct_cref_param,
                                       {false}, std::chrono::seconds{1}, ctc));
    ASSERT_TRUE(tsejhuang::retry_until([](const TestClass& tc) { return true; },
                                       {false}, std::chrono::seconds{1}, ctc));

    // Test function and callable with single struct rvalue reference parameter
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_single_struct_rvalue_param,
                                       {false}, std::chrono::seconds{1},
                                       std::move(tc)));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_single_struct_rvalue_param,
                                       {false}, std::chrono::seconds{1},
                                       std::move(tc)));
    ASSERT_TRUE(tsejhuang::retry_until([](TestClass&& tc) { return true; },
                                       {false}, std::chrono::seconds{1},
                                       std::move(tc)));

    // Test function and callable with multiple parameters
    ASSERT_TRUE(tsejhuang::retry_until(test_callable_multiple_params, {false},
                                       std::chrono::seconds{1}, 1, i, ci, &i,
                                       tc, tc, ctc, std::move(tc)));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_multiple_params, {false},
                                       std::chrono::seconds{1}, 1, i, ci, &i,
                                       tc, tc, ctc, std::move(tc)));
    ASSERT_TRUE(tsejhuang::retry_until(
        [](int i, int& ir, const int& cir, int* ip, TestClass tc,
           TestClass& tcr, const TestClass& ctc,
           TestClass&& tcm) { return true; },
        {false}, std::chrono::seconds{1}, 1, i, ci, &i, tc, tc, ctc,
        std::move(tc)));
}

TEST(BasicCompileTest, CallableTypes) {
    TestClass tc{1};

    ASSERT_TRUE(tsejhuang::retry_until(test_callable_no_params, {false},
                                       std::chrono::seconds{1}));
    ASSERT_TRUE(tsejhuang::retry_until(test_function_no_params, {false},
                                       std::chrono::seconds{1}));
    ASSERT_TRUE(tsejhuang::retry_until([]() { return true; }, {false},
                                       std::chrono::seconds{1}));
    ASSERT_TRUE(tsejhuang::retry_until(TestClass::get, {false},
                                       std::chrono::seconds{1}));
}

TEST(BasicCompileTest, ReturnTypes) {
    // Primitive return type
    tsejhuang::retry_until(test_callable_no_params, {false},
                           std::chrono::seconds{1});

    // Class return type with no default constructor
    TestClassNoDefaultCtr tcndc{1};
    ASSERT_EQ(tcndc, tsejhuang::retry_until(test_function_return_no_default_ctr,
                                            {TestClassNoDefaultCtr(0)},
                                            std::chrono::seconds{1}));

    TestClassNoDefaultCtr tc1{1};
    TestClassNoDefaultCtr tc0{0};

    // Reference class return type
    TestClassNoDefaultCtr& tc0_ref = tc0;

    ASSERT_EQ(tc1, tsejhuang::retry_until(test_function_return_ref, {tc0_ref},
                                          std::chrono::seconds{1}));
    ASSERT_EQ(tc1,
              tsejhuang::retry_until(test_function_return_ref, {std::ref(tc0)},
                                     std::chrono::seconds{1}));

    // Const reference class return type
    const TestClassNoDefaultCtr& tc0_cref = tc0;

    ASSERT_EQ(tc1, tsejhuang::retry_until(test_function_return_cref, {tc0_cref},
                                          std::chrono::seconds{1}));
    ASSERT_EQ(
        tc1, tsejhuang::retry_until(test_function_return_cref, {std::cref(tc0)},
                                    std::chrono::seconds{1}));
}

TEST(InternalFunctionTest, IsInRetryValues) {
    // Arrays of primitive types
    bool retry_value_bools[1] = {true};
    int retry_value_ints[2] = {1, 2};
    double retry_value_doubles[3] = {1.0, 2.0, 3.0};

    // Arrays of class types
    TestClass retry_value_class[2] = {TestClass(1), TestClass(2)};

    // Arrays of class reference types
    std::reference_wrapper<TestClass> retry_value_class_ref[2] = {
        std::ref(retry_value_class[0]), std::ref(retry_value_class[1])};

    ASSERT_TRUE(
        tsejhuang::internal::is_in_retry_values(retry_value_bools, true));
    ASSERT_FALSE(
        tsejhuang::internal::is_in_retry_values(retry_value_bools, false));
    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(retry_value_ints, 1));
    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(retry_value_ints, 2));
    ASSERT_FALSE(tsejhuang::internal::is_in_retry_values(retry_value_ints, 3));
    ASSERT_TRUE(
        tsejhuang::internal::is_in_retry_values(retry_value_doubles, 1.0));
    ASSERT_TRUE(
        tsejhuang::internal::is_in_retry_values(retry_value_doubles, 2.0));
    ASSERT_TRUE(
        tsejhuang::internal::is_in_retry_values(retry_value_doubles, 3.0));
    ASSERT_FALSE(
        tsejhuang::internal::is_in_retry_values(retry_value_doubles, 4.0));

    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(retry_value_class,
                                                        TestClass(1)));
    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(retry_value_class,
                                                        TestClass(2)));
    ASSERT_FALSE(tsejhuang::internal::is_in_retry_values(retry_value_class,
                                                         TestClass(3)));

    TestClass tc3{3};
    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(
        retry_value_class_ref, std::ref(retry_value_class[0])));
    ASSERT_TRUE(tsejhuang::internal::is_in_retry_values(
        retry_value_class_ref, std::ref(retry_value_class[1])));
    ASSERT_FALSE(tsejhuang::internal::is_in_retry_values(retry_value_class_ref,
                                                         std::ref(tc3)));
}

TEST(CoreFunctionTest, ExpectNoRetryOnSuccess) {
    auto start_time = std::chrono::steady_clock::now();
    int count = 0;

    // Return true at the first call, throw exception if called again
    std::function<bool()> success_at_first_time = [&count] {
        count++;
        if (count > 1) {
            throw std::runtime_error("Should not be called again");
        }
        return true;
    };

    // Retry for 1 second
    ASSERT_TRUE(tsejhuang::retry_until(success_at_first_time, {false},
                                       std::chrono::seconds{1}));
    ASSERT_EQ(count, 1);
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::seconds{1});

    count = 0;
    // Retry for 2 attempts
    ASSERT_TRUE(tsejhuang::retry_until(success_at_first_time, {false}, 2));
    ASSERT_EQ(count, 1);
}

TEST(CoreFeatureTest, ExpectThreeRetriesOnSuccess) {
    auto start_time = std::chrono::steady_clock::now();
    int count = 0;

    // Return false at the two calls, true at the third call, throw exception if
    // called more than three times
    std::function<bool()> success_at_third_time = [&count] {
        count++;
        if (count > 3) {
            throw std::runtime_error("Should not be called more than 3 times");
        }
        return count == 3;
    };

    // Retry for 1 second
    ASSERT_TRUE(tsejhuang::retry_until(success_at_third_time, {false},
                                       std::chrono::seconds{1}));
    ASSERT_EQ(count, 3);

    // This should be done very fast so we expect the time to be less than 100ms
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::milliseconds{100});

    count = 0;
    // Retry for 5 attempts
    ASSERT_TRUE(tsejhuang::retry_until(success_at_third_time, {false}, 5));
    ASSERT_EQ(count, 3);
}

TEST(CoreFeatureTest, FailedByTimeExpiration) {
    auto now = std::chrono::steady_clock::now();

    // Return true until time has passed 2 seconds
    std::function<bool()> success_after_two_sec = [&now] {
        return std::chrono::steady_clock::now() - now > std::chrono::seconds{2};
    };

    // Retry for 1 second
    ASSERT_FALSE(tsejhuang::retry_until(success_after_two_sec, {false},
                                        std::chrono::seconds{1}));
    ASSERT_TRUE(std::chrono::steady_clock::now() - now >=
                std::chrono::seconds{1});
    ASSERT_TRUE(std::chrono::steady_clock::now() - now <
                std::chrono::seconds{2});
}

TEST(CoreFeatureTest, FailedByLimitedAttempts) {
    // Return false until 3 attempts
    int count = 0;
    std::function<bool()> success_after_three_attempts = [&count] {
        count++;
        return count == 3;
    };

    // Retry for 2 attempts
    ASSERT_FALSE(
        tsejhuang::retry_until(success_after_three_attempts, {false}, 2));
}

TEST(CoreFunctionTestWithInterval, ExpectNoRetryOnSuccess) {
    auto start_time = std::chrono::steady_clock::now();
    int count = 0;

    // Return true at the first call, throw exception if called again
    std::function<bool()> success_at_first_time = [&count] {
        count++;
        if (count > 1) {
            throw std::runtime_error("Should not be called again");
        }
        return true;
    };

    // Retry for 1 second
    ASSERT_TRUE(tsejhuang::retry_at_intervals_until(
        success_at_first_time, {false}, std::chrono::seconds{1},
        std::chrono::seconds{1}));
    ASSERT_EQ(count, 1);
    // The retry should not sleep so it must be done very fast
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::milliseconds{100});

    count = 0;
    // Retry for 2 attempts
    start_time = std::chrono::steady_clock::now();
    ASSERT_TRUE(tsejhuang::retry_until(success_at_first_time, {false}, 2));
    ASSERT_EQ(count, 1);
    // The retry should not sleep so it must be done very fast
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::milliseconds{100});
}

TEST(CoreFeatureTestWithInterval, ExpectThreeRetriesOnSuccess) {
    auto start_time = std::chrono::steady_clock::now();
    int count = 0;

    // Return false at the two calls, true at the third call, throw exception if
    // called more than three times
    std::function<bool()> success_at_third_time = [&count] {
        count++;
        if (count > 3) {
            throw std::runtime_error("Should not be called more than 3 times");
        }
        return count == 3;
    };

    // Retry for 1 second
    ASSERT_TRUE(tsejhuang::retry_at_intervals_until(
        success_at_third_time, {false}, std::chrono::seconds{1},
        std::chrono::milliseconds{100}));
    ASSERT_EQ(count, 3);
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time >=
                std::chrono::milliseconds{200});
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::milliseconds{300});

    count = 0;
    start_time = std::chrono::steady_clock::now();
    // Retry for 5 attempts
    ASSERT_TRUE(tsejhuang::retry_at_intervals_until(
        success_at_third_time, {false}, 5, std::chrono::milliseconds{100}));
    ASSERT_EQ(count, 3);
}

TEST(CoreFeatureTestWithInterval, FailedByTimeExpiration) {
    auto start_time = std::chrono::steady_clock::now();

    // Return true until time has passed 2 seconds
    std::function<bool()> success_after_two_sec = [&start_time] {
        return std::chrono::steady_clock::now() - start_time >
               std::chrono::seconds{2};
    };

    // Retry for 1 second
    ASSERT_FALSE(tsejhuang::retry_at_intervals_until(
        success_after_two_sec, {false}, std::chrono::seconds{1},
        std::chrono::milliseconds{100}));

    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time >=
                std::chrono::seconds{1});
}

TEST(CoreFeatureTestWithInterval, FailedByLimitedAttempts) {
    auto start_time = std::chrono::steady_clock::now();
    int count = 0;

    // Return false until 4 attempts
    std::function<bool()> success_after_three_attempts = [&count] {
        count++;
        return count == 4;
    };

    // Retry for 3 attempts
    ASSERT_FALSE(tsejhuang::retry_at_intervals_until(
        success_after_three_attempts, {false}, 3,
        std::chrono::milliseconds{100}));
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time >=
                std::chrono::milliseconds{200});
    ASSERT_TRUE(std::chrono::steady_clock::now() - start_time <
                std::chrono::milliseconds{300});
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
