import sys 
import socket
from datetime import datetime

# define our target 
if len(sys.argv) != 2: 
    print("invalid amount of argument")
    print("Syntax: invalid")  
    sys.exit()

target = socket.gethostbyname(sys.argv[1]) # translate hostname to IPv4
print("-" * 50)
print("Scanning target " + target)
print("Time started: " + str(datetime.now()))
print("-" * 50)

try: 
    for port in range(6000, 7000):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socket.setdefaulttimeout(1)
        result = s.connect_ex((target, port))
        if result == 0:
            print('Port {} is open'.format(port))
        s.close()

except KeyboardInterrupt:
    print("\n Exiting program")
    sys.exit()

except socket.gaierror:
    print("hostname could not be solved")
    sys.exit()

except socket.error:
    print("couldnt connect to server")
    sys.exit()

