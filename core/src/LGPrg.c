// # LGOpenGLProgram
//
// Various methods to create and manipulate OpenGL shader programs.
// Currently, only programs using vertex and fragment shaders are
// supported, because they work across mobile and desktop.
//
// In general, you don't have to use these functions unless you want to get
// into the internals of how Ludogram works. For more information on
// shaders, here are a few resources:
//
// - [GLSL 1.2 Tutorial](http://www.lighthouse3d.com/tutorials/glsl-tutorial/)
// - [Check out Graphics Shaders: Theory and Practice, Second Edition by Mike Bailey](http://www.amazon.co.uk/gp/product/1568814348?ie=UTF8&camp=3194&creative=21330&creativeASIN=1568814348&linkCode=shr&tag=nohe-21&ref_=dp_ob_title_bk)
// - [Shaders tag at gamedev.stackexchange](http://gamedev.stackexchange.com/questions/tagged/shaders)
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

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "Ludogram.h"


#pragma mark - GL Logging


// ## _LGvLogGLErrors
//
// Checks for any GL errors and prints any to stdout, along
// with the passed message
GLenum _LGvLogGLErrors(const char *fmt, ...)
{
	GLenum error = glGetError();
	if (GL_NO_ERROR != error)
	{
		va_list ap;
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
		
		switch (error)
		{
			case GL_INVALID_ENUM:
				printf(" - Invalid enum.\n");
				break;
			case GL_INVALID_VALUE:
				printf(" - Invalid value.\n");
				break;
			case GL_INVALID_OPERATION:
				printf(" - Invalid operation.\n");
				break;
			case GL_OUT_OF_MEMORY:
				printf(" - Out of memory.\n");
				break;
				
			default:
				printf(" - Unrecognised gl error code: %d\n", error);
				break;
		}
	}
	
	return error;
}


#pragma mark - LGPrgVar
//
// # LGPrgVar methods
//


// ## LGPrgVarNew
//
// Creates and initialises a new LGPrgVar. Takes a copy of the passed
// name string.
LGPrgVar * LGPrgVarNew(GLint location, GLint size, GLenum type, const char * name)
{
	size_t nameLength = name ? strlen(name) : 0;
	// Work out the real size we need to allocate for the struct, including
	// the name string. By keeping it as a contiguous block like this,
	// hashing is simpler.
	size_t recordSize = offsetof(LGPrgVar, name) + nameLength + 1/* NUL */;

	LGPrgVar * o = malloc(recordSize);
	if (o)
	{
		memset(o, 0, recordSize);

		o->location = location;
		o->size = size;
		o->type = type;
		
		// As we've zeroed the memory already, we
		// don't have to worry about adding a NUL
		// terminator.
		strncpy(o->name, name, nameLength);
	}
	
	return o;
}


// ## LGPrgVarDelete
//
// Releases the LGPrgVar object.
void LGPrgVarDelete(LGPrgVar ** o_)
{
	if (o_)
	{
		LGPrgVar *o = *o_;
		if (o)
		{
			size_t nameLength = strlen(o->name);
			size_t recordSize = offsetof(LGPrgVar, name) + nameLength + 1/* NUL */;
			memset(o, 0, recordSize);
			free(o);
		}
		*o_ = NULL;
	}
}


// ## LGPrgVarHashClear
//
// Deletes the contents of the hash and sets it to NULL.
void LGPrgVarHashClear(LGPrgVar **hash_)
{
	if (NULL != hash_)
	{
		LGPrgVar *current;
		LGPrgVar *temp;
		LGPrgVar *hash = *hash_;
		HASH_ITER(hh, hash, current, temp)
		{
			HASH_DEL(hash, current);
			LGPrgVarDelete(&current);
		}
		*hash_ = hash;
	}
}


