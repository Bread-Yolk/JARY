#include "exec.h"

#include "compiler.h"

#include "jary/error.h"
#include "jary/memory.h"
#include "jary/object.h"

#include <string.h>

#define JY_K_OBJECT                                                            \
JY_K_STR:                                                                      \
	case JY_K_EVENT:                                                       \
	case JY_K_MODULE:                                                      \
	case JY_K_FUNC

union flag8 {
	uint8_t flag;

	struct bits {
		bool b1 : 1;
		bool b2 : 1;
		bool b3 : 1;
		bool b4 : 1;
		bool b5 : 1;
		bool b6 : 1;
		bool b7 : 1;
		bool b8 : 1;
	} bits;
};

struct stack {
	jy_val_t *values;
	uint32_t  size;
	uint32_t  capacity;
};

static inline bool push(struct stack *s, jy_val_t value)
{
	const int growth = 10;

	if (s->size + 1 >= s->capacity) {
		int   newcap = (s->capacity + growth);
		void *mem = jry_realloc(s->values, sizeof(*s->values) * newcap);

		if (mem == NULL)
			return true;

		s->capacity = newcap;
		s->values   = mem;
	}

	s->values[s->size]  = value;
	s->size		   += 1;

	return false;
}

static inline jy_val_t pop(struct stack *s)
{
	jry_assert(s->size > 0);

	return s->values[--s->size];
}

static inline int interpret(const jy_val_t	*vals,
			    const enum jy_ktype *types,
			    const void		*obj,
			    struct stack	*stack,
			    union flag8		*flag,
			    const uint8_t      **code)
{
	const uint8_t *pc     = *code;
	enum jy_opcode opcode = *pc;

	union {
		const uint8_t  *bytes;
		const uint8_t  *u8;
		const int16_t  *i16;
		const uint16_t *u16;
		const uint32_t *u32;
	} arg = { .bytes = pc + 1 };

	switch (opcode) {
	case JY_OP_PUSH8: {
		union {
			jy_val_t val;
			void	*ptr;
		} v = { .val = vals[*arg.u8] };

		switch (types[*arg.u8]) {
		case JY_K_OBJECT:
			v.ptr = memory_fetch(obj, jry_v2long(v.val));
			break;
		default:
			break;
		}

		if (push(stack, v.val))
			goto FATAL;

		pc += 2;
		break;
	}
	case JY_OP_PUSH16: {
		union {
			jy_val_t val;
			void	*ptr;
		} v = { .val = vals[*arg.u16] };

		switch (types[*arg.u8]) {
		case JY_K_OBJECT:
			v.ptr = memory_fetch(obj, jry_v2long(v.val));
			break;
		default:
			break;
		}

		if (push(stack, v.val))
			goto FATAL;

		pc += 3;
		break;
	}
	case JY_OP_EVENT: {
		uint16_t field	    = arg.u16[0];
		uint16_t k_id	    = arg.u16[1];

		long		ofs = jry_v2long(vals[k_id]);
		struct jy_defs *ev  = memory_fetch(obj, ofs);

		if (push(stack, ev->vals[field]))
			goto FATAL;

		pc += 5;
		break;
	}

	case JY_OP_JMPF:
		if (!flag->bits.b8)
			pc += *arg.i16;
		break;
	case JY_OP_JMPT:
		if (flag->bits.b8)
			pc += *arg.i16;
		break;
	case JY_OP_NOT:
		flag->bits.b8  = !flag->bits.b8;
		pc	      += 1;
		break;
	case JY_OP_CMPSTR: {
		struct jy_obj_str *v2 = jry_v2str(pop(stack));
		struct jy_obj_str *v1 = jry_v2str(pop(stack));

		flag->bits.b8	      = *v1->str == *v2->str &&
				memcmp(v1->str, v2->str, v1->size) == 0;

		pc += 1;
		break;
	}
	case JY_OP_CMP: {
		long v2	       = jry_v2long(pop(stack));
		long v1	       = jry_v2long(pop(stack));

		flag->bits.b8  = v1 == v2;

		pc	      += 1;
		break;
	}
	case JY_OP_LT: {
		long v2	       = jry_v2long(pop(stack));
		long v1	       = jry_v2long(pop(stack));

		flag->bits.b8  = v1 < v2;

		pc	      += 1;
		break;
	}
	case JY_OP_GT: {
		long v2	       = jry_v2long(pop(stack));
		long v1	       = jry_v2long(pop(stack));

		flag->bits.b8  = v1 > v2;

		pc	      += 1;
		break;
	}

	case JY_OP_ADD: {
		long v2 = jry_v2long(pop(stack));
		long v1 = jry_v2long(pop(stack));

		push(stack, jry_long2v(v1 + v2));

		pc += 1;
		break;
	}
	}

	// PC is not moving...
	jry_assert(pc != *code);
	*code = pc;
	return 0;
FATAL:
	return 1;
}

int jry_exec(const jy_val_t	 *vals,
	     const enum jy_ktype *types,
	     const void		 *obj,
	     const uint8_t	 *codes,
	     uint32_t		  codesz)
{
	struct stack   stack = { .values = NULL };
	union flag8    flag  = { .flag = 0 };
	int	       res   = 0;
	const uint8_t *pc    = codes;
	const uint8_t *end   = &codes[codesz - 1];

	for (; end - pc > 0;) {
		res = interpret(vals, types, obj, &stack, &flag, &pc);

		if (res != 0)
			break;
	}

	jry_free(stack.values);

	return res;
}