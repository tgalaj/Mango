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
    #define MG_ASSERTS_ENABLED
#endif
namespace mango
{
    namespace asserts
    {
        enum FailBehavior
        {
            Halt,
            Continue,
        };

        enum Type
        {
            Core,
            Application
        };

        using Handler = FailBehavior(*)(Type type,
                                        const char* condition,
                                        const char* msg,
                                        const char* file,
                                        int         line);

        Handler getHandler();
        void setHandler(Handler newHandler);

        FailBehavior reportFailure(Type type,
                                   const char* condition,
                                   const char* file,
                                   int         line,
                                   const char* msg, ...);
    }
}

#ifdef _MSC_VER
    #define MG_HALT() __debugbreak()
#else
    #define MG_HALT()
#endif

#define MG_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef MG_ASSERTS_ENABLED
#define MG_ASSERT_INTERNAL_IMPL(type, cond) \
        do \
        { \
            if (!(cond)) \
            { \
                if (mango::asserts::reportFailure(type, #cond, __FILE__, __LINE__, 0) == \
                    mango::asserts::Halt) \
                    MG_HALT(); \
            } \
        } while(0)

#define MG_ASSERT_MSG_INTERNAL_IMPL(type, cond, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                if (mango::asserts::reportFailure(type, #cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__) == \
                    mango::asserts::Halt) \
                    MG_HALT(); \
            } \
        } while(0)

#define MG_ASSERT_FAIL_INTERNAL_IMPL(type, msg, ...) \
        do \
        { \
            if (mango::asserts::reportFailure(type, 0, __FILE__, __LINE__, (msg), ##__VA_ARGS__) == \
                mango::asserts::Halt) \
            MG_HALT(); \
        } while(0)

#define MG_VERIFY_INTERNAL_IMPL(type, cond)               MG_ASSERT_INTERNAL_IMPL(type, cond)
#define MG_VERIFY_MSG_INTERNAL_IMPL(type, cond, msg, ...) MG_ASSERT_MSG_INTERNAL_IMPL(type, cond, msg, ##__VA_ARGS__)
#else
#define MG_ASSERT_INTERNAL_IMPL(type, condition) \
        do { MG_UNUSED(condition); } while(0)
#define MG_ASSERT_MSG_INTERNAL_IMPL(type, condition, msg, ...) \
        do { MG_UNUSED(condition); MG_UNUSED(msg); } while(0)
#define MG_ASSERT_FAIL_INTERNAL_IMPL(type, msg, ...) \
        do { MG_UNUSED(msg); } while(0)
#define MG_VERIFY_INTERNAL_IMPL(type, cond) (void)(cond)
#define MG_VERIFY_MSG_INTERNAL_IMPL(type, cond, msg, ...) \
        do { (void)(cond); MG_UNUSED(msg); } while(0)
#endif

#define MG_CORE_ASSERT(cond)               MG_ASSERT_INTERNAL_IMPL(mango::asserts::Core, cond)
#define MG_CORE_ASSERT_MSG(cond, msg, ...) MG_ASSERT_MSG_INTERNAL_IMPL(mango::asserts::Core, cond, msg, ##__VA_ARGS__)
#define MG_CORE_ASSERT_FAIL(msg, ...)      MG_ASSERT_FAIL_INTERNAL_IMPL(mango::asserts::Core, msg, ##__VA_ARGS__)
#define MG_CORE_VERIFY(cond)               MG_VERIFY_INTERNAL_IMPL(mango::asserts::Core, cond)
#define MG_CORE_VERIFY_MSG(cond, msg, ...) MG_VERIFY_MSG_INTERNAL_IMPL(mango::asserts::Core, cond, msg, ##__VA_ARGS__)

#define MG_ASSERT(cond)               MG_ASSERT_INTERNAL_IMPL(mango::asserts::Application, cond)
#define MG_ASSERT_MSG(cond, msg, ...) MG_ASSERT_MSG_INTERNAL_IMPL(mango::asserts::Application, cond, msg, ##__VA_ARGS__)
#define MG_ASSERT_FAIL(msg, ...)      MG_ASSERT_FAIL_INTERNAL_IMPL(mango::asserts::Application, msg, ##__VA_ARGS__)
#define MG_VERIFY(cond)               MG_VERIFY_INTERNAL_IMPL(mango::asserts::Application, cond)
#define MG_VERIFY_MSG(cond, msg, ...) MG_VERIFY_MSG_INTERNAL_IMPL(mango::asserts::Application, cond, msg, ##__VA_ARGS__)

#define MG_STATIC_ASSERT(x) typedef char mangoStaticAssert[(x) ? 1 : -1];