#include "llvm/IR/Instructions.h"
#include "DCE.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;

void print_elem(const Value* i) {
  errs() << i->getName() << " ";
}

bool DCE::runOnFunction(Function &F) {

	int r = 0;
	bool cont = false;

	do{
		cont = false;
		Liveness &L = getAnalysis<Liveness>();
		
		inst_iterator i, e;
		i = inst_begin(F);
		e = inst_end(F);

		for(; i != e; i++){
			if(!(L.isLiveOut((&*i), (&*i))) && !(i->mayHaveSideEffects()) &&
				!(isa<TerminatorInst>(&*i)) && !(isa<DbgInfoIntrinsic>(&*i)) &&
				!(isa<LandingPadInst>(&*i))){
				i->dropAllReferences();
				i->eraseFromParent();
				r++;
				cont = true;
				break;
			}
		}
	} while (cont);
errs() << "proof " << r << "\n";
	#ifdef DEBUG
	//checki(F);
	//checkbb(F);
	#endif
	return true;
}

char DCE::ID = 0;

static RegisterPass<DCE> X("dce-liveness", "Dead code elimination", false, false);





