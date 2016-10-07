/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "point-to-point-channel.h"
#include "point-to-point-net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <iostream>

NS_LOG_COMPONENT_DEFINE ("PointToPointChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PointToPointChannel);

TypeId 
PointToPointChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PointToPointChannel")
    .SetParent<Channel> ()
    .AddConstructor<PointToPointChannel> ()
    .AddAttribute ("Delay", "Transmission delay through the channel",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&PointToPointChannel::m_delay),
                   MakeTimeChecker ())
	.AddAttribute ("Jitter", "Implement Jitter",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PointToPointChannel::m_jitter),
                   MakeUintegerChecker<uint16_t> ())
	.AddAttribute ("k", "Set gamma distribution shape value",
                   DoubleValue (5.0),
                   MakeDoubleAccessor (&PointToPointChannel::m_k),
                   MakeDoubleChecker<int64_t>())
        .AddAttribute ("theta", "Set gamma distribution scale value",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&PointToPointChannel::m_theta),
                   MakeDoubleChecker<int64_t>())
         .AddAttribute ("monitor", "download or upload mode, 0 set mode to download",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PointToPointChannel::m_monitor),
                   MakeUintegerChecker<uint16_t> ())		
         .AddAttribute ("mode", "download or upload mode, 0 set mode to download",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PointToPointChannel::m_mode),
                   MakeUintegerChecker<uint16_t> ())
         .AddAttribute ("transparent", "set with no delay unlimited bandwidth, 0 set link transparent",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PointToPointChannel::m_transparent),
                   MakeUintegerChecker<uint16_t> ())
         .AddAttribute ("coreRouter", "core router flag for calculating througput, 1 set in core router",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PointToPointChannel::m_coreRouter),
                   MakeUintegerChecker<uint16_t> ())          
                   				   
    .AddTraceSource ("TxRxPointToPoint",
                     "Trace source indicating transmission of packet from the PointToPointChannel, used by the Animation interface.",
                     MakeTraceSourceAccessor (&PointToPointChannel::m_txrxPointToPoint))
  ;
  return tid;
}

