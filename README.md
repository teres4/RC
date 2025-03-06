# RC

* This project implements a simplified Master Mind game using socket programming. It consists of:

    Game Server (GS): Manages games, enforces rules, and handles player requests over UDP/TCP.
    Player Application: Allows players to start/join games, submit guesses, view past attempts, and check the scoreboard.

* Features:

    Multiplayer support with unique player IDs (IST 6-digit number).
    Secret key: 4 colors chosen from {R, G, B, Y, O, P}, which must be guessed.
    Players have a limited time and 8 attempts to guess the key.
    Supports debug mode for testing with predefined secret keys.
    Uses UDP for gameplay and TCP for scoreboard/history retrieval.
