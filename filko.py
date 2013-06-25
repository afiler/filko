#!/usr/bin/python

import os, subprocess, signal, yaml
from time import localtime, strftime, sleep

class Player(object):
  def __init__(self, config_file="filko.yaml"):
    self.config = yaml.load(file(config_file))
    self.streams = self.config['streams']
    
    self.ext_player = None
    
    try:
      import serial
      self.serial = serial.Serial(self.config['serial_port'])
    except:
      self.serial = open(self.config['serial_port'])
      
    self.selection = 0
    self.power = False
    self.sw1 = False
    self.sw2 = False
    self.timestr = ""

  def get_selection(self, rval):
    for i, j in reversed(list(enumerate(self.rval_lbounds))):
      if rval >= j: break
    return self.selections[i]

  def play(self, selection):
    print "Play: %s" % repr(selection)
    print "ext_player: %s\n%s" % (self.ext_player, dir(self.ext_player))
    self.stop_external()
    
    stream = self.streams[selection]
    print "playing %s" % str(stream)
    if 'player' in stream:
      self.ext_player = self.popen((stream['player'], stream['url']))
    else:
      self.ext_player = self.popen(
        'curl "%s" | madplay -' % stream['url'], shell=True)
    #if selection in streams:
    #  print "playing %s" % str(streams[selection])
    #  self.ext_player = self.popen(streams[selection])
    #else:
    #  self.ext_player = self.popen("/usr/bin/mpc play %s" % selection)

  def stop(self):
    self.stop_external()
    self.stop_mpc()

  def stop_external(self):
    if 'terminate' in dir(self.ext_player):
      print "stopping subprocess"
      #self.ext_player.send_signal(signal.SIGINT)
      self.ext_player.terminate()
    else:
      print "subprocess died?"
  
  def stop_mpc(self):
    print "mpc stop"
    self.popen("mpc stop")
  
  def popen(self, s, *args, **kw):
    try:
      print "[popen] {0}".format(s)
      return subprocess.Popen(s, *args, **kw)
    except Exception as e:
      print "[popen error {0}] {1}".format(e.errno, e.strerror)
      
  def send(self, s):
    print "send[1]: handling"
    self.handle_serial()
    print "send[2]: writing: %s" % s.strip()
    self.serial.write(s)
    print "send[3]: handling"
    self.handle_serial(wait_for_ok=True)
    print "send[4]: done"
  
  def send_status(self, s):
    self.send("s%s" % s)

  def handle_serial(self, wait_for_ok=False):
    ok = False
    
    while self.serial.inWaiting():
      ok = ok or self.handle_line()
    
    if wait_for_ok and not ok:
      self.handle_line()
    
  def handle_line(self):
    line = self.serial.readline().strip()
    print "handle_line: %s" % line

    if line[0] == 'r':
      fields = line[1:].split(',')
      new_selection_str, new_power, new_sw1, new_sw2 = fields
      if new_power == '1':
        new_power = True
      else:
        new_power = False
  
      new_selection = int(new_selection_str) + 1

      if new_power:
        if new_selection != self.selection or not self.power:
          print "%s => %s" % (line, new_selection)
          self.selection = new_selection
          self.play(self.selection)
      
        self.power = new_power
      elif self.power: # self.power is now off but used to be on
        self.stop()
  
        self.power = new_power
    elif line == 'ok':
      pass #print "OK"
    else:
      print "Error: %s" % line

  def setup(self):
    print "setting up"
    for i in range(0, 12):
      s = "t%02dPyTitle #%d\n"  % (i, i)
      print s,
      self.send(s)

  def loop(self):
    timestr_sec = ""

    while True:
      self.handle_serial()
      
      new_timestr = strftime("c%H%M", localtime())
      #new_timestr_sec = strftime("%H:%M:%S", localtime())
      #if timestr_sec != new_timestr_sec:
      #  self.send_status(new_timestr_sec)
      #  timestr_sec = new_timestr_sec
        
      if self.timestr != new_timestr:
        print "sending time"
        self.send(new_timestr)
        self.timestr = new_timestr
        print "done"
      
      sleep(0.1)

p = Player()
#p.setup()
p.loop()