//===-- FencesPass.cpp ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


#include "FencesPass.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

char FencesPass::ID = 0;

static unsigned PlaceFencesInBlock(BasicBlock &BB) {
  unsigned Count = 0;
  for (Instruction &I : BB) {
    Value *Ptr = nullptr;
    Instruction *InsertPt = nullptr;
    switch (I.getOpcode()) {
    case Instruction::Load: {
      LoadInst *LI = dyn_cast<LoadInst>(&I);
      Ptr = LI->getPointerOperand();
      InsertPt = LI->getNextNode();
    } break;
    case Instruction::Store: {
      StoreInst *SI = dyn_cast<StoreInst>(&I);
      Ptr = SI->getPointerOperand();
      InsertPt = SI;
    } break;
    default: break;
    }
    if (Ptr!=nullptr) {
      while (isa<GetElementPtrInst>(Ptr) || isa<CastInst>(Ptr)) {
        if (auto *GEP = dyn_cast<GetElementPtrInst>(Ptr)) {
          Ptr = GEP->getPointerOperand();
        }
        if (auto *Cast = dyn_cast<CastInst>(Ptr)) {
          Ptr = Cast->getOperand(0);
        }
      }
      if (!isa<AllocaInst>(Ptr)) {
        IRBuilder<> Builder(InsertPt);
        Builder.CreateFence(AtomicOrdering::SequentiallyConsistent);
        Count++;
      }
    }
  }
  return Count;
}

static void OptimizeFencesAway(BasicBlock &BB) {
  bool HasEquivalentFence = false;
  for (auto It = BB.begin(), E=BB.end(); It!=E;) {
    Instruction *I = &*It;
    It++;

    switch(I->getOpcode()) {
    case Instruction::Fence:
      if (auto *F = dyn_cast<FenceInst>(I)) {
	if (F->getOrdering()==AtomicOrdering::SequentiallyConsistent) {
          if (HasEquivalentFence) {
            I->eraseFromParent();
          }
          HasEquivalentFence = true;
	} else 
          HasEquivalentFence = false;
      }
      break;
    default:
      if (I->mayReadOrWriteMemory()) {
        HasEquivalentFence = false;
      }
    }
  }
}

bool FencesPass::runOnFunction(Function &F) {
  if (!F.isDeclaration()) {
    errs() << "optimize-fences: ";
    if (OptimizeFences) {
      errs() << "true\n";
    } else {
      errs() << "false\n";
    }
    for (BasicBlock &BB : F) {
      PlaceFencesInBlock(BB);
      if (OptimizeFences)
        OptimizeFencesAway(BB);
    }
  }
  return true;
}

void FencesPass::getAnalysisUsage(AnalysisUsage &AU) const {}

