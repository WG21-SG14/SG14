#include "SG14_test.h"
#include "inplace_function.h"
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

namespace {

static int copied, moved, called_with;
static int expected;

struct Functor {
    Functor() {}
    Functor(const Functor&) { copied += 1; }
    Functor(Functor&&) { moved += 1; }
    void operator()(int i) { assert(i == expected); called_with = i; }
};

struct ConstFunctor {
    ConstFunctor() {}
    ConstFunctor(const ConstFunctor&) { copied += 1; }
    ConstFunctor(ConstFunctor&&) { moved += 1; }
    void operator()(int i) const { assert(i == expected); called_with = i; }
};

void Foo(int i)
{
    assert(i == expected);
    called_with = i;
}

} // anonymous namespace

#define EXPECT_EQ(val1, val2) assert(val1 == val2)
#define EXPECT_TRUE(val) assert(val)
#define EXPECT_FALSE(val) assert(!val)

std::string gLastS;
int gLastI = 0;
double gNextReturn = 0.0;

double GlobalFunction(const std::string& s, int i)
{
    gLastS = s;
    gLastI = i;
    return gNextReturn;
}

void FunctionPointer()
{
    // Even compatible function pointers require an appropriate amount of "storage".
    using CompatibleFunctionType = std::remove_reference_t<decltype(GlobalFunction)>;
    stdext::inplace_function<CompatibleFunctionType, sizeof(CompatibleFunctionType*)> fun(&GlobalFunction);

    EXPECT_TRUE(bool(fun));

    gNextReturn = 7.77;

    double r = fun("hello", 42);

    EXPECT_EQ(gNextReturn, r);
    EXPECT_EQ("hello", gLastS);
    EXPECT_EQ(42, gLastI);
}

void Lambda()
{
    stdext::inplace_function<double(int), 8> fun;
    std::string closure("some closure");
    fun = [&closure](int x) { return GlobalFunction(closure, x); };

    gNextReturn = 7.77;

    double r = fun(42);

    EXPECT_EQ(gNextReturn, r);
    EXPECT_EQ(closure, gLastS);
    EXPECT_EQ(42, gLastI);
}

void Bind()
{
    stdext::inplace_function<double(int), 64> fun;
    std::string closure("some closure");
    fun = std::bind(GlobalFunction, closure, std::placeholders::_1);

    gNextReturn = 7.77;

    double r = fun(42);

    EXPECT_EQ(gNextReturn, r);
    EXPECT_EQ(closure, gLastS);
    EXPECT_EQ(42, gLastI);
}

struct AnotherFunctor
{
    int mTotal = 0;
    static int mDestructorCalls;
    static int mConstructorCalls;
    int operator()(int x) { mTotal += x; return mTotal; }

    AnotherFunctor() { mConstructorCalls++; }
    AnotherFunctor(AnotherFunctor&&) { mConstructorCalls++; }
    AnotherFunctor(const AnotherFunctor&) { mConstructorCalls++; }
    ~AnotherFunctor() { mDestructorCalls++; }
};

int AnotherFunctor::mDestructorCalls = 0;
int AnotherFunctor::mConstructorCalls = 0;

void FunctorDestruction()
{
    AnotherFunctor::mDestructorCalls = 0;
    AnotherFunctor::mConstructorCalls = 0;
    {
        AnotherFunctor ftor;
        stdext::inplace_function<int(int), 4> fun(ftor);

        int r1 = fun(1);
        int r2 = fun(7);

        EXPECT_EQ(1, r1);
        EXPECT_EQ(8, r2);
    }
    EXPECT_EQ(AnotherFunctor::mDestructorCalls, AnotherFunctor::mConstructorCalls);

    AnotherFunctor::mDestructorCalls = 0;
    AnotherFunctor::mConstructorCalls = 0;
    {
        AnotherFunctor ftor;
        stdext::inplace_function<int(int), 4> fun(ftor);
        stdext::inplace_function<int(int), 4> fun2(fun);  // copy-ctor
        stdext::inplace_function<int(int), 4> fun3(std::move(fun));  // move-ctor
        fun3 = fun2;  // copy-asgn
        fun3 = std::move(fun2);  // move-asgn
    }
    EXPECT_EQ(AnotherFunctor::mDestructorCalls, AnotherFunctor::mConstructorCalls);
}

