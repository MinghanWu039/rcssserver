// -*-c++-*-

#include "embeddedsimulator.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE( _core, module )
{
    module.doc() = "Python bindings for the embedded RCSSServer simulator.\n"
                   "Configuration still comes from ~/.rcssserver/server.conf and remains process-global.";

    py::enum_< Side >( module, "Side" )
        .value( "LEFT", LEFT )
        .value( "NEUTRAL", NEUTRAL )
        .value( "RIGHT", RIGHT )
        .export_values();

    py::enum_< PlayMode >( module, "PlayMode" )
        .value( "PM_Null", PM_Null )
        .value( "PM_BeforeKickOff", PM_BeforeKickOff )
        .value( "PM_TimeOver", PM_TimeOver )
        .value( "PM_PlayOn", PM_PlayOn )
        .value( "PM_KickOff_Left", PM_KickOff_Left )
        .value( "PM_KickOff_Right", PM_KickOff_Right )
        .value( "PM_KickIn_Left", PM_KickIn_Left )
        .value( "PM_KickIn_Right", PM_KickIn_Right )
        .value( "PM_FreeKick_Left", PM_FreeKick_Left )
        .value( "PM_FreeKick_Right", PM_FreeKick_Right )
        .value( "PM_CornerKick_Left", PM_CornerKick_Left )
        .value( "PM_CornerKick_Right", PM_CornerKick_Right )
        .value( "PM_GoalKick_Left", PM_GoalKick_Left )
        .value( "PM_GoalKick_Right", PM_GoalKick_Right )
        .value( "PM_AfterGoal_Left", PM_AfterGoal_Left )
        .value( "PM_AfterGoal_Right", PM_AfterGoal_Right )
        .value( "PM_Drop_Ball", PM_Drop_Ball )
        .value( "PM_OffSide_Left", PM_OffSide_Left )
        .value( "PM_OffSide_Right", PM_OffSide_Right )
        .value( "PM_PK_Left", PM_PK_Left )
        .value( "PM_PK_Right", PM_PK_Right )
        .value( "PM_FirstHalfOver", PM_FirstHalfOver )
        .value( "PM_Pause", PM_Pause )
        .value( "PM_Human", PM_Human )
        .value( "PM_Foul_Charge_Left", PM_Foul_Charge_Left )
        .value( "PM_Foul_Charge_Right", PM_Foul_Charge_Right )
        .value( "PM_Foul_Push_Left", PM_Foul_Push_Left )
        .value( "PM_Foul_Push_Right", PM_Foul_Push_Right )
        .value( "PM_Foul_MultipleAttacker_Left", PM_Foul_MultipleAttacker_Left )
        .value( "PM_Foul_MultipleAttacker_Right", PM_Foul_MultipleAttacker_Right )
        .value( "PM_Foul_BallOut_Left", PM_Foul_BallOut_Left )
        .value( "PM_Foul_BallOut_Right", PM_Foul_BallOut_Right )
        .value( "PM_Back_Pass_Left", PM_Back_Pass_Left )
        .value( "PM_Back_Pass_Right", PM_Back_Pass_Right )
        .value( "PM_Free_Kick_Fault_Left", PM_Free_Kick_Fault_Left )
        .value( "PM_Free_Kick_Fault_Right", PM_Free_Kick_Fault_Right )
        .value( "PM_CatchFault_Left", PM_CatchFault_Left )
        .value( "PM_CatchFault_Right", PM_CatchFault_Right )
        .value( "PM_IndFreeKick_Left", PM_IndFreeKick_Left )
        .value( "PM_IndFreeKick_Right", PM_IndFreeKick_Right )
        .value( "PM_PenaltySetup_Left", PM_PenaltySetup_Left )
        .value( "PM_PenaltySetup_Right", PM_PenaltySetup_Right )
        .value( "PM_PenaltyReady_Left", PM_PenaltyReady_Left )
        .value( "PM_PenaltyReady_Right", PM_PenaltyReady_Right )
        .value( "PM_PenaltyTaken_Left", PM_PenaltyTaken_Left )
        .value( "PM_PenaltyTaken_Right", PM_PenaltyTaken_Right )
        .value( "PM_PenaltyMiss_Left", PM_PenaltyMiss_Left )
        .value( "PM_PenaltyMiss_Right", PM_PenaltyMiss_Right )
        .value( "PM_PenaltyScore_Left", PM_PenaltyScore_Left )
        .value( "PM_PenaltyScore_Right", PM_PenaltyScore_Right )
        .value( "PM_Illegal_Defense_Left", PM_Illegal_Defense_Left )
        .value( "PM_Illegal_Defense_Right", PM_Illegal_Defense_Right )
        .export_values();

    py::class_< PVector >( module, "Vector2" )
        .def_property_readonly( "x",
                                []( const PVector & self )
                                {
                                    return self.x;
                                } )
        .def_property_readonly( "y",
                                []( const PVector & self )
                                {
                                    return self.y;
                                } );

    py::class_< EmbeddedPlayerCommand >( module, "PlayerCommand" )
        .def( py::init< Side, int, std::string >(),
              py::arg( "side" ),
              py::arg( "unum" ),
              py::arg( "command" ) )
        .def_readonly( "side", &EmbeddedPlayerCommand::side )
        .def_readonly( "unum", &EmbeddedPlayerCommand::unum )
        .def_readonly( "command", &EmbeddedPlayerCommand::command );

    py::class_< EmbeddedBallState >( module, "BallState" )
        .def_property_readonly( "pos",
                                []( const EmbeddedBallState & self )
                                {
                                    return self.pos;
                                } )
        .def_property_readonly( "vel",
                                []( const EmbeddedBallState & self )
                                {
                                    return self.vel;
                                } );

    py::class_< EmbeddedPlayerState >( module, "PlayerState" )
        .def_readonly( "side", &EmbeddedPlayerState::side )
        .def_readonly( "unum", &EmbeddedPlayerState::unum )
        .def_readonly( "enabled", &EmbeddedPlayerState::enabled )
        .def_readonly( "goalie", &EmbeddedPlayerState::goalie )
        .def_property_readonly( "pos",
                                []( const EmbeddedPlayerState & self )
                                {
                                    return self.pos;
                                } )
        .def_property_readonly( "vel",
                                []( const EmbeddedPlayerState & self )
                                {
                                    return self.vel;
                                } )
        .def_readonly( "body_angle", &EmbeddedPlayerState::body_angle )
        .def_readonly( "neck_angle", &EmbeddedPlayerState::neck_angle );

    py::class_< EmbeddedGameState >( module, "GameState" )
        .def_readonly( "time", &EmbeddedGameState::time )
        .def_readonly( "stoppage_time", &EmbeddedGameState::stoppage_time )
        .def_readonly( "playmode", &EmbeddedGameState::playmode )
        .def_readonly( "score_left", &EmbeddedGameState::score_left )
        .def_readonly( "score_right", &EmbeddedGameState::score_right )
        .def_property_readonly( "ball",
                                []( const EmbeddedGameState & self )
                                {
                                    return self.ball;
                                } )
        .def_property_readonly( "players",
                                []( const EmbeddedGameState & self )
                                {
                                    return self.players;
                                } );

    py::class_< EmbeddedSimulator >( module, "EmbeddedSimulator",
                                    "Embedded simulator wrapper using process-global server parameters." )
        .def( py::init<>() )
        .def( "init", &EmbeddedSimulator::init )
        .def( "set_team_name", &EmbeddedSimulator::setTeamName,
              py::arg( "side" ),
              py::arg( "name" ) )
        .def( "enable_player", &EmbeddedSimulator::enablePlayer,
              py::arg( "side" ),
              py::arg( "unum" ),
              py::arg( "goalie" ) = false,
              py::arg( "version" ) = 18.0 )
        .def( "start_match", &EmbeddedSimulator::startMatch )
        .def( "step", &EmbeddedSimulator::step, py::arg( "commands" ) )
        .def( "snapshot", &EmbeddedSimulator::snapshot );
}