//
// By default, you get a channel that 
// has an "infitely" fast transmission speed and zero delay.
PointToPointChannel::PointToPointChannel()
  :
    Channel (),
    m_delay (Seconds (0.)),
    m_nDevices (0),
    m_jitter (0),
    m_k (5.0),
    m_theta (2.0),
    m_monitor (0),
    m_mode (0),
    m_prevRcvTime(Seconds (0.)),
    m_prevRcvTime1(Seconds (0.)),
    m_prevRcvTime2(Seconds (0.)),
    m_previousDelay (0),
    m_previousDelay1 (0),
    m_previousDelay2 (0),
    m_sumPacketFlow (0),
    m_noPacketFlow (0),
    m_firstRecFlow (Seconds (0.)),
    m_lastRecFlow (Seconds (0.)),
    m_firstPacket (true),
    m_transparent (0),
    m_coreRouter (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
PointToPointChannel::Attach (Ptr<PointToPointNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  NS_ASSERT_MSG (m_nDevices < N_DEVICES, "Only two devices permitted");
  NS_ASSERT (device != 0);

  m_link[m_nDevices++].m_src = device;
//
// If we have both devices connected to the channel, then finish introducing
// the two halves and set the links to IDLE.
//
  if (m_nDevices == N_DEVICES)
    {
      m_link[0].m_dst = m_link[1].m_src;
      m_link[1].m_dst = m_link[0].m_src;
      m_link[0].m_state = IDLE;
      m_link[1].m_state = IDLE;
    }
}

bool
PointToPointChannel::TransmitStart (
  Ptr<Packet> p,
  Ptr<PointToPointNetDevice> src,
  Time txTime)
{
  NS_LOG_FUNCTION (this << p << src);
  NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);

  uint32_t wire = src == m_link[0].m_src ? 0 : 1;
  
  
  //std::cout << wire <<" "<< m_jitter<<" "<<m_transparent<< std::endl;
  
  if (m_transparent==1) {
      
      //std::cout << wire <<" "<<m_coreRouter <<" "<< m_monitor <<" "<<m_mode<< std::endl;
      Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (),
                                                  m_delay, &PointToPointNetDevice::Receive,
                                                  m_link[wire].m_dst, p);
      m_txrxPointToPoint (p, src, m_link[wire].m_dst, Seconds(0), m_delay);
      
      if (m_coreRouter==0) {
       // collect througput data
          if (m_monitor==1) {
                  if (m_mode==1){
		          if (wire==0){
			        int32_t packetSize = p->GetSize();
			        if (packetSize >= 100) {
			               //std::cout << wire <<" nomer = "<< m_noPacketFlow<< std::endl;
				        m_noPacketFlow+=1;
				        if (m_noPacketFlow==1 && m_firstPacket==true) {
				                //std::cout << wire <<" size = "<< p->GetSize()<< std::endl;				           
				                m_firstPacket=false;
					        m_firstRecFlow=Simulator::Now();
					        std::ofstream firstSend;
					        firstSend.open ("firstSend.txt");
					        firstSend << m_firstRecFlow.GetNanoSeconds();
					        firstSend.close();
				         } 
		                }
		          }
	          }

	          if (m_mode==0){
		          if (wire==1){
			        int32_t packetSize = p->GetSize();
			        if (packetSize >= 100) {
			                //std::cout << wire <<" Sending time = "<< Simulator::Now().GetSeconds()<< std::endl;
				        m_lastRecFlow=Simulator::Now();
				        std::ofstream lastReceive;
				        lastReceive.open ("lastReceive.txt");
				        lastReceive << m_lastRecFlow.GetNanoSeconds();
				        lastReceive.close();
				        m_sumPacketFlow+=packetSize-2;
				        std::ofstream recTotal;
				        recTotal.open ("recTotal.txt");
				        recTotal << m_sumPacketFlow;
				        recTotal.close();
				        
			        }
		          }
	         }
	     }
	 }
	 // other pair
	 
	 else if (m_coreRouter==1) {
	 // collect througput data
	 //std::cout <<" test "<< std::endl;
          if (m_monitor==1) {
                if (m_mode==1){
		          if (wire==0){
		                
			        int32_t packetSize = p->GetSize();
			        if (packetSize >= 100) {
			                
				        m_lastRecFlow=Simulator::Now();
				        std::ofstream lastReceive;
				        lastReceive.open ("lastReceive.txt");
				        lastReceive << m_lastRecFlow.GetNanoSeconds();
				        lastReceive.close();
				        m_sumPacketFlow+=packetSize-2;
				        
				        std::ofstream recTotal;
				        recTotal.open ("recTotal.txt");
				        recTotal << m_sumPacketFlow;
				        recTotal.close();
			        }
		          }
	          }

	          if (m_mode==0){
		          if (wire==1){
			        int32_t packetSize = p->GetSize();
			        
			        if (packetSize >= 100) {
			                //std::cout <<" -test "<< std::endl;
				        m_noPacketFlow+=1;
				        if (m_noPacketFlow==1 && m_firstPacket==true) {
				                //std::cout << wire <<" Sending time = "<< Simulator::Now().GetSeconds()<< std::endl;
					        m_firstRecFlow=Simulator::Now();
					        std::ofstream firstSend;
					        firstSend.open ("firstSend.txt");
					        firstSend << m_firstRecFlow.GetNanoSeconds();
					        firstSend.close();
					        
				         } 
			        }
		          }
	          }
	    }
	  }
           
  
          
  }
  
  else if (m_jitter==0)
  {
      
          Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (),
                                          txTime+m_delay, &PointToPointNetDevice::Receive,
                                          m_link[wire].m_dst, p);
                                          
                                                   
          
          // Call the tx anim callback on the net device
          m_txrxPointToPoint (p, src, m_link[wire].m_dst, txTime, txTime+m_delay);
  }
  
  
  else if (m_jitter==1)
  {
          m_previousDelay = (wire==0) ? m_previousDelay1:m_previousDelay2;
          m_prevRcvTime = (wire==0) ? m_prevRcvTime1:m_prevRcvTime2;

          
          
          double cur_delay = GammaRandomValue()+m_delay.GetDouble();
          //std::cout << m_jitter<<" "<< wire <<"  static delay = "<< m_delay.GetMicroSeconds() << std::endl;
          //std::cout << wire <<"  current delay = "<< Time(cur_delay).GetMicroSeconds() << std::endl;
          
          Time rcvTime = Simulator::Now()+ Time(cur_delay);

          // preveting reordering
          if (rcvTime < m_prevRcvTime )
          {            
                  rcvTime= m_prevRcvTime;
                  //std::cout << " new current = "<<rcvTime.GetSeconds()<< std::endl;
                  cur_delay=(rcvTime-Simulator::Now()).GetDouble();
          }



          
          Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (),
                                          Time(cur_delay), &PointToPointNetDevice::Receive,
                                          m_link[wire].m_dst, p);
          
          
          
          
          
          // Call the tx anim callback on the net device
          m_txrxPointToPoint (p, src, m_link[wire].m_dst, Seconds(0),Time(cur_delay));
          
          //std::cout <<"  received time = "<< rcvTime.GetSeconds()<< std::endl;
          
          // store previous delay
          if (wire==0) {
	        m_previousDelay1=cur_delay;
	        m_prevRcvTime1 = rcvTime;
	        //std::cout <<"  size = "<<  p->GetSize()<< std::endl;
          }
            if (wire==1) {
	        m_previousDelay2=cur_delay;
	        m_prevRcvTime2 = rcvTime;
	        
	        
          }
    //}
  }
  
  return true;
}

uint32_t 
PointToPointChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nDevices;
}




Ptr<PointToPointNetDevice>
PointToPointChannel::GetPointToPointDevice (uint32_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (i < 2);
  return m_link[i].m_src;
}

Ptr<NetDevice>
PointToPointChannel::GetDevice (uint32_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetPointToPointDevice (i);
}

Time
PointToPointChannel::GetDelay (void) const
{
  return m_delay;
}

Ptr<PointToPointNetDevice>
PointToPointChannel::GetSource (uint32_t i) const
{
  return m_link[i].m_src;
}

Ptr<PointToPointNetDevice>
PointToPointChannel::GetDestination (uint32_t i) const
{
  return m_link[i].m_dst;
}

bool
PointToPointChannel::IsInitialized (void) const
{
  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);
  return true;
}
double
PointToPointChannel::GammaRandomValue()
{

        
  Ptr<GammaRandomVariable> x = CreateObject<GammaRandomVariable> ();
  x->SetAttribute ("Alpha", DoubleValue (m_k));
  x->SetAttribute ("Beta", DoubleValue (m_theta));
  // The expected value for the k of the values returned by a
  // gammaly distributed random variable is equal to
  //
  // E[value] = alpha * beta .
  //
  double value = x->GetValue ()* double(1000000); // in 1ms
  if (value<0) {
        value =0;
  }
  return value;
}

} // namespace ns3
