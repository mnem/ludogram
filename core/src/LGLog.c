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


#include <stdio.h>
#include <stdarg.h>

#include "LGLog.h"

// Cheap, nasty and non threadsafe global for specifying
// the log level
static LGLogLevel gLGMinimumLogLevel = LGLogLevelDebug;


// ## _LGvLog
//
// Variable argument logging function. Generally never called
// directly, but instead through one of the logging macros.
void _LGvLog(LGLogLevel level, const char *fmt, ...)
{
	if (level >= gLGMinimumLogLevel)
	{
		va_list ap;
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
	}
}


// ## LGSetLogLevel
//
// Sets the amount of logging. Higher numbers log less, but
// you should be passing a LGLogLevel enum anyway.
void LGSetLogLevel(LGLogLevel level)
{
	gLGMinimumLogLevel = level;
}
