#include <string>
#if !defined (STLPORT) || !defined (_STLP_USE_NO_IOSTREAMS)
#include <sstream>
#include <iterator>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class IoiterTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(IoiterTest);
  CPPUNIT_TEST(ioiter_test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void ioiter_test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(IoiterTest);

//
// tests implementation
//
void IoiterTest::ioiter_test()
{
//#ifdef MAIN
//  cout << "Results of ioiter: " << endl;
//#endif

  char c;
  char *pc;
  char *strorg = "abcd";
  // string expected( "abcd" );
  string tmp;

  string objStr(strorg);

  istringstream objIStrStrm1(objStr);
  istringstream objIStrStrm2(objStr);
  istringstream objIStrStrm3(objStr);

//#ifdef MAIN
//  cout << "plain C-string postfix increment operator and indirection: ";
//#endif

  pc = strorg;
  string::size_type sz = strlen(strorg);
  string::size_type i;
  for ( i = 0; i < sz; ++i ) {
    c = *pc++;
    tmp += c;
//#ifdef MAIN
//    cout << c;
//#endif
  }
//#ifdef MAIN
//  cout << " ... " << ( tmp == expected ? "pass" : "fail" ) << endl;
//#else
  // res = tmp == expected ? 0 : -1;
  CPPUNIT_ASSERT( tmp == "abcd" );
//#endif

//#ifdef MAIN
//  cout << "postfix increment operator and indirection: ";
//#endif

  istreambuf_iterator<char, char_traits<char> > objIStrmbIt1( objIStrStrm1.rdbuf() );
  istreambuf_iterator<char, char_traits<char> > end;

  // objIStrmbIt1 != end;

  tmp.clear();

  for ( i = 0; i < sz /* objIStrmbIt1 != end */; ++i ) {
    c = *objIStrmbIt1++;
    tmp += c;
//#ifdef MAIN
//    cout << c;
//#endif
  }
//#ifdef MAIN
//  cout << " ... " << ( tmp == expected ? "pass" : "fail" ) << endl;
//#else
//  res = tmp == expected ? 0 : -1;
//#endif
  CPPUNIT_ASSERT( tmp == "abcd" );

//#ifdef MAIN
//  cout << "indirection, then postfix increment operator: ";
//#endif

  tmp.clear();

  istreambuf_iterator<char, char_traits<char> > objIStrmbIt2( objIStrStrm2.rdbuf() );
  for ( i = 0; i < sz; ++i ) {
    c = *objIStrmbIt2;
    tmp += c;
//#ifdef MAIN
//    cout << c;
//#endif
    objIStrmbIt2++;
  }
//#ifdef MAIN
//  cout << " ... " << ( tmp == expected ? "pass" : "fail" ) << endl;
//#else
//  res = tmp == expected ? 0 : -1;
//#endif
  CPPUNIT_ASSERT( tmp == "abcd" );

  tmp.clear();

//#ifdef MAIN
//  cout << "compare, then postfix increment operator and indirection: ";
//#endif

  istreambuf_iterator<char, char_traits<char> > objIStrmbIt3( objIStrStrm3.rdbuf() );

  while ( objIStrmbIt3 != end ) {
    c = *objIStrmbIt3++;
    tmp += c;
//#ifdef MAIN
//    cout << c;
//#endif
  }
//#ifdef MAIN
//  cout << " ... " << ( tmp == expected ? "pass" : "fail" ) << endl;
//#else
//  res = tmp == expected ? 0 : -1;
//#endif
  CPPUNIT_ASSERT( tmp == "abcd" );
}

#endif
