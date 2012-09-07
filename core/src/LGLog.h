// # LGLog
//
// Simple logging to stdout.
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

#ifndef LGLog_h
#define LGLog_h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum
{
	LGLogLevelDebug,
	LGLogLevelInfo,
	LGLogLevelWarn,
	LGLogLevelError,
	LGLogLevelOOM
}
LGLogLevel;

/**
 * Variadic logging function. Generally this is only accessed
 * through the LGLog* macros.
 */
void _LGvLog(LGLogLevel level, const char *fmt, ...);

/**
 * Sets the level of logging. You may entirely disable
 * logging by defining DISABLE_LGLog
 */
void LGSetLogLevel(LGLogLevel level);

#ifdef DISABLE_LGLog
#	define LGLogDebug(format, ...)
#	define LGLogInfo(format, ...)
#	define LGLogWarn(format, ...)
#	define LGLogError(format, ...)
#	define LGLogOOM(message)
#else
#	define LGLogDebug(format, ...) _LGvLog(LGLogLevelDebug, ("[%s:%d] " format "\n"), __FILE__, __LINE__, ## __VA_ARGS__)
#	define LGLogInfo(format, ...) _LGvLog(LGLogLevelInfo, ("[%s:%d] " format "\n"), __FILE__, __LINE__, ## __VA_ARGS__)
#	define LGLogWarn(format, ...) _LGvLog(LGLogLevelWarn, ("[%s:%d] " format "\n"), __FILE__, __LINE__, ## __VA_ARGS__)
#	define LGLogError(format, ...) _LGvLog(LGLogLevelError, ("[%s:%d] " format "\n"), __FILE__, __LINE__, ## __VA_ARGS__)
#	define LGLogOOM(message) _LGvLog(LGLogLevelOOM, "OUT OF MEMORY [" __FILE__ "] " message "\n")
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LGLog_h
