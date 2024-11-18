#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <sys/socket.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "constants.hpp"

// Thrown when the PacketID does not match what was expected
class UnexpectedPacketException : public std::runtime_error
{
public:
    UnexpectedPacketException()
        : std::runtime_error(
              "The server did not reply with the expected response, so it was "
              "ignored. Please try again or connect to a different game "
              "server.")
    {
    }
};

// Thrown when the PacketID is correct, but the schema is wrong
class InvalidPacketException : public std::runtime_error
{
public:
    InvalidPacketException()
        : std::runtime_error(
              "The response given by the server is not correctly structured, so "
              "it was ignored. Please try again or connect to a different game "
              "server.")
    {
    }
};

// Thrown when serialization error occurs
class PacketSerializationException : public std::runtime_error
{
public:
    PacketSerializationException()
        : std::runtime_error(
              "There was an error while preparing a request, "
              "please try again and restart the application if the problem "
              "persists.")
    {
    }
};

// Thrown when timeout for reading/writing packet occurs
class ConnectionTimeoutException : public std::runtime_error
{
public:
    ConnectionTimeoutException()
        : std::runtime_error(
              "Could not connect to the game server, please check your internet "
              "connection and try again.")
    {
    }
};

// Thrown when an error related to I/O occurs
class IOException : public std::runtime_error
{
public:
    IOException()
        : std::runtime_error(
              "IO error while reading/writting from/to filesystem") {}
};

// Thrown when an error related to I/O occurs
class OperationCancelledException : public std::runtime_error
{
public:
    OperationCancelledException()
        : std::runtime_error("Operation cancelled by user") {}
};

#endif