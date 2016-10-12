#SETUP THE PERF RUN
/toggleslashcommandecho on
/testmap 382
/pause 60
/killinput on
/showdebug on 9
/perfdumpstart
##RUN ONE 
## -p is an option to auto inject a pause between commands
## -p20 will inject "/pause 20"
-p20
/tele -137.95 2651.43 -756.18
/camlookat -160 2660 -756 276.95 2501.43 -756.18
/perfdump shipFront
/takescreenshot
/adddxpixevent
##RUN TWO
/tele 276.95 2651.43 -756.18
/camlookat 277 2691 -756 -137.95 2600.43 -756.18
/perfdump shipBack
/takescreenshot
##RUN THREE
/tele 422 75 -254
/camlookat 434 82 -291 541 10 197
/perfdump monument
/takescreenshot
/adddxpixevent
##END PERF RUN
/showdebug off
/killinput off
/toggleslashcommandecho off