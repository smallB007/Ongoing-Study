// 2014-11-19T13:47+08:00
// The two purposes of this example:
// -------------------------------------------------------------------------------
// 1. Explaining clause 3.3.10/2 in ISO C++11 standard
//    When I was studying a FTP file uploading example(ftpupload.c) in libcurl's
//    example directory today, I came across some C code like this:
//    (You can find this example on: http://curl.haxx.se/libcurl/c/ftpupload.html)
//        struct stat file_info;
//        stat(LOCAL_FILE, &file_info);
//    I was wondering if I can rewrite them like this in C++:
//        stat file_info;
//        stat(LOCAL_FILE, &file_info);
//    The answer is NO.
//    Please refer to this page:
//    http://stackoverflow.com/questions/23329382/function-and-struct-having-the-same-name-in-c
//    2016-12-12T19:02+08:00
//    The C++ Programming Language, Third Edition/5.7 Structures [ptr.struct]
//    >For reasons that reach into the pre-history of C, it is possible to declare a struct and a 
//     non-structure with the same name in the same scope.
//     In that case, the plain name (stat) is the name of the non-structure, and the structure must 
//     be referred to with the prefix struct.
// -------------------------------------------------------------------------------
// 2. Explaining clause 6.4/3 in ISO C++98/11
//    Please refer to this page:
//    http://stackoverflow.com/questions/12655284/defining-a-variable-in-the-condition-part-of-an-if-statement
#include <cassert>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    // Purpose 1:
    // Though we are using C++, we can't write code like this:
    //     stat fileInfo;
    // Since stat is also a function here, which will hide the class with the same name in this scope.
    struct stat fileInfo;

    // Purpose 2:
    // We can define a variable in the condition part of if statement.
#if 1
   // The unusual form for handling the return value.
   // I think this form is more compact, but sometimes confusing.
   if (int res = stat("get_file_size.cpp", &fileInfo)) {
       // stat failed. Do error handling and output the error code.
       // ***Caution:
       // Before you write your code in this form, you must check out what
       // the meaning of the return code is.
        std::cerr << "Failed to obtain file-status information: " << res << '\n';
        return 1;
   } else {
       // The variable 'res' is also accessible here.
       assert(0 == res);
       std::cout << "File size: " << fileInfo.st_size << std::endl;
   }
#else
    // The traditional form for handling the return code of some function.
    // The 'res' variable may not be used anymore after we checking the return code
    // of 'stat'. That's why sometimes I want to declare it in the condition part
    // of the statement.
    int res = stat("get_file_size.cpp", &fileInfo);
    if (res != 0) {
        std::cerr << "Failed to obtain file-status information: " << res << '\n';
        return 1;
    } else {
        std::cout << "File size: " << fileInfo.st_size << std::endl;
    }
#endif

    return 0;
}
