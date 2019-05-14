#ifndef REQUEST_H
#define REQUEST_H


#include "headers.hpp"

namespace webpp {

        class request
        {
            private:
                unsigned int status_code = 200;

            public:
                request() = default;
                request(headers request_headers, unsigned int status_code = 200) noexcept;

        };
}
#endif // REQUEST_H
