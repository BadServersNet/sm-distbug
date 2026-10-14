
#if defined _distbug_included
	#endinput
#endif
#define _distbug_included

#define DISTBUG_VERSION "2.1.0"

// library name, for LibraryExists and OnLibraryAdded
#define DISTBUG_LIBRARY "distbugfix"

#define DISTBUG_MAX_STRAFES     32
#define DISTBUG_MAX_FWD_RELEASE 16 // further than this either way is printed as "Fwd: No"

enum JumpType
{
	// unprintable jumptypes. only for tracking
	JUMPTYPE_NONE,
	
	JUMPTYPE_LJ,  // longjump
	JUMPTYPE_WJ,  // weirdjump
	JUMPTYPE_LAJ, // ladderjump
	JUMPTYPE_BH,  // bunnyhop
	JUMPTYPE_CBH, // crouched bunnyhop
};

enum JumpDir
{
	JUMPDIR_FORWARDS,  // printed as "Forwards"
	JUMPDIR_BACKWARDS, // printed as "Backwards"
	JUMPDIR_LEFT,      // printed as "Sideways"
	JUMPDIR_RIGHT,     // printed as "Sideways"
};

/**
 * A landed jump's stats, as Distbug prints them to the console.
 *
 * Values are not rounded. Each comment gives the console format, so rounding
 * a value that way gives the printed number.
 *
 * A stat with a *Shown flag is only printed when the flag is true. When it is
 * false the value is 0, except fwdRelease, which keeps its tick count.
 *
 * Fields are only ever added at the end, so a listener keeps working with a
 * newer Distbug. A listener needs a Distbug at least as new as the include it
 * was compiled with.
 */
enum struct DistbugJump
{
	int cmdTick;            // tickcount of the usercmd the player landed in (OnPlayerRunCmd's tickcount)
	int serverTick;         // GetGameTickCount() when the player landed
	JumpType jumpType;      // never JUMPTYPE_NONE
	JumpDir jumpDir;
	float distance;         // %.5f
	bool blockShown;
	float block;            // RoundFloat
	bool edgeShown;
	float edge;             // %.4f
	bool landEdgeShown;
	float landEdge;         // %.4f
	float veer;             // %.4f
	float airpath;          // %.4f
	float height;           // %.4f
	float jumpoffAngle;     // %.3f
	bool fwdReleaseShown;   // false when printed as "Fwd: No"
	int fwdRelease;         // ticks, negative is before the jump
	float sync;             // %.2f
	float prespeed;         // %.4f
	float maxspeed;         // %.3f
	int overlap;            // ticks
	int deadair;            // ticks
	int airtime;            // ticks
	int strafes;            // the "Strafes:" number
	bool fogShown;
	int fog;
	bool staminaShown;
	float stamina;          // %.1f
	bool groundOffsetShown;
	float groundOffset;     // %.4f
}

/**
 * The rows of the console strafe table. Only indexes below count are set.
 *
 * Fields are only ever added at the end and DISTBUG_MAX_STRAFES never changes.
 */
enum struct DistbugStrafes
{
	int count;                                 // number of rows, at most DISTBUG_MAX_STRAFES
	float sync[DISTBUG_MAX_STRAFES];           // %5.1f
	float gain[DISTBUG_MAX_STRAFES];           // %6.2f
	float loss[DISTBUG_MAX_STRAFES];           // %6.2f
	float max[DISTBUG_MAX_STRAFES];            // %5.1f
	int airtime[DISTBUG_MAX_STRAFES];          // ticks
	int overlap[DISTBUG_MAX_STRAFES];          // ticks
	int deadair[DISTBUG_MAX_STRAFES];          // ticks
	float avgGain[DISTBUG_MAX_STRAFES];        // %3.2f
	float avgEfficiency[DISTBUG_MAX_STRAFES];  // RoundFloat. NaN (0xffffffff) if the strafe has no samples
	float maxEfficiency[DISTBUG_MAX_STRAFES];  // RoundFloat
}

/**
 * Called when a jump has landed inside its type's distance range, after its
 * stats have been printed. Fires whichever outputs the player has toggled.
 *
 * Not called for players with Distbug off (sm_distbug, off by default): their
 * jumps aren't tracked. Not called for failstats, jumps outside the range or
 * jumps interrupted by a teleport.
 *
 * jump and strafes are copies. Changing them changes nothing.
 *
 * @param client     Jumper.
 * @param jump       Jump stats.
 * @param strafes    Strafe stats.
 */
