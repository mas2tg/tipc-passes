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
	         //if(isSupported(i)){
		errs() << "Looking at " << i << " " << i.getOpcode() << "\n";
		
		if (i.getOpcode() == 56 ){ // 56 corresponds to Inst::Call 
			CallBase* alloc_call = cast<CallBase>(&i); // NEED to get underlying value from interator first, then its reference
			//errs() << "    about to call getName("<< alloc_call->getCalledFunction() <<" )\n";

			if (alloc_call->getCalledFunction() != NULL && alloc_call->getCalledFunction()->getName().endswith("alloc")){
				// errs() << *iter_i << " " << (iter_i)->getOpcode() << "\n";
				//errs() << "about to "
				// alloc_i = &(*iter_i);
				if (PointerMayBeCaptured(&i, false, true)){ //2nd: return; 3rd: stored
					errs() << "    This pointer " << i << " is captured!\n";
					//callocPresent = true; //False positive?
				}
			}
		}
	         //}		     
	    }
	}
	
    }

/*
    errs() << "Visiting function: " << F.getName() << "\n";
    Instruction* ret_i;
    Instruction* alloc_i;
    bool callocPresent = false;
    
    // BACKWARDS (based on https://stackoverflow.com/questions/44274857/iterating-over-basic-blocks-in-reverse-in-llvm-function) 
    auto bb_list = &(F.getBasicBlockList());
    for (auto iter_bb = bb_list->rbegin(); iter_bb != bb_list->rend(); ++iter_bb){
	auto inst_list = &(iter_bb->getInstList());
	for (auto iter_i = inst_list->rbegin(); iter_i != inst_list->rend(); ++iter_i){
		errs() << "Looking at " << *iter_i << " " << (iter_i)->getOpcode() << "\n";
		
    		//WE LOOK FOR ret here
		if (iter_i->getOpcode() == 1){ 
			//errs() << *iter_i << " " << (iter_i)->getOpcode() << "\n";
			ret_i = &(*iter_i);
		}
		
		//THIS WORKS 
		if (iter_i->getOpcode() == 56 ){ // 56 corresponds to Inst::Call 
			CallBase* alloc_call = cast<CallBase>(&(*iter_i)); // NEED to get underlying value from interator first, then its reference
			//errs() << "    about to call getName("<< alloc_call->getCalledFunction() <<" )\n";

			if (alloc_call->getCalledFunction() != NULL && alloc_call->getCalledFunction()->getName().endswith("alloc")){
				// errs() << *iter_i << " " << (iter_i)->getOpcode() << "\n";
				//errs() << "about to "
				alloc_i = &(*iter_i);
				if (PointerMayBeCaptured(alloc_i, false, false)){ //2nd: return; 3rd: stored
					errs() << "    This pointer is captured!\n";
					callocPresent = true; //False positive?
				}
			}
		}
		

	}
    }

    if (callocPresent){
   	 errs() << "Adding free()...\n";
   	 // add free
	 // REMEMBER: this pass does not work interprocedurally! alloc_i will never be called 
   	 CallInst::CreateFree( cast<Value>(alloc_i), ret_i); // insert before ret_i
    }
    */
    errs() << "\n";


    return false; 
}

char PointerCapturePass::ID = 0;
static RegisterPass<PointerCapturePass> X("pcpass", "--pending explanation--");