// ## LGPrgVarHashOfActiveVariables
//
// Using the supplied query, creates a new LGPrgVar hash of all the
// active variables in the specified program
LGPrgVar * LGPrgVarHashOfActiveVariables(GLuint program, const LGActiveVarQuery *query)
{
	LGPrgVar * hash = NULL;
	GLint numVars = 0;
	GLint varNameMax = 0;
	
	glGetProgramiv(program, query->queryType, &numVars);
	glGetProgramiv(program, query->queryTypeNameLength, &varNameMax);
	
	GLchar name[varNameMax];
	for (GLuint attrIndex = 0; attrIndex < numVars; attrIndex++)
	{
		GLsizei nameLength = 0;
		GLint size = 0;
		GLenum type = 0;
		
		query->getActiveVariable(program, attrIndex, varNameMax, &nameLength, &size, &type, name);
		
		GLint location = query->getVariableLocation(program, name);
		
		LGPrgVar * var = LGPrgVarNew(location, size, type, name);
		if (var)
		{
			HASH_ADD_STR(hash, name, var);
		}
		else
		{
			LGLogOOM("Out of memory creating LGPrgVar in LGPrgVarHashOfActiveVariables");
		}
	}
	
	return hash;
}


// ## LGPrgVarHashOfActiveAttributes
//
// Re-parses the program for active attributes, storing their locations
// in the newly created hash.
LGPrgVar * LGPrgVarHashOfActiveAttributes(GLuint program)
{
	LGActiveVarQuery query;
	
	query.queryType = GL_ACTIVE_ATTRIBUTES;
	query.queryTypeNameLength = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH;
	query.getActiveVariable = glGetActiveAttrib;
	query.getVariableLocation = glGetAttribLocation;
	
	return LGPrgVarHashOfActiveVariables(program, &query);
}


// ## LGPrgVarHashOfActiveUniforms
//
// Re-parses the program for active uniforms, storing their locations
// in the newly created hash.
LGPrgVar * LGPrgVarHashOfActiveUniforms(GLuint program)
{
	LGActiveVarQuery query;
	
	query.queryType = GL_ACTIVE_UNIFORMS;
	query.queryTypeNameLength = GL_ACTIVE_UNIFORM_MAX_LENGTH;
	query.getActiveVariable = glGetActiveUniform;
	query.getVariableLocation = glGetUniformLocation;
	
	return LGPrgVarHashOfActiveVariables(program, &query);
}


// ## LGPrgVarLocation
//
// Returns the location for the named variable. If the
// name is not found, returns -1.
GLuint LGPrgVarLocation(const LGPrgVar *hash, const char *name)
{
	LGPrgVar *var = NULL;
	
	HASH_FIND_STR(hash, name, var);
	
	if (NULL == var )
	{
		return -1;
	}
	else
	{
		return var->location;
	}
}


#pragma mark - LGPrgObject
//
// # LGPrgObject methods
//


// ## LGPrgObjectInit
//
// Initialises a new LGPrgObject. Takes ownership of log, which
// will be freed when deleted.
void LGPrgObjectInit(LGPrgObject * o, GLuint reference, GLboolean valid, GLchar * log)
{
	if (o)
	{
		o->reference = reference;
		o->valid = valid;
		o->log = log;
	}
}


// ## LGPrgObjectDestroy
//
// Destroys the contents of the LGPrgObject, also `free`ing any `log` string.
void LGPrgObjectDestroy(LGPrgObject * o)
{
	if (o)
	{
		free(o->log);
		memset(o, 0, sizeof(LGPrgObject));
	}
}


// ## LGPrgObjectCopy
//
// Copies `src` into `dst`, duplicating the log
// if it is present. This will not duplicate the
// OpenGL object that is being referred to.
void LGPrgObjectCopy(LGPrgObject *dst, const LGPrgObject *src)
{
	*dst = *src;
	if (src->log)
	{
		size_t len = strlen(src->log) + 1;
		dst->log = (GLchar *)malloc(len * sizeof(GLchar));
		if (dst->log)
		{
			strncpy(dst->log, src->log, len);
		}
		else
		{
			LGLogOOM("Could not allocate memory to copy log in LGPrgObjectCopy");
		}
	}
}


