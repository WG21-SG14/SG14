Full Documentation: www.plflib.org


plf::colony
===========

What is a colony? It's similar to a vector or a deque, but different. It is it's own abstract datatype, not fitting any of the existing categories exactly.

Vector and deque's reallocations equal iterator/pointer invalidation, which means inability for objects in vectors to reference each other. There are workarounds for this (in given situations - not all situations), but each of them is a hack around vector's limitations, which slows down program speed and development time.

Compared to other STL container types, vectors have better cache coherency and hence better speed regardless of the situation (See Chandler Carruth's talk here: http://www.youtube.com/watch?v=fHNmRkzxHWs). But erasing from anywhere but the end of a vector is painfully slow, and adding/pushing/popping to/from a vector - for large objects - is not as fast we'd like.

Colonies don't reallocate - they form chains of increasingly-large blocks of elements, resulting in a fast container which never invalidates pointers or iterators, generally has twice the add-speed of vectors, with erasing elements being factors-of-ten faster, and iteration speed similar to a vector for larger-than-scalar types.

plf::stack, which plf::colony uses internally, is based on the same principle - pointers/references to non-erased elements never invalidate and it is overall faster than a std::stack.

Run the plf_demo.cpp for an overview of performance characteristics.

SEE WEBSITE FOR FULL DOCUMENTATION AND LICENSING.TXT FOR APPROPRIATE USAGE AND LEGALITIES.
Contact me with bugs etc at: mattreecebentley@gmail.com
plf::stack and plf::colony Copyright (c) 2015 Matt Bentley
