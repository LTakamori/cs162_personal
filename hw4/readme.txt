Though passed the default test, this implementation of mm_alloc is TERRIFICKLY BAD!!! for not checking lot of case of NULL pointer or so
So dont take this as some sort of mature design, but just as a inspiration/reference

mm_alloc存在很大的问题，没有检查空指针等很多问题，但我又懒得去写了（不太有时间，而且没有testcase），所以这里面的内容只能提供一个思路&参考