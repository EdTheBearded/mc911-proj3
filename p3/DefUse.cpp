#include <set>
#include "llvm/Pass.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"

//#define DEBUG

using namespace llvm;
using namespace std;

namespace {
  struct DefUse : public FunctionPass {
    static char ID;
    DefUse() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {

	bool ret = false;
	set<Instruction*> W;

	// itera nas instruções da função
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
		// checa se a instrucao e uma atribuicao (definicao)
		if(!(I->getName().empty())){
			// armazena a instrucao no conjunto W
			W.insert(&*I);
			#ifdef DEBUG
				errs() << "Armazenando a definicao " << (*I) << "\n";
			#endif
		}
	}

	#ifdef DEBUG
		errs() << "Terminamos de armazenar as definicoes.\n ";
	#endif

	while(!W.empty()){

		// pega uma instrucao de W para analisar
		set<Instruction*>::iterator inst = W.begin();
		#ifdef DEBUG		
			errs() << "Analisando a instrucao " << (**inst) << "\n";
		#endif

		// checa se a intrucao esta viva
		if((*inst)->hasNUses(0) && !((*inst)->mayHaveSideEffects()) && !((*inst)->isTerminator()) 
			&& !(isa<DbgInfoIntrinsic>(**inst)) && !(isa<LandingPadInst>(**inst))){
			#ifdef DEBUG
				errs() << "Definicao " << (*inst)->getName() << " e usada " << (*inst)->getNumUses() << " vez(es).\n";
			#endif
			// itera nos operadores da instrucao
			for (Use &U : (*inst)->operands()) {
  				Value *v = U.get();
				// checa se o operador e uma variavel
				if(isa<Instruction>(*v)){
					// tenta inserir a definicao da variavel (operador) no conjunto
					W.insert(cast<Instruction>(v));
					#ifdef DEBUG
						errs() << "Operando " << *v << " e usado " << v->getNumUses() << " vez(es).\n";
					#endif
				}
			}
				// apaga instrucao do codigo
				(*inst)->eraseFromParent();
				// seta o retorno para true (codigo foi alterado)
				ret = true;
		}
		// apaga instrucao do conjunto
		W.erase(inst);	
	}
		
      	return ret;
    }
  };
}

char DefUse::ID = 0;
static RegisterPass<DefUse> X("dce-ssa", "Def-use chain Pass", false, false);
