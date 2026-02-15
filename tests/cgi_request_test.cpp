#include "common/fake_protocol.hpp"
#include "common/test.hpp"

using namespace webpp;

namespace {
    struct app {
        auto operator()(auto&&) const {
            return true;
        }
    };
} // namespace

TEST(CGIRequest, ContentTypeAccessorUsesContentTypeEnvName) {
    fake_proto<app> server;
    auto&           req = server.req;

    req.data["CONTENT_TYPE"]   = "application/json";
    req.data["CONTENT_LENGTH"] = "42";

    EXPECT_EQ(req.content_type(), "application/json");
    EXPECT_EQ(req.content_length(), "42");
}
