#ifndef JAYVM_MODULES_H
#define JAYVM_MODULES_H

#include "jary/object.h"

extern int define_function(const char	       *key,
			   enum jy_ktype	return_type,
			   uint8_t		param_size,
			   const enum jy_ktype *param_types,
			   jy_funcptr_t		func);

extern const char *error_message(int status);

#endif // JAYVM_MODULES_H
