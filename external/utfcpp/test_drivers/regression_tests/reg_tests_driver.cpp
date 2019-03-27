#include <iostream>
#include <vector>
using namespace std;

inline void check_impl (bool condition, const char* file, int line)
 {
   if (!condition) { 
     cout << "Check Failed! File: " << file << " Line: " << line << '\n';
   }
 } 

#define check(c) check_impl(c, __FILE__, __LINE__);

// Release 1.0 Beta 1
#include "r1_0Beta1/invalidutf8.h"
#include "r1_0Beta1/basic_functionality.h"
// Release 1.0 Beta 2
#include "r1_0Beta2/basic_functionality.h"
// Release 1.0 Beta 3
#include "r1_0Beta3/basic_functionality.h"

// Release 2.2.2
#include "r2_2_2/basic_functionality.h"


int main()
{
// Release 1.0 Beta 1
//r1_0Beta1/invalidutf8.h
    id_1524459();
    id_1525236();  
    id_1528369();
//r1_0Beta1/basic_functionality.h
    id_1528544();

// Release 1.0 Beta 2
//r1_0Beta2/basic_functionality.h
    id_1531740();

// Release 1.0 Beta 3
//r1_0Beta3/basic_functionality.h
    id_1538338();   

// Release 2.2.2
//r2_2_2/basic_functionality.h
    id_2852872();
    id_2857454();

}
