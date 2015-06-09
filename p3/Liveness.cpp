#include "llvm/IR/Instructions.h"
#include "Liveness.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;

bool Liveness::isLiveOut(Instruction *I, Value *V){
	return (iLivenessMap[&*I].out.find(&*V) != iLivenessMap[&*I].out.end());
}

void Liveness::computeBBDefUse(Function &F){
	//iterators and end mark
	Function::iterator fi, fe;
	BasicBlock::iterator bi, be;

	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		LivenessInfo livb;
		for(bi = fi->begin(), be = fi->end(); bi != be; bi++){
			LivenessInfo livi;
			if(bi->getName() != "\0"){
				livb.def.insert(bi);
				livi.def.insert(bi);
			}
			for(int i = 0, e = bi->getNumOperands(); i != e; i++){
				if(isa<Function>(bi->getOperand(i)) || isa<Constant>(bi->getOperand(i)))
					continue; 
				if(bi->getOperand(i)->getName() != "\0"){
					livb.use.insert(bi->getOperand(i));
					livi.use.insert(bi->getOperand(i));
				}
			}
			iLivenessMap[&*bi] = livi;
		}
		bbLivenessMap[&*fi] = livb;
	}
}

void Liveness::computeBBInOut(Function &F) {

	std::set<const Value *> inp;
    std::set<const Value *> outp;
	std::set<const Value *> temp1;
	Function::iterator i, e;
	std::set<const Value *>::iterator ii, ie;
	bool cont = false;
	
	inp.clear();
	outp.clear();
	do{
		cont = false;
		for(i = F.begin(), e = F.end(); i != e; i++){
			LivenessInfo &liv = bbLivenessMap[&*i];
			
			inp = liv.in;
			outp = liv.out;
			
			temp1.clear();
			liv.in.clear();
			std::set_difference(liv.out.begin(), liv.out.end(), liv.def.begin(), liv.def.end(), 
									 std::inserter(temp1, temp1.end()));
			std::set_union(liv.use.begin(), liv.use.end(), temp1.begin(), temp1.end(),
									 std::inserter(liv.in, liv.in.end()));
				
			temp1.clear();
			for(succ_iterator si = succ_begin(i), se = succ_end(i); si != se; si++){
				LivenessInfo &iliv = bbLivenessMap[*si];
				std::set_union(liv.out.begin(), liv.out.end(), iliv.in.begin(), iliv.in.end(),
							   std::inserter(temp1, temp1.end()));
				liv.out = temp1;
				temp1.clear();
			}
			if(inp != liv.in || outp != liv.out){
				cont = true;
			}
		}
	} while(cont);
}

void Liveness::computeIInOut(Function &F){

	std::set<const Value *> inp;
    std::set<const Value *> outp;
	std::set<const Value *> temp1;
	Function::iterator fi, fe;
	BasicBlock::reverse_iterator bi, be, si, se;
	std::set<const Value *>::iterator ii, ie;
	bool cont = false;

	inp.clear();
	outp.clear();
	for(fi = F.begin(), fe = F.end(); fi != fe; fi++){
		LivenessInfo &linfo = iLivenessMap[&*(fi->rbegin())];
		linfo.out = bbLivenessMap[fi].out;
		
		do{
			cont = false;
			for(bi = fi->rbegin(), be = fi->rend(); bi != be; bi++){
				LivenessInfo &liv = iLivenessMap[&*bi];
				inp = liv.in;
				outp = liv.out;
				
				temp1.clear();
				std::set_difference(liv.out.begin(), liv.out.end(), liv.def.begin(), liv.def.end(), 
										 std::inserter(temp1, temp1.end()));
				std::set_union(liv.use.begin(), liv.use.end(), temp1.begin(), temp1.end(),
										 std::inserter(liv.in, liv.in.end()));
				temp1.clear();
			
				LivenessInfo &iliv = iLivenessMap[&*prev(bi)];
				if(bi != fi->rbegin())
					liv.out = iliv.in;
				if(inp != liv.in || outp != liv.out){
					cont = true;
				}
			}
		} while (cont);
	}

	temp1.clear();
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





