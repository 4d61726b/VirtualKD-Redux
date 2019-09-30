/* This header is used to turn off warnings of Microsoft compilers generated.
 * while building STLport.
 * For compiling user code, see stlport/config/_msvc_warnings_off.h.
 */

#if (_MSC_VER < 1200) // VC5 and before
#  pragma warning( disable : 4018 ) // signed/unsigned mismatch
#  pragma warning( disable : 4290 ) // c++ exception specification ignored
#  pragma warning( disable : 4389 ) // '==' : signed/unsigned mismatch
#  pragma warning( disable : 4610 ) // struct '...' can never be instantiated - user defined constructor required
#endif

#if (_MSC_VER < 1300) // VC6/eVC4
#  pragma warning( disable : 4097 ) // typedef-name used as based class of (...)
#  pragma warning( disable : 4251 ) // DLL interface needed
#  pragma warning( disable : 4284 ) // for -> operator
#  pragma warning( disable : 4503 ) // decorated name length exceeded, name was truncated
#  pragma warning( disable : 4514 ) // unreferenced inline function has been removed
#  pragma warning( disable : 4660 ) // template-class specialization '...' is already instantiated
#  pragma warning( disable : 4701 ) // local variable 'base' may be used without having been initialized
#  pragma warning( disable : 4710 ) // function (...) not inlined
#  pragma warning( disable : 4786 ) // identifier truncated to 255 characters
#endif

#if (_MSC_VER <= 1310)
#  pragma warning( disable : 4511 ) // copy constructor cannot be generated
#endif

#if (_MSC_VER < 1300) && defined(UNDER_CE)
#  pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#  pragma warning( disable : 4214 ) // nonstandard extension used : bit field types other than int
#endif

#pragma warning( disable : 4075 ) // initializers put in unrecognized initialization area
//This warning is disable only for the c_locale_win32.c file compilation:
#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4127 ) // conditional expression is constant
#pragma warning( disable : 4146 ) // unary minus applied to unsigned type
#pragma warning( disable : 4245 ) // conversion from 'enum ' to 'unsigned int', signed/unsigned mismatch
#pragma warning( disable : 4244 ) // implicit conversion: possible loss of data
#pragma warning( disable : 4512 ) // assignment operator could not be generated
#pragma warning( disable : 4571 ) // catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions
#pragma warning( disable : 4702 ) // unreachable code (appears in release with warning level4)
