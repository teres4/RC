#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <unistd.h>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "utils.hpp"

/**
 * @brief Exception class for protocol-related errors.
 *
 * This exception is thrown when there is an error while communicating with the
 * server.
 */
class ProtocolException : public std::runtime_error {
  public:
    ProtocolException()
        : std::runtime_error(
              "There was an error while communicating with the server."){};
};

/**
 * @brief Exception thrown when a protocol violation occurs
 */
class ProtocolViolationException : public ProtocolException {};

/**
 * @brief Exception class for protocol message errors.
 */
class ProtocolMessageErrorException : public ProtocolException {};

/**
 * @brief The MessageSource class represents a source of messages.
 *
 * This class provides an interface for retrieving characters from a message
 * source. Subclasses of MessageSource must implement the get(), good(), and
 * unget() methods.
 */
class MessageSource {
  public:
    /**
     * @brief Get the next character from the message source.
     *
     * @return The next character from the message source.
     */
    virtual char get() = 0;

    /**
     * @brief Check if the message source is in a good state.
     *
     * @return true if the message source is in a good state, false otherwise.
     */
    virtual bool good() = 0;

    /**
     * @brief Unget the last character read from the message source.
     *
     * This method allows the caller to "unread" the last character read from
     * the message source. After calling unget(), the next call to get() should
     * return the same character that was previously read.
     */
    virtual void unget() = 0;
};

/**
 * @brief Represents a message source that reads from a stringstream.
 */
class StreamMessage : public MessageSource {
  private:
    std::stringstream &_stream;

  public:
    /**
     * @brief Constructs a StreamMessage object with the given stringstream.
     *
     * @param stream The stringstream to read from.
     */
    StreamMessage(std::stringstream &stream) : _stream(stream){};

    /**
     * @brief Reads the next character from the stringstream.
     *
     * @return The next character from the stringstream.
     */
    char get() { return (char)_stream.get(); };

    /**
     * @brief Checks if the stringstream is in a good state.
     *
     * @return True if the stringstream is in a good state, false otherwise.
     */
    bool good() { return _stream.good(); };

    /**
     * @brief Puts the last character extracted from the stringstream back.
     */
    void unget() { _stream.unget(); };
};


/**
 * @brief Represents a TCP message source.
 *
 * This class provides functionality to read characters from a TCP socket
 * and store them in a buffer. It also allows ungetting characters from the
 * buffer.
 */
class TcpMessage : public MessageSource {
  private:
    int _fd;  // The file descriptor of the TCP socket.
    // The buffer to store characters read from the TCP socket.
    std::deque<char> _buffer;
    char _last;  // The last character read from the buffer.

  public:
    /**
     * @brief Constructs a TcpMessage object with the given file descriptor.
     *
     * @param fd The file descriptor of the TCP socket.
     */
    TcpMessage(int fd) : _fd(fd){};

    /**
     * @brief Fills the buffer with characters read from the TCP socket.
     *
     * This function reads characters from the TCP socket and stores them in the
     * buffer. If an error occurs during reading, a ProtocolException is thrown.
     */
    void fillBuffer() {
        char buf[128];

        ssize_t n = read(_fd, buf, 128);

        if (n <= 0) {
            throw ProtocolViolationException();
        }

        for (int i = 0; i < n; i++) {
            _buffer.push_back(buf[i]);
        }
    }

    /**
     * @brief Gets the next character from the buffer.
     *
     * If the buffer is empty, this function calls fillBuffer() to read more
     * characters from the TCP socket. The last character read is stored in the
     * _last member variable.
     *
     * @return The next character from the buffer.
     */
    char get() {
        if (_buffer.size() == 0) {
            fillBuffer();
        }

        _last = _buffer.front();
        _buffer.pop_front();
        return _last;
    };

    /**
     * @brief Checks if the TcpMessage is in a good state.
     *
     * This function always returns true, indicating that the TcpMessage is in a
     * good state.
     *
     * @return true if the TcpMessage is in a good state, false otherwise.
     */
    bool good() { return true; };