forward void Distbug_OnJumpLanded(int client, DistbugJump jump, DistbugStrafes strafes);

// everything below is internal to distbugfix.sp and is not part of the API
#if !defined DISTBUG_INTERNAL
	#endinput
#endif

#define DISTBUG_CONFIG_NAME "distbugfix"

#define CHAT_PREFIX     "{d}[{l}GC{d}]"
#define CONSOLE_PREFIX  "[GC]"
#define CHAT_SPACER     " {d}|{g} "

#define MAX_STRAFES     DISTBUG_MAX_STRAFES
#define MAX_EDGE        32.0
#define MAX_JUMP_FRAMES	150 // for frame based arrays
#define MAX_BHOP_FRAMES	8

#define MAX_COOKIE_SIZE 32

enum
{
	SETTINGS_DISTBUG_ENABLED      = (1 << 0),
	SETTINGS_SHOW_VEER_BEAM       = (1 << 1),
	SETTINGS_SHOW_JUMP_BEAM       = (1 << 2),
	SETTINGS_SHOW_HUD_GRAPH       = (1 << 3),
	SETTINGS_DISABLE_STRAFE_STATS = (1 << 4),
	SETTINGS_DISABLE_STRAFE_GRAPH = (1 << 5),
	SETTINGS_ADV_CHAT_STATS       = (1 << 6),
}

enum StrafeType
{
	STRAFETYPE_OVERLAP,         // IN_MOVELEFT and IN_MOVERIGHT are overlapping and sidespeed is 0
	STRAFETYPE_NONE,            // IN_MOVELEFT and IN_MOVERIGHT are both not pressed and sidespeed is 0
	
	STRAFETYPE_LEFT,            // only IN_MOVELEFT is down and sidespeed isn't 0.
	STRAFETYPE_OVERLAP_LEFT,    // IN_MOVELEFT and IN_MOVERIGHT are overlapping, but sidespeed is smaller than 0 (not 0)
	STRAFETYPE_NONE_LEFT,       // IN_MOVELEFT and IN_MOVERIGHT are both not pressed and sidespeed is smaller than 0 (not 0)
	
	STRAFETYPE_RIGHT,           // only IN_MOVERIGHT is down and sidespeed isn't 0.
	STRAFETYPE_OVERLAP_RIGHT,   // IN_MOVELEFT and IN_MOVERIGHT are overlapping, but sidespeed is bigger than 0 (not 0)
	STRAFETYPE_NONE_RIGHT,      // IN_MOVELEFT and IN_MOVERIGHT are both not pressed and sidespeed is bigger than 0 (not 0)
};

enum JumpBeamColour
{
	JUMPBEAM_NEUTRAL, // speed stays the same
	JUMPBEAM_LOSS,    // speed loss
	JUMPBEAM_GAIN,    // speed gain
	JUMPBEAM_DUCK,    // duck key down
};

enum struct PlayerData
{
	int tickCount;
	int cmdTickCount; // tickcount of the newest usercmd, not comparable to tickCount
	int buttons;
	int lastButtons;
	int flags;
	int lastFlags;
	int framesOnGround;
	int framesInAir;
	MoveType movetype;
	MoveType lastMovetype;
	float stamina;
	float lastStamina;
	float forwardmove;
	float lastForwardmove;
	float sidemove;
	float lastSidemove;
	float gravity;
	float angles[3];
	float lastAngles[3];
	float position[3];
	float lastPosition[3];
	float velocity[3];
	float lastVelocity[3];
	float lastGroundPos[3]; // last position where the player left the ground.
	float ladderNormal[3];
	bool lastGroundPosWalkedOff;
	bool landedDucked;
	
	int prespeedFog;
	float prespeedStamina;
	
	float jumpGroundZ;
	float jumpPos[3];
	float jumpAngles[3];
	float landGroundZ;
	float landPos[3];
	
	int fwdReleaseFrame;
	int jumpFrame;
	bool trackingJump;
	bool failedJump;
	bool jumpGotFailstats;
	
	// jump data
	JumpType jumpType;
	JumpType lastJumpType;
	JumpDir jumpDir;
	float jumpDistance;
	float jumpPrespeed;
	float jumpMaxspeed;
	float jumpVeer;
	float jumpAirpath;
	float jumpSync;
	float jumpEdge;
	float jumpLandEdge;
	float jumpBlockDist;
	float jumpHeight;
	float jumpJumpoffAngle;
	int jumpAirtime;
	int jumpFwdRelease;
	int jumpOverlap;
	int jumpDeadair;
	
