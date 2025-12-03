/*
 * File  :      MCoreSimProjectXml.h
 * Author:      Salah Hessien
 * Email :      salahga@mcmaster.ca
 *
 * Created On February 16, 2020
 */

#include "../header/CpuCoreGenerator.h" 
 


namespace ns3 {

    // override ns3 type
    TypeId CpuCoreGenerator::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::CpuCoreGenerator")
               .SetParent<Object > ();
        return tid;
    }

    // The only constructor
    CpuCoreGenerator::CpuCoreGenerator(CpuFIFO* associatedCpuFIFO):m_rob(32,1),m_lsq(8) {
        // default
        m_coreId         = 1;
        m_cpuCycle       = 1;
        m_bmFileName     = "trace_C0.trc";
        m_dt             = 1;
        m_clkSkew        = 0;
        m_cpuMemReq      = CpuFIFO::ReqMsg();
        m_cpuMemResp     = CpuFIFO::RespMsg();
        m_cpuFIFO        = associatedCpuFIFO;
        m_cpuReqDone     = false;
        m_newSampleRdy   = false;
        m_cpuCoreSimDone = false;
        m_logFileGenEnable = false;
        // m_prevReqFinish    = true;
        // m_prevReqFinishCycle = 0;
        // m_prevReqArriveCycle = 0;
        m_cpuReqCnt      = 0;
        m_cpuRespCnt     = 0;
        // m_number_of_OoO_requests = 1; 
        m_pendingCompute = 0;
        m_hasPendingMem  = false;
        m_pendingMemReq  = ns3::CpuFIFO::ReqMsg();
         
        
    }

    // We don't do any dynamic allocations
    CpuCoreGenerator::~CpuCoreGenerator() {
    }

    // set Benchmark file name
    void CpuCoreGenerator::SetBmFileName (std::string bmFileName) {
        m_bmFileName = bmFileName;
    }

    void CpuCoreGenerator::SetCpuTraceFile (std::string fileName) {
        m_cpuTraceFileName = fileName; 
    }

    void CpuCoreGenerator::SetCtrlsTraceFile (std::string fileName) {
        m_CtrlsTraceFileName = fileName;
    }

    // set CoreId
    void CpuCoreGenerator::SetCoreId (int coreId) {
      m_coreId = coreId;
    }

    // get core id
    int CpuCoreGenerator::GetCoreId () {
      return m_coreId;
    }

    // set dt
    void CpuCoreGenerator::SetDt (double dt) {
      m_dt = dt;
    }

    // get dt
    int CpuCoreGenerator::GetDt () {
      return m_dt;
    }

    // set clk skew
    void CpuCoreGenerator::SetClkSkew (double clkSkew) {
       m_clkSkew = clkSkew;
    }

    // get simulation done flag
    bool CpuCoreGenerator::GetCpuSimDoneFlag() {
      return m_cpuCoreSimDone;
    }

    void CpuCoreGenerator::SetLogFileGenEnable (bool logFileGenEnable ) {
      m_logFileGenEnable = logFileGenEnable;
    }

    void CpuCoreGenerator::SetOutOfOrderStages(int stages)
    {
      m_number_of_OoO_requests = stages;
      //std::cout<<"stages="<<m_number_of_OoO_requests<<std::endl;
    }
    
    // The init function starts the generator calling once very m_dt NanoSeconds.
    void CpuCoreGenerator::init() {
        m_bmTrace.open(m_bmFileName.c_str());
        Simulator::Schedule(NanoSeconds(m_clkSkew), &CpuCoreGenerator::Step, Ptr<CpuCoreGenerator > (this));
    }

    // This function does most of the functionality.
    // void CpuCoreGenerator::ProcessTxBuf() {
    //     std::string fline;
    //     uint64_t newArrivalCycle;

    //     Logger::getLogger()->setClkCount(this->m_coreId, this->m_cpuCycle);
        
    //     //CPU only issues a new memory request if we havent reached the end of the trace file 
    //     //and if the txfifo is not full

    //     if ((m_cpuReqDone == false )    // insert CPU request into buffer when
    //         //(m_cpuCycle >= m_cpuMemReq.cycle      ) &&    // fifo isn't full and cpucycle larger 
    //          //&&     // than or equal cpu issued cycle
    //        // (m_prevReqFinish == true )
    //        )      
    //     {  
    //       //when we get a reading from trace file
    //       if(m_newSampleRdy==true){ 
    //           //Lets handle LOAD and STORES FIRST 
    //           //cpuMEMEReq is of type RequestMsg it has a paramter called REQTYPE that tells us what inst type we reading 
    //           //We need to store these in ROB and LSQ both
    //           if(m_cpuMemReq.type == CpuFIFO::REQTYPE::READ || m_cpuMemReq.type == CpuFIFO::REQTYPE::WRITE){
    //             //if there is space in ROB and LSQ
    //             if(m_rob.canAccept() && m_lsq.canAccept()){
    //               //For load check if there is a STORE that came for a specific LOAD
    //               if((m_cpuMemReq.type == CpuFIFO::REQTYPE::READ)&&
    //               (m_lsq.ldFwd(m_cpuMemReq))){
    //                   m_cpuMemReq.ready = 1;
    //               } 
    //               else if(m_cpuMemReq.type == CpuFIFO::REQTYPE::WRITE){
    //                   m_cpuMemReq.ready = 1;
    //               } 
    //               else{
    //                 m_cpuMemReq.ready = 0;

    //               }
    //             //stamp the request with the cpu cycle
    //             m_cpuMemReq.fifoInserionCycle = m_cpuCycle; 
    //             m_rob.allocate(m_cpuMemReq); 
    //             if()
    //             m_lsq.allocate(m_cpuMemReq); 
    //             //log the request 
    //             Logger::getLogger()->addRequest(this->m_coreId, m_cpuMemReq);
    //             //mark that we read from the trace file
    //             m_newSampleRdy = false; 
    //             m_cpuReqCnt++;
    //             }
    //           } else if(m_cpuMemReq.type == CpuFIFO::REQTYPE::COMPUTE){
    //             m_rob.allocate(m_cpuMemReq); 
    //             Logger::getLogger()->addRequest(this->m_coreId, m_cpuMemReq);
    //             m_cpuReqCnt++;

    //           }
    //       }  
    //       //Ensures we only send one request at a time and wait till we get the response for it
    //        /* ******* In order - 1
           
    //        if(m_sent_requests < m_number_of_OoO_requests)
    //        {

    //         //m_newSampleRdy means we've already read a trace lineinto cpumemreq 
    //         //we are just waiting for issue cycle. Basically we only write to fifo if correct cycle and we read trace

    //         if (m_newSampleRdy == true) { // wait until reading from file 
    //         //we get an ideal cpu cycle from trace but because we are workig with discrete time, etc we recalculate 
    //         //the time cycle at which we issue the request (adjusts for CPU stalling) 

    //         //we can replace this with the ROB logic
    //             newArrivalCycle  = m_prevReqFinishCycle + m_cpuMemReq.cycle - m_prevReqArriveCycle;

    //             if (m_cpuCycle >= newArrivalCycle) {
    //               // reset all flag when new request inserted in the FIFO ----
    //               m_newSampleRdy   = false;
    //               m_prevReqFinish  = false;
                  
    //               //stamp the request with the cycle it gets issued in
    //               m_cpuMemReq.fifoInserionCycle = m_cpuCycle;

    //               //send the request to the fifo
    //               m_cpuFIFO->m_txFIFO.InsertElement(m_cpuMemReq);
    //               //log the request
    //               Logger::getLogger()->addRequest(this->m_coreId, m_cpuMemReq);
    //               //increment in-flight (currently occuring) requests counter
    //               m_sent_requests++;
    //               //---we change the above part with rob and lsq allocation, computation, OoO execution 

    //               if (m_logFileGenEnable) {
    //                 std::cout << "Cpu " << m_coreId << " MemReq: ReqId = " << m_cpuMemReq.msgId << ", CpuRefCycle = " 
    //                           << m_cpuMemReq.cycle << ", CpuClkTic ==================================================== " <<  m_cpuCycle << std::endl;
    //                 std::cout << "\t\tMemAddr = " << m_cpuMemReq.addr << ", ReqType (0:Read, 1:Write) = " << m_cpuMemReq.type << ", CpuTxFIFO Size = " << m_cpuFIFO->m_txFIFO.GetQueueSize() << std::endl << std::endl;
    //               }
    //             }
    //         }
    //        } */
          
    //       //if we dont have request pending read new line
          
    //       if (m_newSampleRdy == false) {
    //         if (getline(m_bmTrace,fline)) { 

    //          //parses through the trace and assigns paramters to cpumemreq 
    //          m_newSampleRdy    = true;
    //          size_t pos        = fline.find(" ");
    //          std::string s     = fline.substr(0, pos); 
    //          std::string dummy = fline.substr(pos+1, 1); 
    //          std::string type  = fline.substr(pos+3, 1);
    //          std::string cyc   = fline.substr(pos+5);

    //          // convert hex string address to decimal 
    //          m_cpuMemReq.addr = (uint64_t) strtol(s.c_str(), NULL, 16);

    //          // convert cycle from string to decimal, same value as the file
    //          m_cpuMemReq.cycle= (uint64_t) strtol(cyc.c_str(), NULL, 10);

    //          m_cpuMemReq.type = (type == "R") ? CpuFIFO::REQTYPE::READ : CpuFIFO::REQTYPE::WRITE;
    //          //m_cpuMemReq.type = CpuFIFO::REQTYPE::WRITE;
    //          Ptr<UniformRandomVariable> uRnd1;
    //          uRnd1 = CreateObject<UniformRandomVariable> ();
    //          uRnd1-> SetAttribute ("Min", DoubleValue (0));
    //          uRnd1-> SetAttribute ("Max", DoubleValue (100));
    //          //m_cpuMemReq.type = (uRnd1->GetValue() <= 50) ? CpuFIFO::REQTYPE::READ : CpuFIFO::REQTYPE::WRITE;

    //          // Generate unique Id for every cpu request (needed to avoid any 
    //          // collisions with other cores and to make debugging easier).
    //          m_cpuMemReq.msgId     = IdGenerator::nextReqId();
    //          m_cpuMemReq.reqCoreId = m_coreId;
    //          m_cpuReqCnt++;
    //         }
    //       } 
    //     }
    //     //once the file ends, cpu trace request generation is done
    //     /* ****----Inorder 2
    //     if (m_bmTrace.eof()) {
    //       m_bmTrace.close();
    //       m_cpuReqDone = true;
    //     }  
    //     */          
    // } // void CpuCoreGenerator::ProcessTxBuf()

  void CpuCoreGenerator::ProcessTxBuf() {
    std::string fline;

    // For stats (unchanged from original)
    Logger::getLogger()->setClkCount(this->m_coreId, this->m_cpuCycle);

    // If we’ve already completely consumed the trace and have no pending work,
    // nothing more to feed from the trace.
    if (m_cpuReqDone) {
        return;
    }

    // ------------------------------------------------------------
    // 1. If we have no pending compute or mem-op from a previous line,
    //    try to read a NEW line from the trace.
    //    New trace format (per project doc):
    //      <compute_count> <addr_hex> <R|W>
    // ------------------------------------------------------------
    if (!m_newSampleRdy && m_pendingCompute == 0 && !m_hasPendingMem) {
        if (std::getline(m_bmTrace, fline)) {
            m_newSampleRdy = true;

            // Parse: C addr type
            // Example: "5 0x00000010 R"
            size_t pos1 = fline.find(' ');
            if (pos1 == std::string::npos) {
                // Malformed line; treat as end of trace
                m_cpuReqDone = true;
                m_bmTrace.close();
                return;
            }

            std::string compStr = fline.substr(0, pos1);

            size_t pos2 = fline.find(' ', pos1 + 1);
            if (pos2 == std::string::npos) {
                // Malformed line; treat as end of trace
                m_cpuReqDone = true;
                m_bmTrace.close();
                return;
            }

            std::string addrStr = fline.substr(pos1 + 1, pos2 - pos1 - 1);

            // Type is the first non-space character after pos2
            size_t typePos = fline.find_first_not_of(' ', pos2 + 1);
            if (typePos == std::string::npos) {
                // Malformed line; treat as end of trace
                m_cpuReqDone = true;
                m_bmTrace.close();
                return;
            }

            char typeChar = fline[typePos];

            // Convert compute count (base 10)
            m_pendingCompute = static_cast<int>(strtol(compStr.c_str(), nullptr, 10));
            if (m_pendingCompute < 0) {
                m_pendingCompute = 0; // safety
            }

            // Setup the pending memory request; convert hex addr
            CpuFIFO::ReqMsg memReq;
            memReq.addr   = static_cast<uint64_t>(strtoull(addrStr.c_str(), nullptr, 16));
            memReq.cycle  = 0;  // we don’t use per-request target cycles in the OoO model
            memReq.type   = (typeChar == 'R') ? CpuFIFO::REQTYPE::READ
                                              : CpuFIFO::REQTYPE::WRITE;
            memReq.msgId     = IdGenerator::nextReqId();
            memReq.reqCoreId = m_coreId;
            memReq.fifoInserionCycle = 0;
            memReq.ready = 0;   // will be set below when we actually allocate

            m_pendingMemReq = memReq;
            m_hasPendingMem = true;
        } else {
            // Reached EOF and no new line
            m_cpuReqDone = true;
            m_bmTrace.close();
            return;
        }
    }

    // ------------------------------------------------------------
    // 2. Allocate COMPUTE instructions from the current line into ROB.
    //    Each line's compute count = m_pendingCompute.
    //    Compute ops live ONLY in ROB and are ready immediately.
    // ------------------------------------------------------------
    while (m_pendingCompute > 0 && m_rob.canAccept()) {
        CpuFIFO::ReqMsg computeReq;
        computeReq.msgId     = IdGenerator::nextReqId();
        computeReq.reqCoreId = m_coreId;
        computeReq.addr      = 0;                      // dummy, no real address
        computeReq.cycle     = 0;
        computeReq.fifoInserionCycle = m_cpuCycle;
        computeReq.type      = CpuFIFO::REQTYPE::COMPUTE;
        computeReq.ready     = 1;                      // computes commit immediately

        m_rob.allocate(computeReq);
        Logger::getLogger()->addRequest(this->m_coreId, computeReq);
        m_cpuReqCnt++;

        m_pendingCompute--;
    }

    // If we still have compute instructions to place but ROB is full,
    // we stall here this cycle.
    if (m_pendingCompute > 0) {
        return;
    }

    // ------------------------------------------------------------
    // 3. When all COMPUTEs from this line are allocated, allocate
    //    the LOAD/STORE for this line into BOTH ROB and LSQ.
    // ------------------------------------------------------------
    if (m_hasPendingMem) {
        // Need space in BOTH ROB and LSQ to allocate the mem-op
        if (!m_rob.canAccept() || !m_lsq.canAccept()) {
            // Stall this cycle; try again next cycle
            return;
        }

        CpuFIFO::ReqMsg memReq = m_pendingMemReq;
        memReq.fifoInserionCycle = m_cpuCycle;

        // STORE: commit immediately in ROB (ready = 1)
        if (memReq.type == CpuFIFO::REQTYPE::WRITE) {
            memReq.ready = 1;
        }
        // LOAD: try forwarding; otherwise wait for cache response
        else if (memReq.type == CpuFIFO::REQTYPE::READ) {
            bool forwarded = m_lsq.ldFwd(memReq);
            if (forwarded) {
                // Treat as MOB hit / forwarded value.
                // LSQ.ldFwd should internally mark the LSQ's load entry ready
                // once the load is allocated. For now we at least mark ROB ready.
                memReq.ready = 1;
            } else {
                memReq.ready = 0;   // will become ready when rxFromCache sees msgId
            }
        }

        // Allocate into ROB and LSQ
        m_rob.allocate(memReq);
        m_lsq.allocate(memReq);

        Logger::getLogger()->addRequest(this->m_coreId, memReq);
        m_cpuReqCnt++;

        // We’ve consumed this line’s mem op.
        m_hasPendingMem = false;
        m_newSampleRdy  = false;
    }

    // ------------------------------------------------------------
    // 4. If the trace file hit EOF earlier and we’ve drained all
    //    pending compute + mem lines, mark m_cpuReqDone.
    // ------------------------------------------------------------
    if (m_bmTrace.eof() && m_pendingCompute == 0 && !m_hasPendingMem) {
        m_cpuReqDone = true;
        m_bmTrace.close();
    }
  }

    //     void CpuCoreGenerator::ProcessRxBuf() {
    //     // process received buffer
    //     if (!m_cpuFIFO->m_rxFIFO.IsEmpty()) {
    //       m_cpuMemResp = m_cpuFIFO->m_rxFIFO.GetFrontElement();
    //       m_cpuFIFO->m_rxFIFO.PopElement();
    //       Logger::getLogger()->updateRequest(m_cpuMemResp.msgId, Logger::EntryId::CPU_RX_CHECKPOINT);
    //       /*
          
    //       m_sent_requests--;
    //       if(m_sent_requests < 0)
    //         std::cout << "error" << std::endl;
    //       if (m_logFileGenEnable) {
    //         std::cout << "Cpu " << m_coreId << " new response is received at cycle " << m_cpuCycle << std::endl;
    //       }
    //       m_prevReqFinish      = true;
    //       m_prevReqFinishCycle = m_cpuCycle;
    //       m_prevReqArriveCycle = m_cpuMemResp.reqcycle;
    //       */
    //       m_cpuRespCnt++;
    //     }
 
    //     // schedule next run or finish simulation if processing end
    //     if (m_cpuReqDone == true && m_cpuRespCnt >= m_cpuReqCnt) {
    //       m_cpuCoreSimDone = true;
    //       Logger::getLogger()->traceEnd(this->m_coreId);
    //       std::cout << "Cpu " << m_coreId << " Simulation End @ processor cycle # " << m_cpuCycle << std::endl;
    //     }
    //     else {
    //       // Schedule the next run
    //       Simulator::Schedule(NanoSeconds(m_dt), &CpuCoreGenerator::Step, Ptr<CpuCoreGenerator > (this));
    //       m_cpuCycle++;
    //     }

    // } // CpuCoreGenerator::ProcessRxBuf()

  void CpuCoreGenerator::ProcessRxBuf() {
    // 1. Process all responses from the cache (L1)
    while (!m_cpuFIFO->m_rxFIFO.IsEmpty()) {
        // Get the oldest response
        m_cpuMemResp = m_cpuFIFO->m_rxFIFO.GetFrontElement();
        m_cpuFIFO->m_rxFIFO.PopElement();

        // Mark RX checkpoint in the logger (baseline behavior)
        Logger::getLogger()->updateRequest(m_cpuMemResp.msgId,
                                           Logger::EntryId::CPU_RX_CHECKPOINT);

        // --- Old in-order bookkeeping (still safe to keep) ---
        m_sent_requests--;
        if (m_sent_requests < 0) {
            std::cout << "error" << std::endl;
        }

        if (m_logFileGenEnable) {
            std::cout << "Cpu " << m_coreId
                      << " new response is received at cycle "
                      << m_cpuCycle << std::endl;
        }

        m_prevReqFinish      = true;
        m_prevReqFinishCycle = m_cpuCycle;
        m_prevReqArriveCycle = m_cpuMemResp.reqcycle;
        m_cpuRespCnt++;

        // --- NEW: LSQ + ROB integration ---

        // Let LSQ know this msgId has completed. It will set the LSQ entry ready.
        // rxFromCache returns true if this was a LOAD.
        bool isLoad = m_lsq.rxFromCache(m_cpuMemResp.msgId);

        // If it's a load, also mark the corresponding ROB entry ready.
        if (isLoad) {
            m_rob.commitByMsgId(m_cpuMemResp.msgId);
        }
        // For stores, ROB was already marked ready at allocation time; LSQ
        // will now be able to retire the store when it reaches the head.
    }

    // 2. Retire ready instructions from the ROB (in order, up to IPC)
    m_rob.step();   // internally calls ROB::retire()

    // 3. Issue memory operations from LSQ to the cache (L1)
    m_lsq.pushToCache(m_cpuFIFO, m_cpuCycle);

    // 4. Retire completed entries from LSQ
    m_lsq.step();   // internally calls LSQ::retire()

    // 5. Check for simulation end or schedule the next cycle

    // Trace is fully read and there is no partially processed line
    bool noMoreTrace =
        m_cpuReqDone &&
        (m_pendingCompute == 0) &&
        !m_hasPendingMem;

    // Pipeline and FIFOs are empty
    bool pipesEmpty =
        m_rob.empty() &&
        m_lsq.empty() &&
        m_cpuFIFO->m_txFIFO.IsEmpty() &&
        m_cpuFIFO->m_rxFIFO.IsEmpty();

    if (noMoreTrace && pipesEmpty) {
        // We are done: all trace consumed, ROB/LSQ drained, no in-flight mem
        m_cpuCoreSimDone = true;
        Logger::getLogger()->traceEnd(this->m_coreId);
        std::cout << "Cpu " << m_coreId
                  << " Simulation End @ processor cycle # "
                  << m_cpuCycle << std::endl;
    } else {
        // Schedule the next run and advance the CPU cycle
        Simulator::Schedule(
            NanoSeconds(m_dt),
            &CpuCoreGenerator::Step,
            Ptr<CpuCoreGenerator>(this));

        m_cpuCycle++;
    }
  }

    /**
     * Runs one mobility Step for the given vehicle generator.
     * This function is called each interval dt
     */
    void CpuCoreGenerator::Step(Ptr<CpuCoreGenerator> cpuCoreGenerator) {
        cpuCoreGenerator->ProcessTxBuf(); 
        // m_rob.step(); //retire compute and memory operations
        // m_lsq.pushToCache(m_cpuFIFO,m_cpuCycle); 
        cpuCoreGenerator->ProcessRxBuf(); 
        // m_lsq.retire()
    }
}
