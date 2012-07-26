#!/usr/bin/lua

port="/dev/ttyUSB0"

stations = {
  "http://mp32.bbc.streamuk.com:80/",
  "http://npr.ic.llnwd.net/stream/npr_live24",
  "http://pri-ice.streamguys.biz/pri1",
  "http://128.208.34.80:8002/",
  "http://128.208.34.80:8010/",
  "http://out1.cbc.icy.abacast.com:80/cbc-cbcr1vcr-96",
  "http://stream3.opb.org/radio.mp3",
  "http://media3.abc.net.au/radionational",
  "http://dradio.ic.llnwd.net/stream/dradio_dlf_m_a",
  "http://kexp-mp3-2.cac.washington.edu:8000/",
  "http://2583.live.streamtheworld.com/CBC_R2_VCR_H",
  "http://currentstream1.publicradio.org:80/"
}

function play(n)
  stop()
  player = io.popen("wget -O - "..stations[n].." 2>/dev/null | madplay - 2>/dev/null & echo $!")
  pid = player:read("*n")
end

function stop()
  if pid then
    os.execute("kill "..pid)
  end
end

function main()
  os.execute("stty -F "..port.." 9600")
  sel = 0
  old_sel = 0
  pid = nil
  
  while true do
    ser = io.open(port, "r")
    sel = ser:read("*n")
    if sel ~= old_sel then
      old_sel = sel
      print(string.format("Playing %s: %s", sel, stations[sel]))
      play(sel)
    end
    ser:close()
  end
end

main()