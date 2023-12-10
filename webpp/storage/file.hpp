#ifndef WEBPP_STORAGE_FILE_HPP
#define WEBPP_STORAGE_FILE_HPP

#include "../std/string.hpp"
#include "./embedded_file.hpp"

#include <filesystem>
#include <fstream>
#include <optional>

namespace webpp {

    struct file_options {
        bool     cache          = true;
        bool     retry          = true;
        unsigned retry_count    = 2;
        void*    global_storage = nullptr; // todo
    };

    /**
     * Read and Write files easily. This class is not designed to replace the stl::filesystem utilities.
     * It's designed to make things easier.
     */
    struct file {
        using char_type     = char;
        using ifstream_type = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

      private:
      public:
        // just read the file and put the result into "out"
        static bool read_to(stl::filesystem::path const& filepath, auto& out) {
            // TODO: performance tests
            // todo: add unix specializations for performance and having fun reasons
            // TODO: change the replace_string with replace_string_view if the file is cached
            // checkout this implementation: https://stackoverflow.com/a/17925143/4987470
            if (auto in = ifstream_type(filepath.c_str(), stl::ios::binary | stl::ios::ate); in.is_open()) {
                // details on this matter:
                // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                // stl::unique_ptr<char_type[]> result(static_cast<char_type*>(
                in.seekg(0, stl::ios_base::end);
                auto const size = in.tellg();
                out.resize(static_cast<stl::size_t>(
                  size)); // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                in.seekg(0L);
                in.read(out.data(), size);
                return true;
            } else {
                return false;
            }
        }

        /**
         * Get the string of the file or empty string if we failed to read it.
         */
        template <istl::String StringType = stl::string, typename... StringArgs>
        static StringType read(stl::filesystem::path const& filepath, StringArgs&&... args) {
            StringType result{stl::forward<StringArgs>(args)...};
            static_cast<void>(read_to(filepath, result));
            return result;
        }

        /**
         * Get the file, search the embedded files first before trying to read the file
         * The method may even cache the file and listen for changes in the file.
         */
        static bool get_to(stl::filesystem::path const& filepath, auto& out) {
            if (auto const efile = embedded_file::search(filepath)) {
                out = efile->content();
                return true;
            }

            // todo: cache
            return file::read_to(filepath, out);
        }

        /**
         * Same as "get_to" but it creates the string type and returns that or an empty string if it failed.
         */
        template <istl::String StringType = stl::string, typename... StringArgs>
        static StringType get(stl::filesystem::path const& filepath, StringArgs&&... args) {
            StringType result{stl::forward<StringArgs>(args)...};
            static_cast<void>(get_to(filepath, result));
            return result;
        }
    };

} // namespace webpp

#endif // WEBPP_STORAGE_FILE_HPP