	// strafes!
	int strafeCount;
	float strafeSync[MAX_STRAFES];
	float strafeGain[MAX_STRAFES];
	float strafeLoss[MAX_STRAFES];
	float strafeMax[MAX_STRAFES];
	int strafeAirtime[MAX_STRAFES];
	int strafeOverlap[MAX_STRAFES];
	int strafeDeadair[MAX_STRAFES];
	float strafeAvgGain[MAX_STRAFES];
	
	float strafeAvgEfficiency[MAX_STRAFES];
	int strafeAvgEfficiencyCount[MAX_STRAFES]; // how many samples are in strafeAvgEfficiency
	float strafeMaxEfficiency[MAX_STRAFES];
	
	StrafeType strafeGraph[MAX_JUMP_FRAMES];
	float mouseGraph[MAX_JUMP_FRAMES];
	float jumpBeamX[MAX_JUMP_FRAMES];
	float jumpBeamY[MAX_JUMP_FRAMES];
	JumpBeamColour jumpBeamColour[MAX_JUMP_FRAMES];
}

/**
 * Check if player is overlapping their MOVERIGHT and MOVELEFT buttons.
 *
 * @param x					Buttons;
 * @return 					True if overlapping, false otherwise.
 */
stock bool IsOverlapping(int buttons, JumpDir jumpDir)
{
	if (jumpDir == JUMPDIR_FORWARDS || jumpDir == JUMPDIR_BACKWARDS)
	{
		return (buttons & IN_MOVERIGHT) && (buttons & IN_MOVELEFT);
	}
	// else if (jumpDir == JUMPDIR_LEFT || jumpDir == JUMPDIR_RIGHT)
	return (buttons & IN_FORWARD) && (buttons & IN_BACK);
}

/**
 * Checks if the player is not holding down their MOVERIGHT and MOVELEFT buttons.
 *
 * @param x					Buttons.
 * @return 					True if they're not holding either, false otherwise.
 */
stock bool IsDeadAirtime(int buttons, JumpDir jumpDir)
{
	if (jumpDir == JUMPDIR_FORWARDS || jumpDir == JUMPDIR_BACKWARDS)
	{
		return (!(buttons & IN_MOVERIGHT) && !(buttons & IN_MOVELEFT));
	}
	// else if (jumpDir == JUMPDIR_LEFT || jumpDir == JUMPDIR_RIGHT)
	return (!(buttons & IN_FORWARD) && !(buttons & IN_BACK));
}

stock void ToggleCVar(ConVar cvar)
{
	cvar.BoolValue = !cvar.BoolValue;
}

stock void GetRealLandingOrigin(float landGroundZ, const float origin[3], const float velocity[3], float result[3])
{
	if ((origin[2] - landGroundZ) == 0.0)
	{
		result = origin;
		return;
	}
	
	// this is like this because it works
	float frametime = GetTickInterval();
	float verticalDistance = origin[2] - (origin[2] + velocity[2] * frametime);
	float fraction = (origin[2] - landGroundZ) / verticalDistance;
	
	float addDistance[3];
	addDistance = velocity;
	ScaleVector(addDistance, frametime * fraction);
	
	AddVectors(origin, addDistance, result);
}

stock int FloatSign(float value)
{
	if (value > 0.0)
	{
		return 1;
	}
	else if (value < 0.0)
	{
		return -1;
	}
	return 0;
}

stock int FloatSign2(float value)
{
	if (value >= 0.0)
	{
		return 1;
	}
	// else if (value < 0.0)
	return -1;
}

stock void ShowPanel(int client, int duration, const char[] message)
{
	Event show_survival_respawn_status = CreateEvent("show_survival_respawn_status");
	if (show_survival_respawn_status == INVALID_HANDLE)
	{
		return;
	}
	
	show_survival_respawn_status.SetString("loc_token", message);
	show_survival_respawn_status.SetInt("duration", duration);
	show_survival_respawn_status.SetInt("userid", -1);
	
	if (client == -1)
	{
		show_survival_respawn_status.Fire();
	}
	else
	{
		show_survival_respawn_status.FireToClient(client);
		show_survival_respawn_status.Cancel();
	}
}
