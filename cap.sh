#!/bin/bash

#raspiraw -md 4 -t 5000 -ts tstamps.csv -hd0 hd0.32k -r "503D,80" -sr 5 -o /dev/shm/w.%04d.raw -g 1000 -e 10000
raspiraw -md 4 -t 5000 -ts tstamps.csv -hd0 hd0.32k $@ -sr 5 -o /dev/shm/j.%04d.raw -g 1000 -e 10000
cat hd0.32k /dev/shm/j.0011.raw >w.0011.raw
dcraw -v -T j.0011.raw 
echo "Picture is j.0011.tiff"
gpicview `pwd`/w.0011.tiff &
