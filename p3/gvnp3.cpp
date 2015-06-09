#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"



using namespace llvm;

namespace {

	struct node_t{
		Value **def;
		Value **use;
		struct node_t *next;
	};

  	struct gvn : public FunctionPass {
    	static char ID;
    	gvn() : FunctionPass(ID) {}

    	virtual bool runOnFunction(Function &F) {

			//iterators and end mark
			Function::iterator fi, fe;
			BasicBlock::iterator bn, bi, be;
			inst_iterator ii, ie;
			bool ret = false;

    		for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
				for(bi = fi->begin(), be = fi->end(); bi != be; bi++){
					bn = bi;
					bn = ++bn;
					if(bn == be) break;
					if(bi->getOpcode() == Instruction::Store &&
					   bn->getOpcode() == Instruction::Load){
						if(bi->getOperand(1) == bn->getOperand(0)){
							errs() << F.getName() << "\n";
							bn->replaceAllUsesWith(bi->getOperand(0));
							bn->eraseFromParent();
							ret = true;
						}
					}
				}
			}
			
			return ret;
		}
	};
}

char gvn::ID = 0;
static RegisterPass<gvn> X("gvn-p3", "Exercicio 3", false, false);
