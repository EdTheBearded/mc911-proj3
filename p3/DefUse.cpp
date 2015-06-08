#include <unordered_set>
#include "llvm/Pass.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;

size_t hash (Instruction i) { return &i; }

namespace {
  struct DefUse : public FunctionPass {
    static char ID;
    DefUse() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {

	bool ret = false;
	unordered_set<Key=Instruction, Hash=hash()> W;
	//inst_iterator I, E, aux;
  	//Value *v;

	// itera nas instruções da função
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
		// checa se a instrucao e uma atribuicao (definicao)
		if(!(I->getName().empty())){
			// armazena a instrucao no conjunto W
			W.insert(*I);
		}
	}

	while(!W.empty()){
		iterator i = W.begin();
		Instruction inst = *i;
		// checa se a intrucao esta viva
		if(inst.hasNUses(0) && !(inst.mayHaveSideEffects()) && !(inst.isTerminator()) 
			&& !(isa<DbgInfoIntrinsic>(inst)) && !(isa<LandingPadInst>(inst))){
		//if(I->hasNUses(0)){
			//errs() << "Definicao " << I->getName() << " is used " << I->getNumUses() << " time(s)\n";
			// itera nos operadores da instrucao
			for (Use &U : inst.operands()) {
  				Value *v = U.get();
				// checa se o operador e uma variavel
				if(isa<Instruction>(*v)){
					//errs() << "Operando " << *v << " is used ";
					//errs() << v->getNumUses() << " times\n";
					W.insert(*v);
						// itera sobre os usos dessa variavel
						/*for(Value::use_iterator i = v->use_begin(), e = v->use_end(); i != e; ++i){
								errs() << i->get() << "\n";
  							//if (Instruction *Inst = dyn_cast<Instruction>(*i)){
								//errs() << i->getType() << "\n";
							//}
						}*/
						/*for (User *us : v->users()) {
  							if (Instruction *Inst = dyn_cast<Instruction>(us)){
								errs() << *Inst << "\n";
							} else { errs() << "nao e instrucao\n"; }
						}*/
					//}
   				}
			}
				//inst_iterator aux = I;
				//++I;
				W.erase(inst);	
				inst.eraseFromParent();
				ret = true;
				//errs() << "--------------------------------------------\n";
				//errs() << "Operando " << *v << " is used ";
				//errs() << v->getNumUses() << " times\n";
				//errs() << "###########################################\n";
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
	
      return ret;
    }
  };
}

char DefUse::ID = 0;
static RegisterPass<DefUse> X("dce-ssa", "Def-use chain Pass", false, false);
