#include "tcp-fr-gfr.h"
#include "tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/simulator.h"


namespace ns3{
NS_LOG_COMPONENT_DEFINE ("TcpFrGfr");


NS_OBJECT_ENSURE_REGISTERED (TcpFrGfr);


TypeId
TcpFrGfr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpFrGfr")
    .SetParent<TcpNewReno> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpFrGfr> ()
  ;
  return tid;
}

TcpFrGfr::TcpFrGfr(void)
    :TcpNewReno(),
    m_alpha(0.8),
    m_a(1),
    m_BWE(0),
    m_lastacktime(0),
    m_minRtt(Time::Max()),
    m_avoidance()
{
    NS_LOG_FUNCTION(this);
}

TcpFrGfr::TcpFrGfr (const TcpFrGfr& sock)
    : TcpNewReno (sock),
    m_alpha (sock.m_alpha),
    m_a(sock.m_a),
    m_BWE(sock.m_BWE),
    m_lastacktime (sock.m_lastacktime),
    m_minRtt (sock.m_minRtt)
{
  NS_LOG_FUNCTION (this);
}
TcpFrGfr::~TcpFrGfr (void)
{
}

void
TcpFrGfr::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt)
{

    
  NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);
  // NS_LOG_UNCOND("Inside PktsAcked()\n");


  //*estimate bw
  Time currTime = Simulator::Now ();
  double m_sampleBWE = (tcb->m_segmentSize*8.0) / (currTime - m_lastacktime).GetSeconds();
  m_BWE = m_BWE*m_alpha + m_sampleBWE*(1-m_alpha);
  m_lastacktime = currTime;

  // NS_LOG_LOGIC ("Estimated BW: " << m_BWE);
  NS_LOG_LOGIC("Estimated BW: " << m_BWE << " Segment Size: " << tcb->m_segmentSize << " cwnd: " << tcb->m_cWnd);


  if (rtt.IsZero ())
    {
      return;
    }

  m_minRtt = std::min (m_minRtt, rtt);
  NS_LOG_DEBUG ("Updated m_minRtt = " << m_minRtt);

}



void
TcpFrGfr::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION (this << tcb << newState);
  if (newState == TcpSocketState::CA_LOSS)
    {
      NS_LOG_INFO("----------------------------TIMEOUT Loss in Congestion State Set\n");
      tcb->m_ssThresh = m_BWE*m_minRtt.GetSeconds()/m_a;
      tcb->m_cWnd = 1;
    }
  else if(newState == TcpSocketState::CA_RECOVERY)
    {
      NS_LOG_INFO("----------------------------Recovery in Congestion State Set\n");
      //  m_avoidance =  Simulator::Schedule(Simulator::Now(), &TcpFrGfr::CongestionAvoidanceEvent, this, tcb);
      // NS_LOG_UNCOND("Calling avoidance event");
      // CongestionAvoidanceEvent(tcb);

      
    }
    else{
      NS_LOG_INFO("----------------------------Some other state in Congestion State Set: " << newState << "\n");
    //   if (m_avoidance.IsRunning ())
    // { 
    //   NS_LOG_UNCOND("Cancelling avoidance event at time: " << Simulator::Now());
    //   Simulator::Cancel (m_avoidance);
    // }
    }
}
void 
TcpFrGfr::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{   
   NS_LOG_FUNCTION (this << tcb << segmentsAcked);
    NS_LOG_INFO("In increasewindow of frgfr\n");
    if (tcb->m_cWnd >= tcb->m_ssThresh)
    {
      CongestionAvoidance (tcb, segmentsAcked);
    }
    else{
      TcpNewReno::IncreaseWindow(tcb, segmentsAcked);

    }
}


uint32_t 
TcpFrGfr::GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight)
{   
   NS_LOG_FUNCTION (this << tcb << bytesInFlight);

    NS_LOG_INFO("In getssthresh of frgfr\n");

    return (m_BWE*m_minRtt.GetSeconds()/m_a);
}


void 
TcpFrGfr::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{   
     NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    NS_LOG_INFO("In congavoid of frgfr\n");
    NS_LOG_INFO("SSthresh: " << tcb->m_ssThresh << "\nCongestion window: " << tcb->m_cWnd << "\nbwe*rtt: " << m_BWE*m_minRtt.GetSeconds());
    // TcpNewReno::CongestionAvoidance(tcb, segmentsAcked);
    if((tcb->m_cWnd) > (tcb->m_ssThresh) && (tcb->m_cWnd < m_BWE*m_minRtt.GetSeconds())){
      tcb->m_ssThresh += (m_BWE*m_minRtt.GetSeconds() - tcb->m_ssThresh)/2;
      NS_LOG_INFO("SSThresh updated in avoidance: " << tcb->m_ssThresh);

    }
}
std::string
TcpFrGfr::GetName () const
{
  return "TcpFrGfr";
}

Ptr<TcpCongestionOps>
TcpFrGfr::Fork ()
{
  return CopyObject<TcpFrGfr> (this);
}

void
TcpFrGfr::CongestionAvoidanceEvent(Ptr<TcpSocketState> tcb){
    

    NS_LOG_INFO("Inside avoidance event. Time: " << Simulator::Now());
    if((tcb->m_cWnd) > (tcb->m_ssThresh) && (tcb->m_cWnd < m_BWE*m_minRtt.GetSeconds())){
    NS_LOG_INFO("Inside avoidance event IF--------------------");

      tcb->m_ssThresh += (m_BWE*m_minRtt.GetSeconds() - tcb->m_ssThresh)/2;
      NS_LOG_INFO("SSThresh updated in avoidance event: " << tcb->m_ssThresh);
    }
    NS_LOG_INFO("Scheduling avoidance event at time: " << Simulator::Now());
    Time newTime = Simulator::Now() + MilliSeconds(500);
    m_avoidance= Simulator::Schedule(newTime, &TcpFrGfr::CongestionAvoidanceEvent, this, tcb);
  }

}