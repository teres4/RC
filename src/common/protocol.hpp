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
class ProtocolException : public std::runtime_error
{
public:
  ProtocolException()
      : std::runtime_error(
            "There was an error while communicating with the server.") {};
};

/**
 * @brief Exception thrown when a protocol violation occurs
 */
class ProtocolViolationException : public ProtocolException
{
};

/**
 * @brief Exception class for protocol message errors - when its ERR.
 */
class ProtocolMessageErrorException : public ProtocolException
{
};

/**
 * @brief The MessageSource class represents a source of messages.
 *
 * This class provides an interface for retrieving characters from a message
 * source. Subclasses of MessageSource must implement the get(), good(), and
 * unget() methods.
 */
class MessageSource
{
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
 * @brief Represents a message source that reads from a string.
 */
class StreamMessage : public MessageSource
{
private:
  std::string &_stream;
  size_t _current_pos; // current index

public:
  /**
   * @brief Constructs a StreamMessage object with the given string.
   *
   * @param stream The string to read from.
   */
  StreamMessage(std::string &stream) : _stream(stream), _current_pos(0) {};

  /**
   * @brief Reads the next character from the string.
   *
   * @return The next character from the string.
   */
  char get()
  {
    if (_current_pos < _stream.size())
      return _stream[_current_pos++];
    throw std::out_of_range("End of stream reached");
  };

  /**
   * @brief Checks if the string is in a good state.
   *
   * @return True if the string is in a good state, false otherwise.
   */
  bool good() { return _current_pos < _stream.size(); };

  /**
   * @brief Puts the last character extracted from the string back.
   */
  void unget()
  {
    if (_current_pos > 0)
      --_current_pos;
  };
};


/**
 * @brief The ProtocolCommunication class is an abstract base class that defines
 * the interface for communication protocols.
 */
class ProtocolCommunication
{
public:
  // Each subclass should implement their information as members.

  virtual std::string encodeRequest() = 0;

  virtual void decodeRequest(MessageSource &message) = 0;

  virtual std::string encodeResponse() = 0;

  virtual void decodeResponse(MessageSource &message) = 0;

  // General purpose methods to allow parsing and encoding.

  /**
   * @brief Reads character from position pos from a string.
   */
  char readChar(std::string &message, size_t pos);

  /**
   * @brief Reads a character from a MessageSource.
   */
  char readChar(MessageSource &message);

  /**
   * @brief Reads a character from a MessageSource.
   */
  void readChar(MessageSource &message, char expected);

  /**
   * @brief Reads a character from a MessageSource.
   */
  char readChar(MessageSource &message, std::vector<char> options);

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

  /**
   * @brief Reads a number from a MessageSource.
   */
  int readInt(MessageSource &message);

  /**
   * @brief Reads an int from a MessageSource with the specified size.
   */
  int readInt(MessageSource &message, size_t size);

  /**
   * @brief Reads a PLID from a MessageSource.
   */
  int readPlid(MessageSource &message);

  /**
   * @brief Reads time from a MessageSource.
   */
  int readTime(MessageSource &message);

  /**
   * @brief Reads key from a MessageSource.
   */
  std::string readKey(MessageSource &message);

  /**
   * @brief Reads a identifier from a MessageSource.
   *
   * @param message The MessageSource to read from.
   * @param identifier The identifier to read.
   */
  void readIdentifier(MessageSource &message, std::string identifier);

  /**
   * @brief Writes a character to string.
   *
   * @param message The string to write to.
   * @param c The character to write.
   */
  void writeChar(std::string &message, char c);

  /**
   * @brief Writes a delimiter to string.
   *
   * @param message The string to write to.
   */
  void writeDelimiter(std::string &message);

  /**
   * @brief Writes a space to string.
   *
   * @param message The string to write to.
   */
  void writeSpace(std::string &message);

  /**
   * @brief Writes a string to string.
   *
   * @param message The string to write to.
   * @param string The string to write.
   */
  void writeString(std::string &message, std::string string);

  /**
   * @brief Writes an int to string.
   *
   * @param message The string to write to.
   * @param number The number to write.
   */
  void writeInt(std::string &message, int number);

  /**
   * @brief Writes the file name to the given message.
   *
   * @param message The message to write the file name to.
   * @param fileName The name of the file to be written.
   */
  void writeFileName(std::string &message, std::string fileName);

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
class StartCommunication : public ProtocolCommunication
{
public:
  // Request parameters:
  int _plid; // The player ID for start request.
  int _time; // The time for start request.

  // Response parameters:
  std::string _status; // The status of the start response.

  std::string encodeRequest();

  void decodeRequest(MessageSource &message);

  std::string encodeResponse();

  void decodeResponse(MessageSource &message);

  bool isTcp() { return false; };
};

class TryCommunication : public ProtocolCommunication
{
public:
  // Request parameters:
  int _plid;        // The player ID for try request.
  std::string _key; // The key for try request.
  int _nT;          // The trial number

  // Response parameters:
  std::string _status; // The status of the try response.
  int _nB;
  int _nW;

  std::string encodeRequest();

  void decodeRequest(MessageSource &message);

  std::string encodeResponse();

  void decodeResponse(MessageSource &message);

  bool isTcp() { return false; };
};

class QuitCommunication : public ProtocolCommunication
{
  public:
  // Request parameters:
  int _plid; // The player ID for quit request.

  // Response parameters:
  std::string _status; // The status of the quit response.
  std::string _key;

  std::string encodeRequest();

  void decodeRequest(MessageSource &message);

  std::string encodeResponse();

  void decodeResponse(MessageSource &message);

  bool isTcp() { return false; } 
};


class DebugCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    int _plid;          // The player ID for debug request.
    int _time;          // The time for game.
    std::string _key;   // The key for debug request.

    // Response parameters:
    std::string _status;

  std::string encodeRequest();

  void decodeRequest(MessageSource &message);

  std::string encodeResponse();

  void decodeResponse(MessageSource &message);

  bool isTcp() { return false; };
};



class ShowTrialsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    int _plid;  // The player ID for show trials request.

    // Response parameters:
    std::string _status;    // The status of the start response.
    std::string _Fname;     // The filename
    int _Fsize;             // The file size, in bytes
    std::string _Fdata;     // Thecontents of the selected file.

    std::string encodeRequest();

    void decodeRequest(MessageSource &message);

    std::string encodeResponse();

    void decodeResponse(MessageSource &message);

    bool isTcp() { return true; };
};


class ScoreboardCommunication : public ProtocolCommunication {
  public:
  
    // Response parameters:
    std::string _status;    // The status of the start response.
    std::string _Fname;     // The filename
    int _Fsize;             // The file size, in bytes
    std::string _Fdata;     // Thecontents of the selected file.

    std::string encodeRequest();

    void decodeRequest(MessageSource &message);

    std::string encodeResponse();

    void decodeResponse(MessageSource &message);

    bool isTcp() { return true; };
};


#endif