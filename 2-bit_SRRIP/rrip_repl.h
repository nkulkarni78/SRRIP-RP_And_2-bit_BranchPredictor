#ifndef RRIP_REPL_H_
#define RRIP_REPL_H_

#include "repl_policies.h"

// Static RRIP. our requirement is to implemen 2-bit SRRIP with rpvMax = 3
class SRRIPReplPolicy : public ReplPolicy {
    protected:
        // add class member variables here
        // pointer to dynamically sizing array to store the re-reference pred
        // iction value (RPV) for each cache line/block.
        uint32_t* block_val;
        // pointer to dynamically sizing array used to store the cache lines/blocks.
        uint32_t* blocks;
        // total number of blocks in the cache
       	uint32_t numLines;
	// max value for re-referencing the block before replacement
        uint32_t rpvMax;
        // flag to check if block is being reused after evicting earlier
	bool new_entry;

    public:
        // add member methods here, refer to repl_policies.h
        // SRRIP is slightly updated version of NRU with more
        // reference intervals to increase the accuracy of pr-
        // diction
        explicit SRRIPReplPolicy(uint32_t _numLines, uint32_t _rpvMax) : numLines(_numLines), rpvMax(_rpvMax) {
            blocks = gm_calloc<uint32_t>(numLines);
            block_val = gm_calloc<uint32_t>(numLines);
            for(uint32_t i = 0; i < numLines; i++) {
                // as per the details given in SRRIP implementation
                // new block entry rrpv will me rrpvmax which is 3
                block_val[i] = 3;
	    }
        }

        // destructor
        ~SRRIPReplPolicy() {
            // free the memory allocated to track each cache line
            gm_free(blocks);
            // free the memory allocated to track rrpv of each cache line
            gm_free(block_val);
        } 

        // update block id in value array for newly inserted block
        void update(uint32_t blockID, const MemReq* req) {
            // reset is_new_entry to be false
            if(new_entry) {
                // if the block is replaced the set rrpv value to 2
                // since it is being reused again as per algorithm
                block_val[blockID] = 2;
            }
            // mark status of block ID as recently used if the cache
            // is hit and set rrpv value as 0
            else {
                block_val[blockID] = 0;
            }
            new_entry = false;
	}

        // used to replace/update flag
        void replaced(uint32_t blockID) {           
            new_entry = true;
        }

        // used to provide blockID to be evicted a similar implementation of NRU
        // with slight modifications to match SRRIP algorithm candidates
        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
            uint32_t blockID;  
            // will remain in here until it finds first entry with MAX_VALUE of
            // rrpv for getting the best block for eviction using block ID
            while (true) {
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                    blockID = *ci;
                    if (block_val[blockID] == rpvMax) {
                        // return blockID with max rpv and halt while loop
                        return blockID;
                    }
                    // increment blockID value if blockID is not rpvMax
                    // block_val[blockID]++;   
                            // if no blockID is found then increment rrpv values
                            // of each block and rerun to find best candidate
                       /* else {
                            // using min function to evaluate saturating counter value
                            // and make sure values do not roll over more than rpvMax.
                            // block_val[blockID] = std::min(block_val[blockID]+1, rpvMax);
                            block_val[blockID] = block_val[blockID]+1;
                        }*/
                }
                // if no entry has rpvMax = 3 it increments all values of blocks 
                // until there is a match. For 2-bit SRRIP rpvMax is 3. Having below
                // condition check in seperate loop does not alter code functionality
                // but improved performance aspect of SRRIP.
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                     blockID = *ci;
                     block_val[blockID]++;
                }                        
            }
        }

	DECL_RANK_BINDINGS;
};
#endif // RRIP_REPL_H_
