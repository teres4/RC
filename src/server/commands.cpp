#include "commands.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> command)
{
    try
    {
        _handlers.insert({command->_name, command});
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return;
}

void CommandManager::registerAllCommands()
{
    this->registerCommand(std::make_shared<StartCommand>());
    this->registerCommand(std::make_shared<TryCommand>());
    this->registerCommand(std::make_shared<QuitCommand>());
    this->registerCommand(std::make_shared<ShowTrialsCommand>());
    this->registerCommand(std::make_shared<ScoreboardCommand>());
    this->registerCommand(std::make_shared<ExitCommand>());
    this->registerCommand(std::make_shared<DebugCommand>());
}

std::string CommandManager::handleCommand(std::string message, Server &receiver)
{
    std::vector<std::string> command_split = split_command(message);

    if (command_split.size() == 0)
        return "ERR";

    std::string commandName = command_split[0]; // The name of the command

    if (commandName.length() == 0 || commandName.length() != 3)
        return "ERR";

    auto handler = _handlers.find(commandName); // find handler of the command

    if (handler == _handlers.end())
    { // If the handler does not exist
        handler = _handlers.find(commandName);
        if (handler == _handlers.end())
        {
            std::cout << "Invalid command: " << commandName << std::endl;
            return "ERR";
        }
    }

    std::string response;
    handler->second->handle(message, response, receiver);

    return response;
}

void StartCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    StartCommunication startComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        startComm.decodeRequest(reqMessage); // Decode the request
        std::string plid = std::to_string(startComm._plid);

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(plid);

        // if has game or has an ongoing game that should've ended bc it ran out of time
        if (!hasGame || (hasGame && DB.gameShouldEnd(plid)))
        {

            if (hasGame && DB.gameShouldEnd(plid))
                DB.gameTimeout(plid);

            std::string current_datetime = currentDateTime();
            time_t time_s = time(NULL);
            DB.createGame(plid, 'P', startComm._time, current_datetime, time_s);
            startComm._status = "OK";
            result = "Player started a game. ";
        }
        else
        {
            startComm._status = "NOK";
            result = "Player has an ongoing game";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        startComm._status = "ERR";
        result = "Protocol Error";
    }
    response = startComm.encodeResponse(); // Encode the response
    return;
}

void TryCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose
    GamedataManager DB = receiver._DB;

    // TRY PLID C1 C2 C3 C4 nT

    // RTR status [nT nB nW][C1 C2 C3 C4]

    TryCommunication tryComm;

    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        tryComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(tryComm._plid));
        if (!hasGame)
        {
            tryComm._status = "NOK";
            result = "Player doesnt have ongoing game";
        }
        else
        {
            if (DB.remainingTime(std::to_string(tryComm._plid)) <= 0)
            {
                DB.gameTimeout(std::to_string(tryComm._plid));
                tryComm._status = "ETM";
                /*reveal secret key*/
                tryComm._key = DB.getsecretKey(std::to_string(tryComm._plid));
            }
            else if (DB.isRepeatedTrial(std::to_string(tryComm._plid),
                                        removeSpaces(tryComm._key)))
            {
                tryComm._status = "DUP";
                result = "Repeated trial";
            }
            else
            {

                int expectedNT = DB.expectedNT(std::to_string(tryComm._plid));
                if (expectedNT == tryComm._nT)
                {
                    tryComm._status = "OK";

                    tryComm._nB = black(removeSpaces(tryComm._key),
                                        DB.getsecretKey(std::to_string(tryComm._plid)));
                    tryComm._nW = white(removeSpaces(tryComm._key),
                                        DB.getsecretKey(std::to_string(tryComm._plid)));

                    DB.registerTry(std::to_string(tryComm._plid),
                                   removeSpaces(tryComm._key), tryComm._nB, tryComm._nW);
                    if (tryComm._nB == 4)
                    {
                        // end game
                        // player wins
                        DB.gameWon(std::to_string(tryComm._plid));
                    }
                    else if (tryComm._nT == 8)
                    {
                        // end game
                        // player loses
                        DB.gameLost(std::to_string(tryComm._plid));
                        tryComm._status = "ENT";
                        /*reveal secret key*/
                        tryComm._key = DB.getsecretKey(std::to_string(tryComm._plid));
                    }
                }
                else if (tryComm._nT == expectedNT - 1)
                {
                    if (DB.isRepeatedTrial(std::to_string(tryComm._plid),
                                           removeSpaces(tryComm._key)))
                    {
                        /*nothing*/
                    }
                    else
                    {
                        tryComm._status = "INV";
                        result = "Invalid nT, not a Dupe";
                    }
                }
            }
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        tryComm._status = "ERR";
        result = "Protocol Error";
    }
    response = tryComm.encodeResponse(); // Encode the response
    return;
}

void ShowTrialsCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    ShowTrialsCommunication stComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        stComm.decodeRequest(reqMessage); // Decode the request

        std::string plid = std::to_string(stComm._plid);

        bool hasGame = DB.hasOngoingGame(plid); // check if theres an ongoing game
        if (hasGame)
        {   // send text with current game summary
            DB.getCurrentGameData(plid, stComm._Fname, 
                                    stComm._Fsize, stComm._Fdata);
            stComm._status = "ACT";                        
            result = "Showing player's ongoing game";
        }
        else if (DB.hasGames(plid)) // if no ongoing game, check if there was ever a game
        {   // send text with most recent game
            DB.getMostRecentGameData(plid, stComm._Fname, 
                                    stComm._Fsize, stComm._Fdata);
            stComm._status = "FIN";
            result = "Showing player's most recently finished game. ";
        }
        else
        {
            stComm._status = "NOK";
            result = "Player has no ongoing game or recently finished games. ";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        stComm._status = "ERR";
        result = "Protocol Error";
    }
    response = stComm.encodeResponse(); // Encode the response
    return;
}

void ScoreboardCommand::handle(std::string &args, std::string &response, Server &receiver)
{
        // TODO check verbose
    SCORELIST list;

    GamedataManager DB = receiver._DB;
    ScoreboardCommunication sbComm;
    
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        sbComm.decodeRequest(reqMessage); // Decode the request

        int nscores = findTopScores(&list);
        if (!nscores)
        {
            sbComm._status = "EMPTY";
            result = "No scores found";
        }
        else
        {
            DB.formatScoreboard(&list, sbComm._Fname, sbComm._Fsize, sbComm._Fdata, nscores);
            sbComm._status = "OK";
            result = "Showing top " + std::to_string(nscores) + " games";
        }
    }
    catch (ProtocolException &e)
    {   // If the protocol is not valid, status = "ERR"
        sbComm._status = "ERR";
        result = "Protocol Error";
    }

    response = sbComm.encodeResponse();

    return;
}

void QuitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    QuitCommunication quitComm;
    std::string result;
    try
    {
        StreamMessage reqMessage(args);
        quitComm.decodeRequest(reqMessage); // Decode the request

        bool hasGame = DB.hasOngoingGame(std::to_string(quitComm._plid));
        if (hasGame) // exit game
        {
            std::string key = DB.getsecretKey(std::to_string(quitComm._plid));
            quitComm._key = DB.formatSecretKey(key);
            DB.quitGame(std::to_string(quitComm._plid));

            quitComm._status = "OK"; // Set the status to OK if everything goes right
            result = "Game has ended sucessfully";
        }
        else
        {
            quitComm._status = "NOK";
            result = "Player does not have an ongoing game";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, set the status to ERR
        quitComm._status = "ERR";
        result = "Protocol Error";
    }
    response = quitComm.encodeResponse(); // Encode the response
    return;
}

void ExitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    QuitCommunication exitComm;
    std::string result;
    try
    {
        StreamMessage reqMessage(args);
        exitComm.decodeRequest(reqMessage); // Decode the request

        bool hasGame = DB.hasOngoingGame(std::to_string(exitComm._plid));
        if (hasGame) // exit game
        {
            std::string key = DB.getsecretKey(std::to_string(exitComm._plid));
            exitComm._key = DB.formatSecretKey(key);
            DB.quitGame(std::to_string(exitComm._plid));

            exitComm._status = "OK"; // Set the status to OK if everything goes right
            result = "Game has ended sucessfully";
        }
        else
        {
            exitComm._status = "NOK";
            result = "Player does not have an ongoing game";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, set the status to ERR
        exitComm._status = "ERR";
        result = "Protocol Error";
    }
    response = exitComm.encodeResponse(); // Encode the response
    return;
}

void DebugCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    DebugCommunication dbgComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        dbgComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(dbgComm._plid));
        if (hasGame)
        {
            dbgComm._status = "NOK";
            result = "Player has an ongoing game";
        }
        else
        {
            dbgComm._status = "OK";
            result = "Player does not have an ongoing game";

            std::string current_datetime = currentDateTime();
            DB.createGame(std::to_string(dbgComm._plid), 'D', removeSpaces(dbgComm._key),
                          dbgComm._time, current_datetime, time_t(&current_datetime));
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        dbgComm._status = "ERR";
        result = "Protocol Error";
    }
    response = dbgComm.encodeResponse(); // Encode the response
    return;
}

std::vector<std::string> split_command(std::string input)
{
    std::stringstream ss(input); // Create a stringstream object
    std::string temp;
    std::vector<std::string> command_split;

    // Splitting the input string by space
    while (std::getline(ss, temp, ' '))
    {
        temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
        command_split.push_back(temp);
    }
    return command_split;
}

std::string removeSpaces(std::string &str)
{
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}
