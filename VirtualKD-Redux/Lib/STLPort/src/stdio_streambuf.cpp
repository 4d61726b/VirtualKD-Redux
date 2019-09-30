/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#include "stlport_prefix.h"
#include "stdio_streambuf.h"
// #include "file_streambuf.h"

#ifdef _STLP_UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#endif /* __unix */

#include <fstream>
#include <limits>
#include "fstream_impl.h"

#if defined (_STLP_USE_WIN32_IO) && !defined(_STLP_WCE)
#  if defined (__BORLANDC__)
// #  include <cio.h>
#    include <cfcntl.h>
#  else
#    include <io.h>
#    include <fcntl.h>
#  endif
#  include <sys/stat.h>
#endif

_STLP_BEGIN_NAMESPACE
_STLP_MOVE_TO_PRIV_NAMESPACE

//----------------------------------------------------------------------
// Class stdio_streambuf_base

stdio_streambuf_base::stdio_streambuf_base(FILE* file)
    : /* _STLP_STD::FILE_basic_streambuf(file, 0), */
    _M_file(file)
{}

stdio_streambuf_base::~stdio_streambuf_base() {
  _STLP_VENDOR_CSTD::fflush(_M_file);
}

_STLP_STD::streambuf* stdio_streambuf_base::setbuf(char* s, streamsize n) {
#ifdef _STLP_WCE
  // no buffering in windows ce .NET
#else
  size_t __n_size_t = (sizeof(streamsize) > sizeof(size_t)) ? __STATIC_CAST(size_t, (min)(__STATIC_CAST(streamsize, (numeric_limits<size_t>::max)()), n))
                                                            : __STATIC_CAST(size_t, n);
  _STLP_VENDOR_CSTD::setvbuf(_M_file, s, (s == 0 && n == 0) ? _IONBF : _IOFBF, __n_size_t);
#endif
  return this;
}

stdio_streambuf_base::pos_type
stdio_streambuf_base::seekoff(off_type off, ios_base::seekdir dir,
                              ios_base::openmode /* mode */) {
  int whence;
  switch (dir) {
  case ios_base::beg:
    whence = SEEK_SET;
    break;
  case ios_base::cur:
    whence = SEEK_CUR;
    break;
  case ios_base::end:
    whence = SEEK_END;
    break;
  default:
    return pos_type(-1);
  }

  //We also check that off is not larger than the fseek parameter that is supposed to take
  //a long integer.
  _STLP_STATIC_ASSERT(sizeof(off_type) >= sizeof(long))
  if (off <= numeric_limits<long>::max() &&
      _STLP_VENDOR_CSTD::fseek(_M_file, __STATIC_CAST(long, off), whence) == 0) {
    fpos_t pos;
    _STLP_VENDOR_CSTD::fgetpos(_M_file, &pos);
    // added 21 june 00 mdb,rjf,wjs: glibc 2.2 changed fpos_t to be a struct instead
    // of a primitive type
#if (defined (__GLIBC__) && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 2))))
    return pos_type((streamoff)pos.__pos);
#elif defined (__ISCPP__) || defined (__MVS__) || defined (__OS400__)
    return pos_type(pos.__fpos_elem[ 0 ]);
#elif defined (__EMX__)
    return pos_type((streamoff)pos._pos);
#else
    return pos_type(pos);
#endif
  }
  else
    return pos_type(-1);
}


stdio_streambuf_base::pos_type
stdio_streambuf_base::seekpos(pos_type pos, ios_base::openmode /* mode */) {
  // added 21 june 00 mdb,rjf,wjs: glibc 2.2 changed fpos_t to be a struct instead
  // of a primitive type
#if (defined(__GLIBC__) && ( (__GLIBC__ > 2) || ( (__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 2) ) ) )
  fpos_t p;
  p.__pos = pos;
#  ifdef _STLP_USE_UCLIBC
#    ifdef __STDIO_MBSTATE
  memset( &(p.__mbstate), 0, sizeof(p.__mbstate) );
#    endif
#    ifdef __STDIO_WIDE
  p.mblen_pending = 0;
#    endif
#  else
  memset( &(p.__state), 0, sizeof(p.__state) );
#  endif
#elif defined (__MVS__) || defined (__OS400__)
  fpos_t p;
  p.__fpos_elem[0] = pos;
#elif defined(__EMX__)
  fpos_t p;
  p._pos = pos;
  memset( &(p._mbstate), 0, sizeof(p._mbstate) );
#else
  fpos_t p(pos);
#endif

  if (_STLP_VENDOR_CSTD::fsetpos(_M_file, &p) == 0)
    return pos;
  else
    return pos_type(-1);
}

