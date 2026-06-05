********************************************************************************
$ontext

   MANAGE project

   GAMS file : RUN_STOCH.GMS

   @purpose  : Driver for stochastic parallel runs
   @author   :
   @date     : 27.02.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
  $$onglobal
  $$offlisting

  $$ifi exist "fromGuiToRun.gms" $include "fromGuiToRun.gms"
  $$if not set RegGtapName $setglobal RegGtapName TUR
  $$setGlobal regF     ./reg/%RegGtapName%
*
* --- load basic flags etc.
*
  $$include "inc/opt.inc"
*
* --- store both the shock file used and given variant
*
  $$setglobal outSimName %simNameFile%
  $$ifi not "%SimNameOri%"=="" $setglobal outSimName %simNameFile%_%simname%
  $$ifi not setglobal SimNamePostFix $$setglobal SimNamePostFix
  $$ifthen.postfix not "%SimNamePostFix%"==""

     $$onEmbeddedCode Python:

        import os
        os.environ["simNamePostFix"] = "%SimNamePostFix%".replace("/","-")

     $$offEmbeddedCode

     $$setglobal SimNamePostFix %sysEnv.simNamePostFix%
     $$setglobal outSimName %outSimName%_%SimNamePostFix%
  $$endif.postfix
  $$ifi not "%iScen%"==""      $setglobal outSimName %outSimName%_%iScen%
*
* --- definition of symbols used to print to GUI title window
*
  $$include "inc/title_def.inc"
*
* --- add CC impact channels from Roson and Sartori
*
  $$batinclude 'inc/RSCCDamages.inc' decl
*
* --- declaration of sets, variables, equations and model found in model
*     model definitions
*
  $$include 'inc/model.inc'
*
* --- load symbols from bridge-file
*
  $$batinclude 'inc/Base.inc'
*
* --- benchmarking
*
  $$include 'inc/cal/inical.inc'
*
* --- declarations for reporting
*
  $$include "inc/output/reportDecl.inc"
*
* ---- rebuild SAM from the calibrated variable values for base year
*
  $$iftheni.samcalc "%outputSAM%"=="on"
     option kill=ts;ts(t0) = YES;
     $$include "inc/output/samCalc.inc"
  $$endif.samcalc
*
* ---- some solver settings for follow-up solves
*
  option cns=conopt4;
  option dnlp=conopt4;
  option MCPRHoldFx=1;
  $$ifi set MCPSsolver  option mcp=%MCPSolver%;
  $$ifi set limrow      option limrow=%limRow%;
  $$ifi set limcol      option limcol=%limcol%;

*
*  --- If a previous simulation run is used as baseline
*       start all values from it
*
  $$ifi "%usedAsBaU%"=="ON" execute_loadpoint "%odir%/%bauFile%.gdx"
*
* --- load the declarations from the shock file and define the stochastic draws
*
  $$batinclude '%simf%/%shk_file%.inc' decl
  $$batinclude '%simf%/%shk_file%.inc' stoch
*
* -- store random variables in to temporary GDX to be picked by child processes
*
  execute_unload "%savf%/randvar.gdx" p_randVar,draws_nodes,p_prob,nCur,t_nodes;

* ------------------------------------------------------------------------------
*
*     deploy and wait for child processes
*
* ------------------------------------------------------------------------------

 parameter p_jobHandles(draws);
 scalar rc;
 alias(draws,draws1);
 scalar count;
*        put_utilities 'gdxin' / '%oDir%/%regGtapName%_%outSimName%_',draws.tl:0:0,'.gdx';

 $$ifi not set SimNamePostFix $setglobal SimNamePostFix
 $$ifi not "%SimNamePostFix%"=="" $setglobal outSimName %outSimName%_%SimNamePostFix%
