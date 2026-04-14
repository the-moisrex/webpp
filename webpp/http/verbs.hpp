#ifndef WEBPP_HTTP_VERBS_HPP
#define WEBPP_HTTP_VERBS_HPP

#include "../std/string_view.hpp"

namespace webpp::http {

    /**
     * HTTP request method verbs
     *
     * Each verb corresponds to a particular method string
     * used in HTTP request messages.
     */
    enum struct [[nodiscard]] verb : stl::uint8_t {
        /**
         * An unknown method.
         *
         * This value indicates that the request method string is not
         * one of the recognized verbs. Callers interested in the method
         * should use an interface which returns the original string.
         */
        unknown = 0,

        // The DELETE method deletes the specified resource
        del,

        /**
         * The GET method requests a representation of the specified resource.
         *
         * Requests using GET should only retrieve data and should have no other effect.
         */
        get,

        /**
         * The HEAD method asks for a response identical to that of a GET request, but without the response
         * body.
         *
         * This is useful for retrieving meta-information written in response
         * headers, without having to transport the entire content.
         */
        head,

        /**
         * The POST method requests that the server accept the entity enclosed in the request as a new
         * subordinate of the web resource identified by the URI.
         *
         * The data POSTed might be, for example, an annotation for existing
         * resources; a message for a bulletin board, newsgroup, mailing list,
         * or comment thread; a block of data that is the result of submitting
         * a web form to a data-handling process; or an item to add to a database
         */
        post,

        /**
         * The PUT method requests that the enclosed entity be stored under the supplied URI.
         *
         * If the URI refers to an already existing resource, it is modified;
         * if the URI does not point to an existing resource, then the server
         * can create the resource with that URI.
         */
        put,

        /**
         * The CONNECT method converts the request connection to a transparent TCP/IP tunnel.
         *
         * This is usually to facilitate SSL-encrypted communication (HTTPS)
         * through an unencrypted HTTP proxy.
         */
        connect,

        /**
         * The OPTIONS method returns the HTTP methods that the server supports for the specified URL.
         *
         * This can be used to check the functionality of a web server by requesting
         * '*' instead of a specific resource.
         */
        options,

        /**
         * The TRACE method echoes the received request so that a client can see what (if any) changes or
         * additions have been made by intermediate servers.
         */
        trace,

        // WebDAV
        copy,
        lock,
        mkcol,
        move,
        propfind,
        proppatch,
        search,
        unlock,
        bind,
        rebind,
        unbind,
        acl,

        // subversion
        report,
        mkactivity,
        checkout,
        merge,

        // upnp

        msearch,
        notify,
        subscribe,
        unsubscribe,

        // RFC-5789
        patch,
        purge,

        // CalDAV
        mkcalendar,

        // RFC-2068, section 19.6.1.2
        link,
        unlink
    };

    /**
     * Converts a string to the request method verb.
     *
     * If the string does not match a known request method,
     * @ref verb::unknown is returned.
     */
    static constexpr verb string_to_verb(stl::string_view str) noexcept {
        using namespace std::literals::string_view_literals;
        if (str.size() < 3) {
            return verb::unknown;
        }
        auto cur = str[0];
        str.remove_prefix(1);
        // todo: add lowercase as well
        switch (cur) {
            case 'A':
                if (str == "CL"sv) {
                    return verb::acl;
                }
                break;

            case 'B':
                if (str == "IND"sv) {
                    return verb::bind;
                }
                break;

            case 'C':
                cur = str[0];
                str.remove_prefix(1);
                switch (cur) {
                    case 'H':
                        if (str == "ECKOUT"sv) {
                            return verb::checkout;
                        }
                        break;

                    case 'O':
                        if (str == "NNECT"sv) {
                            return verb::connect;
                        }
                        if (str == "PY"sv) {
                            return verb::copy;
                        }
                        break;

                    default: break;
                }
                break;

            case 'D':
                if (str == "ELETE"sv) {
                    return verb::del;
                }
                break;

            case 'G':
                if (str == "ET"sv) {
                    return verb::get;
                }
                break;

            case 'H':
                if (str == "EAD"sv) {
                    return verb::head;
                }
                break;

            case 'L':
                if (str == "INK"sv) {
                    return verb::link;
                }
                if (str == "OCK"sv) {
                    return verb::lock;
                }
                break;

            case 'M':
                cur = str[0];
                str.remove_prefix(1);
                switch (cur) {
                    case '-':
                        if (str == "SEARCH"sv) {
                            return verb::msearch;
                        }
                        break;

                    case 'E':
                        if (str == "RGE"sv) {
                            return verb::merge;
                        }
                        break;

                    case 'K':
                        if (str == "ACTIVITY"sv) {
                            return verb::mkactivity;
                        }
                        if (str[0] == 'C') {
                            str.remove_prefix(1);
                            if (str == "ALENDAR"sv) {
                                return verb::mkcalendar;
                            }
                            if (str == "OL"sv) {
                                return verb::mkcol;
                            }
                            break;
                        }
                        break;

                    case 'O':
                        if (str == "VE"sv) {
                            return verb::move;
                        }
                        break;

                    default: break;
                }
                break;

            case 'N':
                if (str == "OTIFY"sv) {
                    return verb::notify;
                }
                break;

            case 'O':
                if (str == "PTIONS"sv) {
                    return verb::options;
                }
                break;

            case 'P':
                cur = str[0];
                str.remove_prefix(1);
                switch (cur) {
                    case 'A':
                        if (str == "TCH"sv) {
                            return verb::patch;
                        }
                        break;

                    case 'O':
                        if (str == "ST"sv) {
                            return verb::post;
                        }
                        break;

                    case 'R':
                        if (str == "OPFIND"sv) {
                            return verb::propfind;
                        }
                        if (str == "OPPATCH"sv) {
                            return verb::proppatch;
                        }
                        break;

                    case 'U':
                        if (str == "RGE"sv) {
                            return verb::purge;
                        }
                        if (str == "T"sv) {
                            return verb::put;
                        }
                        break;

                    default: break;
                }
                break;

            case 'R':
                if (str[0] != 'E') {
                    break;
                }
                str.remove_prefix(1);
                if (str == "BIND"sv) {
                    return verb::rebind;
                }
                if (str == "PORT"sv) {
                    return verb::report;
                }
                break;

            case 'S':
                if (str == "EARCH"sv) {
                    return verb::search;
                }
                if (str == "UBSCRIBE"sv) {
                    return verb::subscribe;
                }
                break;

            case 'T':
                if (str == "RACE"sv) {
                    return verb::trace;
                }
                break;

            case 'U':
                if (str[0] != 'N') {
                    break;
                }
                str.remove_prefix(1);
                if (str == "BIND"sv) {
                    return verb::unbind;
                }
                if (str == "LINK"sv) {
                    return verb::unlink;
                }
                if (str == "LOCK"sv) {
                    return verb::unlock;
                }
                if (str == "SUBSCRIBE"sv) {
                    return verb::unsubscribe;
                }
                break;

            default: break;
        }

        return verb::unknown;
    }

