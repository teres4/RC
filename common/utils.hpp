#ifndef UTILS_H
#define UTILS_H


#include <cstring>
#include <stdexcept>
#include "constants.hpp"


class UnrecoverableError : public std::runtime_error {
 public:
  UnrecoverableError(const std::string& __what) : std::runtime_error(__what) {}
  UnrecoverableError(const std::string& __what, const int __errno)
      : std::runtime_error(__what + ": " + strerror(__errno)) {}
};

void is_valid_port(std::string& port);

int is_valid_plid(char *plid);

void signal_handler(int sig);


#endif