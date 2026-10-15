
#include <sourcemod>
#include <distbugfix>

#pragma newdecls required
#pragma semicolon 1

#define NOT_SHOWN "not shown"

char g_jumpTypes[JumpType][] = {
	"NONE",
	"LJ",
	"WJ",
	"LAJ",
	"BH",
	"CBH",
};

char g_jumpDirString[JumpDir][] = {
	"Forwards",
	"Backwards",
	"Sideways",
	"Sideways"
};

// distbug's g_szStrafeType
char g_strafeTypeSymbols[DistbugStrafeType][] = {
	"$", // DISTBUG_STRAFETYPE_OVERLAP
	".", // DISTBUG_STRAFETYPE_NONE
	
	"█", // DISTBUG_STRAFETYPE_LEFT
	"#", // DISTBUG_STRAFETYPE_OVERLAP_LEFT
	"H", // DISTBUG_STRAFETYPE_NONE_LEFT
	
	"█", // DISTBUG_STRAFETYPE_RIGHT
	"#", // DISTBUG_STRAFETYPE_OVERLAP_RIGHT
	"H", // DISTBUG_STRAFETYPE_NONE_RIGHT
};

public Plugin myinfo =
{
	name = "Distbug forward listener",
	author = "BadServers",
	description = "Prints Distbug_OnJumpLanded's payload the way distbug prints a jump",
	version = DISTBUG_VERSION,
	url = "https://github.com/BadServersNet/sm-distbug"
};

public void Distbug_OnJumpLanded(int client, DistbugJump jump, DistbugStrafes strafes)
{
	char block[32] = "Block: "...NOT_SHOWN;
	if (jump.blockShown)
	{
		FormatEx(block, sizeof(block), "Block: %i", RoundFloat(jump.block));
	}
	
	char edge[32] = "Edge: "...NOT_SHOWN;
	if (jump.edgeShown)
	{
		FormatEx(edge, sizeof(edge), "Edge: %.4f", jump.edge);
	}
	
	char landEdge[32] = "Land Edge: "...NOT_SHOWN;
	if (jump.landEdgeShown)
	{
		FormatEx(landEdge, sizeof(landEdge), "Land Edge: %.4f", jump.landEdge);
	}
	
	char fog[32] = "FOG: "...NOT_SHOWN;
	if (jump.fogShown)
	{
		FormatEx(fog, sizeof(fog), "FOG: %i", jump.fog);
	}
	
	char stamina[32] = "Stamina: "...NOT_SHOWN;
	if (jump.staminaShown)
	{
		FormatEx(stamina, sizeof(stamina), "Stamina: %.1f", jump.stamina);
	}
	
	char offset[32] = "Ground offset: "...NOT_SHOWN;
	if (jump.groundOffsetShown)
	{
		FormatEx(offset, sizeof(offset), "Ground offset: %.4f", jump.groundOffset);
	}
	
	// distbug prints "Fwd: No" for a release that isn't shown. the raw ticks are added here to check them
	char fwdRelease[48];
	if (!jump.fwdReleaseShown)
	{
		FormatEx(fwdRelease, sizeof(fwdRelease), "Fwd: "...NOT_SHOWN..." (%i)", jump.fwdRelease);
	}
	else if (jump.fwdRelease > 0)
	{
		FormatEx(fwdRelease, sizeof(fwdRelease), "Fwd: +%i", jump.fwdRelease);
	}
	else
	{
		FormatEx(fwdRelease, sizeof(fwdRelease), "Fwd: %i", jump.fwdRelease);
	}
	
	PrintToConsole(client, "[LISTENER] cmd tick: %i | server tick: %i", jump.cmdTick, jump.serverTick);
	PrintToConsole(client, "[LISTENER] %s: %.5f [%s | %s | Veer: %.4f | %s | Sync: %.2f | Max: %.3f]\n"...\
						   "[%s | Pre: %.4f | OL/DA: %i/%i | Jumpoff Angle: %.3f | Airpath: %.4f]\n"...\
						   "[Strafes: %i | Airtime: %i | Jump Direction: %s | %s | Height: %.4f | %s | %s]",
		g_jumpTypes[jump.jumpType],
		jump.distance,
		block,
		edge,
		jump.veer,
		fwdRelease,
		jump.sync,
		jump.maxspeed,
		
		landEdge,
		jump.prespeed,
		jump.overlap,
		jump.deadair,
		jump.jumpoffAngle,
		jump.airpath,
		
		jump.strafes,
		jump.airtime,
		g_jumpDirString[jump.jumpDir],
		fog,
		jump.height,
		offset,
		stamina
	);
	
	PrintToConsole(client, " #.  Sync    Gain   Loss   Max  Air  OL  DA  AvgGain  Avg efficiency, (max efficiency)");
	for (int strafe; strafe < strafes.count; strafe++)
	{
		PrintToConsole(client, "%2i. %5.1f%% %6.2f %6.2f  %5.1f %3i %3i %3i  %3.2f     %3i%% (%3i%%)",
			strafe + 1,
			strafes.sync[strafe],
			strafes.gain[strafe],
			strafes.loss[strafe],
			strafes.max[strafe],
			strafes.airtime[strafe],
			strafes.overlap[strafe],
			strafes.deadair[strafe],
			strafes.avgGain[strafe],
			RoundFloat(strafes.avgEfficiency[strafe]),
			RoundFloat(strafes.maxEfficiency[strafe])
		);
	}
	
	char graphLeft[DISTBUG_MAX_GRAPH_TICKS * 3 + 1];
	char graphRight[DISTBUG_MAX_GRAPH_TICKS * 3 + 1];
	BuildStrafeGraph(jump, true, graphLeft, sizeof(graphLeft));
	BuildStrafeGraph(jump, false, graphRight, sizeof(graphRight));
	PrintToConsole(client, "\n[LISTENER] Strafe keys:\nL: %s\nR: %s", graphLeft, graphRight);
	
	int expectedLength = jump.airtime < DISTBUG_MAX_GRAPH_TICKS ? jump.airtime : DISTBUG_MAX_GRAPH_TICKS;
	if (jump.graphLength != expectedLength)
	{
		PrintToConsole(client, "[LISTENER] graph length %i doesn't match airtime %i capped at %i", jump.graphLength, jump.airtime, DISTBUG_MAX_GRAPH_TICKS);
	}
}

// distbug's GetDirectionalStrafeType
DistbugStrafeType GetDirectionalStrafeType(DistbugStrafeType type, bool left)
{
	if (left)
	{
		if (type == DISTBUG_STRAFETYPE_RIGHT || type == DISTBUG_STRAFETYPE_NONE_RIGHT || type == DISTBUG_STRAFETYPE_OVERLAP_RIGHT)
		{
			return DISTBUG_STRAFETYPE_NONE;
		}
		return type;
	}
	if (type == DISTBUG_STRAFETYPE_LEFT || type == DISTBUG_STRAFETYPE_NONE_LEFT || type == DISTBUG_STRAFETYPE_OVERLAP_LEFT)
	{
		return DISTBUG_STRAFETYPE_NONE;
	}
	return type;
}

// the console row of distbug's BuildStrafeGraph
void BuildStrafeGraph(DistbugJump jump, bool left, char[] output, int length)
{
	int index;
	for (int i; i < jump.graphLength && i < DISTBUG_MAX_GRAPH_TICKS; i++)
	{
		index += strcopy(output[index], length - index, g_strafeTypeSymbols[GetDirectionalStrafeType(jump.graph[i], left)]);
	}
}
