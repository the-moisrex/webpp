#ifndef WEBPP_STORAGE_FILE_HPP
#define WEBPP_STORAGE_FILE_HPP

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace webpp {

    struct file {
        using char_type     = char;
        using ifstream_type = typename std::basic_ifstream<char_type, std::char_traits<char_type>>;

      private:
      public:
        // just read the file and return a file
        static bool read_to(std::filesystem::path const& filepath, auto& out) {
            if (auto in = ifstream_type(filepath.c_str(), std::ios::binary | std::ios::ate); in.is_open()) {
                // details on this matter:
                // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                // std::unique_ptr<char[]> buffer{new char[buffer_size]};
                // std::unique_ptr<char_type[]> result(static_cast<char_type*>(
                //  this->alloc_pack.template local_allocator<char_type[]>().allocate(size)));
                in.seekg(0, in.end);
                const auto size = in.tellg();
                out.resize(static_cast<std::size_t>(
                  size)); // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                in.seekg(0L);
                in.read(out.data(), size);
                return true;
            } else {
                return false;
            }
        }
    };

} // namespace webpp

#endif // WEBPP_STORAGE_FILE_HPP
