# JSON for Modern C++

This branch is created for debug purpose of issue #2491

I tried my best to delete as much code as I can, slill being able to reproduce 
the bug

## How to use

Use CMake projec with following options:

```
-DCMAKE_CXX_COMPILER=clang++-10
-DJSON_CPP_STD_LIB=libc++
-DCMAKE_CXX_STANDARD=17
```

Two targets should appear:

* ```test_issue_2491``` - reproducing code. Should fail with error like
```
include/c++/v1/utility:514:58: error: no member named 'value' in 'std::__1::is_copy_assignable<nlohmann::basic_json<nlohmann::ordered_map, std::vector, std::__1::basic_string<char>, bool, long, unsigned long, double, std::allocator, adl_serializer, std::__1::vector<unsigned char, std::__1::allocator<unsigned char> > > >'
                        is_copy_assignable<second_type>::value
```

* ```test_issue_2491_with_magic``` not reproducing code. Should compile somewhy
