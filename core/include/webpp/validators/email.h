#ifndef EMAIL_H
#define EMAIL_H

#include <regex>
#include <string>
#include <vector>

namespace webpp {

    class email {
      private:
        std::string username;
        std::string domain;

      public:
        email(std::string mail);

        /**
         * @brief checks if the specified email has a valid syntax or not
         * @return true if it does
         */
        bool has_valid_syntax() const noexcept;

        /**
         * @brief clear the email from unwanted characters
         * @return cleaned up email address
         */
        std::string clear(std::string const& second) noexcept;

        /**
         * @brief checks if the specified emails are equal
         * @param mail
         * @return
         */
        bool is_equal(std::string const& second) const noexcept;

        /**
         * @brief check if the email provider is in the black list
         * @return
         */
        bool is_blocked() const noexcept;

        /**
         * @brief returns other email domains of the same email providers that
         * are in the database
         * @return a series of domain names in a vector
         */
        std::vector<std::string> alternative_domains() const;

        /**
         * @brief returns the value in string
         * @return string representation of the email address
         */
        std::string to_string() const noexcept;
    };

} // namespace webpp

#endif // EMAIL_H
