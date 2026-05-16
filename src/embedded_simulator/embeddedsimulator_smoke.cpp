// -*-c++-*-

#include "embeddedsimulator.h"
#include "../serverparam.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#if RCSSSERVER_HAVE_MATPLOTLIB_CPP
#include <matplotlibcpp.h>
#endif

namespace {

const EmbeddedPlayerState *
find_player( const EmbeddedGameState & state,
             const Side side,
             const int unum )
{
    for ( const EmbeddedPlayerState & player : state.players )
    {
        if ( player.side == side && player.unum == unum )
        {
            return &player;
        }
    }

    return nullptr;
}

bool
has_ball_velocity( const EmbeddedGameState & state )
{
    return std::fabs( state.ball.vel.x ) > 1.0e-9
        || std::fabs( state.ball.vel.y ) > 1.0e-9;
}

std::string
plot_diagram( const EmbeddedGameState & state,
              const std::string & tag )
{
    const std::string file = tag + ".png";

#if RCSSSERVER_HAVE_MATPLOTLIB_CPP
    namespace plt = matplotlibcpp;

    const double half_length = ServerParam::PITCH_LENGTH * 0.5;
    const double half_width = ServerParam::PITCH_WIDTH * 0.5;

    plt::clf();
    plt::figure_size( 1200, 800 );
    plt::axis( "equal" );
    plt::xlim( -half_length, half_length );
    plt::ylim( -half_width, half_width );

    plt::plot( { -half_length, half_length, half_length, -half_length, -half_length },
               { -half_width, -half_width, half_width, half_width, -half_width },
               { { "color", "black" } } );
    plt::plot( { 0.0, 0.0 }, { -half_width, half_width }, { { "color", "black" } } );
    plt::scatter( std::vector<double>{ state.ball.pos.x },
                  std::vector<double>{ state.ball.pos.y },
                  40.0,
                  { { "c", "orange" } } );

    for ( const EmbeddedPlayerState & player : state.players )
    {
        if ( ! player.enabled )
        {
            continue;
        }

        const double arrow_len = 2.0;
        const double dx = arrow_len * std::cos( player.body_angle );
        const double dy = arrow_len * std::sin( player.body_angle );

        const char * color = player.side == LEFT ? "blue" : "red";
        plt::plot( std::vector<double>{ player.pos.x, player.pos.x + dx },
                   std::vector<double>{ player.pos.y, player.pos.y + dy },
                   { { "color", color } } );
        plt::scatter( std::vector<double>{ player.pos.x },
                      std::vector<double>{ player.pos.y },
                      25.0,
                      { { "c", color } } );
    }

    plt::title( tag );
    plt::save( file );
#else
    static_cast< void >( state );
#endif

    return file;
}

void
print_state( const EmbeddedGameState & state,
             const std::string & tag )
{
    std::cout << "gamestate"
              << " time=" << state.time
              << " stoppage=" << state.stoppage_time
              << " playmode=" << state.playmode
              << " score=(" << state.score_left << ", " << state.score_right << ")"
              << " ball_pos=(" << state.ball.pos.x << ", " << state.ball.pos.y << ")"
              << " ball_vel=(" << state.ball.vel.x << ", " << state.ball.vel.y << ')'
              << '\n';

    for ( const EmbeddedPlayerState & player : state.players )
    {
        if ( ! player.enabled )
        {
            continue;
        }

        std::cout << "  player"
                  << " side=" << player.side
                  << " unum=" << player.unum
                  << " goalie=" << player.goalie
                  << " pos=(" << player.pos.x << ", " << player.pos.y << ")"
                  << " vel=(" << player.vel.x << ", " << player.vel.y << ')'
                  << " body_angle=" << player.body_angle
                  << " neck_angle=" << player.neck_angle
                  << '\n';
    }

    const std::string plot_file = plot_diagram( state, tag );
#if RCSSSERVER_HAVE_MATPLOTLIB_CPP
    std::cout << "plot_file=" << plot_file << '\n';
#else
    std::cout << "plot_skipped=matplotlib-cpp_not_available"
              << " requested_file=" << plot_file
              << '\n';
#endif
}

void
print_commands( const std::vector< EmbeddedPlayerCommand > & commands )
{
    std::cout << "commands\n";
    for ( const EmbeddedPlayerCommand & command : commands )
    {
        std::cout << "  side=" << command.side
                  << " unum=" << command.unum
                  << " cmd=" << command.command
                  << '\n';
    }
}

} // unnamed namespace

