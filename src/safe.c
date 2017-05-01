#include "../include/safe.h"

FILE *safe_open(const char *name, const char *mode) {
  FILE *file = fopen(name, mode);
  if (file) {
    return file;
	} else {
		fprintf(stderr, "Error: Failed accessing file '%s'\n", name);
		exit(1);
	}
}

// ------------------- Checking for NULL pointers ------------------- ~*
void *check_non_null(void *ptr) {
  assert(ptr != 0);
  return ptr;
}

// ------------------- Memory allocation functions ------------------- ~*
void *safe_malloc(size_t size) {
  return check_non_null(malloc(size));
}

void *safe_calloc(size_t num, size_t size) {
  return check_non_null(calloc(num, size));
}

void *safe_realloc(void *ptr, size_t size) {
  return check_non_null(realloc(ptr, size));
}
