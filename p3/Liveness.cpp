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
	return (iLivenessMap[&*I].out.find(&*V) != iLivenessMap[&*I].out.end());
}

void Liveness::computeBBDefUse(Function &F){
	//iterators and end mark
	Function::iterator fi, fe;
	BasicBlock::iterator bi, be;
	LivenessInfo *livb, *livi;

	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		livb = new LivenessInfo;
		for(bi = fi->begin(), be = fi->end(); bi != be; bi++){
			livi = new LivenessInfo;
			if(bi->getName() != "\0"){
				livb->def.insert(bi);
				livi->def.insert(bi);
			}
			for(int i = 0, e = bi->getNumOperands(); i != e; i++){
				if(isa<Function>(bi->getOperand(i)) || isa<Constant>(bi->getOperand(i)))
					continue; 
				if(bi->getOperand(i)->getName() != "\0"){
					livb->use.insert(bi->getOperand(i));
					livi->use.insert(bi->getOperand(i));
				}
			}
			iLivenessMap[&*bi] = *livi;
		}
		bbLivenessMap[&*fi] = *livb;
	}
}

void Liveness::checkbb(Function &F){

	std::set<const Value *>::iterator si, se;
	Function::iterator fi, fe;
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
		errs() << "#######################\n" << "in\n";
		for(si = liv.in.begin(), se = liv.in.end(); si != se; si++){
			errs() << (*si)->getName() << "\n";	
		}
		errs() << "#######################\n" << "out\n";
		for(si = liv.out.begin(), se = liv.out.end(); si != se; si++){
			errs() << (*si)->getName() << "\n";	
		}

		errs() << "#######################\n" << "#######################\n";
		std::cin.get();
	}

}

void Liveness::checki(Function &F){

	std::set<const Value *>::iterator si, se;
	Function::iterator fi, fe;
	BasicBlock::iterator bi, be;
	LivenessInfo liv, bliv;
	
	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		bliv = bbLivenessMap[&*fi];
		for(bi = fi->begin(), be = fi->end(); bi != be; bi++){
			errs() << "in\n";
			for(si = bliv.in.begin(), se = bliv.in.end(); si != se; si++){
				errs() << (*si)->getName() << "\n";	
			}
			errs() << "Instruction:\n" << *bi << "\n";

			liv = iLivenessMap[&*bi];
			errs() << "#######################\n";
			errs() << "defs\n";
			for(si = liv.def.begin(), se = liv.def.end(); si != se; si++){
				errs() << (*si)->getName() << "\n";	
			}
			errs() << "#######################\n" << "use\n";
			for(si = liv.use.begin(), se = liv.use.end(); si != se; si++){
				errs() << (*si)->getName() << "\n";	
			}
			errs() << "#######################\n" << "in\n";
			for(si = liv.in.begin(), se = liv.in.end(); si != se; si++){
				errs() << (*si)->getName() << "\n";	
			}
			errs() << "#######################\n" << "out\n";
			for(si = liv.out.begin(), se = liv.out.end(); si != se; si++){
				errs() << (*si)->getName() << "\n";	
			}

			errs() << "#######################\n" << "#######################\n";
			std::cin.get();
		}
	}

}

void Liveness::computeBBInOut(Function &F) {

	std::set<const Value *> inp;
    std::set<const Value *> outp;
	std::set<const Value *> temp1, temp2;
	Function::iterator i, e;
	bool cont = false;
	
	inp.clear();
	outp.clear();
	do{
		i = F.begin();
		e = F.end();
		cont = false;
		for(; i != e; i++){
			LivenessInfo &liv = bbLivenessMap[&*i];
			
			inp = liv.in;
			outp = liv.out;
			
			temp1.clear();
			std::set_difference(liv.out.begin(), liv.out.end(), liv.def.begin(), liv.def.end(), 
									 std::inserter(temp1, temp1.end()));
			std::set_union(liv.use.begin(), liv.use.end(), temp1.begin(), temp1.end(),
									 std::inserter(liv.in, liv.in.end()));
			temp1.clear();
			temp2.clear();
			for(succ_iterator si = succ_begin(&*i), se = succ_end(&*i); si != se; si++){
//				BasicBlock *succ = *si;
				LivenessInfo &iliv = bbLivenessMap[*si];
				std::set_union(iliv.in.begin(), iliv.in.end(), temp1.begin(), temp1.end(),
							   std::inserter(temp2, temp2.end()));
				temp1 = temp2;
			}
			liv.out = temp1;
			if(inp != liv.in || outp != liv.out){
				cont = true;
			}

		}	
	} while(cont);


}

void Liveness::computeIInOut(Function &F){

	std::set<const Value *> inp;
    std::set<const Value *> outp;
	std::set<const Value *> temp1, temp2;
	Function::iterator fi, fe;
	BasicBlock::reverse_iterator bi, be, si, se;
	std::set<const Value *>::iterator ii, ie;
	bool cont = false;

	inp.clear();
	outp.clear();
	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		LivenessInfo &linfo = iLivenessMap[&*(fi->rbegin())];
		linfo.out = bbLivenessMap[&*fi].out;
		errs() << "#######################\n" << "out\n";
		for(ii = linfo.out.begin(), ie = linfo.out.end(); ii != ie; ii++){
			errs() << (*ii)->getName() << "\n";	
		}
		cont = false;
		do{
			for(bi = fi->rbegin(), be = fi->rend(); bi != be; bi++){
				/*liv = &iLivenessMap[&*bi];
				if(bi == fi->rbegin()){
					iliv = &bbLivenessMap[&*fi];
					liv->out = iliv->out;
				}*/
				LivenessInfo &liv = bbLivenessMap[&*fi];
				inp = liv.in;
				outp = liv.out;
				
				temp1.clear();
				std::set_difference(liv.out.begin(), liv.out.end(), liv.def.begin(), liv.def.end(), 
										 std::inserter(temp1, temp1.end()));
				std::set_union(liv.use.begin(), liv.use.end(), temp1.begin(), temp1.end(),
										 std::inserter(liv.in, liv.in.end()));
				temp1.clear();
				errs() << "#######################\n" << "in\n";
				for(ii = liv.in.begin(), ie = liv.in.end(); ii != ie; ii++){
					errs() << (*ii)->getName() << "\n";	
				}
				

			
				LivenessInfo &iliv = iLivenessMap[&*prev(bi)];
				/*
				std::set_union(iliv.in.begin(), iliv.in.end(), temp1.begin(), temp1.end(),
							   std::inserter(temp2, temp2.end()));
				
				temp1 = temp2;
				temp2.clear();
				*/
				if(bi != fi->rbegin())
					liv.out = iliv.in;
				errs() << "#######################\n" << "out\n";
				for(ii = liv.out.begin(), ie = liv.out.end(); ii != ie; ii++){
					errs() << (*ii)->getName() << "\n";	
				}
				if(inp != liv.in || outp != liv.out)
					cont = true;
				std::cin.get();
			}
		} while (cont);
	}	
}



bool Liveness::runOnFunction(Function &F) {
	computeBBDefUse(F);
 	computeBBInOut(F);
    computeIInOut(F);
	#ifdef DEBUG
	//checki(F);
	//checkbb(F);
	#endif
	return false;
}

char Liveness::ID = 0;

RegisterPass<Liveness> X("liveness", "Live vars analysis", false, false);





