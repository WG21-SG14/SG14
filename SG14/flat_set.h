/*
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

// This is an implementation of the proposed "std::flat_set" as specified in
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1222r1.pdf

#include <stddef.h>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <vector>

namespace stdext {

namespace flatset_detail {
    template<class T, class = void> struct qualifies_as_range : std::false_type {};
    template<class T> struct qualifies_as_range<T, decltype(
        std::begin( std::declval<T()>()() ), void(),
        std::end( std::declval<T()>()() ), void()
    )> : std::true_type {};

    template<class It>
    using is_random_access_iterator = std::is_convertible<
        typename std::iterator_traits<It>::iterator_category,
        std::random_access_iterator_tag
    >;

    template<int I> struct priority_tag : priority_tag<I-1> {};
    template<> struct priority_tag<0> {};

    // As proposed in P0591R4. Guaranteed copy elision makes this do the right thing.
    template<class T, class Alloc, class... Args, class = std::enable_if_t<
        std::uses_allocator<T, Alloc>::value && std::is_constructible<T, std::allocator_arg_t, const Alloc&, Args&&...>::value
    >>
    T make_obj_using_allocator_(priority_tag<3>, const Alloc& alloc, Args&&... args) {
        return T(std::allocator_arg, alloc, static_cast<Args&&>(args)...);
    }
    template<class T, class Alloc, class... Args, class = std::enable_if_t<
        std::uses_allocator<T, Alloc>::value && std::is_constructible<T, Args&&..., const Alloc&>::value
    >>
    T make_obj_using_allocator_(priority_tag<2>, const Alloc& alloc, Args&&... args) {
        return T(static_cast<Args&&>(args)..., alloc);
    }
    template<class T, class Alloc, class... Args, class = std::enable_if_t<
        !std::uses_allocator<T, Alloc>::value && std::is_constructible<T, Args&&...>::value
    >>
    T make_obj_using_allocator_(priority_tag<1>, const Alloc&, Args&&... args) {
        return T(static_cast<Args&&>(args)...);
    }
    template<class T, class Alloc, class... Args>
    T make_obj_using_allocator_(priority_tag<0>, const Alloc&, Args&&...) {
        static_assert(sizeof(T)==0, "this request for uses-allocator construction is ill-formed");
    }
    template <class T, class Alloc, class... Args>
    T make_obj_using_allocator(const Alloc& alloc, Args&&... args) {
        return make_obj_using_allocator_<T>(priority_tag<3>(), alloc, static_cast<Args&&>(args)...);
    }

    template<class Container>
    using cont_value_type = typename Container::value_type;

    template<class InputIterator>
    using iter_value_type = std::remove_const_t<typename std::iterator_traits<InputIterator>::value_type>;

    template<class...> using void_t = void;

    template<class A, class = void>
    struct qualifies_as_allocator : std::false_type {};
    template<class A>
    struct qualifies_as_allocator<A, void_t<
        typename A::value_type,
        decltype(std::declval<A&>().allocate(size_t{}))
    >> : std::true_type {};

    template<class It>
    using qualifies_as_input_iterator = std::bool_constant<!std::is_integral<It>::value>;

} // namespace flatset_detail

#ifndef STDEXT_HAS_SORTED_UNIQUE
#define STDEXT_HAS_SORTED_UNIQUE

struct sorted_unique_t { explicit sorted_unique_t() = default; };

#if defined(__cpp_inline_variables)
inline
#endif
constexpr sorted_unique_t sorted_unique {};

#endif // STDEXT_HAS_SORTED_UNIQUE

template<
    class Key,
    class Compare = std::less<Key>,
    class KeyContainer = std::vector<Key>
>
class flat_set {
    static_assert(flatset_detail::is_random_access_iterator<typename KeyContainer::iterator>::value, "");
    static_assert(std::is_same<Key, typename KeyContainer::value_type>::value, "");
public:
    using key_type = Key;
    using key_compare = Compare;
    using value_type = Key;
    using value_compare = Compare;
    using reference = Key&;
    using const_reference = const Key&;
    using size_type = size_t; // TODO: this should be KeyContainer::size_type
    using difference_type = ptrdiff_t; // TODO: this should be KeyContainer::difference_type
    using iterator = typename KeyContainer::iterator;
    using const_iterator = typename KeyContainer::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using container_type = KeyContainer;

// =========================================================== CONSTRUCTORS
// This is all one massive overload set!

    flat_set() : flat_set(Compare()) {}

    explicit flat_set(KeyContainer ctr)
        : c_(std::move(ctr)), compare_()
    {
        std::sort(std::begin(c_), std::end(c_), compare_);
    }

    // TODO: surely this should be using uses-allocator construction
    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(KeyContainer ctr, const Alloc& a)
        : flat_set(KeyContainer(std::move(ctr), a)) {}

    template<class Container,
             std::enable_if_t<flatset_detail::qualifies_as_range<const Container&>::value, int> = 0>
    explicit flat_set(const Container& cont)
        : flat_set(std::begin(cont), std::end(cont), Compare()) {}

    template<class Container, class Alloc,
             class = std::enable_if_t<flatset_detail::qualifies_as_range<const Container&>::value && std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(const Container& cont, const Alloc& a)
        : flat_set(std::begin(cont), std::end(cont), Compare(), a) {}

    flat_set(stdext::sorted_unique_t, KeyContainer ctr)
        : c_(std::move(ctr)), compare_() {}

    // TODO: surely this should be using uses-allocator construction
    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, KeyContainer ctr, const Alloc& a)
        : flat_set(sorted_unique, KeyContainer(std::move(ctr), a)) {}

    template<class Container,
             class = std::enable_if_t<flatset_detail::qualifies_as_range<const Container&>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, const Container& cont)
        : flat_set(sorted_unique, std::begin(cont), std::end(cont), Compare()) {}

    template<class Container, class Alloc,
             class = std::enable_if_t<flatset_detail::qualifies_as_range<const Container&>::value && std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, const Container& cont, const Alloc& a)
        : flat_set(sorted_unique, std::begin(cont), std::end(cont), Compare(), a) {}

    explicit flat_set(const Compare& comp)
        : c_(), compare_(comp) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(const Compare& comp, const Alloc& a)
        : c_(flatset_detail::make_obj_using_allocator<KeyContainer>(a)), compare_(comp) {}

    template<class Alloc,
             std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value, int> = 0>
    explicit flat_set(const Alloc& a)
        : flat_set(Compare(), a) {}

    // TODO: all templates taking InputIterator probably need to be constrained
    template<class InputIterator>
    flat_set(InputIterator first, InputIterator last, const Compare& comp = Compare())
        : c_(first, last), compare_(comp)
    {
        std::sort(std::begin(c_), std::end(c_), compare_);
    }

    // TODO: this constructor should conditionally use KeyContainer's iterator-pair constructor
    template<class InputIterator, class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(InputIterator first, InputIterator last, const Compare& comp, const Alloc& a)
        : c_(flatset_detail::make_obj_using_allocator<KeyContainer>(a)), compare_(comp)
    {
        while (first != last) {
            c_.insert(c_.end(), *first);
            ++first;
        }
        std::sort(std::begin(c_), std::end(c_), compare_);
    }

    template<class InputIterator, class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(InputIterator first, InputIterator last, const Alloc& a)
        : flat_set(first, last, Compare(), a) {}

    template<class InputIterator>
    flat_set(stdext::sorted_unique_t, InputIterator first, InputIterator last, const Compare& comp = Compare())
        : c_(first, last), compare_(comp) {}

    // TODO: this constructor should conditionally use KeyContainer's iterator-pair constructor
    template<class InputIterator, class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t, InputIterator first, InputIterator last,
             const Compare& comp, const Alloc& a)
        : c_(flatset_detail::make_obj_using_allocator<KeyContainer>(a)), compare_(comp)
    {
        while (first != last) {
            c_.insert(c_.end(), *first);
            ++first;
        }
    }

    template<class InputIterator, class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, InputIterator first, InputIterator last, const Alloc& a)
        : flat_set(sorted_unique, first, last, Compare(), a) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(flat_set&& m, const Alloc& a)
        : c_(std::move(m.c_), a), compare_(std::move(m.compare_)) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(const flat_set& m, const Alloc& a)
        : c_(m.c_, a), compare_(m.compare_) {}

    flat_set(std::initializer_list<Key>&& il, const Compare& comp = Compare())
        : flat_set(il, comp) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(std::initializer_list<Key>&& il, const Compare& comp, const Alloc& a)
        : flat_set(il, comp, a) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(std::initializer_list<Key>&& il, const Alloc& a)
        : flat_set(il, Compare(), a) {}

    flat_set(stdext::sorted_unique_t sorted_unique, std::initializer_list<Key>&& il, const Compare& comp = Compare())
        : flat_set(sorted_unique, il, comp) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, std::initializer_list<Key>&& il, const Compare& comp, const Alloc& a)
        : flat_set(sorted_unique, il, comp, a) {}

    template<class Alloc,
             class = std::enable_if_t<std::uses_allocator<KeyContainer, Alloc>::value>>
    flat_set(stdext::sorted_unique_t sorted_unique, std::initializer_list<Key>&& il, const Alloc& a)
        : flat_set(sorted_unique, il, Compare(), a) {}


// ========================================================== OTHER MEMBERS

    flat_set& operator=(std::initializer_list<Key> il) {
        this->clear();
        this->insert(il);
    }

    iterator begin() noexcept { return std::begin(c_); }
    const_iterator begin() const noexcept { return std::begin(c_); }
    iterator end() noexcept { return std::end(c_); }
    const_iterator end() const noexcept { return std::end(c_); }

    const_iterator cbegin() const noexcept { return std::cbegin(c_); }
    const_iterator cend() const noexcept { return std::cend(c_); }

    reverse_iterator rbegin() noexcept { return std::rbegin(c_); }
    const_reverse_iterator rbegin() const noexcept { return std::rbegin(c_); }
    reverse_iterator rend() noexcept { return std::rend(c_); }
    const_reverse_iterator rend() const noexcept { return std::rend(c_); }

    const_reverse_iterator crbegin() const noexcept { return std::crbegin(c_); }
    const_reverse_iterator crend() const noexcept { return std::crend(c_); }

    [[nodiscard]] bool empty() const noexcept { return c_.empty(); }
    size_type size() const noexcept { return c_.size(); }
    size_type max_size() const noexcept { return c_.max_size(); }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key t(std::forward<Args>(args)...);
        auto it = std::lower_bound(begin(), end(), t, std::ref(compare_));
        if (it == c_.end() || compare_(t, *it)) {
            it = c_.emplace(it, std::move(t));
            return {it, true};
        } else {
            return {it, false};
        }
    }

    // TODO: this function cannot possibly meet its amortized-constant-complexity requirement
    template<class... Args>
    iterator emplace_hint(const_iterator, Args&&... args) {
        return this->emplace(static_cast<Args&&>(args)...).first;
    }

    std::pair<iterator, bool> insert(const Key& t) {
        auto it = std::lower_bound(begin(), end(), t, std::ref(compare_));
        if (it == c_.end() || compare_(t, *it)) {
            it = c_.emplace(it, t);
            return {it, true};
        } else {
            return {it, false};
        }
    }

    std::pair<iterator, bool> insert(Key&& t) {
        auto it = std::lower_bound(begin(), end(), t, std::ref(compare_));
        if (it == c_.end() || compare_(t, *it)) {
            it = c_.emplace(it, std::move(t));
            return {it, true};
        } else {
            return {it, false};
        }
    }

    // TODO: this function cannot possibly meet its amortized-constant-complexity requirement
    iterator insert(const_iterator, const Key& t) {
        return this->insert(t).first;
    }

    // TODO: this function cannot possibly meet its amortized-constant-complexity requirement
    iterator insert(const_iterator, Key&& t) {
        return this->insert(std::move(t)).first;
    }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        while (first != last) {
            this->insert(*first);
            ++first;
        }
    }

    template<class InputIterator>
    void insert(stdext::sorted_unique_t, InputIterator first, InputIterator last) {
        auto it = begin();
        while (first != last) {
            Key t(*first);
            it = std::lower_bound(it, this->end(), t, std::ref(compare_));
            if (it == c_.end() || compare_(t, *it)) {
                it = c_.emplace(it, std::move(t));
            }
            ++it;
            ++first;
        }
    }

    void insert(std::initializer_list<Key> il) {
        this->insert(il.begin(), il.end());
    }

    void insert(stdext::sorted_unique_t sorted_unique, std::initializer_list<Key> il) {
        this->insert(sorted_unique, il.begin(), il.end());
    }

    // TODO: as specified, this function fails to preserve the allocator, and has UB for std::pmr containers
    KeyContainer extract() && {
        KeyContainer temp;
        temp.swap(c_);
        return temp;
    }

    void replace(KeyContainer&& ctr) {
        c_ = std::move(ctr);
    }

    iterator erase(iterator position) {
        return c_.erase(position);
    }

    iterator erase(const_iterator position) {
        return c_.erase(position);
    }

    size_type erase(const key_type& t) {
        auto it = this->find(t);
        if (it != c_.end()) {
            this->erase(it);
            return 1;
        }
        return 0;
    }

    iterator erase(const_iterator first, const_iterator last) {
        c_.erase(first, last);
    }

    template<class KeyContainer_ = KeyContainer,
             // TODO: this is insane
             class = std::enable_if_t<std::is_nothrow_swappable<KeyContainer_>::value && std::is_nothrow_swappable<Compare>::value>>
    void swap(flat_set& m) noexcept {
        using std::swap;
        swap(c_, m.c_);
        swap(compare_, m.compare_);
    }

    void clear() noexcept {
        c_.clear();
    }

    Compare key_comp() const { return compare_; }
    Compare value_comp() const { return compare_; }

    iterator find(const Key& t) {
        auto it = std::lower_bound(this->begin(), this->end(), t, std::ref(compare_));
        if (it == c_.end() || compare_(t, *it)) {
            return this->end();
        }
        return it;
    }

    const_iterator find(const Key& t) const {
        auto it = std::lower_bound(this->begin(), this->end(), t, std::ref(compare_));
        if (it == c_.end() || compare_(t, *it)) {
            return this->end();
        }
        return it;
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    iterator find(const K& x) {
        auto it = std::lower_bound(this->begin(), this->end(), x, std::ref(compare_));
        if (it == c_.end() || compare_(x, *it)) {
            return this->end();
        }
        return it;
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    const_iterator find(const K& x) const {
        auto it = std::lower_bound(this->begin(), this->end(), x, std::ref(compare_));
        if (it == c_.end() || compare_(x, *it)) {
            return this->end();
        }
        return it;
    }

    size_type count(const Key& x) const {
        return this->contains(x) ? 1 : 0;
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    size_type count(const K& x) const {
        return this->contains(x) ? 1 : 0;
    }

    bool contains(const Key& x) const {
        return this->find(x) != this->end();
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    bool contains(const K& x) const {
        return this->find(x) != this->end();
    }

    iterator lower_bound(const Key& t) {
        return std::lower_bound(this->begin(), this->end(), t, std::ref(compare_));
    }

    const_iterator lower_bound(const Key& t) const {
        return std::lower_bound(this->begin(), this->end(), t, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    iterator lower_bound(const K& x) {
        return std::lower_bound(this->begin(), this->end(), x, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    const_iterator lower_bound(const K& x) const {
        return std::lower_bound(this->begin(), this->end(), x, std::ref(compare_));
    }

    iterator upper_bound(const Key& t) {
        return std::upper_bound(this->begin(), this->end(), t, std::ref(compare_));
    }

    const_iterator upper_bound(const Key& t) const {
        return std::upper_bound(this->begin(), this->end(), t, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    iterator upper_bound(const K& x) {
        return std::upper_bound(this->begin(), this->end(), x, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    const_iterator upper_bound(const K& x) const {
        return std::upper_bound(this->begin(), this->end(), x, std::ref(compare_));
    }

    std::pair<iterator, iterator> equal_range(const Key& t) {
        return std::equal_range(this->begin(), this->end(), t, std::ref(compare_));
    }

    std::pair<const_iterator, const_iterator> equal_range(const Key& t) const {
        return std::equal_range(this->begin(), this->end(), t, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    std::pair<iterator, iterator> equal_range(const K& x) {
        return std::equal_range(this->begin(), this->end(), x, std::ref(compare_));
    }

    template<class K,
             class Compare_ = Compare, class = typename Compare_::is_transparent>
    std::pair<const_iterator, const_iterator> equal_range(const K& x) const {
        return std::equal_range(this->begin(), this->end(), x, std::ref(compare_));
    }

private:
    KeyContainer c_;
    Compare compare_;
};

// TODO: all six comparison operators should be invisible friends
template<class Key, class Compare, class KeyContainer>
bool operator==(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return std::equal(x.begin(), x.end(), y.begin(), y.end());
}

template<class Key, class Compare, class KeyContainer>
bool operator!=(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return !(x == y);
}

template<class Key, class Compare, class KeyContainer>
bool operator<(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<class Key, class Compare, class KeyContainer>
bool operator>(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return (y < x);
}

template<class Key, class Compare, class KeyContainer>
bool operator<=(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return !(y < x);
}

template<class Key, class Compare, class KeyContainer>
bool operator>=(const flat_set<Key, Compare, KeyContainer>& x, const flat_set<Key, Compare, KeyContainer>& y)
{
    return !(x < y);
}

template<class Key, class Compare, class KeyContainer,
         // TODO: this is insane
         class = std::enable_if_t<std::is_nothrow_swappable<KeyContainer>::value && std::is_nothrow_swappable<Compare>::value>>
void swap(flat_set<Key, Compare, KeyContainer>& x, flat_set<Key, Compare, KeyContainer>& y) noexcept
{
    return x.swap(y);
}

#if defined(__cpp_deduction_guides)

// TODO: this deduction guide should maybe be constrained by qualifies_as_range
template<class Container,
         class = std::enable_if_t<!flatset_detail::qualifies_as_allocator<Container>::value>>
flat_set(Container)
    -> flat_set<flatset_detail::cont_value_type<Container>>;

template<class Container, class Allocator,
         class = std::enable_if_t<!flatset_detail::qualifies_as_allocator<Container>::value &&
                                  flatset_detail::qualifies_as_allocator<Allocator>::value &&
                                  std::uses_allocator<Container, Allocator>::value>>
flat_set(Container, Allocator)
    -> flat_set<flatset_detail::cont_value_type<Container>>;

template<class Container,
         class = std::enable_if_t<!flatset_detail::qualifies_as_allocator<Container>::value>>
flat_set(sorted_unique_t, Container)
    -> flat_set<flatset_detail::cont_value_type<Container>>;

template<class Container, class Allocator,
         class = std::enable_if_t<!flatset_detail::qualifies_as_allocator<Container>::value &&
                                  flatset_detail::qualifies_as_allocator<Allocator>::value &&
                                  std::uses_allocator<Container, Allocator>::value>>
flat_set(sorted_unique_t, Container, Allocator)
    -> flat_set<flatset_detail::cont_value_type<Container>>;

template<class InputIterator, class Compare = std::less<flatset_detail::iter_value_type<InputIterator>>,
         class = std::enable_if_t<flatset_detail::qualifies_as_input_iterator<InputIterator>::value &&
                                  !flatset_detail::qualifies_as_allocator<Compare>::value>>
flat_set(InputIterator, InputIterator, Compare = Compare())
    -> flat_set<flatset_detail::iter_value_type<InputIterator>, Compare>;

template<class InputIterator, class Compare, class Allocator,
         class = std::enable_if_t<flatset_detail::qualifies_as_input_iterator<InputIterator>::value &&
                                  !flatset_detail::qualifies_as_allocator<Compare>::value &&
                                  flatset_detail::qualifies_as_allocator<Allocator>::value>>
flat_set(InputIterator, InputIterator, Compare, Allocator)
    -> flat_set<flatset_detail::iter_value_type<InputIterator>, Compare>;

template<class InputIterator, class Allocator,
         class = std::enable_if_t<flatset_detail::qualifies_as_input_iterator<InputIterator>::value &&
                                  flatset_detail::qualifies_as_allocator<Allocator>::value>>
flat_set(InputIterator, InputIterator, Allocator, int=0/*to please MSVC*/)
    -> flat_set<flatset_detail::iter_value_type<InputIterator>>;

#endif

} // namespace stdext
