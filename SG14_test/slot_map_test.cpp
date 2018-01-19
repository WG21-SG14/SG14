#include "SG14_test.h"
#include "../SG14/slot_map.h"
#include <assert.h>
#include <inttypes.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>

namespace TestKey {
struct key_16_8_t {
    uint16_t index;
    uint8_t generation;
};
struct key_11_5_t {
    uint16_t index : 11;
    uint8_t generation : 5;
};

#if __cplusplus < 201701
template<int I, class K> auto get(const K& k) { return get(k, std::integral_constant<int, I>{}); }
template<int I, class K> auto& get(K& k) { return get(k, std::integral_constant<int, I>{}); }

const uint16_t& get(const key_16_8_t& k, std::integral_constant<int, 0>) { return k.index; }
const uint8_t& get(const key_16_8_t& k, std::integral_constant<int, 1>) { return k.generation; }
uint16_t& get(key_16_8_t& k, std::integral_constant<int, 0>) { return k.index; }
uint8_t& get(key_16_8_t& k, std::integral_constant<int, 1>) { return k.generation; }

uint16_t get(const key_11_5_t& k, std::integral_constant<int, 0>) { return k.index; }
uint8_t get(const key_11_5_t& k, std::integral_constant<int, 1>) { return k.generation; }
//uint16_t& get(key_11_5_t& k, std::integral_constant<int, 0>) { return k.index; }
//uint8_t& get(key_11_5_t& k, std::integral_constant<int, 1>) { return k.generation; }
#endif
} // namespace TestKey

namespace TestContainer {

template<class T>
struct Vector {
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = unsigned;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<T*>;
    using const_reverse_iterator = std::reverse_iterator<const T*>;

    Vector() = default;
    Vector(const Vector& rhs) { *this = rhs; }
    void operator=(const Vector& rhs) {
        size_ = rhs.size_;
        data_ = std::make_unique<T[]>(size_);
        std::copy(rhs.begin(), rhs.end(), data_.get());
    }
    unsigned size() const { return size_; }
    T *begin() { return data_.get(); }
    T *end() { return data_.get() + size_; }
    const T *begin() const { return data_.get(); }
    const T *end() const { return data_.get() + size_; }
    void pop_back() {
        auto p = std::make_unique<T[]>(size_ - 1);
        std::move(begin(), end() - 1, p.get());
        size_ -= 1;
        data_ = std::move(p);
    }
    void emplace_back(T t) {
        auto p = std::make_unique<T[]>(size_ + 1);
        std::move(begin(), end(), p.get());
        size_ += 1;
        data_ = std::move(p);
        *(end() - 1) = t;
    }
    friend void swap(Vector& a, Vector& b) {
        Vector t = std::move(a);
        a = std::move(b);
        b = std::move(t);
    }
private:
    size_t size_ = 0;
    std::unique_ptr<T[]> data_ = nullptr;
};

} // namespace TestContainer

template<class T, class Key, template<class...> class Container>
void print_slot_map(const stdext::slot_map<T, Key, Container>& sm)
{
    printf("%d slots:", (int)sm.slots_.size());
    for (auto&& slot : sm.slots_) {
        printf(" %d/%d", (int)slot.first, (int)slot.second);
    }
    printf("\n%d values:", (int)sm.values_.size());
    for (auto&& value : sm) {
        printf(" %d", (int)value);
    }
    assert(sm.reverse_map_.size() == sm.size());
    printf("\n%d reverse_map:", (int)sm.reverse_map_.size());
    for (auto&& idx : sm.reverse_map_) {
        printf(" %d", (int)idx);
    }
    printf("\nnext_available_slot_index: %d\n", (int)sm.next_available_slot_index_);
}


