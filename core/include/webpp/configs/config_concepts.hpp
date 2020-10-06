// Created by moisrex on 10/6/20.

#ifndef WEBPP_CONFIG_CONCEPTS_HPP
#define WEBPP_CONFIG_CONCEPTS_HPP


/**
 *
 * This config concepts eventually should:
 *
 * - [ ] you can configure it to be used across other software or even other computers
 * - [ ] should support multiple config formats including:
 *    - [ ] Program Arguments (using argc, and argv)
 *    - [ ] Memory
 *    - [ ] Files
 *      - [ ] INI
 *      - [ ] XML
 *      - [ ] JSON
 *    - [ ] Windows Registry
 *    - [ ] Android properties
 *    - [ ] Redis
 *    - [ ] MySQL / SQLite / SQL Server / ...
 * - [ ] should support cache
 * - [ ] should inform other software / systems about changes that happen in this instance of instance.
 * - [ ] should support user specific configs
 * - [ ] should support session specific configs
 * - [ ] should support instance specific configs (for the whole software)
 *       (just in memory)
 * - [ ] should be able to add more than one backup storage for configs.
 *       should be able to use other type of config formats for those backups.
 *       should be able to make those backups off-site.
 * - [ ] should be able to support flash configs (one write and one read
 *       = push and pop actions)
 * - [ ] should be able to support timed configs (expiration time for configs)
 * - [ ] should support encrypted configs
 *    - [ ] the whole file (if supported in the specified format)
 *    - [ ] the name of the config (if supported in the specified format)
 *    - [ ] the value (probabely in all of the formats)
 *
 *
 * So what we need in order to implement those features?
 *
 * - [ ] A way to easily run a function/method later (run_later function)
 * - [ ] A config port so other software can immediately inform us of changes
 *       features of this port:
 *         - [ ] encrypted connection or in plain (for when the other side is on
 *               the same server and we don't
 *         - [ ] relationships:
 *               - [ ] slave and master relationship (one of them only has
 *                     read-only access to the other one)
 *               - [ ] both master relationship
 *         - [ ] informing others of updates
 *         - [ ] requesting others for updates
 *         - [ ] force syncing (so the admin can do so)
 * - [ ] An encryption standard class
 * - [ ] JSON reader and writer
 * - [ ] XML reader and writer
 * - [ ] RMDB database (ORM also) support
 *
 *
 * Data structure (what user can store in it):
 * - [ ] key/value pairs
 * - [ ] Array of values for a key
 * - [ ] Nested keys
 *
 * Value types:
 * - [ ] string
 * - [ ] integers
 * - [ ] Serializable classes
 * - [ ] Vectors/Lists/Arrays/...
 * - [ ] Blob of data
 * - [ ] practically void*
 *
 * How to retrieve data:
 * - [ ] call it by its key
 * - [ ] call it by its nested key
 * - [ ] get a list of all keys in root
 * - [ ] get a list of all keys in a key (sub-keys)
 */

namespace webpp {

    template <typename T>
    concept Config = requires (T config) {
        {config["key"]}; // string_view
        {config.get("key")}; // optional<string_view>

        {config.set("key", "value")};

        config.has("key"); // bool
        config.clear(); // clear the configs
        config.flush(); // bool
    };


}

#endif // WEBPP_CONFIG_CONCEPTS_HPP
