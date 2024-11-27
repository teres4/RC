#include "protocol.hpp"


char ProtocolCommunication::readChar(MessageSource &message) {
    char c = (char)message
                 .get();  // get() returns an int, so we need to cast it to char

    if (!message.good()) {  // if the stream is not good, throw an exception
        throw ProtocolViolationException();
    }

    return c;
}

void ProtocolCommunication::readChar(MessageSource &message, char expected) {
    if (readChar(message) != expected) {
        // if the read char is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::readDelimiter(MessageSource &message) {
    readChar(message, '\n');  // read the delimiter
}

void ProtocolCommunication::readSpace(MessageSource &message) {
    readChar(message, ' ');  // read a space
}


std::string ProtocolCommunication::readString(MessageSource &message) {
    // To read an arbitrarily sized string, just read a string with a limit of
    // size equal to the maximum possible.
    return readString(message, std::string::npos);
}

std::string ProtocolCommunication::readString(MessageSource &message,
                                              size_t n) {
    std::string result;

    for (size_t i = 0; i < n; i++) {  // read n chars
        // Read a char
        char c = (char)message.get();

        if (!message.good()) {  // if the stream is not good, throw an exception
            throw ProtocolViolationException();
        }

        if (c == ' ' || c == '\n') {
            // If the char is a space or a delimiter, put it back in the stream
            message.unget();
            break;
        }

        // Otherwise, add it to the result
        result.push_back(c);
    }

    return result;
}

void ProtocolCommunication::readString(MessageSource &message,
                                       std::string expected) {
    if (readString(message) != expected) {
        // if the read string is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

std::string ProtocolCommunication::readString(
    MessageSource &message, std::vector<std::string> options) {
    // Read a string
    std::string string = readString(message);

    for (auto option : options) {
        // Check if the read string is one of the options
        if (string == option) {
            return string;
        }
    }
    // If not, throw an exception
    throw ProtocolViolationException();
}


int ProtocolCommunication::readNumber(MessageSource &message, size_t size) {
    std::string string = readString(message, size);

    // Check if string only contains digits
    if (is_not_numeric(string) == VALID) {
        throw ProtocolViolationException();
    }

    return stoi(string);  // Convert string to int
}


int ProtocolCommunication::readPlid(MessageSource &message) {
    int plid = readNumber(message, PLID_MAX_SIZE);  // Read a string
    
    return plid;
}


int ProtocolCommunication::readTime(MessageSource &message) {
    int time = readNumber(message, MAX_PLAYTIME_DIGITS);  // Read a string
    
    return time;
}


void ProtocolCommunication::readIdentifier(MessageSource &message,
                                           std::string identifier) {

    std::string identifierRecieved = readString(message, 3);  // Read a string

    if (identifierRecieved == PROTOCOL_ERROR) {
        // If the identifier is the error identifier, throw an exception
        throw ProtocolMessageErrorException();
    } else if (identifierRecieved != identifier) {
        // If the identifier is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeChar(std::stringstream &message, char c) {
    message.put(c);  // put() takes a char as an argument

    if (!message.good()) {  // if the stream is not good, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeDelimiter(std::stringstream &message) {
    writeChar(message, '\n');  // write the delimiter
}

void ProtocolCommunication::writeSpace(std::stringstream &message) {
    writeChar(message, ' ');  // write a space
}

void ProtocolCommunication::writeString(std::stringstream &message,
                                        std::string string) {
    for (auto c : string) {  // write each char of the string
        writeChar(message, c);
    }
}

void ProtocolCommunication::writeNumber(std::stringstream &message,
                                        int number) {
    std::string value =
        std::to_string(number);  // convert the number to a string

    writeString(message, value);  // write the string
}




std::stringstream StartCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "SNG ");  // write identifier "SNG"
    writeNumber(message, _plid);
    writeSpace(message);
    writeNumber(message, _time);
    writeDelimiter(message);  // delimiter at the end

    return message;
}

void StartCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "SNG"); The identifier is already read by the server
    readSpace(message);

    _plid = readPlid(message);
    readSpace(message);

    _time = readTime(message);
    readDelimiter(message);
}


std::stringstream StartCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RSG ");  // write identifier "RSG"

    writeString(message, _status);

    writeDelimiter(message);  // delimiter at the end

    return message;
}

void StartCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RSG");  // read identifier "RSG"

    readSpace(message);
    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "ERR"});

    readDelimiter(message);  // Read the delimiter
}