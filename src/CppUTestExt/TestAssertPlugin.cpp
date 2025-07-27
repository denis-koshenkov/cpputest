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

static bool isAssertionExpectedToFail_ = false;
static bool assertionFailed_ = false;

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
        /* Test exited right after assertion failed. Fail the test if assertion was not expected to fail. */
        if (!isAssertionExpectedToFail_) {
            const SimpleString message = "Assertion failed, but no assertions expected to fail";
            result.addFailure(TestFailure(&test, message));
        }
    } else {
        /* Test ran to the end, no assertions failed. Fail the test if assertion was expected to fail.*/
        if (isAssertionExpectedToFail_) {
            const SimpleString message = "Expected an assertion to fail, but no assertions failed";
            result.addFailure(TestFailure(&test, message));
        }
    }
}

void TestAssertPlugin::expectAssertion()
{
    isAssertionExpectedToFail_ = true;
}

void TestAssertPlugin::assert() {
    assertionFailed_ = true;
    UtestShell::getCurrent()->exitTest();
}
