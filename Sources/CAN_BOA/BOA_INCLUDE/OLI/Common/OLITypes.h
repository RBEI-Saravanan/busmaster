#ifndef __OLI_TYPES_H
#define __OLI_TYPES_H

/// @todo update doxygen comments
#include "..\..\OCI\ocitypes.h"   //This line was re-added due to a break to the compilation
#include "BeginNamespace.h"
//#include "..\..\Common\stdtypes.h" //This line is removed for the same reason

typedef int64 Time;
typedef int64 RawTime;
/**
	\brief The time in actual nanoseconds since the UNIX epoch (TAI).

	\remark TAITime measures the exact number of seconds since
	        1 Jan 1970 (TAI) as defined by the TAI consortium. Like
	        UTC days, TAI days are exactly 86400 seconds long. Contrary
	        to UTCTime, however, TAI days are not synchronized to
	        Earth's rotation by means of leap seconds.

	\remark Unlike UTCTime, TAITime offers a continuos time
	        scale. Subtracting two TAITime stamps yields the
	        actual number of seconds passed between the two events,
	        as measured using the weighted average of the results
	        of the atomic clocks that TAI is comprised of.
*/

typedef int64 TAITime;

/**
	\brief The time in UTC nanoseconds since the UNIX epoch.

	\remark UTCTime days are defined to be exactly 86400
	        seconds long. That is, if you calculate the difference
	        betwween two UTCTime values by simple subtraction,
	        leap seconds will not be taken into account.
	        Leap days, on the other hand, will.

	\remark The UNIX epoch is typically defined as 1 Jan 1970
	        00:00 UTC. However, this is not entirely correct
	        because UTC was not finalized before 1 Jan 1972.
	        OLI uses the traditional solution for this problem:
	        The start of the epoch is defined to be exactly two
	        regular years (no leap day, no leap second) prior
	        to 1 Jan 1972 UTC, that is, 1 Jan 1972 00:00 UTC is
	        +63,072,000,000,000,000 in UTCTime (63.072 million
	        seconds).
*/

typedef int64 UTCTime;

#include "EndNamespace.h"

#endif
