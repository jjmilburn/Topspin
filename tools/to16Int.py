#Josh Milburn
#git - jjmilburn / jjmilburn@gmail.com
#tool for converting 12 bit csv files to 16 bit
#used for debugging samples from spark core serial output

import csv
import struct

#import the csv file containing 0 to +4095 values from ADC
twelveBitSamplesFile = open('samples12bit.csv', "r")
twelveBitSamplesReader = csv.reader(twelveBitSamplesFile)

sixteenBitSamplesBin = []

#convert these to 16 bit ('short', or 'h') and write back to file
with open('16bit.raw', 'wb') as f:
    for sample in twelveBitSamplesReader:
        #full scale and integer conversion -
        scaledInt = 8*(int(sample[0])-4095)
        print scaledInt
        packedInt = struct.pack('h',int(sample[0]))
        f.write(packedInt)

#to read these as 'full scale', import as unsigned 16 bit

