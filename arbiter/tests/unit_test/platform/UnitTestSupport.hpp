#pragma once

#ifdef WIN32
    #pragma warning(push)
    #pragma warning(disable : 4127 4350)
    #include <UnitTest++/UnitTest++.h>
    #pragma warning(pop)
#else
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wnewline-eof"
    #include <UnitTest++/UnitTest++.h>
    #pragma clang diagnostic pop
#endif

