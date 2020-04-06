#ifndef __EZ_MAP_LOG_H__
#define __EZ_MAP_LOG_H__

#define LOG_ON

class jLog {
public:
	static void Log(const char* format, ...);
};

#ifdef LOG_ON
#define _printlog(...)	jLog::Log(__VA_ARGS__)
#define _echoN(v)		jLog::Log("echo() [%d][%d]\n",__LINE__,v)
#define _echoF(v)		jLog::Log("echo() [%d][%f]\n",__LINE__,v)
#define _echoS(v)		jLog::Log("echo() [%d][%s]\n",__LINE__,v)
#define _trace()			jLog::Log("trace() [%s][%d]\n",__FUNCTION__,__LINE__)
#define _warn()			jLog::Log("warn() [%s][%d]\n",__FUNCTION__,__LINE__)
#define _error()			jLog::Log("error() [%s][%d]\n",__FUNCTION__,__LINE__)
#define _warnif(p)		if(p){jLog::Log("warn() [%s][%d]\n",__FUNCTION__,__LINE__);}
#define _errorif(p)		if(p){jLog::Log("error() [%s][%d]\n",__FUNCTION__,__LINE__);}
#define _exceptif(p, ex)	if(p){jLog::Log("except() [%s][%d]\n",__FUNCTION__,__LINE__); ex;}
#else
#define printlog(...)	
#define echoD(v)		
#define echoF(v)		
#define echoS(v)		
#define trace()
#define warn()			
#define error()			
#define warnif(p)		
#define assertif(p)		
#endif

#define TIME_START __int64 timeStart, timeEnd, timeFrq;\
	float myTime = 0.0f;\
	QueryPerformanceFrequency((LARGE_INTEGER *)&timeFrq);\
	QueryPerformanceCounter((LARGE_INTEGER *)&timeStart)

#define TIME_CHECK QueryPerformanceCounter((LARGE_INTEGER *)&timeEnd);\
	myTime = (float)((double)(timeEnd-timeStart)/timeFrq*1000);\
	echof(myTime);\
	QueryPerformanceCounter((LARGE_INTEGER *)&timeStart)

#endif
