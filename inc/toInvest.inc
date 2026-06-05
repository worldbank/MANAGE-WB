********************************************************************************
$ontext

   MANAGE Project

   GAMS file : TOINVEST.INC

   @purpose  : Send land use change information to INVEST via CSV file
   @author   : W. Britz
   @date     : 29.05.24
   @since    :
   @refDoc   :
   @seeAlso  : relevant section in inc/opt.inc, inc/fromInvest.inc
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- generate CSV file with land cover change information in %
*
  put csvInvest;
  csvInvest.nd = 6;
  csvInvest.lw = 0;
  csvInvest.nw = 0;
  loop((lnd,lCov) $ (lCovArea.l(lnd,lcov,t0) gt eps),
      put "'%regGtapNameOri%','",lnd.te(lnd):0,"','",lCov.tl:0,"','",tt.tl:0,"',",
        ((lCovArea.l(lnd,lcov,tt)/lCovArea.l(lnd,lcov,t0)-1)*100),",",
         (lCovArea.l(lnd,lcov,tt)-lCovArea.l(lnd,lcov,t0))
         /;
  );
  putClose csvInvest;
*
* --- define arguments for Python: directory for file, modules to call, feedback links to pass back
*
  $$setglobal pyhtonArgs -f "%scrDir%"
  $$ifi "%investmodulesCropProduction%"=="ON"    $$setglobal pyhtonArgs %pyhtonArgs% -m_CropProduction
  $$ifi "%investmodulesCropPollination%"=="ON"   $$setglobal pyhtonArgs %pyhtonArgs% -m_CropPollination
  $$ifi "%investmodulesErosion%"=="ON"           $$setglobal pyhtonArgs %pyhtonArgs% -m_Erosion
  $$ifi "%investmodulesCropproduction%"=="ON"    $$setglobal pyhtonArgs %pyhtonArgs% -m_CropProduction

  $$ifi "%investFeedbacksCropProduction%"=="ON"  $$setglobal pyhtonArgs %pyhtonArgs% -f_CropProduction
  $$ifi "%investFeedbacksCropPollination%"=="ON" $$setglobal pyhtonArgs %pyhtonArgs% -f_CropPollination
  $$ifi "%investFeedbacksErosion%"=="ON"         $$setglobal pyhtonArgs %pyhtonArgs% -f_Erosion

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
*
* --- call python script
*
  put_utility 'log' / '%gamsPath%\gmsPython\Python "%curDir%/inc/testInvest.py" %pyhtonArgs%';
  execute '%gamsPath%\gmsPython\Python "%curDir%/inc/testInvest.py" %pyhtonArgs%';
