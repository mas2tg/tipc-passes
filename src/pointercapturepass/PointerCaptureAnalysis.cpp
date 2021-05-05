#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
using namespace llvm;


namespace {
    struct PointerCapturePass : public CallGraphSCCPass {
        public:
            static char ID;
            PointerCapturePass() : CallGraphSCCPass(ID) {}
	    bool isSupported(Instruction &i);
            virtual bool runOnSCC(CallGraphSCC &SCC) override;
    };
}

bool
PointerCapturePass::isSupported(Instruction &i) {
  return (isa<PHINode>(i) ||
          isa<BinaryOperator>(i) ||
          isa<AllocaInst>(i) ||
          isa<LoadInst>(i) ||
          isa<CallInst>(i) ||
          isa<SelectInst>(i) ||
          isa<ICmpInst>(i));
}


bool PointerCapturePass::runOnSCC(CallGraphSCC &SCC) {
    errs() << "Visiting one call graph...\n";
    for(const CallGraphNode* cgn: SCC){
	Function* F = cgn->getFunction();
	if (F == NULL) continue;
    	errs() << "Visiting function: " << F->getName() << "\n";
	for (BasicBlock& bb: *F){
	    for (Instruction& i: bb){
		errs() << "Looking at " << i << " " << i.getOpcode() << "\n";
		
		if (i.getOpcode() == 56 ){ // 56 corresponds to Inst::Call 
			CallBase* alloc_call = cast<CallBase>(&i); 
			if (alloc_call->getCalledFunction() != NULL && alloc_call->getCalledFunction()->getName().endswith("alloc")){
				if (PointerMayBeCaptured(alloc_call, false, false)){ //2nd: return is captured; 3rd: any stored is captured 
					errs() << "    This pointer " << i << " is captured!\n"; 
				}
			}
		}
		if (i.getOpcode() == 47){ // 47 corresponds to ptrtoint
				if (PointerMayBeCaptured(&i, false, false)){ 
					errs() << "    This pointer " << i << " is captured!\n";	
				}
		}
		for (const Use &U : i.uses()) {
			errs() << "    user:" << *(U.getUser()) << "; with operandNo:" << U.getOperandNo() << "\n";
		}


	    }
	}
	
    }

    errs() << "\n";


    return false; 
}

char PointerCapturePass::ID = 0;
static RegisterPass<PointerCapturePass> X("pcpass", "--pending explanation--");
