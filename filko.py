#!/usr/bin/python

import os, subprocess, signal

serial_port = '/dev/ttyUSB0'

resistors = [506, 433, 335, 312, 786, 724, 611, 565, 289, 201, 88, 32]

external_streams = {
  2: ('cvlc', 'http://www.bbc.co.uk/radio/listen/live/r4.asx'),
  3: ('mplayer', 'http://out1.cbc.icy.abacast.com:80/cbc-cbcr1vcr-96'),
  5: ('mplayer', 'mms://streaming.radionz.co.nz/national-mbr'),
}

class Player(object):
  def __init__(self):
     self.ext_player = None
     self.serial = open(serial_port)
     
     rvalues = dict(zip(resistors, range(1, len(resistors)+1)))

     last = 0
     self.rval_lbounds = []
     self.selections = []

     for rval in sorted(resistors):
       self.rval_lbounds.append(rval-((rval-last)/2))
       self.selections.append(rvalues[rval])
       last = rval

  def get_selection(self, rval):
    for i, j in reversed(list(enumerate(self.rval_lbounds))):
      if rval >= j: break
    return self.selections[i]

  def play(self, selection):
    self.stop_external()
    if selection in external_streams:
      print "playing %s" % str(external_streams[selection])
      self.ext_player = subprocess.Popen(external_streams[selection])
    else:
      print "mpc play %s" % selection
      os.system("mpc play %s" % selection)

  def stop(self):
    self.stop_external()
    self.stop_mpc()

  def stop_external(self):
    if 'terminate' in dir(self.ext_player):
      print "stopping subprocess"
      #self.ext_player.send_signal(signal.SIGINT)
      self.ext_player.terminate()
  
  def stop_mpc(self):
    print "mpc stop"
    os.system("mpc stop")

  def loop(self):
    selection = 0
    power = False
    sw1 = False
    sw2 = False
  
    while True:
      line = self.serial.readline().strip().split(',')
      if len(line) == 5:
        #print line
        new_selection, selector_val, new_power, new_sw1, new_sw2 = line
        if new_power == '0':
          new_power = True
        else:
          new_power = False
      
        new_selection = self.get_selection(int(selector_val))
        #print " => %s" % new_selection

        if new_power:
          if new_selection != selection or not power:
            print "%s => %s" % (line, new_selection)
            selection = new_selection
            self.play(selection)
          
          power = new_power
        elif power: # power is now off but used to be on
          self.stop()
      
          power = new_power
      else:
        pass #print "x"

p = Player()
p.loop()