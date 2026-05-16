// -*-c++-*-

#ifndef RCSSSERVER_EMBEDDED_SIMULATOR_H
#define RCSSSERVER_EMBEDDED_SIMULATOR_H

#include "../stadium.h"

#include <string>
#include <vector>

struct EmbeddedPlayerCommand {
    Side side;
    int unum;
    std::string command;
};

struct EmbeddedBallState {
    PVector pos;
    PVector vel;
};

struct EmbeddedPlayerState {
    Side side;
    int unum;
    bool enabled;
    bool goalie;
    PVector pos;
    PVector vel;
    double body_angle;
    double neck_angle;
};

struct EmbeddedGameState {
    int time = 0;
    int stoppage_time = 0;
    PlayMode playmode = PM_Null;
    int score_left = 0;
    int score_right = 0;
    EmbeddedBallState ball;
    std::vector< EmbeddedPlayerState > players;
};

class EmbeddedSimulator {
public:
    EmbeddedSimulator() = default;

    bool init();
    bool setTeamName( Side side, const std::string & name );
    bool enablePlayer( Side side,
                       int unum,
                       bool goalie = false,
                       double version = 18.0 );
    void startMatch();
    EmbeddedGameState step( const std::vector< EmbeddedPlayerCommand > & commands );
    EmbeddedGameState snapshot() const;

private:
    Stadium M_stadium;
    bool M_initialized = false;
};

#endif
