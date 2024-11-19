#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <vector>

#include "player.hpp"


class CommandHandler {
    protected:
    /**
     * @brief Constructs a CommandHandler object with the specified properties.
     *
     * @param name The name of the command.
     * @param args The args of the command.
     * @param usage The description of the command.
     */
    
        CommandHandler(const char* __name, const std::optional<const char*> __args,
                        const char* __usage)
            :   name{__name},
                args{__args},
                usage{__usage} {}
                
    public:
        /**
         * @brief Handles the command with the given arguments.
         * should be implemented by derived classes
        */

        // virtual void handle(std::string args, Player &receiver) = 0;

        virtual void handle(std::string args) = 0;


        const char* name;                           // The name of the command
        const std::optional<const char*> args;      // The args of the command
        const std::optional<const char*> usage;     // The description of the command

        void readCommand(std::string command, Client &state);
    


};



class StartCommand : public CommandHandler {
    void handle(std::string args);

    public:
    StartCommand() : CommandHandler("start", "PLID", "Start a new game") {}
};





/**
 * @brief Exception class for command-related errors.
 *
 * This class is derived from std::runtime_error and is used to represent
 * exceptions that occur during command execution.
 */
class CommandException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a CommandException object with the specified reason.
     *
     * @param reason The reason for the exception.
     */
    CommandException(std::string reason)
        : std::runtime_error("ERROR: " + reason){};
};

/**
 * @brief Exception thrown when invalid arguments are provided for a command.
 */
class CommandArgumentException : public CommandException {
  public:
    /**
     * @brief Constructs a CommandArgumentException with the specified usage
     * information.
     *
     * @param usage The usage information for the command.
     */
    CommandArgumentException(std::string usage)
        : CommandException("Invalid arguments.\nUsage: " + usage){};
};

/**
 * @brief Exception thrown when an unknown command is encountered.
 */
class UnknownCommandException : public CommandException {
  public:
    UnknownCommandException() : CommandException("Unknown Command."){};
};

#endif

