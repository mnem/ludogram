// # LGFile
//
// Simple file access.
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

#ifndef LGFile_h
#define LGFile_h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
 * Read and return the contents of the file specified by path in a new
 * char buffer.
 *
 * @param path Path to read. Must be suitable for passing to stdio functions.
 * @param stdioErrno Optional int point to store any errno. May be NULL.
 *
 * @return On success, returns a newly malloc'd buffer containing the
 *		contents. You must free() this buffer when finished. On failure
 *		returns NULL and sets stdioErrno if available.
 */
extern char* LGFileToString(const char *path, int *stdioErrno);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LGFile_h