template<class SM, class T>
static void BasicTests(T t1, T t2)
{
    SM sm;
    assert(sm.empty());
    assert(sm.size() == 0);
    SM sm2 = sm;
    assert(sm.empty());
    assert(sm.size() == 0);
    auto k1 = sm.insert(t1);
    auto k2 = sm.insert(t2);
    assert(!sm.empty());
    assert(sm.size() == 2);
    assert(sm.begin() + 2 == sm.end());
    assert(sm.find(k1) == sm.begin());
    assert(sm.find(k2) == sm.begin() + 1);
    assert(sm2.empty());
    assert(sm2.size() == 0);
    auto num_removed = sm.erase(k1);
    assert(num_removed == 1);
    assert(sm.size() == 1);
    assert(sm.find(k1) == sm.end());  // find an expired key
    assert(sm.find(k2) == sm.begin());  // find a non-expired key
    assert(sm.erase(k1) == 0);  // erase an expired key
    sm.swap(sm2);
    assert(sm.empty());
    assert(sm2.size() == 1);
    assert(sm2.find(k1) == sm2.end());  // find an expired key
    assert(sm2.find(k2) == sm2.begin());  // find a non-expired key
    assert(sm2.erase(k1) == 0);  // erase an expired key
}

template<class SM, class T>
static void FullContainerStressTest(T t)
{
    const int total = 1000;
    SM sm;
    std::vector<typename SM::key_type> keys;
    for (int i=0; i < total; ++i) {
        auto k = sm.insert(t);
        t += 1;
        keys.push_back(k);
    }
    assert(sm.size() == total);
    std::mt19937 g;
    std::shuffle(keys.begin(), keys.end(), g);
    for (int i = 0; i < total; ++i) {
        assert(sm.size() == static_cast<typename SM::size_type>(total - i));
        assert(sm.find(keys[i]) != sm.end());
        for (int j = 0; j < i; ++j) {
            assert(sm.find(keys[j]) == sm.end());
        }
        auto erased = sm.erase(keys[i]);
        assert(erased == 1);
    }
    assert(sm.empty());
}

template<class SM, class TGen>
static void InsertEraseStressTest(TGen t)
{
    const int total = 1000;
    SM sm;
    std::vector<typename SM::key_type> valid_keys;
    std::vector<typename SM::key_type> expired_keys;
    std::mt19937 g;
    for (int i=0; i < total / 3; ++i) {
        auto k = sm.insert(t());
        valid_keys.push_back(k);
    }
    for (int i = total / 3; i < total; ++i) {
        if (g() % 2 == 0 && !valid_keys.empty()) {
            std::shuffle(valid_keys.begin(), valid_keys.end(), g);
            auto k = valid_keys.back();
            valid_keys.pop_back();
            auto erased = sm.erase(k);
            assert(erased == 1);
            expired_keys.push_back(k);
            for (auto&& ek : expired_keys) {
                assert(sm.find(ek) == sm.end());
            }
        } else {
            auto k = sm.insert(t());
            valid_keys.push_back(k);
        }
    }
}

template<class SM>
static void EraseInLoopTest()
{
    SM sm;
    for (int i=0; i < 100; ++i) {
        sm.insert(i);
    }
    int total = 0;
    for (auto it = sm.begin(); it != sm.end(); /*nothing*/) {
        total += *it;
        if (*it > 50) {
            it = sm.erase(it);
        } else {
            ++it;
        }
    }
    assert(total == 4950);
    int total2 = 0;
    for (auto&& elt : sm) {
        total2 += elt;
    }
    assert(total2 == 1275);
}

template<class SM>
static void EraseRangeTest()
{
    SM sm;
    auto test = [&](int N, int first, int last) {
        sm.erase(sm.begin(), sm.end());
        int expected_total = 0;
        for (int i=0; i < N; ++i) {
            expected_total += i;
            sm.insert(i);
        }
        for (auto it = sm.begin() + first; it != sm.begin() + last; ++it) {
            expected_total -= *it;
        }
        sm.erase(sm.begin() + first, sm.begin() + last);
        int actual_total = 0;
        for (auto it = sm.begin(); it != sm.end(); ++it) {
            actual_total += *it;
        }
        return (actual_total == expected_total);
    };
    assert(test(10, 8, 8));
    assert(test(10, 3, 7));
    assert(test(10, 0, 10));
    assert(test(10, 1, 10));
    assert(test(10, 0, 9));
    assert(test(10, 1, 9));
    for (int N : { 2, 10, 100 }) {
        for (int i=0; i < N; ++i) {
            for (int j=i; j < N; ++j) {
                assert(test(N, i, j));
            }
        }
    }
}

