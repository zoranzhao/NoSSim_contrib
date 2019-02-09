/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "CliWrapper.h"

#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"


int TotalClients = 0;
long TotalPkt = 0;

namespace inet {

Define_Module(CliWrapper);

simsignal_t CliWrapper::sentPkSignal = registerSignal("sentPk");
simsignal_t CliWrapper::rcvdPkSignal = registerSignal("rcvdPk");

CliWrapper::~CliWrapper()
{
    cancelAndDelete(timerMsg);
}

void CliWrapper::initialize(int stage)
{
    cSimpleModule::initialize(stage);


    if (stage == INITSTAGE_LOCAL) {
        reqLength = &par("reqLength");
        respLength = &par("respLength");
        sendInterval = &par("sendInterval");

        localSAP = par("localSAP");
        remoteSAP = par("remoteSAP");
        clientID = par("clientID");
        seqNum = 0;
        WATCH(seqNum);

        // statistics
        packetsSent = packetsReceived = 0;
        WATCH(packetsSent);
        WATCH(packetsReceived);

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        //if (isGenerator())
            //timerMsg = new cMessage("generateNextPacket");

        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));

        //if (isNodeUp() && isGenerator())
        //    scheduleNextPacket(true);
    }
}

void CliWrapper::handleMessage(cMessage *msg)
{
    unsigned int lwip_pkt_size;
    EtherWrapperResp *datapacket;
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");
    if (msg->isSelfMessage()) {
        bool registerSAP = par("registerSAP");
        if (registerSAP)
            registerDSAP(localSAP);
        if (strcmp(msg->getName(), "ServerToCli") == 0) {
		lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();
		datapacket = new EtherWrapperResp("lwip_msg", IEEE802CTRL_DATA);
		datapacket->setFileBufferArraySize(lwip_pkt_size);
		datapacket->setByteLength( lwip_pkt_size);
		for(unsigned int ii=0; ii<lwip_pkt_size; ii++){
			datapacket->setFileBuffer(ii, ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBuffer(ii));
		}
		destMACAddress = resolveDestMACAddress();
		if (destMACAddress.isUnspecified())
		    return;
       		sendPacket(datapacket); 
		System -> NetworkInterfaceCard1->notify_sending();		
		delete msg; 
	} 
    }
    else
        receivePacket(check_and_cast<cPacket *>(msg));
}

bool CliWrapper::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_APPLICATION_LAYER && isGenerator())
	    {
		std::cout<<"handleOperationStage ... ... ... ... ..."<<std::endl;
		//scheduleNextPacket(true);
	    }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            cancelNextPacket();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH)
            cancelNextPacket();
    }
    else
        throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

bool CliWrapper::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

bool CliWrapper::isGenerator()
{
    return par("destAddress").stringValue()[0];
}

void CliWrapper::cancelNextPacket()
{
    if (timerMsg)
        cancelEvent(timerMsg);
}

MACAddress CliWrapper::resolveDestMACAddress()
{
    MACAddress AP;
    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;
    MACAddress AddrNode3;
    MACAddress AddrAll;

    const char *addrNode0 = par("AddrNode0");
    const char *addrNode1 = par("AddrNode1");
    const char *addrNode2 = par("AddrNode2");
    const char *addrNode3 = par("AddrNode3");

    AddrNode0.tryParse(addrNode0);
    AddrNode1.tryParse(addrNode1);
    AddrNode2.tryParse(addrNode2);
    AddrNode3.tryParse(addrNode3);

    AddrAll.tryParse("ff:ff:ff:ff:ff:ff");
    //AP.tryParse("10:00:00:00:00:00");

    //The address is used in 6LowPAN test
    if(System->NodeID == 0) //send to node 1
       return AddrNode1;
    if(System->NodeID == 1) //send to node 0
       return AddrNode0;
    //AddrAll.tryParse("00:10:00:00:00:00");

    return  AddrAll;

}

void CliWrapper::registerDSAP(int dsap)
{
    EV_DEBUG << getFullPath() << " registering DSAP " << dsap << "\n";

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDsap(dsap);
    cMessage *msg = new cMessage("register_DSAP", IEEE802CTRL_REGISTER_DSAP);
    msg->setControlInfo(etherctrl);

    send(msg, "out");
}

void CliWrapper::sendPacket(cMessage *msg)
{
//    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->NodeID)==2)
//    std::cout << "Sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;
//    std::cout << "EtherMserCli sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;    

    EtherWrapperResp *datapacket = check_and_cast<EtherWrapperResp *>(msg);
    seqNum++;

    char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating packet `" << msgname << "'\n";

    datapacket->setName(msgname);   
    datapacket->setRequestId(seqNum);

    //long len = reqLength->longValue();

    //datapacket->setByteLength(len);

    //long respLen = respLength->longValue();
    //datapacket->setResponseBytes(respLen);

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(remoteSAP);
    etherctrl->setDest(destMACAddress);
    datapacket->setControlInfo(etherctrl);



    emit(sentPkSignal, datapacket);

    send(datapacket, "out");
    packetsSent++;
}

void CliWrapper::receivePacket(cPacket *msg)
{

    EtherWrapperResp *datapacket = check_and_cast<EtherWrapperResp *>(msg);
    char* image_buf;
    int buf_size =    datapacket->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=datapacket->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(image_buf, datapacket->getFileBufferArraySize());
    packetsReceived++;
    emit(rcvdPkSignal, msg);
    delete msg;
}

void CliWrapper::finish()
{
    //cancelAndDelete(timerMsg);
    //timerMsg = nullptr;
    std::cout<<System->NodeID<<"	"<<std::endl;
}

} // namespace inet



