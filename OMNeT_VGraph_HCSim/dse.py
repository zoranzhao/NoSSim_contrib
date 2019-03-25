import fileinput
import json 
from subprocess import call
import os
"""
# sim_config.json:
{
   "edge": {
      "total_number": 6,
      "edge_id": [0, 1, 2, 3, 4, 5],
      "edge_type": ["0", "0", "0", "0", "0", "0"],
      "edge_core_number": [1, 1, 1, 1, 1, 1],
      "edge_ipv4_address": ["192.168.4.9", "192.168.4.8", "192.168.4.4", "192.168.4.14", "192.168.4.15", "192.168.4.16"],
      "edge_ipv6_address": ["100:0:200:0:300:0:400:", "100:0:200:0:300:0:500:", "100:0:200:0:300:0:600:", "100:0:200:0:300:0:700:", "100:0:200:0:300:0:800:", "100:0:200:0:300:0:900:"],
      "edge_mac_address": ["00:01:00:00:00:00", "00:02:00:00:00:00", "00:03:00:00:00:00", "00:04:00:00:00:00", "00:05:00:00:00:00", "00:06:00:00:00:00"]
   },
   "gateway": {
      "total_number": 6,
      "gateway_id": 6,
      "gateway_core_number": 1,
      "gateway_ipv4_address": "192.168.4.1",
      "gateway_ipv6_address": "100:0:200:0:300:0:300:",
      "gateway_mac_address": "00:07:00:00:00:00"
   },
   "ap_mac_address": "00:10:00:00:00:00",
   "network":{
      "type": "802.11",
      "mode": "n",
      "bitrate": "600Mbps"
   }
}

# app_config.json:
{
   "N": 5,
   "M": 5,
   "FusedLayers": 16,
   "runtime": "WST" 
}
"""

power_table = {
   "802.11":{
      "g":{
            "receiverIdlePowerConsumption": "2.484mW",
            "receiverBusyPowerConsumption": "190.8mW",
            "receiverReceivingPowerConsumption": "190.8mW",
            "transmitterIdlePowerConsumption": "2.484mW",
            "transmitterTransmittingPowerConsumption": "892.8mW"
       },
      "b":{
            "receiverIdlePowerConsumption": "2.484mW",
            "receiverBusyPowerConsumption": "190.8mW",
            "receiverReceivingPowerConsumption": "190.8mW",
            "transmitterIdlePowerConsumption": "2.484mW",
            "transmitterTransmittingPowerConsumption": "979.2mW"
       }
   },
   "802.15.4":{
         "receiverIdlePowerConsumption": "0.0038mW",
         "receiverBusyPowerConsumption": "0mW",
         "receiverReceivingPowerConsumption": "23.81mW",
         "transmitterIdlePowerConsumption": "0.0038mW",
         "transmitterTransmittingPowerConsumption": "34.58mW"
   },
   "pi0":{
      "idle": 0.5, #0.1A * 5V
      "busy": 1.15 #0.23A * 5V
   },
   "pi3":{
      "idle": 1.5, #0.3A * 5V
      "busy": 4.25 #0.85A * 5V   
   }
}



def sim_config(total_number = 6, 
                      data_source_number = 1, 
                      edge_type = ["2", "2", "2", "2", "2", "2"],
                      edge_core_number = [1, 1, 1, 1, 1, 1],
                      edge_ipv4_address = ["192.168.4.9", "192.168.4.8", "192.168.4.4",
                                           "192.168.4.14", "192.168.4.15", "192.168.4.16"],
                      edge_ipv6_address = ["100:0:200:0:300:0:400:", "100:0:200:0:300:0:500:", "100:0:200:0:300:0:600:",
                                           "100:0:200:0:300:0:700:", "100:0:200:0:300:0:800:", "100:0:200:0:300:0:900:"],
                      edge_mac_address = ["00:01:00:00:00:00", "00:02:00:00:00:00", "00:03:00:00:00:00", 
                                          "00:04:00:00:00:00", "00:05:00:00:00:00", "00:06:00:00:00:00"],
                      gateway_core_number = 1, 
                      gateway_ipv4_address = "192.168.4.1",
                      gateway_ipv6_address = "100:0:200:0:300:0:300:",
                      gateway_mac_address = "00:07:00:00:00:00",
                      ap_mac_address = "00:10:00:00:00:00", 
                      network = {
                         "type": "802.11",
                         "mode": "n", 
                         "bitrate": "600Mbps"
                      },
                      N = 5,
                      M = 5,
                      FusedLayers = 16,
                      runtime = "WST"
                     ):