    // Returns the text representation of a request method verb.
    [[nodiscard]] static constexpr stl::string_view to_string(verb const inp_verb) noexcept {
        using namespace std::literals::string_view_literals;
        switch (inp_verb) {
            case verb::del: return "DELETE"sv;
            case verb::get: return "GET"sv;
            case verb::head: return "HEAD"sv;
            case verb::post: return "POST"sv;
            case verb::put: return "PUT"sv;
            case verb::connect: return "CONNECT"sv;
            case verb::options: return "OPTIONS"sv;
            case verb::trace: return "TRACE"sv;

            case verb::copy: return "COPY"sv;
            case verb::lock: return "LOCK"sv;
            case verb::mkcol: return "MKCOL"sv;
            case verb::move: return "MOVE"sv;
            case verb::propfind: return "PROPFIND"sv;
            case verb::proppatch: return "PROPPATCH"sv;
            case verb::search: return "SEARCH"sv;
            case verb::unlock: return "UNLOCK"sv;
            case verb::bind: return "BIND"sv;
            case verb::rebind: return "REBIND"sv;
            case verb::unbind: return "UNBIND"sv;
            case verb::acl: return "ACL"sv;

            case verb::report: return "REPORT"sv;
            case verb::mkactivity: return "MKACTIVITY"sv;
            case verb::checkout: return "CHECKOUT"sv;
            case verb::merge: return "MERGE"sv;

            case verb::msearch: return "M-SEARCH"sv;
            case verb::notify: return "NOTIFY"sv;
            case verb::subscribe: return "SUBSCRIBE"sv;
            case verb::unsubscribe: return "UNSUBSCRIBE"sv;

            case verb::patch: return "PATCH"sv;
            case verb::purge: return "PURGE"sv;

            case verb::mkcalendar: return "MKCALENDAR"sv;

            case verb::link: return "LINK"sv;
            case verb::unlink: return "UNLINK"sv;

            case verb::unknown:
            default: return ""sv;
        }
    }

    // Returns true if the request method is "safe" (per section 4.2.1 of RFC 7231).
    [[nodiscard]] static constexpr bool is_verb_safe(stl::string_view const method) noexcept {
        using namespace std::literals::string_view_literals;
        return method == "GET"sv || method == "HEAD"sv || method == "OPTIONS"sv || method == "TRACE"sv;
    }

    // Returns true if the request method is "safe" (per section 4.2.1 of RFC 7231).
    [[nodiscard]] static constexpr bool is_verb_safe(verb const method) noexcept {
        switch (method) {
            case verb::get:
            case verb::head:
            case verb::options:
            case verb::trace: return true;
            default: return false;
        }
    }

    // Returns true if the request method is idempotent (per section 4.2.2 of RFC 7231).
    [[nodiscard]] static constexpr bool is_verb_idempotent(verb const method) noexcept {
        switch (method) {
            case verb::put:
            case verb::del: return true;
            default: return is_verb_safe(method);
        }
    }

    // Returns true if the request method is idempotent (per section 4.2.2 of RFC 7231).
    [[nodiscard]] static constexpr bool is_verb_idempotent(stl::string_view const method) noexcept {
        return is_verb_idempotent(string_to_verb(method));
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_VERBS_HPP
