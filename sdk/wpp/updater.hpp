#ifndef UPDATER_HPP
#define UPDATER_HPP

struct update {
    enum : unsigned int {
        email_database = 0x1 // update the email database
    };
};

void update(unsigned int flags);

#endif // UPDATER_HPP
