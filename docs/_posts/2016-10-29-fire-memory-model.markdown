---
layout: post
title: Memory Model
permalink: /design/memory_model
category: design
---

The memory model in Fire is meant to be simple and efficient. The idea is that sometimes striving for the
best performance _and_ the highest architectural purity is stupid and, furthermore, YAGNI.

# Public Domain Data

The proper implementation of a pure, public data structure in Fire is to use a *struct* instead of a class, which would look like:

```cpp
struct MyData {
    int A;
    int B;
    int C;
};
```

### When to use accessors

Fire uses structs over classes for pure data structures because, in some cases, *accessor functions* may be evil. Accessors - or functions that start with "get" or "set" - are used as a means of managing access to or hiding the state of "private" data in a class. (Hiding internal state is called *encapsulation.*) However, in a class like the following it is clear that the accessors are just fluff since the getA() and setA() functions do no other work than provide access to A, B, and C.

```cpp
class MyData {
    int A;
    int B;
    int C;
    // More properties
public:
    int getA() {return A;};
    void setA(int otherA) {A = otherA;};
    // More get/set functions for B and C 
};
```

If the getA() and setA() functions did some extra work or required other information about internal state, then they would be just fine and well warranted. If they do not do any extra work, then encapsulation is completely violated and can no longer be used as an excuse. That is, the internal state is not actually hidden, so it is better to just make them public. Both C and C++ provide the *struct* construct for defining a data structure with all public members, and it is a better fit here. 

There is another important reason for skipping accessors for pure data structures: Accessors are a common source of bugs! While these types of bugs will be easily caught by a good unit test, they can be avoided all together by not using accessors where that makes sense.

# Data Arrays


Fire combines smart pointers and shared pointers for data arrays. Arrays are loaded into shared pointers and passed to functions as raw pointers. The _free()_ and _delete()_ **must not be called** on raw pointers in Fire routines because the shared pointers are responsible for the lifecycle of the memory to which the pointer points.

Here's an example for creating arrays of data and using them appropriately in Fire:

```cpp
int main() {
    // Will be added. Sorry!
}
```

### More details

The best performance for arrays of data in C++ comes from using raw pointers, which comes with the obvious
down side of memory leaks. The smart pointer was invented to get around the latter problem, but that safety comes at the cost of performance. In the case of C++ smart pointers, and the standard shared pointer in particular, the cost is atomic increments and decrements every time the smart pointer is passed to a function. In Fire, smart pointers should be used if a client class is going to be responsible
for modifying the existence of an object (create it, delete it, reallocate it,
etc.) or needs to know when such an event happens. That is, if a client class is going to participate in the _memory lifecycle_ of an object, then it needs to have the shared pointer. If it does not need to participate in the memory lifecycle, then it is only working with the array and only needs some type of reference to it, for which a raw pointer will work just fine. (Actual references are fine too if they work for your problem.)

This is consistent with guidance from the broader C++11 community, (c.f. - [Herb Sutter's Going Native 2011 talk][1]), but it does run the risk that some unknowing developer will call delete on a pointer. That risk is acceptable because it is minimized when developers RTFM. In practice, accidental deallocations have never happened.

C++ references are not broadly used because Fire makes extensive use of templates and references cannot be stored in templated classes. However, as mentioned above, use them if it works.

This strategy has several benefits that can be summed up simply. Combining the two allows Fire to use an exceptionally small amount of memory, provides very fast access to data on the heap and eliminates memory leaks from its own classes. The latter can be witnessed by testing with memory profilers like
Valgrind. (N.B. - We can not do anything about memory leaks in third party
dependencies.)

[1]: http://channel9.msdn.com/Events/GoingNative/GoingNative-2012/C-11-VC-11-and-Beyond