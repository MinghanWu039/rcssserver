// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "embeddedsimulator.h"

#include "../player.h"
#include "../serverparam.h"
#include "../team.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <vector>

namespace {

bool S_server_param_initialized = false;

bool
port_available( const int port )
{
    const int fd = ::socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 )
    {
        return false;
    }

    sockaddr_in addr;
    std::memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port = htons( static_cast< unsigned short >( port ) );

    const bool available = ( ::bind( fd,
                                     reinterpret_cast< const sockaddr * >( &addr ),
                                     sizeof( addr ) ) == 0 );
    ::close( fd );
    return available;
}

int
find_base_port()
{
    const int preferred_start = 20000 + ( static_cast< int >( ::getpid() ) % 10000 );
    const int preferred_end = 60000;

    for ( int port = preferred_start; port < preferred_end; port += 3 )
    {
        if ( port_available( port )
             && port_available( port + 1 )
             && port_available( port + 2 ) )
        {
            return port;
        }
    }

    return -1;
}

bool
init_server_params()
{
    if ( S_server_param_initialized )
    {
        return true;
    }

    const int base_port = find_base_port();
    if ( base_port < 0 )
    {
        return false;
    }

    const std::vector< std::string > args = {
        "embeddedsimulator",
        "--server::port=" + std::to_string( base_port ),
        "--server::coach_port=" + std::to_string( base_port + 1 ),
        "--server::olcoach_port=" + std::to_string( base_port + 2 ),
        "--server::text_logging=0",
        "--server::game_logging=0"
    };

    std::vector< const char * > argv;
    argv.reserve( args.size() );
    for ( const std::string & arg : args )
    {
        argv.push_back( arg.c_str() );
    }

    S_server_param_initialized = ServerParam::init( static_cast< int >( argv.size() ),
                                                    argv.data() );
    return S_server_param_initialized;
}

} // unnamed namespace

bool
EmbeddedSimulator::init()
{
    if ( M_initialized )
    {
        return true;
    }

    if ( ! init_server_params() )
    {
        return false;
    }

    M_initialized = M_stadium.init();
    return M_initialized;
}

bool
EmbeddedSimulator::setTeamName( Side side, const std::string & name )
{
    return M_initialized && M_stadium.setEmbeddedTeamName( side, name );
}

bool
EmbeddedSimulator::enablePlayer( Side side,
                                 int unum,
                                 bool goalie,
                                 double version )
{
    return M_initialized && M_stadium.enableEmbeddedPlayer( side, unum, version, goalie );
}

void
EmbeddedSimulator::startMatch()
{
    if ( M_initialized )
    {
        M_stadium.startEmbeddedKickoff();
    }
}

EmbeddedGameState
EmbeddedSimulator::step( const std::vector< EmbeddedPlayerCommand > & commands )
{
    if ( ! M_initialized )
    {
        return snapshot();
    }

    for ( const EmbeddedPlayerCommand & player_command : commands )
    {
        M_stadium.applyEmbeddedPlayerCommand( player_command.side,
                                              player_command.unum,
                                              player_command.command );
    }

    M_stadium.newSimulatorStep();
    return snapshot();
}

EmbeddedGameState
EmbeddedSimulator::snapshot() const
{
    EmbeddedGameState state;
    if ( ! M_initialized )
    {
        return state;
    }

    state.time = M_stadium.time();
    state.stoppage_time = M_stadium.stoppageTime();
    state.playmode = M_stadium.playmode();
    state.score_left = M_stadium.teamLeft().point();
    state.score_right = M_stadium.teamRight().point();
    state.ball.pos = M_stadium.ball().pos();
    state.ball.vel = M_stadium.ball().vel();

    state.players.reserve( M_stadium.players().size() );
    for ( const Player * player : M_stadium.players() )
    {
        if ( ! player )
        {
            continue;
        }

        state.players.push_back( EmbeddedPlayerState{
            player->side(),
            player->unum(),
            player->isEnabled(),
            player->isGoalie(),
            player->pos(),
            player->vel(),
            player->angleBodyCommitted(),
            player->angleNeckCommitted()
        } );
    }

    return state;
}
