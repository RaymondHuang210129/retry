#include <functional>

// Data structures and functions for BasicCompileTest

class TestClass {
   public:
    TestClass(int i) : i(i) {}
    TestClass(const TestClass& tc) : i(tc.i) {}
    TestClass(TestClass&& tc) : i(tc.i) { tc.i = 0; }
    static bool get() { return true; }
    bool operator==(const TestClass& tc) const { return i == tc.i; }
    int i;
};

class TestClassNoDefaultCtr {
   public:
    TestClassNoDefaultCtr(int i) : i(i) {}
    bool operator==(const TestClassNoDefaultCtr& tc) const { return i == tc.i; }
    bool operator<(const TestClassNoDefaultCtr& tc) const { return i < tc.i; }
    int i;
};

std::function<bool()> test_callable_no_params = []() { return true; };
std::function<bool(int)> test_callable_single_primitive_param = [](int i) {
    return true;
};
std::function<bool(int&)> test_callable_single_primitive_ref_param =
    [](int& i) { return true; };
std::function<bool(const int&)> test_callable_single_primitive_cref_param =
    [](const int& i) { return true; };
std::function<bool(int*)> test_callable_single_pointer_param = [](int* i) {
    return true;
};
std::function<bool(TestClass)> test_callable_single_struct_param =
    [](TestClass tc) { return true; };
std::function<bool(TestClass&)> test_callable_single_struct_ref_param =
    [](TestClass& tc) { return true; };
std::function<bool(const TestClass&)> test_callable_single_struct_cref_param =
    [](const TestClass& tc) { return true; };
std::function<bool(TestClass&&)> test_callable_single_struct_rvalue_param =
    [](TestClass&& tc) { return true; };
std::function<bool(int, int&, const int&, int*, TestClass, TestClass&,
                   const TestClass&, TestClass&&)>
    test_callable_multiple_params = [](int i, int& ir, const int& cir, int* ip,
                                       TestClass tc, TestClass& tcr,
                                       const TestClass& ctcr,
                                       TestClass&& tcmr) { return true; };

bool test_function_no_params() { return true; }

bool test_function_single_primitive_param(int i) { return true; }

bool test_function_single_primitive_ref_param(int& i) { return true; }

bool test_function_single_primitive_cref_param(const int& i) { return true; }

bool test_function_single_pointer_param(int* i) { return true; }

bool test_function_single_struct_param(TestClass tc) { return true; }

bool test_function_single_struct_ref_param(TestClass& tc) { return true; }

bool test_function_single_struct_cref_param(const TestClass& tc) {
    return true;
}

bool test_function_single_struct_rvalue_param(TestClass&& tc) { return true; }

bool test_function_multiple_params(int i, int& ir, const int& cir, int* ip,
                                   TestClass tc, TestClass& tcr,
                                   const TestClass& ctcr, TestClass&& tcmr) {
    return true;
}

TestClassNoDefaultCtr test_function_return_no_default_ctr() {
    return TestClassNoDefaultCtr(1);
}

TestClassNoDefaultCtr& test_function_return_ref() {
    static TestClassNoDefaultCtr tc(1);
    return tc;
}

const TestClassNoDefaultCtr& test_function_return_cref() {
    static TestClassNoDefaultCtr tc(1);
    return tc;
}

// Data structures and functions for Function test
