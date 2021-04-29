#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/Analysis/CaptureTracking.h"

using namespace llvm;


namespace {
    struct AddFreePass : public FunctionPass {
        public:
            static char ID;
            AddFreePass() : FunctionPass(ID) {}
	    bool isSupported(Instruction &i);
            virtual bool runOnFunction(Function &F) override;
    };
}

bool
AddFreePass::isSupported(Instruction &i) {
  return (isa<PHINode>(i) ||
          isa<BinaryOperator>(i) ||
          isa<AllocaInst>(i) ||
          isa<LoadInst>(i) ||
          isa<CallInst>(i) ||
          isa<SelectInst>(i) ||
          isa<ICmpInst>(i));
}


bool AddFreePass::runOnFunction(Function &F) {
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
	 // REMEMBER: this pass does not work interprocedurally! 
   	 CallInst::CreateFree( cast<Value>(alloc_i), ret_i); // insert before ret_i
    }
    errs() << "\n";

    /* FORWARD
    for (BasicBlock& bb: F){
        for (Instruction& i: bb){
             //if(isSupported(i)){
                  errs() << i << "\n";
	     //}		     
        }
    }
    */

    return false; 
}

char AddFreePass::ID = 0;
static RegisterPass<AddFreePass> X("afpass", "--pending explanation--");
