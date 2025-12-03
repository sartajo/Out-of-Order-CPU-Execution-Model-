#ifndef _LSQ_H
#define _LSQ_H 

#include <vector> 
#include <string> 
#include "MemTemplate.h" 

class LSQ {
public: 
    struct LSQEntry {
        ns3::CpuFIFO::ReqMsg inst;
        bool ready = false;   
        bool sentTocache = false;
    }; 

    int MAX_ENTRIES; 
    int num_entries; 
    std::vector<LSQEntry> lsq_q;

    LSQ(int size = 64);
    void step(); 
    bool canAccept(); 
    void allocate(const ns3::CpuFIFO::ReqMsg& inst); 
    void retire(); 
    bool ldFwd(const ns3::CpuFIFO::ReqMsg& inst);  
    void pushToCache(ns3::CpuFIFO* fifo, int cycle);
    bool rxFromCache(uint64_t msgId);
    bool empty() const {
        return lsq_q.empty();
    }
}; 

#endif
