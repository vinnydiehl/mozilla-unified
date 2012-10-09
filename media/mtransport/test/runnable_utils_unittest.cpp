/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: ekr@rtfm.com
#include <iostream>

#include "prio.h"

#include "nsCOMPtr.h"
#include "nsNetCID.h"
#include "nsXPCOM.h"
#include "nsXPCOMGlue.h"

#include "nsIComponentManager.h"
#include "nsIComponentRegistrar.h"
#include "nsIIOService.h"
#include "nsIServiceManager.h"
#include "nsISocketTransportService.h"

#include "nsASocketHandler.h"
#include "nsServiceManagerUtils.h"
#include "nsThreadUtils.h"

#include "runnable_utils.h"
#include "mtransport_test_utils.h"

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

using namespace mozilla;
MtransportTestUtils test_utils;

namespace {

class TargetClass {
 public:
  TargetClass(int *ran) : ran_(ran) {}

  void m1(int x) {
    std::cerr << __FUNCTION__ << " " << x << std::endl;
    *ran_ = 1;
  }

  void m2(int x, int y) {
    std::cerr << __FUNCTION__ << " " << x << " " << y << std::endl;
    *ran_ = 2;
  }

  void m1set(bool *z) {
    std::cerr << __FUNCTION__ << std::endl;
    *z = true;
  }
  int return_int(int x) {
    std::cerr << __FUNCTION__ << std::endl;
    return x;
  }

  int *ran_;
};

class RunnableArgsTest : public ::testing::Test {
 public:
  RunnableArgsTest() : ran_(0), cl_(&ran_){}

  void Test1Arg() {
    nsRunnable * r = WrapRunnable(&cl_, &TargetClass::m1, 1);
    r->Run();
    ASSERT_EQ(1, ran_);
  }

  void Test2Args() {
    nsRunnable* r = WrapRunnable(&cl_, &TargetClass::m2, 1, 2);
    r->Run();
    ASSERT_EQ(2, ran_);
  }

 private:
  int ran_;
  TargetClass cl_;
};

class DispatchTest : public ::testing::Test {
 public:
  DispatchTest() : ran_(0), cl_(&ran_) {}

  void SetUp() {
    nsresult rv;
    target_ = do_GetService(NS_SOCKETTRANSPORTSERVICE_CONTRACTID, &rv);
    ASSERT_TRUE(NS_SUCCEEDED(rv));
  }

  void Test1Arg() {
    nsRunnable* r = WrapRunnable(&cl_, &TargetClass::m1, 1);
    target_->Dispatch(r, NS_DISPATCH_SYNC);
    ASSERT_EQ(1, ran_);
  }

  void Test2Args() {
    nsRunnable* r = WrapRunnable(&cl_, &TargetClass::m2, 1, 2);
    target_->Dispatch(r, NS_DISPATCH_SYNC);
    ASSERT_EQ(2, ran_);
  }

  void Test1Set() {
    bool x = false;
    target_->Dispatch(WrapRunnable(&cl_, &TargetClass::m1set, &x),
                      NS_DISPATCH_SYNC);
    ASSERT_TRUE(x);
  }

  void TestRet() {
    int z;
    int x = 10;

    target_->Dispatch(WrapRunnableRet(&cl_, &TargetClass::return_int, x, &z),
                      NS_DISPATCH_SYNC);
    ASSERT_EQ(10, z);
  }

 private:
  int ran_;
  TargetClass cl_;
  nsCOMPtr<nsIEventTarget> target_;
};


TEST_F(RunnableArgsTest, OneArgument) {
  Test1Arg();
}

TEST_F(RunnableArgsTest, TwoArguments) {
  Test2Args();
}

TEST_F(DispatchTest, OneArgument) {
  Test1Arg();
}

TEST_F(DispatchTest, TwoArguments) {
  Test2Args();
}

TEST_F(DispatchTest, Test1Set) {
  Test1Set();
}

TEST_F(DispatchTest, TestRet) {
  TestRet();
}


} // end of namespace


int main(int argc, char **argv) {
    test_utils.InitServices();

  // Start the tests
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

