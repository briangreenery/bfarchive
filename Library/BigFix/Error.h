#ifndef BigFix_Error_h
#define BigFix_Error_h

#include <exception>
#include <stddef.h>

namespace BigFix
{

class Error : public std::exception
{
public:
  template <size_t n>
  explicit Error( const char ( &literal )[n] ) throw()
    : m_what( literal )
  {
  }

  virtual const char* what() const throw();

private:
  const char* m_what;
};

}

#endif
