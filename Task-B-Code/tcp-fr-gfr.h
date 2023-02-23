
#ifndef TCPFRGFR_H
#define TCPFRGFR_H

#include "tcp-congestion-ops.h"

namespace ns3{


class TcpSocketState;

class TcpFrGfr : public TcpNewReno{

public:

  static TypeId GetTypeId (void);

  TcpFrGfr(void);

  TcpFrGfr(const TcpFrGfr& sock);

  virtual ~TcpFrGfr(void);

  virtual std::string GetName () const;

   /** 
   * \brief estimate the bandwidth following the pseudocode.
   * sample_BWE = pkt_size*8/(now - lastacktime)
   * BWE = BWE*alpha + sample_BWE*(1-alpha)
   * Here, 
   * pkt_size = segment size in bytes
   * now = current time, lastacktime = time when prev ack was received
   * alpha = smoothing constant, value is 0.8 according to paper.
   * pkt_size could be avg size of last n received segments
   *
   * \param tcb internal congestion state
   * \param segmentsAcked count of segments ACKed
   * \param rtt last RTT
   *
   */
  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);


  /**
   * \brief 
   * */  
  virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight);

  virtual Ptr<TcpCongestionOps> Fork ();

  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

  virtual void CongestionStateSet (Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState);

  virtual void CongestionAvoidanceEvent(Ptr<TcpSocketState> tcb);
  
private:
    double m_alpha;       //*smoothing factor
    uint32_t m_a;           //*reduction factor 
    double m_BWE;         //*Bandwidth Estimate
    Time m_lastacktime;     //*Time of last ack receive
    Time m_minRtt;          //*smallest RTT recorded by TCP for this specific connection
    EventId m_avoidance;
  

};
}

#endif