# edge parameters
    #total_number = 6
    #data_source_number = 1
    edge_id = range(total_number)
    edge_type = edge_type[:total_number]
    edge_core_number = edge_core_number[:total_number]
    edge_ipv4_address = edge_ipv4_address[:total_number]
    edge_ipv6_address = edge_ipv6_address[:total_number]
    edge_mac_address = edge_mac_address[:total_number]

# gateway parameters
    #gateway_core_number = 1
    #gateway_ipv4_address = "192.168.4.1"
    #gateway_ipv6_address = "100:0:200:0:300:0:300:"
    #gateway_mac_address = "00:07:00:00:00:00"
    
# ap and network parameters
    #ap_mac_address = "00:10:00:00:00:00"
    #network = {
    #  "type": "802.11",
    #  "mode": "n", 
    #  "bitrate": "600Mbps"
    #}
   
    sim_config_json = {
        "edge": {
            "total_number": total_number,
            "edge_id": edge_id,
            "edge_type": edge_type,
            "edge_core_number": edge_core_number,
            "edge_ipv4_address": edge_ipv4_address,
            "edge_ipv6_address": edge_ipv6_address,
            "edge_mac_address": edge_mac_address
         },
        "gateway": {
            "total_number": total_number,
            "gateway_id": total_number,
            "gateway_core_number": gateway_core_number,
            "gateway_ipv4_address": gateway_ipv4_address,
            "gateway_ipv6_address": gateway_ipv6_address,
            "gateway_mac_address": gateway_mac_address
         },
        "ap_mac_address": ap_mac_address,
        "network":network
    }
    with open("src/sim_config.json", "w") as jfile:
        json.dump(sim_config_json, jfile, sort_keys=False, indent=4, separators=(',', ': ')) 

    app_config_json = {
      "N": N,
      "M": M,
      "FusedLayers": FusedLayers,
      "runtime": runtime 
    }

    with open("src/app_config.json", "w") as jfile:
        json.dump(app_config_json, jfile, sort_keys=False, indent=4, separators=(',', ': ')) 


