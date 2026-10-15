
#if defined _distbug_included
	#endinput
#endif
#define _distbug_included

#define DISTBUG_VERSION "2.3.0"

// library name, for LibraryExists and OnLibraryAdded
#define DISTBUG_LIBRARY "distbugfix"

#define DISTBUG_MAX_STRAFES     32
#define DISTBUG_MAX_FWD_RELEASE 16 // further than this either way is printed as "Fwd: No"
#define DISTBUG_MAX_GRAPH_TICKS 150 // the strafe graph stops after this many airborne ticks. never changes

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
 * One tick of the strafe graph, with the symbol Distbug prints for it.
 *
 * Left and right are the sides of the jump's jumpDir, the sides the graph
 * prints, not raw A and D. They are the player's left and right while facing
 * the way they travel: on a backwards jump left is D and right is A, on a
 * JUMPDIR_LEFT jump left is S and right is W, and on a JUMPDIR_RIGHT jump left
 * is W and right is S. The sidespeed below is the wishspeed along those sides.
 *
 * A type on one side prints on that side's row and as "." on the other.
 * DISTBUG_STRAFETYPE_OVERLAP and DISTBUG_STRAFETYPE_NONE print on both rows.
 */
enum DistbugStrafeType
{
	DISTBUG_STRAFETYPE_OVERLAP,        // "$": both strafe keys down and sidespeed is 0
	DISTBUG_STRAFETYPE_NONE,           // ".": neither key down and sidespeed is 0, or one key down without sidespeed its way
	
	DISTBUG_STRAFETYPE_LEFT,           // "█": only the left key down and sidespeed is to the left
	DISTBUG_STRAFETYPE_OVERLAP_LEFT,   // "#": both keys down, but sidespeed is to the left
	DISTBUG_STRAFETYPE_NONE_LEFT,      // "H": neither key down, but sidespeed is to the left
	
	DISTBUG_STRAFETYPE_RIGHT,          // "█": only the right key down and sidespeed is to the right
	DISTBUG_STRAFETYPE_OVERLAP_RIGHT,  // "#": both keys down, but sidespeed is to the right
	DISTBUG_STRAFETYPE_NONE_RIGHT,     // "H": neither key down, but sidespeed is to the right
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
	int graphLength;        // ticks in graph: airtime, capped at DISTBUG_MAX_GRAPH_TICKS
	DistbugStrafeType graph[DISTBUG_MAX_GRAPH_TICKS]; // one per airborne tick, the first is airtime 1. only indexes below graphLength are set
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
