********************************************************************************
$ontext

   MANAGE project

   GAMS file : TITLE1.GMS

   @purpose  : Defines a file to which messages are written. Under
               java, that is the GAMS logfile piped into stdout.

   @author   : W.Britz
   @date     : 08.04.11
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
  $$ifi not setglobal scrdir $setGlobal scrdir %gams.scrdir%
  $$iftheni "%JAVA%"=="ON"
     file logfile "writing directly into the log" / "%scrdir%%system.dirsep%nonense.txt" /;
    logFile.pw = 9999;
  $$endif

  file batch /'%SCRDIR%%system.dirsep%%PGMNAME%titlebatch.bat'/;
  batch.pw = 9999;

  $$iftheni NOT %VERSION%==GAMS222

     batch.ap = 0;
     batch.nw = 0;
     batch.lw = 0;
  $$endif

  scalar p_newTime    "New time where title.inc is called"    /0/
         p_diffTime   "Difference between last and now time"  /0/
         p_lastTime   "Last time where title.inc was called"  /0/
         p_startTime  "First time where title.inc was called" /0/
         p_execTime   "Current time";

  $$ifi not setglobal iScen $setglobal iScen
  $$iftheni not "%iScen%"==""
     $$batinclude 'inc%system.dirsep%title.inc' "'Suppress output'"
     $$setglobal titlePrefix 'console Scenario:%iScen%'
  $$else
     $$setglobal titlePrefix
  $$endif

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