void Swapping()
{
    AnotherFunctor::mDestructorCalls = 0;
    AnotherFunctor::mConstructorCalls = 0;
    {
        AnotherFunctor ftor;
        auto lambda = [](int x){ return x + 10; };
        stdext::inplace_function<int(int), 4> fun(ftor);
        stdext::inplace_function<int(int), 4> fun2(lambda);

        fun.swap(fun2);  // swap...
        fun2.swap(fun);  // ...and swap back

        int r1 = fun(1);
        int r2 = fun(7);
        EXPECT_EQ(1, r1);
        EXPECT_EQ(8, r2);

        int r3 = fun2(1);
        int r4 = fun2(7);
        EXPECT_EQ(11, r3);
        EXPECT_EQ(17, r4);
    }
    EXPECT_EQ(AnotherFunctor::mDestructorCalls, AnotherFunctor::mConstructorCalls);
}

void Copying()
{
    auto sptr = std::make_shared<int>(42);
    EXPECT_EQ(1, sptr.use_count());

    stdext::inplace_function<int(), 16> fun1 = [sptr]() { return *sptr; };
    stdext::inplace_function<int(), 16> fun2;

    EXPECT_EQ(2, sptr.use_count());
    EXPECT_TRUE(bool(fun1));
    EXPECT_FALSE(bool(fun2));

    fun2 = fun1;
    EXPECT_EQ(3, sptr.use_count());
    EXPECT_TRUE(bool(fun1));
    EXPECT_TRUE(bool(fun2));

    // this should call destructor on existing functor
    fun1 = nullptr;
    EXPECT_EQ(2, sptr.use_count());
    EXPECT_FALSE(bool(fun1));
    EXPECT_TRUE(bool(fun2));
}

void ContainingStdFunction()
{
    // build a big closure, bigger than 32 bytes
    uint64_t offset1 = 1234;
    uint64_t offset2 = 77;
    uint64_t offset3 = 666;
    std::string str1 = "12345";

    std::function<int(const std::string&)> stdfun
                    = [offset1, offset2, offset3, str1](const std::string& str)
    {
        return int(offset1 + offset2 + offset3 + str1.length() + str.length());
    };

    stdext::inplace_function<int(const std::string&), 64> fun = stdfun;

    int r = fun("123");
    EXPECT_EQ(r, int(offset1+offset2+offset3+str1.length()+3));
}

void SimilarTypeCopy()
{
    auto sptr = std::make_shared<int>(42);
    EXPECT_EQ(1, sptr.use_count());

    stdext::inplace_function<int(), 16> fun1 = [sptr]() { return *sptr; };
    stdext::inplace_function<int(), 17> fun2(fun1); // fun1 is bigger than 17, but we should be smart about it
    stdext::inplace_function<int(), 18> fun3;

    EXPECT_EQ(3, sptr.use_count());
    EXPECT_FALSE(fun3);
    fun3 = fun2;
    EXPECT_EQ(4, sptr.use_count());
    EXPECT_TRUE(bool(fun2));
    EXPECT_TRUE(bool(fun3));

    fun1 = nullptr;
    fun2 = nullptr;
    EXPECT_EQ(2, sptr.use_count());
    fun3 = nullptr;
    EXPECT_EQ(1, sptr.use_count());

    stdext::inplace_function<int(), 17> fun4;
    fun4 = fun1; // fun1 is bigger than 17, but we should be smart about it
}

void AssignmentDifferentFunctor()
{
    int calls = 0;
    stdext::inplace_function<int(int,int), 16> add = [&calls] (int a, int b) { ++calls; return a+b; };
    stdext::inplace_function<int(int,int), 16> mul = [&calls] (int a, int b) { ++calls; return a*b; };

    int r1 = add(3, 5);
    EXPECT_EQ(8, r1);

    int r2 = mul(2, 5);
    EXPECT_EQ(10, r2);

    EXPECT_EQ(2, calls);

    add = mul;

    int r3 = add(3, 5);
    EXPECT_EQ(15, r3);

    int r4 = mul(2, 5);
    EXPECT_EQ(10, r4);

    EXPECT_EQ(4, calls);
}

