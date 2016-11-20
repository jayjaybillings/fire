---
layout: post
title: Memory Model
permalink: /design/memory_model
category: design
---

The memory model in Fire is meant to be simple and efficient. The idea is that sometimes striving for the best performance _and_ the highest architectural purity is stupid and, furthermore, YAGNI.

Fire takes advantage of the latest updates to C++ and relies heavily on C++11 memory management enhancements to get the best performance.

# Return by Value

There is no need to return by reference (pointer) for most functions in Fire because it uses C++11. Thus the following code is performs very well:

```cpp

vector<int> getVec() {
	vector<int> myVec;
	... // Some code to fill the vector
	return myVec;
}

int main(int argc, char** argv) {
    vector<int> mainVec = getVec();
    return EXIT_SUCCESS;
}
```

This code would have performed very slowly in earlier versions of C++ because they did not provide so called "move semantics" like C++11. The problem in older versions of C++ comes from the fact that there would be at least two copies the above code. First, myVec is copied into the return value of getVec() when it returns. Second, the return value of getVec() is copied into mainVec. 

C++11 eliminates the second copy with move semantics. The first copy is eliminated by a process called *return value optimization,* which is implemented directly by the compiler.

The important implication for this is that functions in Fire can be much simpler and easier to understand while simultaneously performing very well, or, as we like to say with purely technical language, "running like a scalded dog." ;-) This performance is especially important for the build<>()
template, which encapsulates the construction of objects. Without move semantics and return value optimization, it would perform very poorly unless it returned a pointer.

# Public Data

The proper implementation of a pure, public data structure in Fire is to use a *struct* instead of a class. It would would look like:

```cpp
struct MyData {
    int A;
    int B;
    const int C;
    
    MyData(int otherC) : C(otherC) {};
};
```

### Handling Const Members 
As shown in the example, constructors and initializer lists should be used for *const* data members. This also means that special care should be taken so that structs do not have a large amount of const-qualified members. This makes initialization complicated since const-qualified members must be initialized in the initializer list instead of the
body of the constructor. In extreme cases it may be necessary to replace a constructor like

```cpp
MyConstructor(int a, int b, int c,...) : myA(a), myB(b), myC(c), ... {};
```

with something like

```cpp
MyConstructor(const vector<int> & values) : myA(values[0]), myB(values[1]), myC(values[2]), ... {};

```

which has the benefit of having a shorter input argument list. This is convenient for line-fed data
parsed from legacy data formats, as is the case for fire::astrophysics::Species.

Another possibility is to do something more drastic, like

```cpp
MyConstructor(const vector<int> & values) : myValues(values), ... {};
```

where explicitly defining myA, myB, myC and other members has been replaced with an vector. This is not recommended in the case where the vector contains separately accessed values like myA, myB and myC because it is bad design. However, it is ideal in the case where myValues is actually an immutable vector of dense data of a primitive type used for iteration.

If a struct would have so many individual const-qualified data members that a vector would be required to initialize it, then it would be better to switch to a class instead, hold the vector data as private
state, and provide access via *accessors.* 

### When to use accessors

Fire uses structs over classes for small data structures of pure data because, in some cases, *accessor functions* may be evil. Accessors - or functions that start with "get" or "set" - are used as a means of managing access to or hiding the state of "private" data in a class. (Hiding internal state is called *encapsulation.*) In a class like the following it is clear that the accessors are just fluff since the getA() and setA() functions do no other work than provide access to A, B, and C.

```cpp
class MyData {
    int A;
    int B;
    int C;
    ... // More properties!
public:
    int getA() {return A;};
    void setA(int otherA) {A = otherA;};
    ... // More get/set functions for B and C 
};
```

If the getA() and setA() functions did some extra work or required other information about internal state, then they would be just fine and well warranted. If they do not do any extra work, then encapsulation is completely violated and can no longer be used as an excuse. That is, the internal state is not actually hidden, so it is better to just make them public. Both C and C++ provide the *struct* construct for defining a data structure with all public members, and it is a better fit here. 

There is another important reason for skipping accessors for pure data structures: Accessors are a common source of bugs! While these types of bugs will be easily caught by a good unit test, they can be avoided all together by not using accessors where that makes sense.

In the last section, one valid case for accessors was discussed: what if a data class with many const-qualified members is needed? Accessors are ideal for providing this information because they keep the
code succinct:

```cpp
class MyData {
    const vector<int> myConstValues;
    ... // More properties!
public:
    
    /**
     * The constructor which copies the data
     */
    MyData(const vector<int> values) : myConstValues(values) {};
    
    /**
     * Return the property A, which requires knowing the secret index in the secret vector.
     */
    int getA() {return vector[8675309];};
    ... // More get/set functions for B, C and others  like A.
};
```

# Heap-Allocated Data Arrays


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