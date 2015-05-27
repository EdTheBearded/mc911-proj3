#include "llvm/IR/Instructions.h"
#include "Liveness.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;

void print_elem(const Value* i) {
  errs() << i->getName() << " ";
}

bool Liveness::isLiveOut(Instruction *I, Value *V){
    return false;
}

void Liveness::computeBBDefUse(Function &F){
	//iterators and end mark
	Function::iterator fi, fe;
	BasicBlock::iterator bi, be;
	LivenessInfo *liv;

	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		liv = new LivenessInfo;
		for(bi = fi->begin(), be = fi->end(); bi != be; bi++){
			if(bi->getName() != "\0")
				liv->def.insert(bi);
			for(int i = 0, e = bi->getNumOperands(); i != e; i++){
				if(isa<Function>(bi->getOperand(i)))
					continue; 
				if(bi->getOperand(i)->getName() != "\0")
					liv->use.insert(bi->getOperand(i));
			}
		}
		bbLivenessMap[&*fi] = *liv;
	}
}

void Liveness::check(Function &F){

	std::set<const Value *>::iterator si, se;
	Function::iterator fi, fe;
	BasicBlock::reverse_iterator bi, be;
	LivenessInfo liv;
	
	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		errs() << "Basic Block:\n" << *fi << "\n";
		liv = bbLivenessMap[fi];
		errs() << "#######################\n";
		errs() << "defs\n";
		for(si = liv.def.begin(), se = liv.def.end(); si != se; si++){
			errs() << (*si)->getName() << "\n";	
		}
		errs() << "#######################\n" << "use\n";
		for(si = liv.use.begin(), se = liv.use.end(); si != se; si++){
			errs() << (*si)->getName() << "\n";	
		}
		errs() << "#######################\n" << "#######################\n";
		std::cin.get();
	}

}

void Liveness::computeBBInOut(Function &F){

	std::set<const Value *> inp;
    std::set<const Value *> outp;
	std::set<const Value *> temp1, temp2;
	std::set<const Value *>::iterator it;
	Function::iterator i, e;
	LivenessInfo *liv;
	bool cont = false;

	inp.clear();
	outp.clear();
	do{
		for(i = F.begin(), e = F.end(); i != e; i++){
			liv = &bbLivenessMap[&*i];
			inp = liv->in;
			outp = liv->out;
			temp1.clear();
			it = std::set_difference(liv->out.begin(), liv->out.end(), liv->def.begin(), liv->def.end(), temp1.begin());
			temp2.clear();
			it = std::set_union(liv->in.begin(), liv->in.end(), temp1.begin(), temp1.end(), temp2.begin);
			liv->in = temp2;
			temp1.clear();
			temp2.clear();
			cont = false;
			for(auto pi = pred_begin(&*i), pe = pred_end(&*i); i != e; i++){
				BasicBlock *pred = *pi;
				liv = &bbLivenessMap[pred];
				std::set_union
				(liv->in.begin(), liv->in.end(), temp1.begin(), temp1.end(), temp2.begin());
				temp1.clear();
				temp1 = temp2;
				if(inp != liv->in && outp != liv->out)
					cont = true;
			}
		}	
	} while(cont);

}

void Liveness::computeIInOut(Function &F) {
}

bool Liveness::runOnFunction(Function &F) {
	computeBBDefUse(F);
 	#ifdef DEBUG
	check(F);
	#endif
 	computeBBInOut(F);
    computeIInOut(F);
	return false;
}

char Liveness::ID = 0;

RegisterPass<Liveness> X("liveness", "Live vars analysis", false, false);





