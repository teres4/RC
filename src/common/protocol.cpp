#include "protocol.hpp"

char ProtocolCommunication::readChar(std::string &message, size_t pos)
{
    return message[pos];
}

char ProtocolCommunication::readChar(MessageSource &message)
{
    char c = (char)message.get(); // get() returns an int, so we need to cast it to char

    return c;
}

void ProtocolCommunication::readChar(MessageSource &message, char expected)
{
    if (readChar(message) != expected)
    {
        // if the read char is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

char ProtocolCommunication::readChar(MessageSource &message,
                                     std::vector<char> options)
{
    char c = readChar(message); // read a char

    for (auto option : options)
    {
        // check if the read char is one of the options
        if (option == c)
        {
            return c;
        }
    }
    // if not, throw an exception
    throw ProtocolViolationException();
}

void ProtocolCommunication::readDelimiter(MessageSource &message)
{
    readChar(message, '\n'); // read the delimiter
}

void ProtocolCommunication::readSpace(MessageSource &message)
{
    readChar(message, ' '); // read a space
}

std::string ProtocolCommunication::readString(MessageSource &message)
{
    // To read an arbitrarily sized string, just read a string with a limit of
    // size equal to the maximum possible.
    return readString(message, std::string::npos);
}

std::string ProtocolCommunication::readString(MessageSource &message, size_t n)
{
    std::string result;

    for (size_t i = 0; i < n; i++)
    { // read n chars
        // Read a char
        char c = (char)message.get();

        if (c == ' ' || c == '\n')
        {
            // If the char is a space or a delimiter, put it back in the stream
            message.unget();
            break;
        }

        // Otherwise, add it to the result
        result.push_back(c);
    }

    return result;
}

void ProtocolCommunication::readString(MessageSource &message, std::string expected)
{
    if (readString(message) != expected)
    {
        // if the read string is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

std::string ProtocolCommunication::readString(MessageSource &message,
                                              std::vector<std::string> options)
{

    // Read a string
    std::string string = readString(message);

    for (auto option : options)
    {
        // Check if the read string is one of the options
        if (string == option)
        {
            return string;
        }
    }
    // If not, throw an exception
    throw ProtocolViolationException();
}

int ProtocolCommunication::readInt(MessageSource &message)
{
    std::string string = readString(message);

    // Check if string only contains digits
    if (is_not_numeric(string))
    {
        throw ProtocolViolationException();
    }

    return stoi(string); // Convert string to int
}

int ProtocolCommunication::readInt(MessageSource &message, size_t size)
{
    std::string string = readString(message, size);

    // Check if string only contains digits
    if (is_not_numeric(string))
    {
        throw ProtocolViolationException();
    }

    return stoi(string); // Convert string to int
}

int ProtocolCommunication::readPlid(MessageSource &message)
{
    int plid = readInt(message, PLID_MAX_SIZE); // Read a string

    return plid;
}

int ProtocolCommunication::readTime(MessageSource &message)
{
    int time = readInt(message, MAX_PLAYTIME_DIGITS); // Read a string

    return time;
}

std::string ProtocolCommunication::readKey(MessageSource &message)
{
    std::vector<char> colors = {'R', 'G', 'B', 'Y', 'O', 'P'};

    char c1 = readChar(message, colors);
    readSpace(message);
    char c2 = readChar(message);
    readSpace(message);
    char c3 = readChar(message);
    readSpace(message);
    char c4 = readChar(message);

    return std::string(1, c1) + " " + c2 + " " + c3 + " " + c4;
}

void ProtocolCommunication::readIdentifier(MessageSource &message,
                                           std::string identifier)
{
    std::string identifierReceived = readString(message, 3); // Read a string

    if (identifierReceived == PROTOCOL_ERROR)
    {
        // If the identifier is the error identifier, throw an exception
        throw ProtocolMessageErrorException();
    }
    else if (identifierReceived != identifier)
    {
        // If the identifier is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

// std::string ProtocolCommunication::readfileName(MessageSource &message) {
//     std::string filename = readString(message, MAX_FNAME);

//     return filename;
// }

void ProtocolCommunication::writeChar(std::string &message, char c)
{
    try
    {
        message.push_back(c);
    }
    catch (const std::exception &)
    {
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeDelimiter(std::string &message)
{
    writeChar(message, '\n'); // write the delimiter
}

void ProtocolCommunication::writeSpace(std::string &message)
{
    writeChar(message, ' '); // write a space
}

void ProtocolCommunication::writeString(std::string &message, std::string string)
{
    for (auto c : string)
    { // write each char of the string
        writeChar(message, c);
    }
}

void ProtocolCommunication::writeInt(std::string &message, int number)
{
    // convert the number to a string
    std::string value = std::to_string(number);

    writeString(message, value); // write the string
}

void ProtocolCommunication::writeFileName(std::string &message, std::string fileName)
{
    if (fileName.length() > MAX_FNAME)
        throw ProtocolViolationException();

    writeString(message, fileName);
}

std::string StartCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "SNG"); // write identifier "SNG"
    writeSpace(message);
    writeInt(message, _plid);
    writeSpace(message);
    writeInt(message, _time);
    writeDelimiter(message); // delimiter at the end

    return message;
}

void StartCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "SNG");
    readSpace(message);

    _plid = readPlid(message);
    readSpace(message);

    _time = readTime(message);
    readDelimiter(message);
}

std::string StartCommunication::encodeResponse()
{
    std::string message;

    writeString(message, "RSG"); // write identifier "RSG"
    writeSpace(message);
    writeString(message, _status);

    writeDelimiter(message); // delimiter at the end

    return message;
}

void StartCommunication::decodeResponse(MessageSource &message)
{

    readIdentifier(message, "RSG"); // read identifier "RSG"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "ERR"});

    readDelimiter(message); // Read the delimiter
}

std::string TryCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "TRY"); // write identifier "TRY"
    writeSpace(message);
    writeInt(message, _plid);
    writeSpace(message);
    writeString(message, _key); // writes key to try
    writeSpace(message);
    writeInt(message, _nT);
    writeDelimiter(message); // delimiter at the end

    return message;
}

void TryCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "TRY");
    readSpace(message);

    _plid = readPlid(message);
    readSpace(message);

    _key = readKey(message);
    readSpace(message);

    _nT = readInt(message);
    readDelimiter(message);
}