// ## LGPrgShaderNew
//
// Compiles the supplied shader string and returns a new program object. You
// are responsible for destroying the object with LGPrgObjectDestroy when
// you are finished with it.
//
// If any errors occur during creation, NULL is returned.
LGPrgObject * LGPrgShaderNew(const GLchar *source, GLenum type)
{
	LGLogGLErrors("Preparing to create and compile shader.");
	
	GLuint shader = glCreateShader(type);
	
	if (0 == shader)
	{
		LGLogGLErrors("Creating shader.");
		return NULL;
	}
	
	// Set the source code in the shader. We always treat the shader
	// source as 1 long NUL terminated string rather than array
	// of strings.
	glShaderSource(shader, 1, &source, NULL);
	if (GL_NO_ERROR != LGLogGLErrors("Setting shader source."))
	{
		glDeleteShader(shader);
		return NULL;
	}
	
    glCompileShader(shader);
	
	// If we get this far, even if there are errors we want
	// to continue as the log output may have some useful information
	LGLogGLErrors("Compiling shader.");
	
	GLint logLength;
	GLchar *log = NULL;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		log = (GLchar *)malloc(logLength * sizeof(GLchar));
		glGetShaderInfoLog(shader, logLength, NULL, log);
	}
	
	// Check the compile status, although still return a new
	// object even if there is an error. This is more for
	// debug/development use
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (GL_FALSE == status)
	{
		LGLogError("Could not compile shader. Log: \n%s", (log ? log : "<NO LOG>"));
	}
	
	// Finally, create the new LGPrgObject to return
	LGPrgObject * o = (LGPrgObject *)malloc(sizeof(LGPrgObject));
	if (o)
	{
		LGPrgObjectInit(o, shader, status, log);
	}
	else
	{
		glDeleteShader(shader);
		free(log);
		LGLogOOM("Out of memory when returning new shader object from LGPrgCompileShader");
	}
	return o;
}


// ## LGPrgShaderDestroy
//
// Destroys the shader contents, releasing the shader
// reference and the log.
void LGPrgShaderDestroy(LGPrgObject * shader)
{
	if (shader)
	{
		glDeleteShader(shader->reference);
		LGPrgObjectDestroy(shader);
	}
}


// ## LGPrgShaderDelete
//
// Delete the shader object, releasing alls resources
// `free`ing the LGPrgObject.
void LGPrgShaderDelete(LGPrgObject ** shader_)
{
	if (shader_)
	{
		LGPrgObject *shader = *shader_;
		if (shader)
		{
			LGPrgShaderDestroy(shader);
			free(shader);
		}
		*shader_ = NULL;
	}
}


#pragma mark - LGPrg
//
// # LGPrg methods
//


// ## LGPrgStoreActiveVariables
//
// Re-parses the program for active attributes and uniforms, storing their
// locations respective hashes for future reference.
void LGPrgStoreActiveVariables(LGPrg * prg)
{
	LGPrgVarHashClear(&prg->attributes);
	prg->attributes = LGPrgVarHashOfActiveAttributes(prg->program.reference);
	
	LGPrgVarHashClear(&prg->uniforms);
	prg->uniforms = LGPrgVarHashOfActiveAttributes(prg->program.reference);
}


