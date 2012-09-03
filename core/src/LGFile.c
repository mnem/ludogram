// # LGFile
//
// Collection of simple cross platform file utilities used throughout
// [Ludogram](https://github.com/mnem/ludogram/).
//
// - - -
// (c) Copyright 2012 David Wagner.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// - - -


#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#include "PGLog.h"

#define SAFE_DEREF_AND_STORE(n, m) if (n) *(n) = (m)

// ## LGFileToString
//
// Reads the contents of `path` and stores in a newly `malloc`d string. It
// assumes that the passed path is suitable for use with `stdio.h` functions.
//
// If `stdioErrno` is not `NULL` it will be used to store the `errno` for
// any failure.
//
// ##### NOTE:
// You must `free` the returned string when you are finished with it.
char* LGFileToString(const char *path, int *stdioErrno)
{
	SAFE_DEREF_AND_STORE(stdioErrno, 0);
	
	if (NULL == path)
	{
		SAFE_DEREF_AND_STORE(stdioErrno, EINVAL);
		return NULL;
	}
	
	struct stat status = { 0 };
	if (0 != stat(path, &status))
	{
		SAFE_DEREF_AND_STORE(stdioErrno, errno);
		pgLog(PGL_Error, "Could not get status for %s: %s", path, strerror(errno));
		return NULL;
	}
	
	FILE *filePointer = fopen(path, "r");
	if (NULL == filePointer)
	{
		SAFE_DEREF_AND_STORE(stdioErrno, errno);
		pgLog(PGL_Error, "Could not open %s: %s", path, strerror(errno));
		return NULL;
	}

	char *string = (char*)malloc(status.st_size + 1);
	if (!string)
	{
		SAFE_DEREF_AND_STORE(stdioErrno, ENOMEM);
		pgLog(PGL_Error, "Could not allocate buffer to read %s. Need %dz bytes free.", path, (status.st_size + 1));
		return NULL;
	}
	
	size_t read = fread(string, 1, status.st_size, filePointer);
	int closeErr = fclose(filePointer);
	string[read] = '\0';
	
	// ##### NOTE:
	// Treats read errors as warnings and returns the buffer containing
	// what was read. This may not be the best thing to do.
	if (read != status.st_size)
	{
		SAFE_DEREF_AND_STORE(stdioErrno, EIO);
		pgLog(PGL_Warn, "Did not read all bytes in %s. Expected to read %dz, but read %dz", path, read, status.st_size);
	}
	if (EOF == closeErr)
	{
		SAFE_DEREF_AND_STORE(stdioErrno, errno);
		pgLog(PGL_Warn, "Failed to close %s: %s", path, strerror(errno));
	}
	
	return string;
}
