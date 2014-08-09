NoLeak
======

NoLeak is an easy-use leak detector for C and C++ programs.

Building
********

The build files are generated using `Bakefile <http://bakefile.org/>`_. Unix users can just run `make`. Visual Studio users will a solution file under `vsproj/<version>/noleak.sln`, replacing `<version>` with your Visual Studio version. For instance, VS 2012 users would open `vsproj/2012/noleak.sln`.

C usage
*******

Replace calls to `malloc` with `NL_MALLOC`, calls to `free` with `NL_FREE`, and calls to `realloc` with `NL_REALLOC`. Then, call `noleak_init()` at the start of your program's main function and include `noleak.h` at the top of the file. For example, change this:

.. code:: c
   
   int main()
   {
       int* p = malloc(4);
       ...
       // forget to free p
       return 0;
   }

to this:

.. code:: c
   
   #include <noleak.h>
   
   int main()
   {
       noleak_init();
       int* p = NL_MALLOC(4);
       ...
       return 0;
   }

Now, all memory leaks will be printed on program exit. The program's output will look like this::
   
   Leak #1 at test.c:main:6
   Total leaks: 1

C++ usage
*********

With C++, replace calls to `new` with `new (NOLEAK)` and again include `noleak.h`. For instance:

.. code:: c++
   
   #include <noleak.h>
   
   int main()
   {
       int* p = new (NOLEAK) int;
       // forget to delete p
       return 0;
   }

There's no need to replace `delete` calls.
