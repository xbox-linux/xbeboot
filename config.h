////////////////////// compile-time options ////////////////////////////////

// selects between the supported video modes, see boot.h for enum listing those available
#define VIDEO_PREFERRED_MODE VIDEO_MODE_640x480

#if 1
	#define LOADHDD
	//#define LOADHDD_CFGFALLBACK
	#undef LOADHDD_CFGFALLBACK
	#undef LOADXBE

#else

	#define LOADXBE
	#undef LOADHDD
 
#endif  


// Do not change this
#ifdef LOADXBE
#define LOADHDD_CFGFALLBACK
#endif 
