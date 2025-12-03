#include "../header/LSQ.h"
#include <vector> 

LSQ::LSQ(int size){
    MAX_ENTRIES = size; 
    num_entries = 0; 
    lsq_q.reserve(size); // pre-allocate minimum size
} 

bool LSQ::canAccept(){
    return lsq_q.size() < static_cast<size_t>(MAX_ENTRIES);
} 

void LSQ::allocate(const ns3::CpuFIFO::ReqMsg& inst){
    LSQEntry a; 
    a.inst = inst;  
    a.ready = false; 
    a.sentTocache = false; 
    lsq_q.push_back(a);
    num_entries++;
} 

bool LSQ::ldFwd(const ns3::CpuFIFO::ReqMsg& inst) {
    for(int i = (int)lsq_q.size() - 1; i >= 0; i--){
        if(lsq_q[i].inst.type == ns3::CpuFIFO::REQTYPE::WRITE &&
           lsq_q[i].inst.addr == inst.addr){
            return true; // Load has a prior store in LSQ
        } 
    } 
    return false;
}  

void LSQ::pushToCache(ns3::CpuFIFO* fifo, int cycle) {
    // Walk the LSQ and send any entry that:
    //  - has not yet been sent to cache
    //  - and the CPU->cache FIFO has space
    for (auto &entry : lsq_q) {
        if (entry.sentTocache) {
            continue; // already issued
        }

        if (fifo->m_txFIFO.IsFull()) {
            break; // backpressure from fifo
        }

        ns3::CpuFIFO::ReqMsg b = entry.inst;
        b.fifoInserionCycle = cycle;
        fifo->m_txFIFO.InsertElement(b);
        entry.sentTocache = true;
    }
}



bool LSQ::rxFromCache(uint64_t msgId){
    for(int i = (int)lsq_q.size() - 1; i >= 0; i--){
        if(lsq_q[i].inst.msgId == msgId){
            lsq_q[i].ready = true; 
            return (lsq_q[i].inst.type == ns3::CpuFIFO::REQTYPE::READ);
        }  
    }   
    return false;
} 

void LSQ::retire(){
    if(lsq_q.empty()){
        return;
    } 

    if(lsq_q.front().ready == true){
        lsq_q.erase(lsq_q.begin()); 
        if(num_entries > 0) num_entries--; 
    } 
} 

void LSQ::step() {
    retire();
}
