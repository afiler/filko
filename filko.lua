#!/usr/bin/lua

port="/dev/ttyUSB0"

stations = {
  "http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk",
  "http://nprdmp.ic.llnwd.net/stream/nprdmp_live01_mp3",
  "http://pri-ice.streamguys.biz/pri1",
  "http://128.208.34.80:8002/",
  "http://128.208.34.80:8010/",
  "http://stream3.opb.org/radio.mp3",
  "http://newsstream1.publicradio.org:80/",
  "http://dradio.ic.llnwd.net/stream/dradio_dlf_m_a",
  "http://kexp-mp3-2.cac.washington.edu:8000/",
  "http://currentstream1.publicradio.org:80/",
  "http://2583.live.streamtheworld.com/CBC_R2_VCR_H",
  "http://6093.live.streamtheworld.com:80/CBC_R1_VCR_H_SC", -- AAC
}

function play(n)
  stop()
  -- player = io.popen("wget -O - "..stations[n].." 2>/dev/null | madplay - 2>/dev/null & echo $!")
  url = stations[n]
  player = io.popen("while true; do echo Trying "..url.." 1>&2 ; wget -O - "..url.." | madplay - ; sleep 1 ; done")
  -- pid = player:read("*n")
  pid = 1
end

function stop()
  --if pid then
  --  os.execute("kill "..pid)
  --end
  os.execute("killall madplay wget sh; killall -9 madplay wget sh")
end

function main()
  os.execute("stty -F "..port.." 9600")
  sel = 0
  old_sel = 0
  pid = nil
  
  while true do
    ser = io.open(port, "r")
    sel = ser:read("*n")
    if sel and stations[sel] and sel ~= old_sel then
      old_sel = sel
      print(string.format("Playing %s: %s", sel, stations[sel]))
      play(sel)
    end
    ser:close()
  end
end

main()