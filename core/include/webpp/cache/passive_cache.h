#ifndef WEBPP_CACHE_PASSIVE_CACHE
#define WEBPP_CACHE_PASSIVE_CACHE

#include <string_view>
#include <type_traits>

namespace webpp {

    /**
     * This class does not store anything, it's there just for when the user has
     * written old code that uses cache, but now, he/she doesn't want cache to
     * be used anymore. so he/she switches to this calss for his/her caching
     * system. This class will return everything when you get them and does not
     * store anything.
     */
    template <typename KeyT = std::string, typename ValT = std::string>
    class passive_cache {
      public:
        using key_type   = KeyT;
        using value_type = ValT;

      private:
      public:
        passive_cache() noexcept;

        template <typename KeyValue, typename DataType>
        auto get(KeyValue&& /* key */,
                 DataType const& default_value) const noexcept {
            // yes, that's how useless this class is!
            return default_value;
        }

        template <typename KeyValue, typename DataType>
        auto& set(KeyValue& /* key */, DataType&& /* value */) const noexcept {
            // we will store nothing. that's it!
            return *this;
        }

        template <typename KeyValue, typename Callable>
        auto& set(KeyValue&& /* key */, Callable&& callback) noexcept {
            if constexpr (std::is_invocable_v<Callable, passive_cache>) {
                return callback(*this);
            } else if (std::is_invocable_v<Callable>) {
                return callback();
            }
            return callback;
        }
    };

} // namespace webpp

#endif // WEBPP_CACHE_PASSIVE_CACHE
