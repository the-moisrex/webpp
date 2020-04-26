// Created by moisrex on 4/26/20.

#ifndef WEBPP_HTTP_COMMON_H
#define WEBPP_HTTP_COMMON_H

namespace webpp {

    /**
     * This enum is used to check if a header is a request header or a response
     * header. We could have avoided creating this, but if we don't, we might
     * get ourselves in a jam that's hard to get out.
     * It's best if the header knows if it's a request header or a response
     * header since it'll have a deep knowledge of the header fields.
     */
    enum class header_type : uint_fast8_t { request, response };

} // namespace webpp


#endif // WEBPP_HTTP_COMMON_H
