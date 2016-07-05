<span class="c10">Document number</span><span>:</span>

<span class="c10">Date</span><span>: 2016-07-04</span>

<span class="c10">Audience</span><span>: LEWG, SG14</span>

<span class="c10">Reply-to</span><span>: Carl Cook, </span><span
class="c2"><carlcook@optiver.com></span>

<span class="c10">Reply-to</span><span>: Nicolas Fleury, </span><span
class="c2"><nidoizo@gmail.com></span>

<span></span>

<span>Non-allocating standard functions</span> {#h.vem8eanc1cip .c7 .c8}
==============================================

<span class="c2">[Non-allocating standard
functions](#h.vem8eanc1cip){.c1}</span>

<span class="c2">[1. Introduction](#h.e1fsool6rkss){.c1}</span>

<span class="c2">[2. Motivation](#h.2rmr1bf3t90m){.c1}</span>

<span class="c2">[3. Impact on the
standard](#h.mu2kipya0hsk){.c1}</span>

<span class="c2">[4. Design decisions](#h.fwkm8cswsn15){.c1}</span>

<span class="c2">[Relation with
std::function](#h.e2emxsxrnl5j){.c1}</span>

<span class="c2">[Name](#h.mp1ubv50ocpk){.c1}</span>

<span class="c2">[Class signature](#h.pweswbwpz4rk){.c1}</span>

<span class="c2">[Compilation-time
guarantee](#h.ewvkbatoneg3){.c1}</span>

<span class="c2">[Copy/Move/Destruction](#h.2bfcprsu3jtb){.c1}</span>

<span class="c2">[Memory layout](#h.t6nq5w6cwbob){.c1}</span>

<span class="c2">[Trivial/non trivial classes
split](#h.vnvywso9s2n7){.c1}</span>

<span class="c2">[Base class without size](#h.ij2cbif6y8ho){.c1}</span>

<span class="c2">[Swapping](#h.h3c73x899wlo){.c1}</span>

<span class="c2">[5. Technical
specifications](#h.k6cwhky1yjtc){.c1}</span>

<span class="c2">[6. Sample use](#h.po7uwc3i8km8){.c1}</span>

<span class="c2">[7. Future work](#h.aaghmstz8r9f){.c1}</span>

<span class="c2">[8. Acknowledgements](#h.d9cwpby1wg3y){.c1}</span>

<span class="c2">[9. Existing
implementations](#h.71oh0sm385wa){.c1}</span>

<span class="c2">[10. References](#h.ilguggivzf00){.c1}</span>

<span class="c2">[11. Related work](#h.ixbije8rxa48){.c1}</span>

<span></span>

<span></span>

<span>1. Introduction</span> {#h.e1fsool6rkss .c7 .c8}
----------------------------

<span></span>

<span>This paper is to outline the motivation for adding non-allocating
standard functions to the standard library.</span>

<span></span>

<span>2. Motivation</span> {#h.2rmr1bf3t90m .c7 .c8}
--------------------------

<span></span>

<span>The introduction of std::function, a polymorphic wrapper over
callable targets, has been widely appreciated by C++ users. It gives the
ability to assign from several callable target types, pass functions by
value, and invoke targets with the familiar function call syntax.</span>

<span></span>

<span>std::function generally incurs a dynamic allocation on assignment
of the target function (the exception being the small object
optimization for function pointers and std::reference\_wrappers). For
performance critical software, this overhead, while seemingly low, is
unacceptable.</span>

<span></span>

<span>Within the SG14 reflector, so far we have found six
implementations of non allocating functions that are used in commercial
games and high frequency trading
applications.</span>^[\[1\]](#ftnt1){#ftnt_ref1}^<span> This suggests
that the problem of dynamic allocation is real, and that a standardised
non-allocating function would be of use.</span>

<span></span>

<span>3. Impact on the standard</span> {#h.mu2kipya0hsk .c7 .c8}
--------------------------------------

<span></span>

<span>This proposal is a pure library extension. It does not require
changes to any standard classes, functions or headers, and it does not
affect the application binary interface.</span>

<span></span>

<span>4. Design decisions</span> {#h.fwkm8cswsn15 .c7 .c8}
--------------------------------

### <span>Relation with std::function</span> {#h.e2emxsxrnl5j .c7 .c8}

<span>The first discussion on SG14 was about adding a base class to
std::function (or make std::function a template typedef) that is more
flexible to prevent heap usage.  However as discussion evolved, the
conclusion is that is what is wanted is another class,
std::inplace\_function, dedicated to being allocation-less.</span>

<span></span>

<span>For that new class, sharing a base class with </span><span
class="c4">std::function</span><span> was discussed, to be able to pass
function objects by reference without dependence on how it’s stored.
 However, that might not be worth the burden in implementation
restrictions, and would break the ABI with the existing </span><span
class="c4">std::function</span><span>.  Instead, std::</span><span
class="c4">inplace\_function</span><span> class can prioritize
performance without compromise, and still conform to the </span><span
class="c4">std::function</span><span> interface.</span>

<span></span>

<span>Copying from </span><span
class="c4">std::inplace\_function</span><span> to </span><span
class="c4">std::function</span><span> of the same function signature
should be supported, as </span><span
class="c4">std::function</span><span> supports any function size.
 However, so far, copying from </span><span
class="c4">std::function</span><span> to </span><span
class="c4">std::inplace\_function</span><span> would not be allowed, as
it risks breaking the compile-time guarantees of </span><span
class="c4">std::inplace\_function</span><span> (an option here is to
throw a runtime exception if the target buffer is too small).</span>

<span></span>

<span>It might be worth noting that a codebase preferring </span><span
class="c4">std::inplace\_function</span><span> to </span><span
class="c4">std::function</span><span> will probably always prefer
it.</span>

### <span>Name</span> {#h.mp1ubv50ocpk .c7 .c8}

<span>The name static\_function is something that could first come up
when thinking of an embedded buffer, however with the meaning of “static
function” in C++, it would sound confusing.  So far the name suggested
is inplace\_function, as it implies the buffer is embedded, whatever the
size of the function.  Since a lambda could end up with multiple
closures, this is a detail important to be understood as a programmer
has to explicitly increase the template size argument.  It could make
sense to adopt the same nomenclature of proposals like inline\_vector,
so inline\_function (or inplace\_vector), to have a common suffix for
different standard utilities with embedded buffers.</span>

### <span>Class signature</span> {#h.pweswbwpz4rk .c7 .c8}

<span class="c4">template&lt;typename Signature, size\_t Capacity =
/\*default-capacity\*/, size\_t Alignment =
/\*default-alignment\*/&gt;</span>

<span class="c4">class inplace\_function;</span>

<span></span>

-   <span class="c4">Capacity</span><span> is the size of the internal
    buffer</span>
-   <span class="c4">Alignment</span><span> is the largest supported
    alignment of assigned functions</span>
-   <span class="c4">Default-capacity</span><span> is
    implementation-defined</span>
-   <span class="c4">Default-alignment</span><span> is
    implementation-defined</span>

### <span>Compilation-time guarantee</span> {#h.ewvkbatoneg3 .c7 .c8}

<span>Since the buffer size and alignment is known at compilation-time,
then assigned functions are validated at compilation-time to be of
proper size and alignment.  Function size can be at most the buffer
size, and function alignment can be at most the alignment.  Internal to
</span><span class="c4">std::inplace\_function</span><span>,
</span><span class="c4">static\_assert</span><span> should be used for
these validations.</span>

<span></span>

<span>The only run-time error inside </span><span
class="c4">std::inplace\_function</span><span> itself is when calling it
without any function assigned.</span>

### <span>Copy/Move/Destruction</span> {#h.2bfcprsu3jtb .c7 .c8}

<span>Proper copy, move and destruction are all supported for the
embedded function.</span>

### <span>Memory layout</span> {#h.t6nq5w6cwbob .c7 .c8}

<span>Memory layout is left to implementation, however we can note that
all implementations we have found so far have taken the same approach of
storing function pointers directly as members to avoid the indirection
of type-erasing using a vtable, as well as a properly aligned buffer to
store the function.  The function pointers are used for four things:
calling, copying, moving and destroying.  The same function can be used
for multiple tasks.  However, since calling performance is the most
important and has a unique signature, the function pointer for calling
should probably be dedicated to that task.  Also, the buffer storing the
function will be used for calling, but its last bytes may have a high
chance of not being used.  So optimal memory layout can actually depends
on </span><span class="c4">Alignment</span><span>, as follows:</span>

<span></span>

<span class="c4">Alignment &lt;= sizeof(void\*)</span>

<span>I</span><span>t is optimal to store the members in this
order:</span>

1.  <span>CallerFctPtr</span>
2.  <span>Buffer</span>
3.  <span>ManagementFctPtr</span>

<span></span>

<span class="c4">Alignment == 2\*sizeof(void\*)</span>

<span>Y</span><span>ou want avoid wasted space in padding in the first
cache line, and members should be stored in this order:</span>

1.  <span>CallerFctPtr</span>
2.  <span>ManagementFctPtr</span>
3.  <span>Buffer</span>

<span></span>

<span class="c4">Alignment &gt; 2\*sizeof(void\*)</span>

<span>T</span><span>hen the same logic applies if the implementation
would use more than two function pointers:</span>

1.  <span>CallerFctPtr</span>
2.  <span>DestructionFctPtr</span>
3.  <span>CopyAndMoveFctPtr</span>
4.  <span>Buffer</span>

<span></span>

<span>Overall we tend to think it’s better to put the Destroy, Copy and
Move routines inside the same management function, similar to gcc’s
implementation of </span><span class="c4">std::function.</span>

### <span>Trivial/non trivial classes split</span> {#h.vnvywso9s2n7 .c7 .c8}

<span>An additional </span><span
class="c4">std::inplace\_trivial\_function</span><span> class could be
provided to avoid storing function pointers to management routines that
are not used.  However, the flexible member layout that can be used
depending on alignment reduces this need, by storing members in terms of
optimal cache locality.</span>

### <span>Base class without size</span> {#h.ij2cbif6y8ho .c7 .c8}

<span>A base class </span><span
class="c4">std::inplace\_function\_base</span><span> without the
</span><span class="c4">Capacity</span><span> template argument could be
added, to allow passing a </span><span
class="c4">std::inplace\_function</span><span> object of any capacity as
an argument.  It would contain the caller function pointer.  However to
be fully functional it would need to pass the </span><span
class="c4">this</span><span> pointer to the caller function or have an
additional template argument with alignment to be able to perform a
proper down cast upon invocation.</span>

<span></span>

<span>The base class would require deleted or protected copy
constructors to avoid object slicing, meaning a solution like proposal
of </span><span
class="c4">std::unique\_function</span>^[\[2\]](#ftnt2){#ftnt_ref2}^<span> could
be used instead.  A proposal like </span><span
class="c4">std::unique\_function</span><span> sounds more powerful for
this kind of need, by allowing wrapping of any </span><span
class="c4">callable</span><span> type.</span>

### <span>Swapping</span> {#h.h3c73x899wlo .c7 .c8}

<span>We have seen some implementations with support for swapping.
 However, we have seen some implementations that would not properly
support certain functor types.  For example, suppose the buffer is
implemented by the following member:</span>

<span></span>

<span>    </span><span class="c4">std::aligned\_storage&lt;CapacityT,
AlignmentT&gt; \_M\_data;</span>

<span></span>

<span>You cannot do something as simple as this in the swap
function:</span>

<span></span>

<span class="c4">    std::swap(\_M\_data, other.\_M\_data);</span>

<span></span>

<span>Since the two buffers can contain different types (functors),
swapping must be done through three different moves and would only work
for two buffers of same size:</span>

<span></span>

<span class="c4">    std::aligned\_storage&lt;Capacity, Alignment&gt;
tempData;</span>

<span class="c4">    std::move(\_M\_data, tempData);</span>

<span class="c4">    std::move(other.\_M\_data,
this-&gt;\_M\_data);</span>

<span class="c4">    std::move(tempData, other.\_M\_data);</span>

<span></span>

<span>5. Technical specifications</span> {#h.k6cwhky1yjtc .c7 .c8}
----------------------------------------

<span class="c4 c11"></span>

<span class="c4 c6">\
</span><span class="c6 c9">template &lt;typename Signature, size\_t
Capacity = /\*InplaceFunctionDefaultCapacity\*/, size\_t Alignment =
/\*InplaceFunctionDefaultAlignment\*/&gt;\
class inplace\_function;\
\
template &lt;typename R, typename... Args, size\_t Capacity, size\_t
Alignment&gt;\
class inplace\_function&lt;R(Args...), Capacity, Alignmen</span><span
class="c6 c9">t</span><span class="c6 c9">&gt;\
{\
Public:</span>

<span class="c6 c9">// Creates an empty function\
        inplace\_function();</span>

<span class="c6 c9"></span>

<span class="c6 c9">// Destroys the inplace\_function. If the stored
callable is valid, it is destroyed also\
        \~inplace\_function();\
</span>

<span class="c6 c9">// Creates an implace function, copying the target
of other within the internal buffer</span>

<span class="c6 c9">// If the callable is larger than the internal
buffer, a compile-time error is issued</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when copying the target object\
        template&lt;typename Callable&gt;\
        inplace\_function(const Callable& target);\
        \
        // Moves the target of  an implace function, storing the
callable within the internal buffer</span>

<span class="c6 c9">// If the callable is larger than the internal
buffer, a compile-time error is issued</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when moving the target object\
        template&lt;typename Callable&gt;\
        inplace\_function(Callable&& target);\
        </span>

<span class="c6 c9">// Copy construct an implace\_function, storing a
copy of other’s target internally</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when copying the target object\
        inplace\_function(const inplace\_function& other);\
        </span>

<span class="c6 c9">// Move construct an implace\_function, moving the
other’s target to this inplace\_function’s internal buffer</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when moving the target object\
        inplace\_function(inplace\_function&& other);</span>

<span class="c6 c9">\
        // Allows for copying from inplace\_function object of the same
type, but with a smaller buffer</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when copying the target object</span>

<span class="c6 c9">// If OtherCapacity is greater than Capacity, a
compile-time error is issued.\
        template&lt;size\_t OtherCapacity&gt;\
        inplace\_function(const inplace\_function&lt;R(Args...),
OtherCapacity&gt;& other);</span>

<span class="c6 c9">\
        // Allows for moving an inplace\_function object of the same
type, but with a smaller buffer</span>

<span class="c6 c9">// May throw any exception encountered by the
constructor when moving the target object. If OtherCapacity is greater
than Capacity, a compile-time error is issued.\
        template&lt;size\_t OtherCapacity&gt;\
        inplace\_function(inplace\_function&lt;R(Args...),
OtherCapacity&gt;&& other);\
        </span>

<span class="c6 c9">// Assigns a copy of other’s target</span>

<span class="c6 c9">// May throw any exception encountered by the
assignment operator when copying the target object\
        inplace\_function& operator=(const inplace\_function& other);\
        </span>

<span class="c6 c9">// Assigns the other’s target by way of
moving</span>

<span class="c6 c9">// May throw any exception encountered by the
assignment operator when moving the target object\
        inplace\_function& operator=(inplace\_function&& other);</span>

<span class="c6 c9">\
        // Allows for copy assignment of an inplace\_function object of
the same type, but with a smaller buffer</span>

<span class="c6 c9">// If the copy constructor of target object throws,
this is left in uninitialized state</span>

<span class="c6 c9">// If OtherCapacity is greater than Capacity, a
compile-time error is issued\
        template&lt;size\_t OtherCapacity&gt;\
        inplace\_function& operator=(const
inplace\_function&lt;R(Args...), OtherCapacity&gt;& other);\
\
        // Allows for move assignment of an inplace\_function object of
the same type, but with a smaller buffer</span>

<span class="c6 c9">// If the move constructor of target object throws,
this is left in uninitialized state</span>

<span class="c6 c9">// If OtherCapacity is greater than Capacity, a
compile-time error is issued\
        template&lt;size\_t OtherCapacity&gt;\
        inplace\_function& operator=(inplace\_function&lt;R(Args...),
OtherCapacity&gt;&& other);</span>

<span class="c6 c9">\
        // Assign a new target</span>

<span class="c6 c9">// If the copy constructor of target object throws,
this is left in uninitialized state\
        template&lt;typename Callable&gt;\
        inplace\_function& operator=(const Callable& target);</span>

<span class="c6 c9"></span>

<span class="c6 c9">        // Assign a new target by way of
moving</span>

<span class="c6 c9">// If the move constructor of target object throws,
this is left in uninitialized state\
        template&lt;typename Callable&gt;\
        inplace\_function& operator=(Callable&& target);\
\
        // Converts to 'true' if assigned.\
        explicit operator bool() const throw();\
        \
        // Invokes the target</span>

<span class="c6 c9">// Throws std::bad\_function\_call if not assigned.\
        R operator () (Args... args) const;\
        </span>

<span class="c6 c9">// Swap two targets\
        void swap(inplace\_function& other);        \
};</span>

<span>6. Sample use</span> {#h.po7uwc3i8km8 .c7 .c8}
--------------------------

<span></span>

<span class="c9 c18">\#include &lt;iostream&gt;\
\
struct Functor\
{\
        Functor() {}\
        Functor(const Functor&) { std::cout &lt;&lt; "copy" &lt;&lt;
std::endl; }\
        Functor(Functor&&) { std::cout &lt;&lt; "move" &lt;&lt;
std::endl; }\
        void operator()()\
        {\
                std::cout &lt;&lt; "functor" &lt;&lt; std::endl;\
        }\
};\
\
void Foo()\
{\
        std::cout &lt;&lt; "foo" &lt;&lt; std::endl;\
}\
\
template &lt;typename T&gt;\
void SomeTest()\
{\
        T func = \[\] { std::cout &lt;&lt; "lambda" &lt;&lt; std::endl;
};\
        func();\
        std::cout &lt;&lt; "func = &Foo" &lt;&lt; std::endl;\
        func = &Foo;\
        func();\
        std::cout &lt;&lt; "T func2 = Functor()" &lt;&lt; std::endl;\
        T func2 = Functor();\
        std::cout &lt;&lt; "func.swap(func2)" &lt;&lt; std::endl;\
\
        // with inplace\_function, this cannot simply swap pointers\
        func.swap(func2);\
}\
\
int main()\
{\
        inplace\_function&lt;void()&gt; func = \[\] { std::cout &lt;&lt;
"lambda" &lt;&lt; std::endl; };\
        func();\
        func = &Foo;\
        func();\
        inplace\_function&lt;void()&gt; func2 = Functor();\
        func.swap(func2);\
\
        std::cout &lt;&lt;
"SomeTest&lt;inplace\_function&lt;void()&gt;&gt;" &lt;&lt; std::endl;\
        SomeTest&lt;inplace\_function&lt;void()&gt;&gt;();\
}</span>

<span></span>

<span></span>

<span>7. Future work</span> {#h.aaghmstz8r9f .c7 .c8}
---------------------------

<span></span>

<span>To do</span>

<span></span>

<span>8. Acknowledgements</span> {#h.d9cwpby1wg3y .c7 .c8}
--------------------------------

<span></span>

<span>The authors would like to thank Maciej Gajewski from Optiver B.V.
and Edward Catmur from Maven Securities, for contributing their
reference implementations, and for their insightful comments.</span>

<span></span>

<span>9. Existing implementations</span> {#h.71oh0sm385wa .c7 .c8}
----------------------------------------

<span></span>

1.  <span>Optiver B.V.</span>

<!-- -->

1.  <span>Non allocating function which has a user specified capacity.
    Static\_assert is used to detect buffer overflows. Lambdas record
    destructors and constructors</span>

<!-- -->

2.  <span>Maven Securities:</span>

<!-- -->

1.  <span>Non allocating function which supports only trivial types,
    meaning no pointer to constructors or destructors is required (only
    the buffer and an invocation pointer). A user defined capacity of N
    bytes, with static\_asserts for overflow</span>
2.  <span>Non allocating function which supports copying, moving and
    destructing of callable targets. A user defined capacity of
    N bytes.</span>

<!-- -->

3.  <span>Ubisoft</span>

<!-- -->

1.  <span>Non allocating function that was a wrapper over std::function
    using TLS to work with specific stateless allocator.  Was working
    with VS2012 but with variadic templates it’s now much simpler to
    make a custom type without wrapping std::function.</span>

<!-- -->

4.  <span>Wargaming Seattle</span>

<!-- -->

1.  <span>To do</span>

<!-- -->

5.  <span>Erik Ringtorp</span>

<!-- -->

1.  <span>To do</span>

<!-- -->

6.  <span>https://github.com/rukkal/static-stl/blob/master/include/sstl/function.h</span>

<span></span>

<span>10. References</span> {#h.ilguggivzf00 .c7 .c8}
---------------------------

<span></span>

-   <span
    class="c2">[https://github.com/carlcook/SG14/SG14/inplace\_function.h](https://www.google.com/url?q=https://github.com/carlcook/SG14/SG14/inplace_function.h&sa=D&ust=1467759931187000&usg=AFQjCNEUpR-FAzmyrRCWgvD43hgaTXFxEw){.c1}</span><span> </span>

<span></span>

<span>11. Related work</span> {#h.ixbije8rxa48 .c7 .c8}
-----------------------------

<span></span>

-   <span
    class="c2">[http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4543.pdf](https://www.google.com/url?q=http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4543.pdf&sa=D&ust=1467759931189000&usg=AFQjCNE3-YnVsyv6CZWLCi5d3HMSBJEeDQ){.c1}</span><span> </span>

------------------------------------------------------------------------

<div>

[\[1\]](#ftnt_ref1){#ftnt1}<span class="c6"> See </span><span
class="c2 c6">[Existing Implementations](#h.71oh0sm385wa){.c1}</span>

</div>

<div>

[\[2\]](#ftnt_ref2){#ftnt2}<span class="c6"> See </span><span
class="c2 c6">[Related Work](#h.ixbije8rxa48){.c1}</span>

</div>
