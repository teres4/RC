#include "commands.hpp"


// void CommandManager::registerCommand(std::shared_ptr<CommandHandler> command)
// {
//     try
//     {
//         _handlers.insert({command->_name, command});
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << e.what() << '\n';
//     }
//     return;
// }

// void CommandManager::registerAllCommands()
// {
//     this->registerCommand(std::make_shared<StartCommand>());
//     this->registerCommand(std::make_shared<TryCommand>());
//     this->registerCommand(std::make_shared<QuitCommand>());
//     this->registerCommand(std::make_shared<ShowTrialsCommand>());
//     this->registerCommand(std::make_shared<ScoreboardCommand>());
//     this->registerCommand(std::make_shared<ExitCommand>());
//     this->registerCommand(std::make_shared<DebugCommand>());
// }



// std::string CommandManager::handleCommand(std::string message, Server &receiver)
// {
//     std::vector<std::string> command_split = split_command(message);

//     if (command_split.size() == 0)
//         return;

//     std::string commandName = command_split[0]; // The name of the command

//     if (commandName.length() == 0)
//         return;

//     auto handler = _handlers.find(commandName); // find handler of the command

//     if (handler == _handlers.end())
//     { // If the handler does not exist
//         handler = _handlers.find(commandName);
//         if (handler == _handlers.end())
//         {
//             std::cout << "Invalid command: " << commandName << std::endl;
//             return;
//         }
//     }

//     std::string args = "";
//     if (command_split.size() > 1)
//         args = command_split[1];

//     handler->second->handle(args, receiver);
// }


// void StartCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();

// //     StartCommunication startComm; 
// //     std::string result;

// //     try {
// //         startComm.decodeRequest(message);  // Decode the request

// //         if (!receiver._database->loginUser(startComm._uid, startComm._password)) {  // Check if the user is registered
// //             startComm._status = "OK";
// //             result = "Login Sucessful";
// //         } else {  // If the user is not registered, register the user
// //             startComm._status = "REG";
// //             result = "Registration Sucessful";
// //         }
// //     } catch (LoginException const &e) {  // If the login fails, set the status to NOK
// //         startComm._status = "NOK";
// //         result = "Login Failed";
// //     } catch (ProtocolException const
// //                  &e) {  // If the protocol is not valid, set the status to ERR
// //         startComm._status = "ERR";
// //         result = "Protocol Error";
// //     }
// //     response = startComm.encodeResponse();  // Encode the response
// //     receiver.log(Message::ServerRequestDetails(loginCommunication._uid, "Login",
// //                                                result));  // Display the message


// }


// void TryCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();

// }

// void ShowTrialsCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();

// }

// void ScoreboardCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose
//     std::cout << args.get() << response << receiver.isverbose();

// }


// void QuitCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();

// }


// void ExitCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();

// }


// void DebugCommand::handle(MessageSource &args, std::string &response, Server &receiver){
// //     // TODO check verbose

//     std::cout << args.get() << response << receiver.isverbose();
// }