static void TypedefTests()
{
    if (true) {
        using SM = stdext::slot_map<int>;
        static_assert(std::is_same<typename SM::key_type, std::pair<unsigned, unsigned>>::value, "");
        static_assert(std::is_same<typename SM::mapped_type, int>::value, "");
        static_assert(std::is_same<typename SM::key_size_type, unsigned>::value, "");
        static_assert(std::is_same<typename SM::key_generation_type, unsigned>::value, "");
        static_assert(std::is_same<typename SM::container_type, std::vector<int>>::value, "");
        static_assert(std::is_same<typename SM::reference, int&>::value, "");
        static_assert(std::is_same<typename SM::const_reference, const int&>::value, "");
        static_assert(std::is_same<typename SM::pointer, int*>::value, "");
        static_assert(std::is_same<typename SM::const_pointer, const int*>::value, "");
        static_assert(std::is_same<typename SM::iterator, std::vector<int>::iterator>::value, "");
        static_assert(std::is_same<typename SM::const_iterator, std::vector<int>::const_iterator>::value, "");
        static_assert(std::is_same<typename SM::reverse_iterator, std::vector<int>::reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::const_reverse_iterator, std::vector<int>::const_reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::size_type, std::vector<int>::size_type>::value, "");
        static_assert(std::is_same<typename SM::value_type, int>::value, "");
    }
    if (true) {
        using SM = stdext::slot_map<bool>;
        static_assert(std::is_same<typename SM::key_type, std::pair<unsigned, unsigned>>::value, "");
        static_assert(std::is_same<typename SM::mapped_type, bool>::value, "");
        static_assert(std::is_same<typename SM::key_size_type, unsigned>::value, "");
        static_assert(std::is_same<typename SM::key_generation_type, unsigned>::value, "");
        static_assert(std::is_same<typename SM::container_type, std::vector<bool>>::value, "");
        static_assert(std::is_same<typename SM::reference, std::vector<bool>::reference>::value, "");
        static_assert(std::is_same<typename SM::const_reference, std::vector<bool>::const_reference>::value, "");
        static_assert(std::is_same<typename SM::pointer, std::vector<bool>::pointer>::value, "");
        static_assert(std::is_same<typename SM::const_pointer, std::vector<bool>::const_pointer>::value, "");
        static_assert(std::is_same<typename SM::iterator, std::vector<bool>::iterator>::value, "");
        static_assert(std::is_same<typename SM::const_iterator, std::vector<bool>::const_iterator>::value, "");
        static_assert(std::is_same<typename SM::reverse_iterator, std::vector<bool>::reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::const_reverse_iterator, std::vector<bool>::const_reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::size_type, std::vector<bool>::size_type>::value, "");
        static_assert(std::is_same<typename SM::value_type, bool>::value, "");
    }
    if (true) {
        using SM = stdext::slot_map<double, TestKey::key_16_8_t>;
        static_assert(std::is_same<typename SM::key_type, TestKey::key_16_8_t>::value, "");
        static_assert(std::is_same<typename SM::mapped_type, double>::value, "");
        static_assert(std::is_same<typename SM::key_size_type, uint16_t>::value, "");
        static_assert(std::is_same<typename SM::key_generation_type, uint8_t>::value, "");
        static_assert(std::is_same<typename SM::container_type, std::vector<double>>::value, "");
        static_assert(std::is_same<typename SM::reference, double&>::value, "");
        static_assert(std::is_same<typename SM::const_reference, const double&>::value, "");
        static_assert(std::is_same<typename SM::pointer, double*>::value, "");
        static_assert(std::is_same<typename SM::const_pointer, const double*>::value, "");
        static_assert(std::is_same<typename SM::iterator, std::vector<double>::iterator>::value, "");
        static_assert(std::is_same<typename SM::const_iterator, std::vector<double>::const_iterator>::value, "");
        static_assert(std::is_same<typename SM::reverse_iterator, std::vector<double>::reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::const_reverse_iterator, std::vector<double>::const_reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::size_type, std::vector<double>::size_type>::value, "");
        static_assert(std::is_same<typename SM::value_type, double>::value, "");
    }
    if (true) {
        using SM = stdext::slot_map<int, std::pair<char, int>, TestContainer::Vector>;
        static_assert(std::is_same<typename SM::key_type, std::pair<char, int>>::value, "");
        static_assert(std::is_same<typename SM::mapped_type, int>::value, "");
        static_assert(std::is_same<typename SM::key_size_type, char>::value, "");
        static_assert(std::is_same<typename SM::key_generation_type, int>::value, "");
        static_assert(std::is_same<typename SM::container_type, TestContainer::Vector<int>>::value, "");
        static_assert(std::is_same<typename SM::reference, int&>::value, "");
        static_assert(std::is_same<typename SM::const_reference, const int&>::value, "");
        static_assert(std::is_same<typename SM::pointer, int*>::value, "");
        static_assert(std::is_same<typename SM::const_pointer, const int*>::value, "");
        static_assert(std::is_same<typename SM::iterator, TestContainer::Vector<int>::iterator>::value, "");
        static_assert(std::is_same<typename SM::const_iterator, TestContainer::Vector<int>::const_iterator>::value, "");
        static_assert(std::is_same<typename SM::reverse_iterator, TestContainer::Vector<int>::reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::const_reverse_iterator, TestContainer::Vector<int>::const_reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::size_type, unsigned>::value, "");
        static_assert(std::is_same<typename SM::value_type, int>::value, "");
    }
#if __cplusplus >= 201701
    if (true) {
        using SM = stdext::slot_map<double, TestKey::key_11_5_t>;
        static_assert(std::is_same<typename SM::key_type, TestKey::key_11_5_t>::value, "");
        static_assert(std::is_same<typename SM::mapped_type, double>::value, "");
        static_assert(std::is_same<typename SM::key_size_type, uint16_t>::value, "");
        static_assert(std::is_same<typename SM::key_generation_type, uint8_t>::value, "");
        static_assert(std::is_same<typename SM::container_type, std::vector<double>>::value, "");
        static_assert(std::is_same<typename SM::reference, double&>::value, "");
        static_assert(std::is_same<typename SM::const_reference, const double&>::value, "");
        static_assert(std::is_same<typename SM::pointer, double*>::value, "");
        static_assert(std::is_same<typename SM::const_pointer, const double*>::value, "");
        static_assert(std::is_same<typename SM::iterator, std::vector<double>::iterator>::value, "");
        static_assert(std::is_same<typename SM::const_iterator, std::vector<double>::const_iterator>::value, "");
        static_assert(std::is_same<typename SM::reverse_iterator, std::vector<double>::reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::const_reverse_iterator, std::vector<double>::const_reverse_iterator>::value, "");
        static_assert(std::is_same<typename SM::size_type, std::vector<double>::size_type>::value, "");
        static_assert(std::is_same<typename SM::value_type, double>::value, "");
    }
#endif
}

