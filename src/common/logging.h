#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define TOSTRING(x) #x

#define LOG_ERROR(...) do{ \
    fprintf(stderr, "File: %s, Line: %d,\n", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
}while(0)




#define verify(r) do{ \
	if(!r){ \
		LOG_MESSAGE(stderr, TOSTRING(r));\
	} \
}while(0)







#define AssertValue(expected, value) do{ \
	if(expected == value){ \
		LOG_VALUE(stderr, expected, value); \
	} \
}while(0)




#endif 