def omnetpp_ini():
    with open("src/sim_config.json") as jfile:
        sim_config_json = json.load(jfile) 
    with open("src/omnetpp.ini.template", "r") as f_in, open("src/omnetpp.ini", "w") as f_out:
        for line in f_in:
            #number of edges
            f_out.write(line)
            if "energy settings" in line:
                consumer_path = "**.cliHost[*].wlan.radio.energyConsumer."
                if sim_config_json["network"]["type"] == "802.11": 
                    power_number = power_table["802.11"][sim_config_json["network"]["mode"]]
                else:
                    power_number = power_table["802.15.4"]
                f_out.write(consumer_path + "receiverIdlePowerConsumption = " + power_number["receiverIdlePowerConsumption"] + os.linesep)
                f_out.write(consumer_path + "receiverBusyPowerConsumption = " + power_number["receiverBusyPowerConsumption"] + os.linesep)
                f_out.write(consumer_path + "receiverReceivingPowerConsumption = " + power_number["receiverReceivingPowerConsumption"] + os.linesep)
                f_out.write(consumer_path + "transmitterIdlePowerConsumption = " + power_number["transmitterIdlePowerConsumption"] + os.linesep)
                f_out.write(consumer_path + "transmitterTransmittingPowerConsumption = " + power_number["transmitterTransmittingPowerConsumption"] + os.linesep)
            if "number of edges" in line:
                f_out.write("Cluster.numCli = " + str(sim_config_json["edge"]["total_number"]+1) + os.linesep)
            if "nic settings" in line:
                for device_id in sim_config_json["edge"]["edge_id"]:
                    f_out.write("**.cliHost[" + str(device_id) + "].wlan.mac.address = " + "\"" + sim_config_json["edge"]["edge_mac_address"][device_id] + "\"" + os.linesep)
                f_out.write("# gateway mac address"+ os.linesep)
                f_out.write("**.cliHost[" + str(sim_config_json["gateway"]["gateway_id"]) + "].wlan.mac.address = " 
                            + "\"" + sim_config_json["gateway"]["gateway_mac_address"] + "\"" + os.linesep)
                #802.11 will be deployed with a Access Point 
                if sim_config_json["network"]["type"] == "802.11":
                   f_out.write("**.ap.wlan[*].mac.address = "+ "\"" + sim_config_json["ap_mac_address"]+ "\"" + os.linesep)
                   f_out.write("**.mgmt.accessPointAddress = "+ "\"" + sim_config_json["ap_mac_address"]+ "\"" + os.linesep)
            if "phy settings" in line:
                if sim_config_json["network"]["type"] == "802.11":
                   f_out.write("**.opMode = " + "\"" + sim_config_json["network"]["mode"] + "\"" + os.linesep)
                   f_out.write("**.bitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep)
                   f_out.write("**.basicBitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep)
                   f_out.write("**.controlBitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep + os.linesep)
                   if sim_config_json["network"]["mode"] == "n": 
                       f_out.write("**.errorModelType = \"\"" + os.linesep)
                       f_out.write("**.numAntennas = 4" + os.linesep)
                   f_out.write("**.mac.EDCA = true" + os.linesep)
                   f_out.write("**.mac.maxQueueSize = 100000" + os.linesep)
                   f_out.write("**.mac.rtsThresholdBytes = 2346B" + os.linesep)
                   f_out.write("**.mac.retryLimit = 7" + os.linesep)
                   f_out.write("**.mac.cwMinData = 31" + os.linesep)
                   f_out.write("**.mac.cwMinBroadcast = 31" + os.linesep)
                   f_out.write("**.radio.transmitter.power = 200mW" + os.linesep)
                   f_out.write("**.radio.transmitter.headerBitLength = 100b" + os.linesep)
                   f_out.write("**.radio.transmitter.carrierFrequency = 2.4GHz" + os.linesep)
                   f_out.write("**.radio.transmitter.bandwidth = 40MHz" + os.linesep)
                   f_out.write("**.radio.receiver.bandwidth = 40MHz" + os.linesep)
                   f_out.write("**.radio.receiver.sensitivity = -85dBm" + os.linesep)
                   f_out.write("**.radio.receiver.snirThreshold = 4dB" + os.linesep)

                if sim_config_json["network"]["type"] == "802.15.4":
                   f_out.write("**.queueLength = 1000" + os.linesep)
                   f_out.write("**.macMaxFrameRetries = 100" + os.linesep)
                   f_out.write("**.macMaxCSMABackoffs = 101" + os.linesep)
                   f_out.write("**.wlan.radio.transmitter.preambleDuration = 0" + os.linesep)

def cluster_ned():
    with open("src/sim_config.json") as jfile:
        sim_config_json = json.load(jfile) 
    if sim_config_json["network"]["type"] == "802.11":
        call(["cp", "src/Cluster.ned.wifi", "src/Cluster.ned"])        
    if sim_config_json["network"]["type"] == "802.15.4":
        call(["cp", "src/Cluster.ned.6lowpan", "src/Cluster.ned"])        

def remake_lwip():
    with open("src/sim_config.json") as jfile:
        sim_config_json = json.load(jfile) 

    for line in fileinput.input("/home/slam/OMNET/lwip-hcsim/ports/hcsim/lwipopts.h", inplace=True):
       if "#define LWIP_IPV6 " in line:
           if sim_config_json["network"]["type"] == "802.15.4":
               print "#define LWIP_IPV6          1\n",
           else :
               print "#define LWIP_IPV6          0\n",
       elif "#define LWIP_IPV4 " in line:
           if sim_config_json["network"]["type"] == "802.15.4":
               print "#define LWIP_IPV4          0\n",
           else :
               print "#define LWIP_IPV4          1\n",
       else:
           print "%s" % (line),

    call(["make", "-C", "~/OMNET/lwip-hcsim/ports", "clean"])
    call(["make", "-C", "~/OMNET/lwip-hcsim/ports", "-j", "4"])

def remake_runtime():
    call(["make", "-C", "~/OMNET/NoSSim_contrib/DeepThings/port/hcsim_lwip", "clean"])
    call(["make", "-C", "~/OMNET/NoSSim_contrib/DeepThings/port/hcsim_lwip"])

def remake_app():
    call(["make", "cleanall"])
    call(["make", "makefiles"])
    call(["make", "clean"])
    call(["make"])

if __name__ == "__main__":
   sim_config(
                       total_number = 2, 
                       edge_type = ["0", "0", "1", "1", "1", "0"],
                       data_source_number = 0,
                       network = {
                         "type": "802.15.4",
                         "mode": "g", 
                         "bitrate": "54Mbps"
                       }
                    )
   omnetpp_ini()
   cluster_ned()

   remake_lwip()
   remake_runtime()
   remake_app()
   call(["make", "test"])

