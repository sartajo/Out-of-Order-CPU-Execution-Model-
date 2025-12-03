

#include "../header/ROB.h" 
#include <vector>
//constructor
ROB::ROB(int size,int ipc) {
    MAX_ENTRIES = size;
    IPC = ipc; 
    num_entries = 0;
} 

bool ROB::canAccept(){
    return rob_q.size() < MAX_ENTRIES;
    } 

void ROB::allocate(const ns3::CpuFIFO::ReqMsg &msg){
    ROBEntry a;
    a.inst = msg; 
    //check canaccept first before using this function 
    if(a.inst.type == ns3::CpuFIFO::REQTYPE::COMPUTE || a.inst.type == ns3::CpuFIFO::REQTYPE::WRITE){
        a.ready = true;
    }  
    else if(a.inst.type == ns3::CpuFIFO::REQTYPE::READ){
        a.ready = false;
    }
    rob_q.push_back(a);
    } 

void ROB::commit(int index){
    rob_q[index].ready = true;
} 

void ROB::retire(){
    //this function is just to remove stuff from the ROB 
    //ROB only removes the entry if it gets to the head and is flagged ready
    int ipc_counter = 0;
    while((rob_q.size()>0)&&(rob_q[0].ready)&&(ipc_counter<IPC)){
        rob_q.erase(rob_q.begin()); 
        ipc_counter++;
    }
}

void ROB::step(){
    retire();
}

void ROB::commitByMsgId(uint64_t msgId){
    for (auto &entry : rob_q) {
        if (entry.inst.msgId == msgId) {
            entry.ready = true;
            break;
        }
    }
}

