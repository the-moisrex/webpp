#include "../../../include/webpp/http/response.h"

#include <fstream>

using namespace webpp;


#ifdef CONFIG_FILE
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    else
extern std::string_view get_static_file(std::string_view const&) noexcept;
#    endif
#endif

response response::file(std::filesystem::path const& _file) noexcept {
    response res;
#ifdef CONFIG_FILE
    if (auto content = ::get_static_file(filepath); !content.empty()) {
        res.body.replace_string(content);
        return res;
    }
#endif

    // TODO: performance tests
    // TODO: change the replace_string with replace_string_view if the file is cached

    if (std::ifstream in{_file.c_str(), std::ios::binary | std::ios::ate}) {
        // details on this matter:
        // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
        // std::unique_ptr<char[]> buffer{new char[buffer_size]};
        // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I
        // think
        auto                    size = in.tellg();
        std::unique_ptr<char[]> result(new char[size]);
        in.seekg(0);
        in.read(result.get(), size);
        res.body.replace_string(std::string{
          result.get(), static_cast<std::string_view::size_type>(size)});
    }

    return res;
}
