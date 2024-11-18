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


void validate_port(std::string &port);

int validate_plid(std::string plid);

void signal_handler(int sig);


#endif