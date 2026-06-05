********************************************************************************
$ontext

   GTAP8 in GAMS project

   GAMS file : TITLE.GMS

   @purpose  : Write a block of strings to the console
   @author   : W.Britz
   @date     : 16.02.11
   @since    :
   @refDoc   :
   @seeAlso  : util\title1.gms
   @calledBy :

$offtext
********************************************************************************
  $$iftheni.java %JAVA%==on

     put logfile;

     $$iftheni.showtime %ShowTimeinTitleBatch%==ON

         p_newTime = TimeElapsed;
         p_diffTime = p_newTime  - p_lastTime;
         p_execTime = ( p_newTime -p_startTime )/60;
         put_utility "log"  / 'title %PGMNAME% : %TitlePrefix% ' %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 ' Last:' '%LastTitlebatchTxt%'  ' - (' p_diffTime:6:0 ' sec. '  p_execTime:6:2 ' min. )';
         p_lastTime = TimeElapsed;

         $$setglobal LastTitlebatchTxt %1

     $$elseifi.showtime %ShowTimeElapasedinTitleBatch%==ON

         p_execTime = TimeElapsed;
         put_utility  "log" / 'title%PGMNAME% ('p_execTime:0:0' sec):  %TitlePrefix% '  %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14;

     $$else.showtime

         put_utility "log" / 'title %PGMNAME% : %TitlePrefix% ' %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14;

     $$endif.showtime

  $$else.java

     put_utility "shell" 'title' / ' %PGMNAME% : %TitlePrefix% ' %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14;

  $$endif.java
  $$if not errorfree $abort Compilation error after file: %system.fn%
