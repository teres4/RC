#include "protocol.hpp"


char ProtocolCommunication::readChar(MessageSource &message) {
    char c = (char)message.get();  // get() returns an int, so we need to cast it to char

    return c;
}

void ProtocolCommunication::readChar(MessageSource &message, char expected) {
    if (readChar(message) != expected) {
        // if the read char is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

char ProtocolCommunication::readChar(MessageSource &message,
                                     std::vector<char> options) {
    char c = readChar(message);  // read a char

    for (auto option : options) {
        // check if the read char is one of the options
        if (option == c) {
            return c;
        }
    }
    // if not, throw an exception
    throw ProtocolViolationException();
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

std::string ProtocolCommunication::readString(MessageSource &message, size_t n) {
    std::string result;

    for (size_t i = 0; i < n; i++) {  // read n chars
        // Read a char
        char c = (char) message.get();

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

void ProtocolCommunication::readString(MessageSource &message, std::string expected) {
    if (readString(message) != expected) {
        // if the read string is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

std::string ProtocolCommunication::readString(MessageSource &message, 
                                    std::vector<std::string> options) {

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

int ProtocolCommunication::readNumber(MessageSource &message) {
    std::string string = readString(message);

    // Check if string only contains digits
    if (is_not_numeric(string) == true) {
        throw ProtocolViolationException();
    }

    return stoi(string);  // Convert string to int
}

int ProtocolCommunication::readNumber(MessageSource &message, size_t size) {
    std::string string = readString(message, size);

    // Check if string only contains digits
    if (is_not_numeric(string) == true) {
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


std::string ProtocolCommunication::readKey(MessageSource &message) {
    std::vector<char> colors = {'R', 'G', 'B', 'Y', 'O', 'P'};
    
    char c1 = readChar(message, colors);
    readSpace(message);
    char c2 = readChar(message);
    readSpace(message);
    char c3 = readChar(message);
    readSpace(message);
    char c4 = readChar(message);
    readSpace(message);

    return std::string(1, c1) + " " + c2 + " " + c3 + " " + c4;
}

void ProtocolCommunication::readIdentifier(MessageSource &message,
                                           std::string identifier) {

    std::string identifierReceived = readString(message, 3);  // Read a string

    if (identifierReceived == PROTOCOL_ERROR) {
        // If the identifier is the error identifier, throw an exception
        throw ProtocolMessageErrorException();

    } else if (identifierReceived != identifier) {
        // If the identifier is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeChar(std::string &message, char c) {
    try {
        message.push_back(c);
    } catch (const std::exception &) {
        throw ProtocolViolationException();
    }

}

void ProtocolCommunication::writeDelimiter(std::string &message) {
    writeChar(message, '\n');  // write the delimiter
}

void ProtocolCommunication::writeSpace(std::string &message) {
    writeChar(message, ' ');  // write a space
}

void ProtocolCommunication::writeString(std::string &message,
                                        std::string string) {
    for (auto c : string) {  // write each char of the string
        writeChar(message, c);
    }
}

void ProtocolCommunication::writeNumber(std::string &message,
                                        int number) {
    std::string value =
        std::to_string(number);  // convert the number to a string

    writeString(message, value);  // write the string
}




std::string StartCommunication::encodeRequest() {
    std::string message;

    writeString(message, "SNG");  // write identifier "SNG"
    writeSpace(message);
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

std::string StartCommunication::encodeResponse() {
    std::string message;

    writeString(message, "RSG");  // write identifier "RSG"
    writeSpace(message);
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



std::string TryCommunication::encodeRequest() {
    std::string message;

    writeString(message, "TRY");  // write identifier "TRY"
    writeSpace(message);
    writeNumber(message, _plid);
    writeSpace(message);
    writeString(message, _key); // writes key to try
    writeSpace(message);
    writeNumber(message, _nT);
    writeDelimiter(message);  // delimiter at the end

    return message;
}

void TryCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "TRY"); The identifier is already read by the server
    readSpace(message);

    _plid = readPlid(message);
    readSpace(message);

    _key = readKey(message);
    readSpace(message);

    _nT = readNumber(message);
    readDelimiter(message);
}


std::string TryCommunication::encodeResponse() {
    std::string message;
    //TODO
    writeString(message, "RTR");  // write identifier "RTR"
    writeSpace(message);
    writeString(message, _status);
    writeSpace(message);



    writeDelimiter(message);  // delimiter at the end

    return message;
}

void TryCommunication::decodeResponse(MessageSource &message) {
    //TODO
    readIdentifier(message, "RTR");  // read identifier "RSG"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "DUP", "INV", "NOK", "ENT", "ETM", "ERR"});
    readSpace(message);
    
    readDelimiter(message);  // Read the delimiter
}