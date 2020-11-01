#include "../benchmark.hpp"

using namespace std;


namespace test {
    struct basic_path : public std::vector<std::string> {
        static constexpr std::string_view parent_dir= "..";
        static constexpr std::string_view current_dir = ".";

        template <typename... T>
        constexpr basic_path(T&&... args) : std::vector<std::string>{std::forward<T>(args)...} {}

        [[nodiscard]] bool is_absolute() const noexcept {
            return !this->empty() && this->front().empty();
        }

        [[nodiscard]] bool is_relative() const noexcept {
            return !is_absolute();
        }

        void normalize() {
            remove_dot_segments(is_absolute());
        }

        void normalize_erase() {
            remove_dot_segments_erase(is_absolute());
        }

        void normalize_poco() {
            remove_dot_segments_poco(is_absolute());
        }

        void remove_dot_segments_erase(bool remove_leading) {
            if (this->empty())
                return;

            auto it = this->begin();

            // handle the first part
            while (it < this->end()) {
                if (*it == current_dir) {
                    this->erase(it);
                    continue;
                } else if (*it == parent_dir) {
                    if (it != this->begin()) {
                        const auto p = std::prev(it);
                        if (p->empty()) {
                            // remove just this one
                            this->erase(it);
                            continue;
                        } else if (*p != parent_dir) {
                            // remove the previous one and this one
                            this->erase(p, std::next(it));
                            --it;
                            continue;
                        }
                    } else if (remove_leading) {
                        this->erase(it);
                        continue;
                    }
                }
                it++;
            }
        }

        // from poco library (with a little optimization)
        void remove_dot_segments_poco(bool removeLeading) {
            if (this->empty()) return;

            std::vector<std::string> normalizedSegments;
            normalizedSegments.reserve(this->size());
            for (const auto& s: *this) {
                if (s == "..") {
                    if (!normalizedSegments.empty()) {
                        if (normalizedSegments.back() == "..")
                            normalizedSegments.push_back(s);
                        else
                            normalizedSegments.pop_back();
                    }
                    else if (!removeLeading) {
                        normalizedSegments.push_back(s);
                    }
                }
                else if (s != ".") {
                    normalizedSegments.push_back(s);
                }
            }
            this->swap(normalizedSegments);
        }

        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * todo: check if "erase", "shift_left" or "rotate" is better
         */
        void remove_dot_segments(bool remove_leading) {
            if (this->empty())
                return;

            auto the_end = this->end();
            auto it      = this->begin();

            // handle the first part
            while (it < the_end) {
                if (*it == current_dir) {
                    // remove it (move it to the last element)
                    the_end = std::rotate(it, std::next(it), the_end);
                    continue;
                } else if (*it == parent_dir) {
                    if (it != this->begin()) {
                        const auto p = std::prev(it);
                        if (p->empty()) {
                            // remove just this one
                            the_end = std::rotate(it, std::next(it), the_end);
                            continue;
                        } else if (*p != parent_dir) {
                            // remove the previous one and this one
                            the_end = std::rotate(p, std::next(it), the_end);
                            --it;
                            continue;
                        }
                    } else if (remove_leading) {
                        the_end = std::rotate(it, std::next(it), the_end);
                        continue;
                    }
                }
                it++;
            }
            this->erase(the_end, this->end()); // actually remove them from the vector
        }

        void normalize_utopia() {
            this->erase(
              this->begin(),
              (this->rend() -
               std::remove_if(this->rbegin(), this->rend(),
                              [borrowed_paths = 0](const auto &element) mutable {

                                if (element == current_dir) {
                                    return true;
                                } else if (element == parent_dir) {
                                    borrowed_paths++;
                                    return true;
                                } else if (borrowed_paths) {
                                    borrowed_paths--;
                                    return true;
                                }

                                return false;
                              })) +
              this->begin());
        }


        std::string to_string() const {
            std::string str{this->get_allocator()};
            if (this->size() == 0)
                return str;

            auto it = this->cbegin();
            str.append(*it++);
            for (; it != this->cend(); ++it) {
                str.append("/");
                str.append(*it);
            }
            return str;
        }
    };
}


auto const paths = []() {
  vector<test::basic_path> _paths {
    test::basic_path {"..", "home", "..", ".", "test"},
    test::basic_path {".", "home", "..", ".", "test"},
    test::basic_path {"..", "home", "...", ".", "test"},
    test::basic_path {"", "home", ".", ".", "test", ".."},
    test::basic_path {"..", "home", "..", ".", "test", "."},
    test::basic_path {"", "..", "home", "..", ".", "test"},
    test::basic_path {"", ".", "home", "..", ".", "test", "."},
    test::basic_path {"..", "home", "..", ".", "test", ""}
    };
  return _paths;
}();

static void URINormalize_Rotate(benchmark::State& state) {
    for (auto _ : state) {
        auto the_paths = paths;
        for (auto p : the_paths) {
            p.normalize();
            benchmark::DoNotOptimize(p);
        }
        benchmark::DoNotOptimize(the_paths);
    }
}
BENCHMARK(URINormalize_Rotate);

static void URINormalize_Erase(benchmark::State& state) {
    for (auto _ : state) {
        auto the_paths = paths;
        for (auto p : the_paths) {
            p.normalize_erase();
            benchmark::DoNotOptimize(p);
        }
        benchmark::DoNotOptimize(the_paths);
    }
}
BENCHMARK(URINormalize_Erase);

static void URINormalize_Poco(benchmark::State& state) {
    for (auto _ : state) {
        auto the_paths = paths;
        for (auto p : the_paths) {
            p.normalize_poco();
            benchmark::DoNotOptimize(p);
        }
        benchmark::DoNotOptimize(the_paths);
    }
}
BENCHMARK(URINormalize_Poco);

static void URINormalize_SimpleRemoveIf(benchmark::State& state) {
    for (auto _ : state) {
        auto the_paths = paths;
        for (auto p : the_paths) {
            p.normalize_utopia();
            benchmark::DoNotOptimize(p);
        }
        benchmark::DoNotOptimize(the_paths);
    }
}
BENCHMARK(URINormalize_SimpleRemoveIf);

