********************************************************************************
$ontext

   MANAGE project

   GAMS file : checkModelSolution.INC

   @purpose  : Reporting on model outcome, abort if infeasible or walras
               check fails
   @author   : MANAGE Team
   @date     : 29.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- reset model report diagnostics by GAMS with values triggering erors if no solution is returned
*     (mapval tests for numeric value such as UNDF or INF)
*
  cge_%modelVariant%.sumInfes    $ mapVal(cge_%modelVariant%.sumInfes)  = 1.E+10;
  cge_%modelVariant%.numInfes    $ mapVal(cge_%modelVariant%.numInfes)  = 1.E+10;
  cge_%modelVariant%.modelstat   $ mapVal(cge_%modelVariant%.modelstat) = 6;
  cge_%modelVariant%.solvestat   $ mapVal(cge_%modelVariant%.solvestat) = 6;
*
* --- reset status to solved if on infeasibilities are reported
*
  cge_%modelVariant%.modelStat $ ((cge_%modelVariant%.modelStat eq 6) $ (cge_%modelVariant%.numInfes eq 0)) = 7;
  cge_%modelVariant%.solveStat $ ((cge_%modelVariant%.solveStat eq 5) $ (cge_%modelVariant%.numInfes eq 0)) = 3;
*
* --- report diagnostics for final trial
*
  diagnostics("modStatus",tt)        = cge_%modelVariant%.modelstat ;
  diagnostics("solStatus",tt)        = cge_%modelVariant%.solvestat ;
  diagnostics("Walras % of GDP",tt)  = walras.l(tt)/gdpmp.l(tt)*100;
  diagnostics("numEqu",tt)           = cge_%modelVariant%.numEqu;
  diagnostics("numInfes",tt)         = cge_%modelVariant%.numInfes;
  diagnostics("sumInfes",tt)         = cge_%modelVariant%.sumInfes;
  diagnostics("numRedef",tt)         = cge_%modelVariant%.numRedef;
*
* -- stop further solves if the last trial was infeasible or walras > 0.01% of GDP
*
  if( (   ( (cge_%modelVariant%.modelstat > 2)  and (cge_%modelVariant%.modelstat ne 16) and (cge_%modelVariant%.modelstat ne 7))
        OR  ( (cge_%modelVariant%.solvestat > 1) $ (dummy.l eq 0))
        OR  (cge_%modelVariant%.solvestat > 4)
        OR (abs(walras.l(tt)/gdpmp.l(tt)*100) > 0.01) ),
*
*    ---- analyze solve point (comprises equations and variables of current model instance)
*
     $$iftheni.task "%iScen%"==""
*
*       --- call Python embedded code to report infeasible equations and
*           variables hitting bounds
*
        $$include 'inc/solve/checkInfes.inc'
*
*       --- delete savepoint gdx container
*
        execute 'rm -f %gams.wdir%cge_%modelVariant%_p.gdx"';
     $$endif.task

     $$batinclude "inc/title.inc" '"%regGtapNameOri%: No solution found  "' tt.tl '"%regGtapName%:  check out %odir%/%regGtapName%_%simName%_Debug.gdx for potential hints."'
*
*    --- rescale all variables relating to volumes and values back to original
*        SAM scaling (includes some parameters)
*
     $$include 'inc/cal/rescale.inc'
     lCovArea.l(actHa,lCov,tScale) =  lCovArea.l(actHa,lCov,tScale)   * 1/inscale * 1/usd;
*
*    --- rebuild SAM for debugging purposes if SAM report is switched on
*
     $$ifi "%outputSAM%"=="on"    $$include 'inc/output/samCalc.inc'

*    --- dump everything (sets, parameter, variables, equations) before aborting into GDX container
*        for debugging purposes

     execute_unload "%odir%/%regGtapNameOri%_%outSimName%_Debug.gdx";
     abort $ (abs(walras.l(tt)/gdpmp.l(tt)*100) > 0.01) "Walras > 0.01% of GDP (and model might infeasible), check listing and %odir%/%regGtapNameOri%_%simName%_Debug.gdx, file: %system.fn%, line: %system.incline%";
     abort "Infeasible solution, check listing and %odir%\%regGtapNameOri%_%outSimName%_Debug.gdx, in file: %system.fn%, line: %system.incline%";

  );
*
* --- make your any savepoint is deleted and remove generation of savepoint
*
  $$iftheni.task not "%iScen%"==""
     execute 'rm -f %gams.wdir%cge_%modelVariant%_p.gdx"';
     cge_%modelVariant%.savepoint = 0;
  $$endif.task

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
