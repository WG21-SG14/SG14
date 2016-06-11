Full Documentation: www.plflib.org


plf::colony
===========

A colony is a C++ template-based unordered data container which provides better performance than any std:: library containers when:

1. Many insertions and erasures to the container are occuring in realtime ie. in performance-critical code, and/or
2. Pointers and iterators which point to non-erased container elements must not be invalided by insertion or erasure.

Vector and deque's reallocations create iterator/pointer invalidation, which means an inability for objects in vectors to directly reference each other via pointers. There are various workarounds for this, which colony outperforms for larger numbers of container modifications.

Compared to other STL container types, vectors have better cache coherency and hence better speed regardless of the situation (See Chandler Carruth's talk here: http://www.youtube.com/watch?v=fHNmRkzxHWs). But erasing from anywhere but the end of a vector can be painfully slow, and adding/pushing/popping to/from a vector - for large objects - is not terrifically fast.

Colonies don't reallocate - they form chains of increasingly-large blocks of elements, resulting in a fast container which never invalidates pointers or iterators, generally has better insertion performance than vectors, with erasure generally being factors-of-ten faster when remove_if conventions are not used.

plf::stack, which plf::colony uses internally, is based on the same principle - pointers/references to non-erased elements never invalidate and it is overall faster than a std::stack or std::vector in the context of a stack.

SEE WEBSITE FOR FULL DOCUMENTATION AND LICENSING.TXT FOR APPROPRIATE USAGE AND LEGALITIES.
Contact me with bugs etc at: mattreecebentley@gmail.com
plf::stack and plf::colony Copyright (c) 2016 Matt Bentley
