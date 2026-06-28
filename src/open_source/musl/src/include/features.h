#ifndef FEATURES_H
#define FEATURES_H

#include "../../include/features.h"

#ifdef __LITEOS__
#define weak
#else
#define weak __attribute__((__weak__))
#endif
#ifdef LOSCFG_COMPILER_ICCARM
#define hidden
#else
#define hidden __attribute__((__visibility__("hidden")))
#endif
#define weak_alias(old, new) \
	extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

#endif
