#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include <string>

/**
 * This class will not load, cache, or parse the body until the user needs it.
 * This means that we need access to the Interface so we can use it to read the
 * data of the body, then we parse it to the thing that the user needs.
 *
 */
namespace webpp {

  template <class Interface>
  class body {
    private:
      Interface _interface;

    public:

      void* const & json() const;
      std::string const& string() const;

      // TODO: add more methods for the images and stuff

  };


}; // webpp namespace


#endif // WEBPP_BODY_H