void sg14_test::slot_map_test()
{
    TypedefTests();

    BasicTests<stdext::slot_map<int>>(42, 37);
    FullContainerStressTest<stdext::slot_map<int>>(0);
    InsertEraseStressTest<stdext::slot_map<int>>([i=3]() mutable { return ++i; });
    EraseInLoopTest<stdext::slot_map<int>>();
    EraseRangeTest<stdext::slot_map<int>>();

    BasicTests<stdext::slot_map<unsigned long, TestKey::key_16_8_t>>(42, 37);
    FullContainerStressTest<stdext::slot_map<unsigned long, TestKey::key_16_8_t>>(42);
    InsertEraseStressTest<stdext::slot_map<unsigned long, TestKey::key_16_8_t>>([i=3]() mutable { return ++i; });
    EraseInLoopTest<stdext::slot_map<int, TestKey::key_16_8_t>>();
    EraseRangeTest<stdext::slot_map<int, TestKey::key_16_8_t>>();

    BasicTests<stdext::slot_map<int, std::pair<uint16_t, uint16_t>, TestContainer::Vector>>(42, 37);

#if __cplusplus >= 201701
    BasicTests<stdext::slot_map<int, TestKey::key_11_5_t>>(42, 37);
    FullContainerStressTest<stdext::slot_map<unsigned long, TestKey::key_11_5_t>>(42);
    InsertEraseStressTest<stdext::slot_map<unsigned long, TestKey::key_11_5_t>>([i=3]() mutable { return ++i; });
    EraseInLoopTest<stdext::slot_map<int, TestKey::key_11_5_t>>();
    EraseRangeTest<stdext::slot_map<int, TestKey::key_11_5_t>>();
#endif
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::slot_map_test();
}
#endif
