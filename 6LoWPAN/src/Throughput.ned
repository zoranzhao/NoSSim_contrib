//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


//package .;


import NoS.applications.ethernet.CliWrapper;
import NoS.applications.ethernet.SrvWrapper;




import inet.mobility.single.CircleMobility;
import inet.mobility.static.StationaryMobility;
import inet.node.wireless.AccessPoint;

import inet.power.contract.IEnergyStorage;
import inet.power.contract.IEnergyGenerator;


import inet.linklayer.ieee802154.Ieee802154NarrowbandNic;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;


module ThroughputClient
{
    parameters:
        @display("i=device/wifilaptop");
        @networkNode();
        *.interfaceTableModule = "";
        string energyStorageType = default("");
        string energyGeneratorType = default("");
        *.energySourceModule = default(energyStorageType != "" ? absPath(".energyStorage") : "");
        //@node();

    gates:
        input radioIn @directIn;

    submodules:
        energyStorage: <energyStorageType> like IEnergyStorage if energyStorageType != "" {
            parameters:
                @display("p=50,100;i=block/plug;is=s");
        }
        energyGenerator: <energyGeneratorType> like IEnergyGenerator if energyGeneratorType != "" {
            parameters:
                @display("p=50,150;i=block/plug;is=s");
        }

        cli: CliWrapper {
            parameters:
                registerSAP = false;
                @display("b=40,24;p=180,60,col");
        }
        wlan: Ieee802154NarrowbandNic {
           parameters:
                @display("p=120,158;q=queue");
        }

        //wlan: Ieee80211Nic {
        //    parameters:
        //        @display("p=112,134;q=queue");
        //        mgmtType = "Ieee80211MgmtSTASimplified";
        //}


        mobility: StationaryMobility {
            parameters:
                @display("p=50,141");
        }

    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        cli.out --> wlan.upperLayerIn;
        wlan.upperLayerOut --> cli.in;
}

module ThroughputServer
{
    parameters:
        @display("i=device/wifilaptop");
        @networkNode();
        *.interfaceTableModule = "";
        string energyStorageType = default("");
        string energyGeneratorType = default("");
        *.energySourceModule = default(energyStorageType != "" ? absPath(".energyStorage") : "");

        //@node();
    gates:
        input radioIn @directIn;

    submodules:
	//Old model
        //notificationBoard: NotificationBoard {
        //    parameters:
        //        @display("p=60,70");
        //}

        energyStorage: <energyStorageType> like IEnergyStorage if energyStorageType != "" {
            parameters:
                @display("p=50,100;i=block/plug;is=s");
        }
        energyGenerator: <energyGeneratorType> like IEnergyGenerator if energyGeneratorType != "" {
            parameters:
                @display("p=50,150;i=block/plug;is=s");
        }

        srv: SrvWrapper {
            parameters:
                registerSAP = false;
                @display("p=210,68,col");
        }

        wlan: Ieee802154NarrowbandNic {
           parameters:
                @display("p=120,158;q=queue");
        }

        //wlan: Ieee80211Nic {
        //   parameters:
        //        @display("p=120,158;q=queue");
        //        mgmtType = "Ieee80211MgmtSTASimplified";
        //}
        mobility: StationaryMobility {
            parameters:
                @display("p=50,141");
        }
    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        wlan.upperLayerOut --> srv.in;
        srv.out --> wlan.upperLayerIn;
}

network Throughput
{
    parameters:
        int numCli;
        @display("b=400,400");
    submodules:
        cliHost[numCli]: ThroughputClient {
            parameters:
                @display("p=100,100;r=,,#707070");
        }
        srvHost: ThroughputServer {
            parameters:
                @display("p=150,150;r=,,#707070");
        }
        //ap: AccessPoint {
        //    parameters:
        //        @display("p=200,200;r=,,#707070");
        //        wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
        //}
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            parameters:
                @display("p=61,46");
        }
        //radioMedium: Ieee80211ScalarRadioMedium {
        //    parameters:
        //        @display("p=61,46");
        //}

}