std::string TryCommunication::encodeResponse()
{
    std::string message;
    writeString(message, "RTR"); // write identifier "RTR"
    writeSpace(message);
    writeString(message, _status);

    if (_status == "OK" || _status == "ENT" || _status == "ETM")
    {
        writeSpace(message);
        writeInt(message, _nT);
        writeSpace(message);
        writeInt(message, _nB);
        writeSpace(message);
        writeInt(message, _nW);
    }
    if (_status == "ENT" || _status == "ETM")
    {
        writeSpace(message);
        writeString(message, _key);
        // if key doesnt have spaces
    }
    writeDelimiter(message); // delimiter at the end

    return message;
}

void TryCommunication::decodeResponse(MessageSource &message)
{
    readIdentifier(message, "RTR"); // read identifier "RSG"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "DUP", "INV", "NOK", "ENT", "ETM", "ERR"});

    if (_status == "OK" || _status == "ENT" || _status == "ETM")
    {
        readSpace(message);
        _nT = readInt(message);
        readSpace(message);
        _nB = readInt(message);
        readSpace(message);
        _nW = readInt(message);
    }
    if (_status == "ENT" || _status == "ETM")
    {
        readSpace(message);
        _key = readKey(message);
    }

    readDelimiter(message); // Read the delimiter
}

std::string QuitCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "QUT"); // write identifier "QUT"
    writeSpace(message);
    writeInt(message, _plid);
    writeDelimiter(message); // delimiter at the end

    return message;
}

std::string QuitCommunication::encodeResponse()
{
    std::string message;

    writeString(message, "RQT"); // write identifier "RQT"
    writeSpace(message);
    writeString(message, _status);
    if (_status == "OK")
    {
        writeSpace(message);
        writeString(message, _key);
    }
    writeDelimiter(message); // delimiter at the end

    return message;
}

void QuitCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "QUT");
    readSpace(message);

    _plid = readPlid(message);
    readDelimiter(message);
}

void QuitCommunication::decodeResponse(MessageSource &message)
{
    readIdentifier(message, "RQT"); // read identifier "RQT"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "ERR"});

    if (_status == "OK")
    {
        // read the rest of the string
        readSpace(message);
        _key = readKey(message);
    }

    readDelimiter(message); // Read the delimiter
}

std::string DebugCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "DBG"); // write identifier "DBG"
    writeSpace(message);
    writeInt(message, _plid);
    writeSpace(message);
    writeInt(message, _time); // writes key to try
    writeSpace(message);
    writeString(message, _key);
    writeDelimiter(message); // delimiter at the end

    return message;
}

void DebugCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "DBG");
    readSpace(message);

    _plid = readPlid(message);
    readSpace(message);

    _time = readInt(message);
    readSpace(message);

    _key = readKey(message);
    readDelimiter(message);
}

std::string DebugCommunication::encodeResponse()
{
    std::string message;
    writeString(message, "RDB"); // write identifier "RDB"
    writeSpace(message);
    writeString(message, _status);

    writeDelimiter(message); // delimiter at the end

    return message;
}

void DebugCommunication::decodeResponse(MessageSource &message)
{
    readIdentifier(message, "RDB"); // read identifier "RDB"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "ERR"});

    readDelimiter(message); // Read the delimiter
}

std::string ShowTrialsCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "STR"); // write identifier "STR"
    writeSpace(message);
    writeInt(message, _plid);
    writeDelimiter(message); // delimiter at the end

    return message;
}

void ShowTrialsCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "STR");
    readSpace(message);

    _plid = readPlid(message);

    readDelimiter(message);
}

std::string ShowTrialsCommunication::encodeResponse()
{
    std::string message;
    writeString(message, "RST"); // write identifier "RST"
    writeSpace(message);
    writeString(message, _status);

    if (_status == "ACT" || _status == "FIN")
    { // with ongoing game or no ongoing game for player
        if (_Fsize > MAX_FSIZE)
        {
            throw ProtocolViolationException();
        }
        writeSpace(message);

        writeFileName(message, _Fname);

        writeSpace(message);
        writeInt(message, _Fsize);
        writeSpace(message);

        for (int i = 0; i < _Fsize; i++)
        {
            char c = readChar(_Fdata, (size_t)i);
            writeChar(message, c);
        }
    }
    writeDelimiter(message); // delimiter at the end

    return message;
}

void ShowTrialsCommunication::decodeResponse(MessageSource &message)
{
    readIdentifier(message, "RST"); // read identifier "RST"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, {"ACT", "FIN", "NOK"});

    if (_status == "ACT" || _status == "FIN")
    { // with ongoing game or no ongoing game for player
        readSpace(message);
        _Fname = readString(message);
        readSpace(message);
        _Fsize = readInt(message);

        if (_Fsize > MAX_FSIZE)
        {
            throw ProtocolViolationException();
        }
        readSpace(message);

        for (int i = 0; i < _Fsize; i++)
        {
            char c = readChar(message);
            writeChar(_Fdata, c);
        }
    }

    readDelimiter(message); // Read the delimiter
}

std::string ScoreboardCommunication::encodeRequest()
{
    std::string message;

    writeString(message, "SSB"); // write identifier "SSB"
    writeDelimiter(message);     // delimiter at the end

    return message;
}

void ScoreboardCommunication::decodeRequest(MessageSource &message)
{
    readIdentifier(message, "SSB");
    readDelimiter(message);
}

std::string ScoreboardCommunication::encodeResponse()
{
    std::string message;
    writeString(message, "RSS"); // write identifier "RSS"
    writeSpace(message);
    writeString(message, _status);

    if (_status == "OK")
    {
        if (_Fsize > MAX_FSIZE)
        {
            throw ProtocolViolationException();
        }
        writeSpace(message);

        writeFileName(message, _Fname);

        writeSpace(message);
        writeInt(message, _Fsize);
        writeSpace(message);

        for (int i = 0; i < _Fsize; i++)
        {
            char c = readChar(_Fdata, (size_t)i);
            writeChar(message, c);
        }
    }
    writeDelimiter(message); // delimiter at the end

    return message;
}

void ScoreboardCommunication::decodeResponse(MessageSource &message)
{
    readIdentifier(message, "RSS"); // read identifier "RSS"
    readSpace(message);

    // Read the status, and check if it is one of the options
    _status = readString(message, std::vector<std::string>{"EMPTY", "OK"});

    if (_status == "OK")
    {
        readSpace(message);
        _Fname = readString(message);
        readSpace(message);
        _Fsize = readInt(message);

        if (_Fsize > MAX_FSIZE)
        {
            throw ProtocolViolationException();
        }
        readSpace(message);

        for (int i = 0; i < _Fsize; i++)
        {
            char c = readChar(message);
            writeChar(_Fdata, c);
        }
    }

    readDelimiter(message); // Read the delimiter
}