struct ThrowingFunctor {
    static int countdown;
    static int constructed;
    static int destructed;
    static int called;
    static void reset(int k) {
        countdown = k;
        constructed = 0;
        destructed = 0;
        called = 0;
    }
    static void check_countdown() {
        if (countdown > 0 && --countdown == 0) {
            throw 42;
        }
    }
    ThrowingFunctor() { check_countdown(); ++constructed; }
    ThrowingFunctor(const ThrowingFunctor&) {
        check_countdown();
        ++constructed;
    }
    ThrowingFunctor(ThrowingFunctor&&) noexcept { ++constructed; }
    ThrowingFunctor& operator=(const ThrowingFunctor&) = delete;
    ThrowingFunctor& operator=(ThrowingFunctor&&) = delete;
    ~ThrowingFunctor() noexcept { ++destructed; }
    void operator()() const { ++called; }
};
int ThrowingFunctor::countdown = 0;
int ThrowingFunctor::constructed = 0;
int ThrowingFunctor::destructed = 0;
int ThrowingFunctor::called = 0;

static void test_exception_safety()
{
    using IPF = stdext::inplace_function<void(), sizeof(ThrowingFunctor)>;
    ThrowingFunctor tf;
    EXPECT_EQ(ThrowingFunctor::constructed, 1);
    EXPECT_EQ(ThrowingFunctor::destructed, 0);
    EXPECT_EQ(ThrowingFunctor::called, 0);
    bool caught;

    // Copy-construction from a ThrowingFunctor might throw.
    try { tf.reset(1); caught = false; IPF a(tf); } catch (int) { caught = true; }
    EXPECT_TRUE((caught) && (tf.countdown == 0) && (tf.constructed == 0) && (tf.destructed == 0));
    try { tf.reset(2); caught = false; IPF a(tf); } catch (int) { caught = true; }
    EXPECT_TRUE((not caught) && (tf.countdown == 1) && (tf.constructed == 1) && (tf.destructed == 1));

    // Copy-construction from an IPF (containing a ThrowingFunctor) might throw.
    try { tf.reset(2); caught = false; IPF a(tf); IPF b(a); } catch (int) { caught = true; }
    EXPECT_TRUE((caught) && (tf.countdown == 0) && (tf.constructed == 1) && (tf.destructed == 1));
    try { tf.reset(3); caught = false; IPF a(tf); IPF b(a); } catch (int) { caught = true; }
    EXPECT_TRUE((not caught) && (tf.countdown == 1) && (tf.constructed == 2) && (tf.destructed == 2));

    // Move-construction and destruction should be assumed not to throw.
    try { tf.reset(1); caught = false; IPF a(std::move(tf)); IPF b(std::move(a)); } catch (int) { caught = true; }
    EXPECT_TRUE((not caught) && (tf.countdown == 1) && (tf.constructed == 2) && (tf.destructed == 2));

    // The assignment operators are implemented as "construct an IPF, then move from it"; so, one copy and one move of the ThrowingFunctor.
    try { tf.reset(1); caught = false; IPF a; a = tf; } catch (int) { caught = true; }
    EXPECT_TRUE((caught) && (tf.countdown == 0) && (tf.constructed == 0) && (tf.destructed == 0));
    try { tf.reset(2); caught = false; IPF a; a = tf; } catch (int) { caught = true; }
    EXPECT_TRUE((not caught) && (tf.countdown == 1) && (tf.constructed == 2) && (tf.destructed == 2));

    // The assignment operators are implemented as "construct an IPF, then move from it"; so, two moves of the ThrowingFunctor.
    try { tf.reset(1); caught = false; IPF a; a = std::move(tf); } catch (int) { caught = true; }
    EXPECT_TRUE((not caught) && (tf.countdown == 1) && (tf.constructed == 2) && (tf.destructed == 2));
}

template<size_t Cap>
constexpr size_t expected_alignment_for_capacity()
{
    constexpr size_t alignof_ptr = std::alignment_of<void*>::value;
    constexpr size_t alignof_cap = std::alignment_of<std::aligned_storage_t<Cap>>::value;
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
#if defined(__GLIBCXX__)  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61458
    return MAX(MIN(Cap, alignof_cap), alignof_ptr);
#else  // other STLs
    return MAX(alignof_cap, alignof_ptr);
#endif
#undef MAX
#undef MIN
}

