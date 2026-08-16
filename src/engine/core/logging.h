#ifndef ENGINE_LOGGING_H
#define ENGINE_LOGGING_H


static inline void verify(int result) {
	if (!result) {
		exit(result);
	}
}

#endif 