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

def deepthings_config(total_number = 6, 
                      data_source_number = 1, 
                      edge_type = ["1", "0", "3", "0", "0", "0"],
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
            if "number of edges" in line:
                f_out.write("Cluster.numCli = " + str(sim_config_json["edge"]["total_number"]+1) + os.linesep)
            if "nic settings" in line:
                for device_id in sim_config_json["edge"]["edge_id"]:
                    f_out.write("**.cliHost[" + str(device_id) + "].wlan.mac.address = " + "\"" + sim_config_json["edge"]["edge_mac_address"][device_id] + "\"" + os.linesep)
                f_out.write("# gateway mac address"+ os.linesep)
                f_out.write("**.cliHost[" + str(sim_config_json["gateway"]["gateway_id"]) + "].wlan.mac.address = " 
                            + "\"" + sim_config_json["gateway"]["gateway_mac_address"] + "\"" + os.linesep)
                f_out.write("**.ap.wlan[*].mac.address = "+ "\"" + sim_config_json["ap_mac_address"]+ "\"" + os.linesep)
                f_out.write("**.mgmt.accessPointAddress = "+ "\"" + sim_config_json["ap_mac_address"]+ "\"" + os.linesep)
            if "phy settings" in line:
                f_out.write("**.opMode = " + "\"" + sim_config_json["network"]["mode"] + "\"" + os.linesep)
                f_out.write("**.bitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep)
                f_out.write("**.basicBitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep)
                f_out.write("**.controlBitrate = "+ sim_config_json["network"]["bitrate"] + os.linesep)
                if sim_config_json["network"]["mode"] == "n": 
                    f_out.write("**.errorModelType = \"\"" + os.linesep)
                    f_out.write("**.numAntennas = 4" + os.linesep)

if __name__ == "__main__":
   deepthings_config(
                       total_number = 3, 
                       data_source_number = 0,
                       network = {
                         "type": "802.11",
                         "mode": "g", 
                         "bitrate": "54Mbps"
                       }
                    )
   omnetpp_ini()
   call(["make", "test"])

