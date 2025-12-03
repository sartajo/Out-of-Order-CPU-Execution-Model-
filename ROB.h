

#ifndef _ROB_H
#define _ROB_H 

#include <vector> 
#include <string> 
#include "MemTemplate.h" 

class ROB{
    public: 
        struct ROBEntry {
    ns3::CpuFIFO::ReqMsg inst;
    bool ready = false;   // committed flag
    }; 

    int MAX_ENTRIES; 
    int num_entries;  
    int IPC;
    std::vector<ROBEntry> rob_q;

    ROB(int size = 64, int ipc = 4); 

    bool canAccept();                       // space available 
    void allocate(const ns3::CpuFIFO::ReqMsg&);  // push instruction
    void commit(int index);                 // mark ready=true
    void retire();                          // retire in-order 
    void step();
    bool empty() const {
        return rob_q.empty();
    }
    void commitByMsgId(uint64_t msgId);     // mark ready by msgId  
};

#endif