int
main()
{
    {
        EmbeddedSimulator sim;
        if ( ! sim.init() )
        {
            std::cerr << "embedded simulator init failed\n";
            return 1;
        }

        if ( ! sim.setTeamName( LEFT, "LeftEmbedded" )
             || ! sim.setTeamName( RIGHT, "RightEmbedded" ) )
        {
            std::cerr << "team setup failed\n";
            return 2;
        }

        if ( ! sim.enablePlayer( LEFT, 1 )
             || ! sim.enablePlayer( RIGHT, 1 ) )
        {
            std::cerr << "player enable failed\n";
            return 3;
        }

        EmbeddedGameState before_kickoff
            = sim.step( {
                    { LEFT, 1, "(move 10 10)" },
                    { RIGHT, 1, "(move 10 10)" }
                } );

        const EmbeddedPlayerState * left_before = find_player( before_kickoff, LEFT, 1 );
        const EmbeddedPlayerState * right_before = find_player( before_kickoff, RIGHT, 1 );
        if ( ! left_before || ! right_before )
        {
            std::cerr << "snapshot missing players\n";
            return 4;
        }

        sim.startMatch();
        EmbeddedGameState kickoff_state = sim.snapshot();
        if ( kickoff_state.playmode != PM_KickOff_Left
             && kickoff_state.playmode != PM_KickOff_Right )
        {
            std::cerr << "kickoff state not reached\n";
            return 5;
        }

        EmbeddedGameState after_commands
            = sim.step( {
                    { LEFT, 1, "(kick 100 0)" },
                    { RIGHT, 1, "(turn 30)" }
                } );

        if ( after_commands.time < 1 )
        {
            std::cerr << "time did not advance\n";
            return 6;
        }

        const EmbeddedPlayerState * left_after_commands = find_player( after_commands, LEFT, 1 );
        if ( ! left_after_commands )
        {
            std::cerr << "left player missing after command step\n";
            return 7;
        }

        EmbeddedGameState after_dash = sim.step( { { LEFT, 1, "(dash 50)" } } );
        const EmbeddedPlayerState * left_after_dash = find_player( after_dash, LEFT, 1 );
        if ( ! left_after_dash )
        {
            std::cerr << "left player missing after dash\n";
            return 8;
        }

        if ( left_after_dash->pos == left_after_commands->pos
             && left_after_dash->vel == left_after_commands->vel
             && ! has_ball_velocity( after_commands ) )
        {
            std::cerr << "commands did not affect state\n";
            return 9;
        }
    }

    EmbeddedSimulator square_sim;
    if ( ! square_sim.init() )
    {
        std::cerr << "square test init failed\n";
        return 10;
    }

    if ( ! square_sim.setTeamName( LEFT, "LeftSquare" )
         || ! square_sim.setTeamName( RIGHT, "RightSquare" ) )
    {
        std::cerr << "square test team setup failed\n";
        return 11;
    }

    if ( ! square_sim.enablePlayer( LEFT, 1 )
         || ! square_sim.enablePlayer( RIGHT, 1 ) )
    {
        std::cerr << "square test player enable failed\n";
        return 12;
    }

    square_sim.step( {
                    { LEFT, 1, "(move 10 10)" },
                    { RIGHT, 1, "(move 10 10)" }
                } );

    std::vector< std::vector< EmbeddedPlayerCommand > > square_steps;
    square_steps.reserve( 17 );
    square_steps.push_back( {
            { LEFT, 1, "(dash 80)" },
            { RIGHT, 1, "(dash 80)" }
        } );
    for ( int edge = 0; edge < 4; ++edge )
    {
        square_steps.push_back( {
                { LEFT, 1, "(dash 80)" },
                { RIGHT, 1, "(dash 80)" }
            } );
        square_steps.push_back( {
                { LEFT, 1, "(dash 80)" },
                { RIGHT, 1, "(dash 80)" }
            } );
        square_steps.push_back( {
                { LEFT, 1, "(dash 80)" },
                { RIGHT, 1, "(dash 80)" }
            } );
        square_steps.push_back( {
                { LEFT, 1, "(turn -60)" },
                { RIGHT, 1, "(turn 60)" }
            } );
    }

    std::cout << "square test\n";
    square_sim.startMatch();
    for ( std::size_t i = 0; i < square_steps.size(); ++i )
    {
        std::cout << "iteration " << ( i + 1 ) << '\n';
        print_commands( square_steps[i] );
        char tag[64];
        std::snprintf( tag, sizeof( tag ), "square_step_%02zu", i + 1 );
        print_state( square_sim.step( square_steps[i] ), tag );
    }

    return 0;
}