*
* --- delete listings and GDX outputs from previous stochastic runs
*
 loop(draws,
*     --- delete the GDX result file for that instance should it exist so that we do not read later old stuff
      put_utility 'shell'   / 'rm -f ',draws.tl:0,'.lst';
      put_utility 'shell'   / 'rm -f %oDir%/%regGtapName%_%outSimName%_',draws.tl:0,'.gdx';
 );
*
* --- span the child processes
*
 set notSolved(draws);
 set started(draws);

 loop(draws,
*
*   --- execute run as a seperate program, no wait
*
    put_utility 'exec.async'   / '%GAMSEXE% "%CURDIR%/run.gms"'
                         ' --iScen='draws.tl:0:0' -maxProcDir=255 -optdir=opt -output='draws.tl:0'.lst execerr=100',
                         ' lo=3 --pgmName="'draws.tl:0' (',draws.pos:0:0,' of ',card(draws):0:0,')"  %gamsarg%';
    p_jobHandles(draws) = JobHandle;

    $$batinclude 'inc/title.inc' "'Allow output'"
    $$batinclude 'inc/title.inc' "'Scenario '" draws.pos:0:0 "' of '" card(draws):0:0 "' started '"
    $$batinclude 'inc/title.inc' "'Suppress output'"

    started(draws) = YES;
    notSolved(started) = YES $ (jobStatus(p_jobHandles(started)) eq 1);

    count = 1;
    while ( card(notSolved) ge %parallelThreads%,

        count = count + 1;
        if ( mod(count,10) eq 0,
            $$batinclude 'inc/title.inc' "'Allow output'"
            $$batinclude 'inc/title.inc' card(notSolved):0:0 "' jobs are running, '" (card(started)-card(notSolved)):0:0 '" solved, "' (card(draws)-card(started)+card(notSolved)):0:0 '" remaining "'
            $$batinclude 'inc/title.inc' "'Suppress output'"
        );
        rc=sleep(0.10);
        notSolved(started) = YES $ (jobStatus(p_jobHandles(started)) eq 1);
    );

 );

* ------------------------------------------------------------------------------
*
*     Wait until all runs are ready
*
* ------------------------------------------------------------------------------

 count = 1;
 while( card(notSolved),
       count = count + 1;
       if ( mod(count,10) eq 0,
          $$batinclude 'inc/title.inc' "'Allow output'"
          $$batinclude 'inc/title.inc' "'Wait until '" card(notSolved):0 "' remaining jobs are finalized'"
          $$batinclude 'inc/title.inc' "'Suppress output'"
       );
       rc=sleep(0.10);
       notSolved(started) = YES $ (jobStatus(p_jobHandles(started)) eq 1);
 );

* ------------------------------------------------------------------------------
*
*     Collect and merge results
*
* ------------------------------------------------------------------------------

  $$batinclude 'inc/title.inc' "'Allow output'"
*
* -- make sure that labels used by p_toGui are known to mother process
*
  set dummyProb / prob /;

  loop(draws,

        $$batinclude 'inc/title.inc' "'Collect results for '" draws.tl:0:0
        if ( execError,execerror=0;);
        put_utilities 'gdxin' / '%oDir%/%regGtapName%_%outSimName%_',draws.tl:0:0,'.gdx';
        if ( execError eq 0,
           execute_loadpoint p_toGui;
           if ( p_toGui("%RegGtapName%","V","prob","tot","%baseYear%",draws),
*             --- remove listing from successful runs
              put_utility 'shell'   / 'rm -f ',draws.tl:0,'.lst';
*             --- and delete GDX
              put_utility 'shell'   / 'rm -f %oDir%/%regGtapName%_%outSimName%_',draws.tl:0,'.gdx';
           );
        );
  );
  if ( execError,execerror=0;);

  $$batinclude 'inc/title.inc' "'Unload merged results'"
  execute_unload "%odir%/%regGtapName%_%outSimName%.gdx" s_meta,p_toGui;
  $$batinclude 'inc/title.inc' "'Done'"
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
