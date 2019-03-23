#ifndef WEBPP_WEBPP_H
#define WEBPP_WEBPP_H

#include "router.h"

namespace webpp {

  class app {
    router m_router;
    
    public:
      void run();
  };


};


#endif // WEBPP_WEBPP_H
