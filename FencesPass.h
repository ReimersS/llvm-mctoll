//===-- FencesPass.h ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLVM_MCTOLL_FENCESPASS_H
#define LLVM_TOOLS_LLVM_MCTOLL_FENCESPASS_H

#include "llvm/Bitcode/BitcodeWriterPass.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

class FencesPass : public FunctionPass {
  bool OptimizeFences;
public:
  static char ID;
  FencesPass(bool OptimizeFences=false)
      : FunctionPass(ID), OptimizeFences(OptimizeFences) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override;
};
#endif // LLVM_TOOLS_LLVM_MCTOLL_FENCESPASS_H
