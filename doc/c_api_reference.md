# C API Reference
This document contains all information needed to use the functions available in `JARY/include/jary.h`.

## Functions
| Return Type | Function Signature |
|--------|------|
| `int` | `jary_open(struct jary **ctx)` |
| `int` | `jary_close(struct jary *ctx)` |
| `int` | `jary_modulepath(struct jary *, const char *path)` |
| `int` | `jary_event(struct jary *ctx, const char *name, unsigned int *event)` |
| `int` | `jary_field_str(struct jary *ctx, unsigned int event, const char *field, const char *value)` |
| `int` | `jary_field_long(struct jary *ctx, unsigned int event, const char *field, long value)` |
| `int` | `jary_field_ulong(struct jary *ctx, unsigned int event, const char *field, unsigned long value)` |
| `int` | `jary_field_bool(struct jary *ctx, unsigned int event, const char *field, unsigned char value)` |
| `int` | `jary_rule_clbk(struct jary *ctx, const char *name, int (*callback)(void *, const struct jyOutput *), void *data)` |
| `int` | `jary_compile_file(struct jary *ctx, const char *path, char **errmsg)` |
| `int` | `jary_compile(struct jary *ctx, unsigned int size, const char *source, char **errmsg)` |
| `int` | `jary_execute(struct jary *ctx)` |
| `void` | `jary_output_len(const struct jyOutput *output, unsigned int *length)` |
| `int` | `jary_output_str(const struct jyOutput *output, unsigned int index, const char **value)` |
| `int` | `jary_output_long(const struct jyOutput *output, unsigned int index, long *value)` |
| `int` | `jary_output_ulong(const struct jyOutput *output, unsigned int index, unsigned long *value)` |
| `int` | `jary_output_bool(const struct jyOutput *output, unsigned int index, unsigned char *value)` |
| `const char*` | `jary_errmsg(struct jary *ctx)` |
| `void` | `jary_free(void *ptr)` |

## Function Descriptions

### `int jary_open`
```c
int jary_open(struct jary **ctx)
```
This function initialize the jary `ctx` and **must be called** before running other functions that expects a jary `ctx`. 

#### Return value
- `JARY_OK` everything went well, and no error.
- `JARY_ERR_OOM` Out of memory
- `JARY_ERROR` Generic error, check `jary_errmsg()` for detail, and then close the `ctx` using `jary_close()`.

#### Example usage:
```c
struct jary *jary;

switch (jary_open(&jary)) {
case JARY_OK:
	break;
case JARY_ERROR:
	printf("%s\n", jary_errmsg(jary));
	jary_close(jary);
	break;
case JARY_ERR_OOM:
	// handle this
	break;
};
```

### `int jary_close`
```c
int jary_close(struct jary *ctx)
```
This function must be called to properly close an open `ctx`.

#### Return value
- `JARY_OK` everything went well, and no error.
- `JARY_ERR_SQLITE3` closing the sqlite3 db went wrong. Check `jary_errmsg()` to see the `sqlite3_errmsg()`. 

### `int jary_modulepath`
```c
int `jary_modulepath(struct jary *ctx, const char *path)
```
This function is used to set the directory for `import` resolution. When using this function, you must always suffix the `path` with the `/` character. Not doing so will lead to undefined behaviour.

Internally, whenever an `import <name>` statement is being compiled, it will perform the following concatenation:
```
solib_path = <path> + "lib" + <name>
```
this will then be passed as an argument to `dlopen(solib_path)`.

#### Return value
- `JARY_OK` everything went well, and no error.
- `JARY_ERR_OOM` out of memory.

#### Example usage
```c
switch (jary_modulepath(jary, "./myproject/")) {
case JARY_OK:
	break;
case JARY_ERR_OOM:
	printf("%s", jary_errmsg(jary));
	break;
}
```

### `int jary_event`
```c
int jary_event(struct jary *ctx, const char *name, unsigned int *event)
```

Create a new event of type `name` and push it into the event queue. The `name` argument is used to determine the type of event corresponding to the identified ingress declaration. The `event` argument will store the `id` of the of the newly queued event to be used in the `jary_field_*` function variant.

#### Return value
- `JARY_OK` everything went well, and no error.
- `JARY_ERR_NOTEXIST` there's no such event identified by `name`.
- `JARY_ERR_OOM` out of memory.

#### Example usage
```c
unsigned int event;

