#include "SG14_test.h"
#include "../SG14/inplace_function.h"
#include <cassert>

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

void sg14_test::inplace_function_test()
{
    using IPF = stdext::inplace_function<void(int)>;
    static_assert(std::is_nothrow_default_constructible<IPF>::value, "");
    static_assert(std::is_copy_constructible<IPF>::value, "");
    static_assert(std::is_move_constructible<IPF>::value, "");
    static_assert(std::is_copy_assignable<IPF>::value, "");
    static_assert(std::is_move_assignable<IPF>::value, "");
    // static_assert(std::is_swappable<IPF&>::value, "");  // C++17
    static_assert(std::is_nothrow_destructible<IPF>::value, "");

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
    // static_assert(!std::is_assignable<IPF&, const IPF40&>::value, "");  // TODO: SFINAE
    // static_assert(!std::is_assignable<IPF&, IPF40&&>::value, "");  // TODO: SFINAE
    // static_assert(!std::is_swappable_with<IPF40&, IPF&>::value, ""); // C++17
    // static_assert(!std::is_swappable_with<IPF&, IPF40&>::value, ""); // C++17
    static_assert(std::is_nothrow_destructible<IPF40>::value, "");

    IPF40 func40;
    assert(!func40);
    assert(!bool(func40));
    assert(func40 == nullptr);
    assert(!(func40 != nullptr));
    expected = 0; try { func40(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);

    func = nullptr;
    func40 = func;
#if 0  // TODO: this is a bug
    assert(!func40);
    assert(!bool(func40));
    assert(func40 == nullptr);
    assert(!(func40 != nullptr));
#endif
    expected = 0; try { func40(42); } catch (std::bad_function_call&) { expected = 1; } assert(expected == 1);
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::inplace_function_test();
}
#endif
