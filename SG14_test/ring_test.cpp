#include "SG14_test.h"

#include "ring.h"

#include <array>
#include <numeric>
#include <string>
#include <vector>

static void basic_test()
{
	std::array<int, 5> A;
	std::array<int, 5> B;

	sg14::ring_span<int> Q(std::begin(A), std::end(A));

	Q.push_back(7);
	Q.push_back(3);
	assert(Q.size() == 2);
	assert(Q.front() == 7);

	Q.pop_front();
	assert(Q.size() == 1);

	Q.push_back(18);
	auto Q3 = std::move(Q);
	assert(Q3.front() == 3);
	assert(Q3.back() == 18);

	sg14::ring_span<int> Q5(std::move(Q3));
	assert(Q5.front() == 3);
	assert(Q5.back() == 18);
	assert(Q5.size() == 2);

	Q5.pop_front();
	Q5.pop_front();
	assert(Q5.empty());

	sg14::ring_span<int> Q6(std::begin(B), std::end(B));
	Q6.push_back(6);
	Q6.push_back(7);
	Q6.push_back(8);
	Q6.push_back(9);
	Q6.emplace_back(10);
	Q6.swap(Q5);
	assert(Q6.empty());
	assert(Q5.size() == 5);
	assert(Q5.front() == 6);
	assert(Q5.back() == 10);
}

static void filter_test()
{
	std::array< double, 3 > A;
	sg14::ring_span< double > buffer( std::begin( A ), std::end( A ) );

	buffer.push_back( 1.0 );
	buffer.push_back( 2.0 );
	buffer.push_back( 3.0 );
	buffer.push_back( 5.0 );

	assert( buffer.front() == 2.0 );

	constexpr std::array< double, 3 > filter_coefficients = {{ 0.25, 0.5, 0.25 }};

	// In an update loop, interrupt routine or the like
	buffer.push_back( 7.0 );

	assert( std::inner_product( buffer.begin(), buffer.end(), filter_coefficients.begin(), 0.0 ) == 5.0 );
}

static void iterator_regression_test()
{
    std::array<double, 3> A;
    sg14::ring_span<double> r(A.begin(), A.end());
    r.push_back(1.0);
    decltype(r)::iterator it = r.end();
    decltype(r)::const_iterator cit = r.end();  // test conversion from non-const to const
    assert(it == cit);  // test comparison of const and non-const
    assert(it + 0 == it);
    assert(it - 1 == r.begin());
    assert(cit + 0 == cit);
    assert(cit - 1 == r.cbegin());
    assert(it - cit == 0);
    assert(cit - r.begin() == 1);

    std::array<double, 4> B;
    sg14::ring_span<double> r2(B.begin(), B.end());
    swap(r, r2);  // test existence of ADL swap()

    // Set up the ring for the TEST_OP tests below.
    r = sg14::ring_span<double>(A.begin(), A.end(), A.begin(), 2);
    assert(r.size() == 2);

#define TEST_OP(op, a, b, c) \
    assert(a(r.begin() op r.end())); \
    assert(b(r.end() op r.begin())); \
    assert(c(r.begin() op r.begin()))
#define _
    TEST_OP(==, !, !, _);
    TEST_OP(!=, _, _, !);
    TEST_OP(<, _, !, !);
    TEST_OP(<=, _, !, _);
    TEST_OP(>, !, _, !);
    TEST_OP(>=, !, _, _);
#undef _
#undef TEST_OP
}

static void copy_popper_test()
{
    std::vector<std::string> v { "quick", "brown", "fox" };
    sg14::ring_span<std::string, sg14::copy_popper<std::string>> r(v.begin(), v.end(), {"popped"});
    r.emplace_back("slow");
    assert((v == std::vector<std::string>{"slow", "brown", "fox"}));
    r.emplace_back("red");
    assert((v == std::vector<std::string>{"slow", "red", "fox"}));
    std::string result = r.pop_front();
    assert((v == std::vector<std::string>{"popped", "red", "fox"}));
    assert(result == "slow");
}

static void reverse_iterator_test()
{
    std::array<int, 3> A;
    sg14::ring_span<int> r(A.begin(), A.end());
    const sg14::ring_span<int> c(A.begin(), A.end());

    r.push_back(1);
    r.push_back(2);
    r.push_back(3);
    r.push_back(4);
    std::vector<double> v(3);

    std::copy(r.begin(), r.end(), v.begin());
    assert((v == std::vector<double>{2,3,4}));

    std::copy(r.cbegin(), r.cend(), v.begin());
    assert((v == std::vector<double>{2,3,4}));

    std::copy(r.rbegin(), r.rend(), v.begin());
    assert((v == std::vector<double>{4,3,2}));

    std::copy(r.crbegin(), r.crend(), v.begin());
    assert((v == std::vector<double>{4,3,2}));

    static_assert(std::is_same<decltype(r.begin()), decltype(r)::iterator>::value, "");
    static_assert(std::is_same<decltype(c.begin()), decltype(r)::const_iterator>::value, "");
    static_assert(std::is_same<decltype(r.cbegin()), decltype(r)::const_iterator>::value, "");
    static_assert(std::is_same<decltype(c.cbegin()), decltype(r)::const_iterator>::value, "");

    static_assert(std::is_same<decltype(r.end()), decltype(r)::iterator>::value, "");
    static_assert(std::is_same<decltype(c.end()), decltype(r)::const_iterator>::value, "");
    static_assert(std::is_same<decltype(r.cend()), decltype(r)::const_iterator>::value, "");
    static_assert(std::is_same<decltype(c.cend()), decltype(r)::const_iterator>::value, "");

    static_assert(std::is_same<decltype(r.rbegin()), decltype(r)::reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(c.rbegin()), decltype(r)::const_reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(r.crbegin()), decltype(r)::const_reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(c.crbegin()), decltype(r)::const_reverse_iterator>::value, "");

    static_assert(std::is_same<decltype(r.rend()), decltype(r)::reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(c.rend()), decltype(r)::const_reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(r.crend()), decltype(r)::const_reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(c.crend()), decltype(r)::const_reverse_iterator>::value, "");
}

void sg14_test::ring_test()
{
    basic_test();
    filter_test();
    iterator_regression_test();
    copy_popper_test();
    reverse_iterator_test();
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::ring_test();
}
#endif
