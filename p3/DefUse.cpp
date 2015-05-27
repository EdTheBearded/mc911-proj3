#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  struct DefUse : public FunctionPass {
    static char ID;
    DefUse() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
	errs() << F.getName() << "\n";
	inst_iterator I, E, aux;
	for (I = inst_begin(F), E = inst_end(F); I != E; ++I){
		if(!(I->getName().empty())){
			if(I->hasNUses(0) && !(I->mayHaveSideEffects()) && !(I->isTerminator()) && !(isa<DbgInfoIntrinsic>(*I)) && !(isa<LandingPadInst>(*I))){
			//if(I->hasNUses(0)){
			errs() << I->getName() << " is used " << I->getNumUses() << " time(s)\n";
			/*for (Use &U : pi->operands()) {
  				Value *v = U.get();
   			}*/
			/*for (User *U : I->users()) {
				if (Instruction *Inst = dyn_cast<Instruction>(U)) {
					errs() << *Inst << "\n";
				}
			}*/
			//aux = I;
			//I++;
			//aux->eraseFromParent();
			errs() << "###########################################\n";
		}
		}
	}
      	// W: uma lista de variaveis da função
	// enquanto W não esta vazio
		// remove uma variavel v de W
		// se a lista de usos de v é vazia -> Value::hasNUses(), Value::getNumUses()
			// seja s o statement onde v é declarada
			// se s não tem efeitos colaterais ->
				// delete s da função
				// para cada variavel u usada por s
					// delete s da lista de usos de u
					// insira u em W  
	
      return false;
    }
  };
}

char DefUse::ID = 0;
static RegisterPass<DefUse> X("def-use", "Def-use chain Pass", false, false);