    /**
     * @brief Puts the last character back into the buffer.
     *
     * This function adds the last character read back to the front of the
     * buffer. The character can then be read again by calling get().
     */
    void unget() { _buffer.push_front(_last); };
};



/**
 * @brief The ProtocolCommunication class is an abstract base class that defines
 * the interface for communication protocols.
 */
class ProtocolCommunication {
  public:
    // Each subclass should implement their information as members.

    virtual std::stringstream encodeRequest() = 0;

    virtual void decodeRequest(MessageSource &message) = 0;

    virtual std::stringstream encodeResponse() = 0;

    virtual void decodeResponse(MessageSource &message) = 0;


    // General purpose methods to allow parsing and encoding.

    /**
     * @brief Reads a character from a MessageSource.
     */
    char readChar(MessageSource &message);

    /**
     * @brief Reads a character from a MessageSource.
     */
    void readChar(MessageSource &message, char expected);

    /**
     * @brief Reads a delimiter from a MessageSource.
     */
    void readDelimiter(MessageSource &message);

    /**
     * @brief Reads a space from a MessageSource.
     */
    void readSpace(MessageSource &message);

    /**
     * @brief Reads a string from a MessageSource.
     */
    std::string readString(MessageSource &message);

    /**
     * @brief Reads a string from a MessageSource.
     */
    std::string readString(MessageSource &message, size_t n);

    /**
     * @brief Reads a string from a MessageSource.
     */
    void readString(MessageSource &message, std::string expected);

    /**
     * @brief Reads a string from a MessageSource.
     */
    std::string readString(MessageSource &message,
                           std::vector<std::string> options);


    int readNumber(MessageSource &message, size_t size);

    /**
     * @brief Reads a PLID from a MessageSource.
     */
    int readPlid(MessageSource &message);


    /**
     * @brief Reads time from a MessageSource.
     */
    int readTime(MessageSource &message);


    /**
     * @brief Reads a identifier from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param identifier The identifier to read.
     */
    void readIdentifier(MessageSource &message, std::string identifier);

    /**
     * @brief Writes a character to stringstream.
     *
     * @param message The stringstream to write to.
     * @param c The character to write.
     */
    void writeChar(std::stringstream &message, char c);

    /**
     * @brief Writes a delimiter to stringstream.
     *
     * @param message The stringstream to write to.
     */
    void writeDelimiter(std::stringstream &message);

    /**
     * @brief Writes a space to stringstream.
     *
     * @param message The stringstream to write to.
     */
    void writeSpace(std::stringstream &message);

    /**
     * @brief Writes a string to stringstream.
     *
     * @param message The stringstream to write to.
     * @param string The string to write.
     */
    void writeString(std::stringstream &message, std::string string);

    /**
     * @brief Writes a number to stringstream.
     *
     * @param message The stringstream to write to.
     * @param number The number to write.
     */
    void writeNumber(std::stringstream &message, int number);

    /**
     * @brief Writes a UID to stringstream.
     *
     * @param message The stringstream to write to.
     * @param uid The UID to write.
     */
    void writePlid(std::stringstream &message, std::string uid);

    /**
     * @brief Writes the file name to the given message.
     *
     * @param message The message to write the file name to.
     * @param fileName The name of the file to be written.
     */
    void writeFileName(std::stringstream &message, std::string fileName);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    virtual bool isTcp() = 0;
};



/**
 * @brief Represents a communication protocol for start functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for start communication.
 */
class StartCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    int _plid;       // The user ID for start request.
    int _time;      // The password for start request.

    // Response parameters:
    std::string _status;    // The status of the start response.


    /**
     * @brief Encodes the start request into a stringstream.
     *
     * @return The encoded start request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes the start request from a stringstream.
     *
     * @param message The stringstream containing the start request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes the start response into a stringstream.
     *
     * @return The encoded start response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes the start response from a stringstream.
     *
     * @param message The stringstream containing the start response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication is using TCP.
     *
     * @return True if the communication is using TCP, false otherwise.
     */
    bool isTcp() { return false; };
};





#endif