static void test_struct_layout()
{
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 1> >::value == expected_alignment_for_capacity<1>(), "");
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 2> >::value == expected_alignment_for_capacity<2>(), "");
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 4> >::value == expected_alignment_for_capacity<4>(), "");
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 8> >::value == expected_alignment_for_capacity<8>(), "");
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 16> >::value == expected_alignment_for_capacity<16>(), "");
    static_assert(std::alignment_of< stdext::inplace_function<void(int), 32> >::value == expected_alignment_for_capacity<32>(), "");
    static_assert(sizeof( stdext::inplace_function<void(int), sizeof(void*)> ) == 2 * sizeof(void*), "");
}

static void test_nullptr()
{
    using IPF = stdext::inplace_function<void()>;
    auto nil = nullptr;
    const auto cnil = nullptr;

    IPF f;                    assert(not bool(f));
    f = nullptr;              assert(not bool(f));
    f = IPF(nullptr);         assert(not bool(f));
    f = IPF();                assert(not bool(f));
    f = IPF{};                assert(not bool(f));
    f = {};                   assert(not bool(f));
    f = nil;                  assert(not bool(f));
    f = IPF(nil);             assert(not bool(f));
    f = IPF(std::move(nil));  assert(not bool(f));
    f = cnil;                 assert(not bool(f));
    f = IPF(cnil);            assert(not bool(f));
    f = IPF(std::move(cnil)); assert(not bool(f));
}

void sg14_test::inplace_function_test()
{
    // first set of tests (from Optiver)
    AssignmentDifferentFunctor();
    FunctionPointer();
    Lambda();
    Bind();
    Swapping();
    Copying();
    ContainingStdFunction();
    SimilarTypeCopy();
    FunctorDestruction();

    // second set of tests
    using IPF = stdext::inplace_function<void(int)>;
    static_assert(std::is_nothrow_default_constructible<IPF>::value, "");
    static_assert(std::is_copy_constructible<IPF>::value, "");
    static_assert(std::is_move_constructible<IPF>::value, "");
    static_assert(std::is_copy_assignable<IPF>::value, "");
    static_assert(std::is_move_assignable<IPF>::value, "");
#if __cplusplus >= 201703L
    static_assert(std::is_swappable<IPF&>::value, "");
    static_assert(std::is_invocable<const IPF&, int>::value, "");
    static_assert(std::is_invocable_r<void, const IPF&, int>::value, "");
#endif
    static_assert(std::is_nothrow_destructible<IPF>::value, "");

    test_struct_layout();

    IPF func;
    assert(!func);
    assert(!bool(func));
    assert(func == nullptr);
    assert(!(func != nullptr));
    expected = 0; try { func(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);

    func = Foo;
    assert(!!func);
    assert(func);
    assert(!(func == nullptr));
    assert(func != nullptr);
    called_with = 0; expected = 42; func(42); assert(called_with == 42);

    func = nullptr;
    assert(!func);
    assert(!bool(func));
    assert(func == nullptr);
    assert(!(func != nullptr));
    expected = 0; try { func(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);

    using IPF40 = stdext::inplace_function<void(int), 40>; // the default is 32
    static_assert(std::is_constructible<IPF40, const IPF&>::value, "");
    static_assert(std::is_constructible<IPF40, IPF&&>::value, "");  // TODO: nothrow
    static_assert(std::is_assignable<IPF40&, const IPF&>::value, "");
    static_assert(std::is_assignable<IPF40&, IPF&&>::value, "");  // TODO: nothrow
    //static_assert(!std::is_assignable<IPF&, const IPF40&>::value, "");
    //static_assert(!std::is_assignable<IPF&, IPF40&&>::value, "");
#if __cplusplus >= 201703L
    static_assert(!std::is_swappable_with<IPF40&, IPF&>::value, "");
    static_assert(!std::is_swappable_with<IPF&, IPF40&>::value, "");
#endif
    static_assert(std::is_nothrow_destructible<IPF40>::value, "");

    IPF40 func40;
    assert(!func40);
    assert(!bool(func40));
    assert(func40 == nullptr);
    assert(!(func40 != nullptr));
    expected = 0; try { func40(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);

    func = nullptr;
    func40 = func;
    assert(!func40);
    assert(!bool(func40));
    assert(func40 == nullptr);
    assert(!(func40 != nullptr));
    expected = 0; try { func40(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);

    test_exception_safety();
    test_nullptr();
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::inplace_function_test();
}
#endif
