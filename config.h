////////////////////// compile-time options ////////////////////////////////


#ifdef HDLOAD
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