switch (jary_event(jary, "user", &event)) {
case JARY_OK:
	break;
case JARY_ERR_NOTEXIST:
case JARY_ERR_OOM
	printf("%s", jary_errmsg(jary));
	break;
}
```
### `int jary_field_*`
```c
int jary_field_str(struct jary *ctx, unsigned int event, const char *field, const char *value)
int jary_field_long(struct jary *ctx, unsigned int event, const char *field, long value)
int jary_field_ulong(struct jary *ctx, unsigned int event, const char *field, unsigned long value)
int jary_field_bool(struct jary *ctx, unsigned int event, const char *field, unsigned char value)
```
Define the field value for an event that has been queued. The `field` argument is used to determine the field to be defined with the `value` argument.

#### Return value
- `JARY_OK` everything went well, and no error.
- `JARY_ERR_NOTEXIST` either the `event` id or the identified `field` does not exist, check `jary_errmsg`.
- `JARY_ERR_MISMATCH` invalid `value` type for the identified `field`.
- `JARY_ERR_OOM` out of memory.

#### Example usage
```c
unsigned int event;

// assuming "user" ingress is declared
jary_event(jary, "user", &event);

// set user.name = "John Doe"
switch(jary_field_str(jary, event, "name", "John Doe")) {
case JARY_OK:
	break;
case JARY_ERR_NOTEXIST:
	printf("%s", jary_errmsg(jary));
	break;
case JARY_ERR_MISTMATCH:
	//handle this
case JARY_ERR_OOM:
	//handle this
}
```

### `int jary_rule_clbk`
```c
int jary_rule_clbk(struct jary *ctx, const char *name, int (*callback)(void *, const struct jyOutput *), void *data)
```

Attach a `callback` function to be called when rule identified by `name` is triggered. The `data` argument will be passed as the first argument of the `callback` function.

The `callback` function can return an interrupt return code to effect the execution of the current rule. The following is all available interrupt code:
- `JARY_INT_CRASH` crash the jary runtime, and exit the context. No more rules will be executed.
- `JARY_INT_FINAL` stop the callback from being called for subsequent rows.

#### Return value
- `JARY_OK` everything went well, and no error
- `JARY_ERR_NOTEXIST` no rule identified by `name` exist
- `JARY_ERR_OOM` out of memory

#### Example usage
```c
int callback(void *data, const struct jyOutput *output) {
	// will print "hello world" whenever my_rule is triggered
	printf("%s", (char*) data);

	return JARY_OK;
}

if (jary_clbk(jary, "my_rule", callback, "hello world") != JARY_OK)
	// handle this
	;
```

### `int jary_compile_file`
```c
int jary_compile_file(struct jary *ctx, const char *path, char **errmsg)
```
Compile a jary rule specified by the `path` argument. This function must only be called once after opening a jary context. Once a rule has been compiled to a context it cannot compile any more files. Use a different context to compile a different rule file if needed, and not on a previously compiled context. 

If the `errmsg` is not NULL, it will be allocated with an error message if there's a compiler error, and it must be freed using the `jary_free` function.

#### Return value
- `JARY_OK` everything went well, and no error
- `JARY_ERROR` something went wrong when compiling the file, check `jary_errmsg()`
- `JARY_ERR_COMPILE` rule compile/parsing error, `errmsg` will be allocated.
- `JARY_ERR_OOM` out of memory
  
#### Example usage
```c
char *errmsg;