int stdio_streambuf_base::sync() {
  return _STLP_VENDOR_CSTD::fflush(_M_file) == 0 ? 0 : -1;
}

//----------------------------------------------------------------------
// Class stdio_istreambuf

stdio_istreambuf::~stdio_istreambuf() {}

streamsize stdio_istreambuf::showmanyc() {
  if (feof(_M_file))
    return -1;
  else {
    int fd = _FILE_fd(_M_file);
#ifdef _STLP_WCE
   (fd); // prevent warning about unused variable
// not sure if i can mix win32 io mode with ftell but time will show
// cannot use WIN32_IO implementation since missing stat
    long tmp= _STLP_VENDOR_CSTD::ftell(_M_file);
     _STLP_VENDOR_CSTD::fseek(_M_file, 0, SEEK_END);
    streamoff size= _STLP_VENDOR_CSTD::ftell(_M_file)-tmp;
     _STLP_VENDOR_CSTD::fseek(_M_file, tmp, SEEK_SET);
#elif defined (_STLP_USE_WIN32_IO)
    // in this case, __file_size works with Win32 fh , not libc one
    streamoff size;
    struct stat buf;
    if(fstat(fd, &buf) == 0 && ( _S_IFREG & buf.st_mode ) )
      size = ( buf.st_size > 0  ? buf.st_size : 0);
    else
      size = 0;
#else
    streamoff size = __file_size(fd);
#endif
    // fbp : we can use ftell as this flavour always use stdio.
    long pos = _STLP_VENDOR_CSTD::ftell(_M_file);
    return pos >= 0 && size > pos ? size - pos : 0;
  }
}

stdio_istreambuf::int_type stdio_istreambuf::underflow()
{
#ifdef _STLP_WCE
  int c = fgetc(_M_file);
#else
  int c = getc(_M_file);
#endif
  if (c != EOF) {
    _STLP_VENDOR_CSTD::ungetc(c, _M_file);
    return c;
  }
  else
    return traits_type::eof();
}

stdio_istreambuf::int_type stdio_istreambuf::uflow() {
#ifdef _STLP_WCE
  int c = fgetc(_M_file);
#else
  int c = getc(_M_file);
#endif
  return c != EOF ? c : traits_type::eof();
}

stdio_istreambuf::int_type stdio_istreambuf::pbackfail(int_type c) {
  if (c != traits_type::eof()) {
    int result = _STLP_VENDOR_CSTD::ungetc(c, _M_file);
    return result != EOF ? result : traits_type::eof();
  }
  else{
    if (this->eback() < this->gptr()) {
      this->gbump(-1);
      return traits_type::not_eof(c);
    }
    else
      return traits_type::eof();
  }
}

//----------------------------------------------------------------------
// Class stdio_ostreambuf

stdio_ostreambuf::~stdio_ostreambuf() {}

streamsize stdio_ostreambuf::showmanyc() {
  return -1;
}

stdio_ostreambuf::int_type stdio_ostreambuf::overflow(int_type c) {
  // Write the existing buffer, without writing any additional character.
  if (c == traits_type::eof()) {
    // Do we have a buffer to write?
    ptrdiff_t unwritten = this->pptr() - this->pbase();
    if (unwritten != 0) {
      _STLP_VENDOR_CSTD::fflush(_M_file);
      // Test if the write succeeded.
      if (this->pptr() - this->pbase() < unwritten)
        return traits_type::not_eof(c);
      else
        return traits_type::eof();
    }

    // We always succeed if we don't have to do anything.
    else
      return traits_type::not_eof(c);
  }

  // Write the character c, and whatever else might be in the buffer.
  else {
#ifdef _STLP_WCE
    int result = fputc(c, _M_file);
#else
    int result = putc(c, _M_file);
#endif
    return result != EOF ? result : traits_type::eof();
  }
}

_STLP_MOVE_TO_STD_NAMESPACE
_STLP_END_NAMESPACE

// Local Variables:
// mode:C++
// End:

