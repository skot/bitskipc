import json

#load the json lines
json_lines = []
with open("stratum_dump.json") as file:
    for line in file:
        json_lines.append(json.loads(line))

#load the stratum serial hex lines
hex_lines = []
with open("stratum_serial_1M_only.txt") as file:
    for line in file:
        hex_lines.append(line)

#iterate through the hex lines and pull out the TX: lines
serial_tx_lines = []
for line in hex_lines:
    #print only the TX: lines
    if line.startswith("TX:"):
        #strip newlines before adding to list
        hex = line.rstrip()
        #strip string "TX: " before adding to list
        hex = hex[4:]
        serial_tx_lines.append(hex)


#iterate through the json lines and match nonces from serial_tx_lines
for jline in json_lines:
    if jline.get("method") == "mining.submit":
        for line in serial_tx_lines:
            reversed_hex_nonce = line[15:17] + line[12:14] + line[9:11] + line[6:8]
            #match the nonce from the serial_tx_lines to the nonce from the json_lines
            stratum_nonce = jline.get("params")[4]
            #make stratum_nonce capital letters
            stratum_nonce = stratum_nonce.upper()
            if stratum_nonce == reversed_hex_nonce:
                print("["+line+"]")
    print(jline)
