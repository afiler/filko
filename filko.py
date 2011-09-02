#!/usr/bin/python
import os

resistors = [506, 433, 335, 312, 786, 724, 611, 565, 289, 201, 88, 32]
serial = open('/dev/ttyUSB0')


rvalues = dict(zip(resistors, range(1, len(resistors)+1)))

last = 0
rval_lbounds = []
selections = []

for rval in sorted(resistors):
  rval_lbounds.append(rval-((rval-last)/2))
  selections.append(rvalues[rval])
  last = rval

def get_selection(rval):
  for i, j in reversed(list(enumerate(rval_lbounds))):
    if rval >= j: break
  return selections[i]

def loop():
  selection = 0
  power = False
  sw1 = False
  sw2 = False
  
  while True:
    line = serial.readline().strip().split(',')
    if len(line) == 5:
      print line
      new_selection, selector_val, new_power, new_sw1, new_sw2 = line
      if new_power == '0':
        new_power = True
      else:
        new_power = False
      
      new_selection = get_selection(int(selector_val))
      print " => %s" % new_selection

      if new_power:
        if new_selection != selection or not power:
          selection = new_selection
          print "mpc play %s" % selection
          os.system("mpc play %s" % selection)
        
        power = new_power
      elif power: # power is now off but used to be on
        print "mpc stop"
        os.system("mpc stop")
      
        power = new_power
    else:
      print "x"

loop()