// ## LGPrgNew
//
// Creates a new LGPrg object from the specified shaders. All program
// attribute and uniform locations are stored as hashes in `attributes`
// and `uniforms` respectively.
LGPrg * LGPrgNew(const LGPrgObject * vertexShader, const LGPrgObject * fragmentShader)
{
	if (!vertexShader || vertexShader->valid == GL_FALSE)
	{
		LGLogError("Cannot create new LGPrg: vertexShader is NULL or not valid.");
		return NULL;
	}
	
	if (!fragmentShader || fragmentShader->valid == GL_FALSE)
	{
		LGLogError("Cannot create new LGPrg: fragmentShader is NULL or not valid.");
		return NULL;
	}
	
	GLuint program = glCreateProgram();
	if (0 == program)
	{
		LGLogError("Cannot create new LGPrg: glCreateProgram failed.");
		return NULL;
	}
	
	glAttachShader(program, vertexShader->reference);
	LGLogGLErrors("Attached vertex shader.");
	
	glAttachShader(program, fragmentShader->reference);
	LGLogGLErrors("Attached fragment shader.");

	glLinkProgram(program);
	LGLogGLErrors("Linked program.");

	GLint logLength;
	GLchar *log = NULL;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		log = (GLchar *)malloc(logLength * sizeof(GLchar));
		glGetProgramInfoLog(program, logLength, NULL, log);
	}
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
	{
		LGLogError("Could not link program. Log: \n%s", (log ? log : "<NO LOG>"));
    }
	
	// Finally, create the new LGPrg to return
	LGPrg * prg = (LGPrg *)malloc(sizeof(LGPrg));
	if (prg)
	{
		memset(prg, 0, sizeof(LGPrg));
		
		LGPrgObjectInit(&prg->program, program, status, log);
		LGPrgObjectCopy(&prg->vertexShader, vertexShader);
		LGPrgObjectCopy(&prg->fragmentShader, fragmentShader);
		
		LGPrgStoreActiveVariables(prg);
	}
	else
	{
		glDeleteProgram(program);
		free(log);
		LGLogOOM("Out of memory when returning new LGPrg from LGPrgNew");
	}
	return prg;
}


// ## LGPrgNewFromSource
//
// Creates a new LGPrg from the supplied source strings
LGPrg * LGPrgNewFromSource(const char * vertexShader, const char * fragmentShader)
{
	LGPrgObject * vertex = LGPrgShaderNew(vertexShader, GL_VERTEX_SHADER);
	LGPrgObject * fragment = LGPrgShaderNew(fragmentShader, GL_FRAGMENT_SHADER);
	LGPrg * program = LGPrgNew(vertex, fragment);
	
	LGPrgShaderDelete(&vertex);
	LGPrgShaderDelete(&fragment);
	
	return program;
}


// ## LGPrgNewFromFiles
//
// Creates a new LGPrg from the contents of the specified files
LGPrg * LGPrgNewFromFiles(const char * vertexShaderPath, const char * fragmentShaderPath)
{
	char *vertexShader = LGFileToString(vertexShaderPath, NULL);
	char *fragmentShader = LGFileToString(fragmentShaderPath, NULL);
	LGPrg * program = LGPrgNewFromSource(vertexShader, fragmentShader);
	
	free(vertexShader);
	free(fragmentShader);
	
	return program;
}


// ## LGPrgAttribLocation
//
// Returns the location for the named attribute. If the
// attribute is not found, returns -1.
GLuint LGPrgAttribLocation(const LGPrg *prg, const char *name)
{
	if (prg)
	{
		return LGPrgVarLocation(prg->attributes, name);
	}
	else
	{
		return -1;
	}
}


// ## LGPrgUniformLocation
//
// Returns the location for the named uniform. If the
// attribute is not found, returns -1.
GLuint LGPrgUniformLocation(const LGPrg *prg, const char *name)
{
	if (prg)
	{
		return LGPrgVarLocation(prg->attributes, name);
	}
	else
	{
		return -1;
	}
}


// ## LGPrgDelete
//
// Deletes the program, releasing all resources and `free`ing
// the LGPrg object
void LGPrgDelete(LGPrg ** prg_)
{
	if (prg_)
	{
		LGPrg *prg = *prg_;
		if (prg)
		{
			glDetachShader(prg->program.reference, prg->vertexShader.reference);
			LGPrgShaderDestroy(&prg->vertexShader);

			glDetachShader(prg->program.reference, prg->fragmentShader.reference);
			LGPrgShaderDestroy(&prg->fragmentShader);
			
			glDeleteProgram(prg->program.reference);
			LGPrgObjectDestroy(&prg->program);
			
			LGPrgVarHashClear(&prg->attributes);
			LGPrgVarHashClear(&prg->uniforms);
			
			memset(prg, 0, sizeof(LGPrg));
			free(prg);
		}
		*prg_ = NULL;
	}
}
