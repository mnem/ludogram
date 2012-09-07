// # LGTypes
//
// Various data structures used throughout Ludogram.
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
#ifndef LGTypes_h
#define LGTypes_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "../../external/uthash/uthash-1.9.6/src/uthash.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// # Data structures for LGPrg*
//

// ## LGPrgVar structure
//
// This structure holds information about various attribute
// and uniform variables in a program.
//
// Generally used as part of an `LGPrg` object.
typedef struct {
	// Make the structure hashable for uthash
	UT_hash_handle hh;
	
	// OpenGL properties for the variable
	GLint location;
	GLint size;
	GLenum type;
	
	// Name of the variable, which is also the has key. This string is
	// variable size (but NUL terminated, which means LGPrgVar structures
	// are variable size, so you can't put them in an array (although
	// you have an array of pointers to structures of course).
	char name[1];
} LGPrgVar;


// ## LGPrgObject structure
//
// Structure to hold a program object and any relevant log. A
// program object is a shader or the final linked program itself
//
// Generally used as part of an `LGPrg` object.
typedef struct {
	// The shader or program reference
	GLuint reference;
	// Whether it was compiled or linked successfully
	GLboolean valid;
	// Any associated log
	GLchar * log;
} LGPrgObject;


// ## LGPrg structure
//
// Structure to hold the details of a linked shader program. The
// structure layout is such that you can cast a `struct LGPrg *`
// to a `GLuint *` and pass it as an OpenGL progam object, although
// that feels a bit hacky - you should use program.reference instead.
//
// LGPrg also stores hashes for all the active attributes and uniforms
// in a shader program. You can either access these hashes directly
// using [uthash](http://uthash.sourceforge.net/userguide.html)
// string-within-structure key functions. Alternatively, you can use
// the convienience methods `LGPrgAttribLocation` and
// `LGPrgUniformLocation`
typedef struct {
	LGPrgObject program;
	LGPrgObject vertexShader;
	LGPrgObject fragmentShader;
	
	LGPrgVar * attributes;
	LGPrgVar * uniforms;
} LGPrg;


// ## LGActiveVarQuery structure
//
// Structure to hold details for extractive active variables
// from a program. Primarily used internally by functions which
// populate the LGPrg variable hashes
typedef struct {
	GLenum queryType;
	GLenum queryTypeNameLength;
	void (*getActiveVariable)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
	GLint (*getVariableLocation)(GLuint, const GLchar *);
} LGActiveVarQuery;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LGTypes_h
