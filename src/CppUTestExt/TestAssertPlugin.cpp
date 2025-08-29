/*
 * Copyright (c) 2025, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/TestAssertPlugin.h"

static constexpr size_t MAX_ASSERTION_TEXT_LENGTH = 100;
static const char ASSERTION_TEXT_OVERFLOW_MESSAGE[] = "Your assertion did not fit into buffer, use a smaller one";

static constexpr size_t MAX_ASSERTION_FUNCTION_NAME_TEXT_LENGTH = 100;
static const char ASSERTION_FUNCTION_NAME_OVERFLOW_MESSAGE[] = "Your assertion function name did not fit into buffer, use a smaller one";

static bool isAssertionExpectedToFail_ = false;
static bool assertionFailed_ = false;
static char expectedAssertionText[MAX_ASSERTION_TEXT_LENGTH + 1]; // +1 for NULL terminator
static char actualAssertionText[MAX_ASSERTION_TEXT_LENGTH + 1]; // +1 for NULL terminator
static char expectedAssertionFunctionName[MAX_ASSERTION_FUNCTION_NAME_TEXT_LENGTH + 1]; // +1 for NULL terminator
static char actualAssertionFunctionName[MAX_ASSERTION_FUNCTION_NAME_TEXT_LENGTH + 1]; // +1 for NULL terminator

TestAssertPlugin::TestAssertPlugin(const SimpleString& name) : TestPlugin(name) {}

TestAssertPlugin::~TestAssertPlugin() {}

void TestAssertPlugin::preTestAction(UtestShell&, TestResult&)
{
    isAssertionExpectedToFail_ = false;
    assertionFailed_ = false;
}

void TestAssertPlugin::postTestAction(UtestShell& test, TestResult& result)
{
    if (assertionFailed_) {
        /* Test exited right after assertion failed. Fail the test if no assertions were expected to fail,
        or if not the expected assertion failed. */
        if (!isAssertionExpectedToFail_) {
            SimpleString message = "Assertion \"";
            message += actualAssertionText;
            message += "\" in function \"";
            message += actualAssertionFunctionName;
            message += "\" failed, but no assertions expected to fail";
            result.addFailure(TestFailure(&test, message));
        } else if ((SimpleString::StrCmp(expectedAssertionText, actualAssertionText) != 0)
                   || (SimpleString::StrCmp(expectedAssertionFunctionName, actualAssertionFunctionName) != 0)) {
            /* Assertion failed, but not the one that was expected */
            SimpleString message = "Assertion failed, but not the expected one. Expected \"";
            message += expectedAssertionText;
            message += "\" in function \"";
            message += expectedAssertionFunctionName;
            message += "\" to fail, but \"";
            message += actualAssertionText;
            message += "\" in function \"";
            message += actualAssertionFunctionName;
            message += "\" failed.";
            result.addFailure(TestFailure(&test, message));
        }
    } else {
        /* Test ran to the end, no assertions failed. Fail the test if assertion was expected to fail.*/
        if (isAssertionExpectedToFail_) {
            SimpleString message = "Expected assertion \"";
            message += expectedAssertionText;
            message += "\" to fail, but no assertions failed";
            result.addFailure(TestFailure(&test, message));
        }
    }
}

void TestAssertPlugin::expectAssertion(const char *assertion, const char *function_name)
{
    isAssertionExpectedToFail_ = true;
    if (SimpleString::StrLen(assertion) > MAX_ASSERTION_TEXT_LENGTH) {
        assertion = ASSERTION_TEXT_OVERFLOW_MESSAGE;
    }
    if (SimpleString::StrLen(function_name) > MAX_ASSERTION_FUNCTION_NAME_TEXT_LENGTH) {
        assertion = ASSERTION_FUNCTION_NAME_OVERFLOW_MESSAGE;
    }
    /* +1 to also copy the NULL terminator */
    SimpleString::StrNCpy(expectedAssertionText, assertion, SimpleString::StrLen(assertion) + 1);
    SimpleString::StrNCpy(expectedAssertionFunctionName, function_name, SimpleString::StrLen(function_name) + 1);
}

void TestAssertPlugin::assert(const char *assertion, const char *function_name) {
    assertionFailed_ = true;
    if (SimpleString::StrLen(assertion) > MAX_ASSERTION_TEXT_LENGTH) {
        assertion = ASSERTION_TEXT_OVERFLOW_MESSAGE;
    }
    if (SimpleString::StrLen(function_name) > MAX_ASSERTION_FUNCTION_NAME_TEXT_LENGTH) {
        assertion = ASSERTION_FUNCTION_NAME_OVERFLOW_MESSAGE;
    }
    /* +1 to also copy the NULL terminator */
    SimpleString::StrNCpy(actualAssertionText, assertion, SimpleString::StrLen(assertion) + 1);
    SimpleString::StrNCpy(actualAssertionFunctionName, function_name, SimpleString::StrLen(function_name) + 1);
    UtestShell::getCurrent()->exitTest();
}
