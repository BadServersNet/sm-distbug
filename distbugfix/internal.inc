
#if defined _distbug_internal_included
	#endinput
#endif
#define _distbug_internal_included

#define DISTBUG_CONFIG_NAME "distbugfix"

#define CHAT_PREFIX     "{d}[{l}GC{d}]"
#define CONSOLE_PREFIX  "[GC]"
#define CHAT_SPACER     " {d}|{g} "

#define MAX_STRAFES     DISTBUG_MAX_STRAFES
#define MAX_EDGE        32.0
#define MAX_JUMP_FRAMES	150 // for frame based arrays
#define MAX_BHOP_FRAMES	8

#define MAX_COOKIE_SIZE 32

// rounded stats go through the 5 decimals gokz stores distbug stats at, so they print what it stores
#define ROUNDED_STAT_DECIMALS  5
#define ROUNDED_STAT_PRECISION 100000
#define ROUNDED_STAT_LIMIT     20000.0

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

// a stat ScaleStat can take: finite, and within an int once scaled
stock bool IsStatRoundable(float value)
{
	return (view_as<int>(value) & 0x7F800000) != 0x7F800000 && FloatAbs(value) <= ROUNDED_STAT_LIMIT;
}

// ScaleStat and FormatScaledStat are GOKZ_DB_ScaleJSValue and GOKZ_DB_FormatJSValue from
// BadServersNet/gokz commit b8711fa9 (addons/sourcemod/scripting/include/gokz/localdb.inc), renamed

/**
 * Scales a jump stat to an integer with the given decimals, rounded half away
 * from zero: 276.259 at 2 decimals is 27626, -0.15625 at 4 is -1563. This is
 * the value GOKZ stores. It rounds from the digits SourceMod prints at
 * one more decimal, which is exact where value * 10^decimals in a 32-bit float
 * is not.
 *
 * @param value			Stat to scale. Not NaN or infinite, and at most
 *						2147483646 once scaled.
 * @param decimals		Decimals to keep.
 * @return				Scaled and rounded stat.
 */
stock int ScaleStat(float value, int decimals)
{
	char format[8], digits[32];
	FormatEx(format, sizeof(format), "%%.%df", decimals + 1);
	int length = FormatEx(digits, sizeof(digits), format, value);
	// The extra digit only decides the rounding, so the rest fits an int.
	// x87 builds of SourceMod print an exact tie such as 1.125 one digit low;
	// a value is one when value * 2^(decimals + 1) is an odd integer.
	float twos = value * float(1 << (decimals + 1));
	int wholeTwos = RoundToFloor(twos);
	bool roundAway = digits[length - 1] >= '5' || (twos == float(wholeTwos) && (wholeTwos & 1) == 1);
	digits[length - 1] = '\0';
	ReplaceString(digits, sizeof(digits), ".", "");
	int scaled = StringToInt(digits);
	if (!roundAway)
	{
		return scaled;
	}
	return digits[0] == '-' ? scaled - 1 : scaled + 1;
}

/**
 * Formats a scaled jump stat at the given decimals, rounded half away from
 * zero: 8350 at precision 100 is "84" at 0 decimals. Showing stored stats
 * through this, never through a float, keeps every place in agreement.
 *
 * @param value			Scaled stat, e.g. from ScaleStat.
 * @param precision		Power of ten it is scaled by, e.g. ROUNDED_STAT_PRECISION.
 * @param decimals		Decimals to show, at most the scale's.
 * @return				Formatted stat, e.g. "262.4168".
 */
stock char[] FormatScaledStat(int value, int precision, int decimals)
{
	char formatted[16], digits[12];
	int shown = 1;
	for (int i = 0; i < decimals; i++)
	{
		shown *= 10;
	}
	int divisor = precision / shown;
	int whole = value / precision;
	int rest = value % precision;
	int fraction = ((rest < 0 ? -rest : rest) + divisor / 2) / divisor;
	if (fraction == shown)
	{
		whole += value < 0 ? -1 : 1;
		fraction = 0;
	}
	if (decimals == 0)
	{
		IntToString(whole, formatted, sizeof(formatted));
		return formatted;
	}
	// A leading 1 keeps the fraction's leading zeros: 5 hundredths is "105"
	IntToString(shown + fraction, digits, sizeof(digits));
	FormatEx(formatted, sizeof(formatted), "%s%d.%s", value < 0 && whole == 0 && fraction != 0 ? "-" : "", whole, digits[1]);
	return formatted;
}