switch (jary_compile_file(jary, "./rule.jary", &errmsg)) {
case JARY_OK:
	break;
case JARY_ERROR:
	printf("%s\n", jary_errmsg(jary));
	break;
case JARY_ERR_COMPILE:
	printf("%s", errmsg);
	jary_free(errmsg);
	break;
case JARY_ERR_OOM:
	// handle this..
}
```

### `int jary_compile`
```c
int jary_compile(struct jary *ctx, unsigned int size, const char *source, char **errmsg)
```
Compile the string within `source` as a jary rule file where `size` is the length of `source`. This function is actually called by `jary_compile_file` internally and just passed the entire content of the file into the `3rd` argument `source` when using that function. This can be used alternatively the main program wants to open the file itself. The 4th argument `errmsg` has the same behaviour as the `jary_compile_file` variant.

#### Return value
- `JARY_OK` everything went well, and no error
- `JARY_ERROR` something went wrong when compiling the file, check `jary_errmsg()`
- `JARY_ERR_COMPILE` rule compile/parsing error, `errmsg` will be allocated.
- `JARY_ERR_OOM` out of memory

### `int jary_execute`
```c
int jary_execute(struct jary *ctx)
```
This function will ingest all queued event and subject it to all defined rules. After all compiled rules get executed, the function will exit, and all state within the context will be updated.

It is a good idea to run `jary_execute` each time you queued an event so it get processed immediately after defining its field.

#### Return value
- `JARY_OK` everything went well, and no error
- `JARY_ERR_EXEC` something went wrong executing the bytecode, check `jary_errmsg`.
- `JARY_ERR_OOM` out of memory

#### Example usage
```c
// defined somehow
char *names[i] = ...

// create and execute 10 events
for (int i = 0; i < 10; ++i) {
	unsigned int event;
	
	// assuming "user" ingress is declared
	jary_event(jary, "user", &event);
	
	// set user.name = "John Doe"
	jary_field_str(jary, event, "name", names[i]);
	
	switch(jary_execute(jary)) {
	case JARY_OK:
		break;
	case JARY_ERR_EXEC:
		printf("%s\n", jary_errmsg(jary))
		break;
	case JARY_ERR_OOM:
		// handle this
	}
}
```

### `void jary_output_len`
```c
void jary_output_len(const struct jyOutput *output, unsigned int *length)
```
Get the total size of values outputted by a rule and store it into the `length` argument. This should only be used within a rule callback function.

### `int jary_output_*`
```c
int jary_output_str(const struct jyOutput *output, unsigned int index, const char **value)
int jary_output_long(const struct jyOutput *output, unsigned int index, long *value)
int jary_output_ulong(const struct jyOutput *output, unsigned int index, unsigned long *value)
int jary_output_bool(const struct jyOutput *output, unsigned int index, unsigned char *value)
```
This function is used to get an outputed value from a specific `index` and store it in the `value` argument. The `index` argument correspond to the order of value listed in the `output:` section of the rule that triggered the callback.

Specific to the `jary_output_str` variant, the `value` argument must not be referred outside of the current callback function, as the lifetime for the `char *` ends when that specific call ends. Referring to the `char *value` outside of its lifetime will lead to undefined behaviour. Duplicate the value string if it need to be referred else where.

Using a different type variant than the actual outputted type will invoke the same behaviour as using a union:
```c
char *output = "my_value";

union {
	const char   *str;
	long 	      i64;
	unsigned long u64;
	unsigned char u8;
} view = { .str = output };

long value = view.i64;

```

#### Return value
- `JARY_OK` everything went well, and no error
- `JARY_ERR_NOTEXIST` no such value can be referenced by the `index` argument
#### Example usage
```c
int callback(void *data, const struct jyOutput *output) {
	const char *value;

	// get string value at index 0
	if (jary_output_str(output, 0, &value) == JARY_OK)
		printf("string value: %s", value);
	else
		printf("no value!");

	return JARY_OK;
}
```

### `const char* jary_errmsg`
```c
const char* jary_errmsg(struct jary *ctx)
```
Get the error message, if any, after calling a `jary_*` function that returns an error code.

#### Return value
Return the last error message.

### `void jary_free`
```c
void jary_free(void *ptr)
```
Must be called to free any allocated data by jary. Not doing so will lead to a memory leak.
