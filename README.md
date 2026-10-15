# Distbug

Distbug is a SourceMod plugin for Counter-Strike: Global Offensive that fixes the long-jump distance bug and reports detailed jump and strafe statistics.

## Features

- Measures long jumps, weird jumps, ladder jumps, bunnyhops, and crouched bunnyhops.
- Reports distance, edge, prestrafe, maximum speed, synchronization, airtime, overlap, dead air, veer, and strafe efficiency.
- Provides chat, console, HUD graph, jump beam, and veer beam output.
- Saves each player's display preferences with SourceMod client preferences.

## Installation

Download the latest release archive and extract it into the game server's `csgo` directory. The archive places the compiled plugin in `addons/sourcemod/plugins` and the source and bundled includes in `addons/sourcemod/scripting`.

SourceMod creates `cfg/sourcemod/distbugfix.cfg` after the plugin first loads. Use that file to adjust the supported distance ranges for each jump type.

Distbug prints stats cut off at the decimals it shows, so an edge of 1.239965 prints as `1.2399`. Set `distbug_round_output 1` to print them rounded instead, as the BadServers GOKZ fork stores them (`1.2400`). SourceMod doesn't add new cvars to an existing `distbugfix.cfg`, so add the line to that file or to `server.cfg` yourself.

## Commands

| Command | Description |
| --- | --- |
| `sm_distbug` | Toggle Distbug. |
| `sm_distbugversion` | Print the installed version. |
| `sm_distbugbeam` | Toggle the jump beam. |
| `sm_distbugveerbeam` | Toggle the veer beam. |
| `sm_distbughudgraph` | Toggle the HUD strafe graph. |
| `sm_strafestats` | Toggle detailed strafe statistics. |
| `sm_distbugstrafegraph` | Toggle the console strafe graph. |
| `sm_distbugadvchat` | Toggle advanced chat statistics. |
| `sm_distbughelp` | Print the command list to the console. |

## For plugin developers

Distbug registers the library `distbugfix` and calls one forward, declared in `include/distbugfix.inc`:

```sourcepawn
forward void Distbug_OnJumpLanded(int client, DistbugJump jump, DistbugStrafes strafes);
```

That file is the whole public API. The plugin's own definitions are in `distbugfix/internal.inc`, which other plugins shouldn't include.

It is called once for each jump that Distbug prints as landed, after the output has been sent. `DistbugJump` holds the three console lines and `DistbugStrafes` holds the strafe table. The values aren't rounded, and the include gives the console format of each one. A stat that Distbug left out of its output, such as an edge of 32 or more, arrives with its `Shown` flag set to false.

The forward is not called for:

- players who have Distbug off. It is off until a player runs `sm_distbug`, and their jumps aren't tracked. The other toggles (strafe stats, graphs, beams, advanced chat) don't affect the forward.
- failstats.
- jumps outside the distance range for their type.
- jumps interrupted by a teleport.
- a jump the player toggled `sm_distbug` during.

`jump.graph` holds the strafe graph Distbug prints under "Strafe keys", one `DistbugStrafeType` per airborne tick, `jump.graphLength` entries long. The length is `jump.airtime`, unless the jump was airborne for more than `DISTBUG_MAX_GRAPH_TICKS` (150) ticks, where Distbug's graph stops too. Left and right are the sides Distbug prints for the jump's `jumpDir`, not raw A and D: on a backwards jump left is D, and a sideways jump's rows are W and S, in the order the include gives for each `jumpDir`. The include also lists the symbol each value prints as.

`jump.cmdTick` is the `tickcount` that `OnPlayerRunCmd` received for the command the player landed in. MovementAPI takes its landing tick from the same argument. `jump.serverTick` is `GetGameTickCount()` at the landing. They count different things, so compare each only with its own kind.

A plugin that only listens needs nothing to keep Distbug optional: it includes `distbugfix.inc` and declares the forward's function, which is never called while Distbug is unloaded. Use `LibraryExists(DISTBUG_LIBRARY)` to find out whether it is loaded.

`test/distbugfix_listener.sp` prints the forward's payload to the jumper's console in Distbug's layout, to compare the two by eye. It isn't part of the release archive. Compile it the same way as the plugin.

## Building

Compile `distbugfix.sp` with SourceMod 1.11 and add `include` to the compiler's include search path. All non-SourceMod includes required to build the plugin are bundled in this repository.

## Releases

Version tags follow Semantic Versioning. See [CHANGELOG.md](CHANGELOG.md) for the original release notes.

## License

This project is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE).
