#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  struct DefUse : public FunctionPass {
    static char ID;
    DefUse() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
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
