from pathlib import Path
import sys

import rcssserver_embedded as simmod

sim = simmod.EmbeddedSimulator()
sim.init()
sim.set_team_name(simmod.Side.LEFT, "L")
sim.set_team_name(simmod.Side.RIGHT, "R")
sim.enable_player(simmod.Side.LEFT, 1)
sim.enable_player(simmod.Side.RIGHT, 1)
state = sim.step([
    simmod.PlayerCommand(simmod.Side.LEFT, 1, "(move -10 0)"),
    simmod.PlayerCommand(simmod.Side.RIGHT, 1, "(move 10 0)"),
])
