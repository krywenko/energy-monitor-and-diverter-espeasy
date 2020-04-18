# energy-monitor-and-diverter-espeasy D2 uno/wifi boards with espeasy install on the  esp8266 chip and  these sketched flashed to uno
- the sensor board is emontx arduino shield ...
the hz detection version suit able for any frequence grid  standard or non standard   uses  pin  as 9,10,3. and 11  as cascadeing ssr diversion
 pin 5 and 6 are used to  show  activity but can be used as ssr diversion too but only if on 60hz grids ( you can use it on other grids  diversion  accracy drops)
 
 you can display info on lcd screen or operate headless only using  espeasy to display info and send it to contoler software
  but if using  lcd  ct4  has to be disable  pull the pin or cut trace  on the shield going to A4 on the uno

there is a newer autosensing verision 60/50hz  in a  zip file - it works with both 60 and 50hz grid systems--   it  based on a emontx shield  it is highly configurable it you can  fire SSR ar 30hz   but will not be as smooth as firing at 1/4 the  hetz 12- 15 hertz..   when editing  the  uno firmware     you can support upto  4 SSR if using the standard UNO pwm  software at 30 hz
or  up to 3  SSR with  with much lower heztz   -- generally  1/4 works the best 60 hertz grid  requires a 15 hertz pwm  or a 50 hertz grid prefers a 12 hertz pwm  --   it also  provide control of 4 relays  that are activated once it  passes  the second  SSR.    so in total it  can control 7 diversions....  if using  for charging an electric car  place the charger on   the  1st relay  control .. as long as long as solar output  is greater then it stepping down to the first SSR  it will stay in electric  car charging mode.. if you like it to stay on longer just  comment out  the  step down for relay 1  found under if (stat==0 )-- ie -- this line  "if (r1 ==1){ digitalWrite(relay1, LOW); r1=0;} "   then  relay one will only shut off when ever the grid usage goes positive..  but  then come back on again once it  passes  ssr2 in the casscade ssr function-- if you like it to come on sooner  then  just copy "if (r1 ==0){ digitalWrite(relay1, HIGH); r1=1;}"   and paste in if (stat ==1) section but  you will have to disable the the step down  for relay 1  found in the  if (stat=0) other wise as soon as the relay kicks in  it will drop into ssr1 firing pattern and will be autoamtically turned off




![movie of energy monitor  sending data to espeasy firmware -- click raw to view ]( https://github.com/krywenko/energy-monitor-and-diverter-espeasy/blob/master/simplescreenrecorder-111.mp4)
