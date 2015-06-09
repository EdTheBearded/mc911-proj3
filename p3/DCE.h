#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Instructions.h"
#include <set>
#include <list>
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/IntrinsicInst.h"

#include "Liveness.h"

#define DEBUG

using namespace llvm;

namespace llvm {

class DCE: public FunctionPass {
private:
	
    DenseMap<const Instruction*, LivenessInfo> iLivenessMap;
    DenseMap<const BasicBlock*, LivenessInfo> bbLivenessMap;
    DenseMap<const Instruction*, int> instMap;


public:
	static char ID; 
	DCE() : FunctionPass(ID) {}

	virtual bool runOnFunction(Function &F);
    void computeBBDefUse(Function &F);
    void computeBBInOut(Function &F);

    void computeIInOut(Function &F);

    bool isLiveOut(Instruction *I, Value *V);
    void addToMap(Function &F);

	virtual void getAnalysisUsage(AnalysisUsage &AU) const{
		AU.addRequired<Liveness>();
	}



};

}


