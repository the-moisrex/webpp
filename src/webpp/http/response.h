#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

namespace  {

        class response {
            private:
            public:
                response() = default;

                response* body(std::string const & body) noexcept;
                response* body(std::string &&body) noexcept;
                std::istream& body() noexcept;

        };

}
#endif // RESPONSE_H
