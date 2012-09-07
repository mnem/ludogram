// # LGPrg
//
// Functions and data structures relating to the creation and compilation
// of OpenGL shader programs.
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

#ifndef LGPrg_h
#define LGPrg_h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
 * Variadic logging function which also checks glGetError. Generally this is
 * only accessed through the LGLogGLErrors macro.
 *
 * Will only display output if there is an error.
 */
GLenum _LGvLogGLErrors(const char *fmt, ...);

#ifdef DISABLE_LGLog
#	define LGLogGLErrors(format, ...)
#else
#	define LGLogGLErrors(format, ...) _LGvLogGLErrors(("[%s:%d] " format "\n"), __FILE__, __LINE__, ## __VA_ARGS__)
#endif

/**
 * Create a new LGPrg object from the specified shaders.
 *
 * When you are finished with the program, delete it with LGPrgDelete.
 *
 * @param vertexShader vertex shader to use in the program
 * @param vertexShader fragment shader to use in the program
 *
 * @return new LGPrg object. NULL may be returned if either of the shaders
 *         are invalid. If an object is returned, you must check if
 *         LGPrg->program.valid is GL_TRUE before using the program. If
 *         it is false, more information about why the compilation failed
 *         may be contained in LGPrg->program.log.
 */
extern LGPrg * LGPrgNew(const LGPrgObject * vertexShader, const LGPrgObject * fragmentShader);

/**
 * Create a new LGPrg object from the specified shader source strings.
 *
 * This is a convienience function wrapping LGPrgNew.
 */
extern LGPrg * LGPrgNewFromSource(const char * vertexShader, const char * fragmentShader);

/**
 * Create a new LGPrg object from the specified shader source files.
 *
 * This is a convienience function wrapping LGPrgNew.
 */
extern LGPrg * LGPrgNewFromFiles(const char * vertexShaderPath, const char * fragmentShaderPath);

/**
 * Deletes and frees all resources associated with an LGPrg object.
 */
extern void LGPrgDelete(LGPrg **prg);

/**
 * Returns the location of the named uniform for the program.
 */
extern GLuint LGPrgUniformLocation(const LGPrg *prg, const char *name);
	
/**
 * Returns the location of the named attribute for the program.
 */
extern GLuint LGPrgAttribLocation(const LGPrg *prg, const char *name);

/**
 * Creates a new shader from the specified source string.
 */
extern LGPrgObject* LGPrgShaderNew(const GLchar *source, GLenum type);
	
/**
 * Deletes and frees all resources associated with an LGPrg object.
 */
extern void LGPrgShaderDelete(LGPrgObject **shader);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LGPrg_h
