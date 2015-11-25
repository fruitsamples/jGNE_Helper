	//	//	This is "jGNE Helper", formerly a monthly posting to	//	the Usenet newsgroup alt.sources.mac.	//	It provides an example for INIT programmers	//	interested in filtering events before they are handed to	//	applications calling GetNextEvent (which is called by	//	WaitNextEvent).	//		//	The jGNE filter is the Apple-sanctioned method for	//	filtering events. It is possible to patch event traps. It	//	is sometimes even advisable. But since the jGNE filter is	//	the sanctioned method, one ought to attempt to use it	//	before patching traps.	//	//	Prospective users of this code should know that the	//	plan of record for Copland does not include support	//	for jGNEFilter. However, it is likely that Copland	//	will include some form of global event filtering service.	//	Carefully isolate your use of this code and you may	//	be able to make the move to Copland without too much	//	pain.	//	//	This code sample has a specific conflict with older	//	versions of PopChar. Newer versions of PopChar have a fix	//	which makes them compatible with this code. Users of older	//	versions of PopChar should probably upgrade to the current	//	version anyway -- this is a very old conflict. The conflict	//	manifests itself by rendering a good portion of the screen	//	"impervious" to clicks unless the option key is held down.	//		//	This version of jGNE Helper was built with both Symantec	//  THINK C 7 and Metrowerks CodeWarrior CW7 for 68K machines.	//	It's entirely possible to write a native jGNEFilter. I've	//	even done it before. However, it hasn't occurred to me how	//	to fit a native filter into the context of this sample.	//		//	For further info on the jGNE filter, consult your Technotes.	//#ifndef THINK_C#	ifndef __MWERKS__#		error unknown compiler#	endif#endif#define OLDROUTINENAMES			0#define OLDROUTINELOCATIONS		0#ifndef __RESOURCES__#	include <Resources.h>#endif#ifndef __MEMORY__#	include <Memory.h>#endif#ifndef __EVENTS__#	include <Events.h>#endif#ifndef __SETUPA4__#	include <SetUpA4.h>#endif#ifndef __LOWMEM__#	include <LowMem.h>#endif	//	//	This example filter simply watches for clicks	//	with the command, option, and control keys held	//	down. If it finds any, it beeps and returns a	//	value which indicates the event should not be	//	passed to the application which called GNE.	//static Boolean myGNE (EventRecord *event, Boolean preResult){	Boolean postResult = preResult;	if (preResult)	{		if (event->what == mouseDown)		{			if (event->modifiers & cmdKey)			{				if (event->modifiers & optionKey)				{					if (event->modifiers & controlKey)					{						SysBeep (10);						postResult = false;					}				}			}		}	}	return postResult;}static void		*gOldJGNE;static Boolean	inJGNE;#if defined (THINK_C)static pascal void myJGNE (void){	asm	{			MOVE.L		A1,A0			// save event record pointer from __GetA4			JSR			__GetA4			// point A1 at our A4			MOVE.L		A4,-(A7)		// save old A4			MOVE.L		(A1),A4			// get new A4			MOVE.L		A0,A1			// restore old A1#elif defined (__MWERKS__)static pascal asm void myJGNE (void){			MOVE.L		D0,A0			// save pre-result from SetUpA4			JSR			SetUpA4			// fix A4, stomp D0			MOVE.L		D0,-(A7)		// save old A4			MOVE.L		A0,D0			// restore pre-result#endif			TST.B		inJGNE			// is myJGNE busy?			BNE			@1				// yes, so bail			MOVE.B		#true,inJGNE	// mark myJGNE busy			MOVE.W		D0,-(A7)		// push pre-result			MOVE.L		A1,-(A7)		// push event record pointer			JSR			myGNE			// do the real work			MOVE.L		(A7)+,A1		// restore event record pointer			ADDQ.L		#2,A7			// pop pre-result; post-result in D0			ASL.W		#8,D0			// bump C boolean to Lisa			MOVE.W		D0,8(A7)		// stash result where caller expects it			MOVE.B		#false,inJGNE	// mark myJGNE not busy@1:			MOVE.L		gOldJGNE,A0		// get previous jGNE			MOVE.L		(A7)+,A4		// restore A4			MOVE.L		A0,-(A7)		// return to previous jGNE#if defined (THINK_C)		}#elif defined (__MWERKS__)			RTS#endif}#if defined (__MWERKS__)#	include <A4Stuff.h>#elif defined (THINK_C)	pascal void * GetA0 (void) = { 0x2E88 };#endifvoid main (void){#if defined (__MWERKS__)	void __Startup__ (void);	long oldA4 = SetCurrentA4 ( );	RememberA4 ( );	DetachResource (RecoverHandle ((Ptr) __Startup__));#elif defined (THINK_C)	void *me = GetA0 ( );	RememberA0 ( );	DetachResource (RecoverHandle (me));	SetUpA4 ( );#endif	gOldJGNE = LMGetGNEFilter ( );#if defined (__MWERKS__)	LMSetGNEFilter (myJGNE);#elif defined (THINK_C)	LMSetGNEFilter ((ProcPtr) myJGNE);#endif#if defined (__MWERKS__)	SetA4 (oldA4);#elif defined (THINK_C)	RestoreA4 ( );#endif}