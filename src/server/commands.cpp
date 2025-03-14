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
    GamedataManager DB = receiver._DB;

    StartCommunication startComm;

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
            {
                DB.gameTimeout(plid);
                std::cout << "Previous game has timed out. Starting a new game. " << std::endl;
            }

            std::string current_datetime = currentDateTime();
            time_t time_s = time(NULL);
            DB.createGame(plid, 'P', startComm._time, current_datetime, time_s);
            startComm._status = "OK";
        }
        else
            startComm._status = "NOK";
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        startComm._status = "ERR";
    }
    response = startComm.encodeResponse(); // Encode the response

    if (receiver.isverbose())
    {
        std::cout << "Start Request by: " << startComm._plid << std::endl;
        std::cout << "Response: " << response;
    }

    return;
}

void TryCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;

    TryCommunication tryComm;

    try
    {
        StreamMessage reqMessage(args);
        tryComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(tryComm._plid));
        if (!hasGame)
            tryComm._status = "NOK";

        else
        {
            if (DB.remainingTime(std::to_string(tryComm._plid)) <= 0)
            {
                tryComm._key = DB.getsecretKey(std::to_string(tryComm._plid));
                tryComm._key = DB.formatSecretKey(tryComm._key);
                DB.gameTimeout(std::to_string(tryComm._plid));
                tryComm._status = "ETM";
            }
            else if (DB.isRepeatedTrial(std::to_string(tryComm._plid),
                                        removeSpaces(tryComm._key)))
            {
                tryComm._status = "DUP";
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

                    if (tryComm._nB == 4) // end game, player won
                        DB.gameWon(std::to_string(tryComm._plid));
                    else if (tryComm._nT == 8) // end game, player loss
                    {
                        tryComm._status = "ENT";
                        // reveal secret key
                        tryComm._key = DB.getsecretKey(std::to_string(tryComm._plid));
                        tryComm._key = DB.formatSecretKey(tryComm._key);
                        DB.gameLost(std::to_string(tryComm._plid));
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
                        tryComm._status = "INV";
                }
            }
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        tryComm._status = "ERR";
    }
    response = tryComm.encodeResponse(); // Encode the response
    if (receiver.isverbose())
    {
        std::cout << "Try Request by: " << tryComm._plid << std::endl;
        std::cout << "Response: " << response;
    }
    return;
}

void ShowTrialsCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;
    ShowTrialsCommunication stComm;

    try
    {
        StreamMessage reqMessage(args);
        stComm.decodeRequest(reqMessage); // Decode the request

        std::string plid = std::to_string(stComm._plid);

        bool hasGame = DB.hasOngoingGame(plid); // check if theres an ongoing game
        if (hasGame)
        { // send text with current game summary
            DB.getCurrentGameData(plid, stComm._Fname,
                                  stComm._Fsize, stComm._Fdata);
            stComm._status = "ACT";
        }
        else if (DB.hasGames(plid)) // if no ongoing game, check if there was ever a game
        {                           // send text with most recent game
            DB.getMostRecentGameData(plid, stComm._Fname,
                                     stComm._Fsize, stComm._Fdata);
            stComm._status = "FIN";
        }
        else
            stComm._status = "NOK";
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        stComm._status = "ERR";
    }
    response = stComm.encodeResponse(); // Encode the response
    if (receiver.isverbose())
    {
        std::cout << "Show Trials Request by: " << stComm._plid << std::endl;
        std::cout << "Response: " << response;
    }
    return;
}

void ScoreboardCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    SCORELIST list;

    GamedataManager DB = receiver._DB;
    ScoreboardCommunication sbComm;

    try
    {
        StreamMessage reqMessage(args);
        sbComm.decodeRequest(reqMessage); // Decode the request

        int nscores = findTopScores(&list);
        if (!nscores)
            sbComm._status = "EMPTY";

        else
        {
            DB.formatScoreboard(&list, sbComm._Fname, sbComm._Fsize, sbComm._Fdata, nscores);
            sbComm._status = "OK";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        sbComm._status = "ERR";
    }
    response = sbComm.encodeResponse();
    if (receiver.isverbose())
    {
        std::cout << "ScoreBoard Request" << std::endl;
        std::cout << "Response: " << response;
    }

    return;
}

void QuitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;

    QuitCommunication quitComm;
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
        }
        else
            quitComm._status = "NOK";
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, set the status to ERR
        quitComm._status = "ERR";
    }
    response = quitComm.encodeResponse(); // Encode the response
    if (receiver.isverbose())
    {
        std::cout << "Quit Request by: " << quitComm._plid << std::endl;
        std::cout << "Response: " << response;
    }
    return;
}

void ExitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;

    QuitCommunication exitComm;

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
        }
        else
            exitComm._status = "NOK";
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, set the status to ERR
        exitComm._status = "ERR";
    }
    response = exitComm.encodeResponse(); // Encode the response
    if (receiver.isverbose())
    {
        std::cout << "Exit Request by: " << exitComm._plid << std::endl;
        std::cout << "Response: " << response;
    }
    return;
}

void DebugCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;

    DebugCommunication dbgComm;

    try
    {
        StreamMessage reqMessage(args);
        dbgComm.decodeRequest(reqMessage); // Decode the request
        std::string plid = std::to_string(dbgComm._plid);

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(plid);

        if (!hasGame || (hasGame && DB.gameShouldEnd(plid)))
        {
            if (hasGame && DB.gameShouldEnd(plid))
                // Previous game has timed out. Starting a new game.
                DB.gameTimeout(plid);

            std::string current_datetime = currentDateTime();
            time_t time_s = time(NULL);
            DB.createGame(std::to_string(dbgComm._plid), 'D', removeSpaces(dbgComm._key),
                          dbgComm._time, current_datetime, time_s);
            dbgComm._status = "OK";
        }
        else
            dbgComm._status = "NOK";
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        dbgComm._status = "ERR";
    }
    response = dbgComm.encodeResponse(); // Encode the response
    if (receiver.isverbose())
    {
        std::cout << "Debug Request by: " << dbgComm._plid << std::endl;
        std::cout << "Response: " << response;
    }
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
