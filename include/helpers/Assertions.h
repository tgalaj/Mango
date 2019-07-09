/*
* Copyright (c) 2008, Power of Two Games LLC
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Power of Two Games LLC nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY POWER OF TWO GAMES LLC ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL POWER OF TWO GAMES LLC BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef NDEBUG
    #define VERTEX_ASSERTS_ENABLED
#endif
namespace Vertex
{
    namespace Assert
    {
        enum FailBehavior
        {
            Halt,
            Continue,
        };

        typedef FailBehavior(*Handler)(const char* condition,
                                       const char* msg,
                                       const char* file,
                                       int         line);

        Handler getHandler();
        void setHandler(Handler newHandler);

        FailBehavior reportFailure(const char* condition,
                                   const char* file,
                                   int         line,
                                   const char* msg, ...);
    }
}

#ifdef _MSC_VER
    #define VERTEX_HALT() __debugbreak()
#else
    #define VERTEX_HALT()
#endif

#define VERTEX_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef VERTEX_ASSERTS_ENABLED
#define VERTEX_ASSERT(cond) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (Vertex::Assert::reportFailure(#cond, __FILE__, __LINE__, 0) == \
					Vertex::Assert::Halt) \
					VERTEX_HALT(); \
			} \
		} while(0)

#define VERTEX_ASSERT_MSG(cond, msg, ...) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (Vertex::Assert::reportFailure(#cond, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
					Vertex::Assert::Halt) \
					VERTEX_HALT(); \
			} \
		} while(0)

#define VERTEX_ASSERT_FAIL(msg, ...) \
		do \
		{ \
			if (Vertex::Assert::reportFailure(0, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
				Vertex::Assert::Halt) \
			VERTEX_HALT(); \
		} while(0)

#define VERTEX_VERIFY(cond) VERTEX_ASSERT(cond)
#define VERTEX_VERIFY_MSG(cond, msg, ...) VERTEX_ASSERT_MSG(cond, msg, ##__VA_ARGS__)
#else
#define VERTEX_ASSERT(condition) \
		do { VERTEX_UNUSED(condition); } while(0)
#define VERTEX_ASSERT_MSG(condition, msg, ...) \
		do { VERTEX_UNUSED(condition); VERTEX_UNUSED(msg); } while(0)
#define VERTEX_ASSERT_FAIL(msg, ...) \
		do { VERTEX_UNUSED(msg); } while(0)
#define VERTEX_VERIFY(cond) (void)(cond)
#define VERTEX_VERIFY_MSG(cond, msg, ...) \
		do { (void)(cond); VERTEX_UNUSED(msg); } while(0)
#endif

#define VERTEX_STATIC_ASSERT(x) \
	typedef char vertexStaticAssert[(x) ? 1 : -1];