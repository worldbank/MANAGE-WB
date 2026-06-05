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
  $$include "inc/title_def.inc"
*
* --- store both the shock file used and given variant
*
  $$setglobal outSimName %simNameFile%
  $$ifi not "%SimNameOri%"=="" $setglobal outSimName %simNameFile%_%simname%
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
*
*
* --- missing names for potential parameters to be included are replaced by an emtpy string
*     (which will kick them out)
*
  $$ifi not set name_2 $set name_2
  $$ifi not set name_3 $set name_3
  $$ifi not set name_4 $set name_4
  $$ifi not set name_5 $set name_5
*
* --- if the upper is equal to the lower truncation, the distribution is none ...
*     set the name to blank which kicks it out
*
  $$ife %lo_1%==%up_1% $set name_1
  $$ife %lo_2%==%up_2% $set name_2
  $$ife %lo_3%==%up_3% $set name_3
  $$ife %lo_4%==%up_4% $set name_4
  $$ife %lo_5%==%up_5% $set name_5

  $$onempty
  set distNames / "Trunc. normal","uniform","triangular" /;
  set factors "All factors (= parameters) to be randomized" /

  $$ifi not "%name_1%"=="" "%name_1%"
  $$ifi not "%name_2%"=="" "%name_2%"
  $$ifi not "%name_3%"=="" "%name_3%"
  $$ifi not "%name_4%"=="" "%name_4%"
  $$ifi not "%name_5%"=="" "%name_5%"

  /;
*
* --- replace empty sets by a blank char. Note: the code assumes that any any unused
*     dimension come last
*
  $$ifi not set set1_1 $set set1_1  ""
  $$ifi not set set1_2 $set set1_2  ""
  $$ifi not set set1_3 $set set1_3  ""

  $$ifi not set set2_1 $set set2_1  ""
  $$ifi not set set2_2 $set set2_2  ""
  $$ifi not set set2_3 $set set2_3  ""

  $$ifi not set set3_1 $set set3_1  ""
  $$ifi not set set3_2 $set set3_2  ""
  $$ifi not set set3_3 $set set3_3  ""

  $$ifi not set set4_1 $set set4_1  ""
  $$ifi not set set4_2 $set set4_2  ""
  $$ifi not set set4_3 $set set4_3  ""

  $$ifi not set set5_1 $set set5_1  ""
  $$ifi not set set5_2 $set set5_2  ""
  $$ifi not set set5_3 $set set5_3  ""
*
* --- population the cross-set between the paramaeters and the distribution used
*

  set distribution(factors,distNames);
  $$ifi not "%name_1%"=="" distribution("%name_1%","%form_1%") = yes;
  $$ifi not "%name_2%"=="" distribution("%name_2%","%form_2%") = yes;
  $$ifi not "%name_3%"=="" distribution("%name_3%","%form_3%") = yes;
  $$ifi not "%name_4%"=="" distribution("%name_4%","%form_4%") = yes;
  $$ifi not "%name_5%"=="" distribution("%name_5%","%form_5%") = yes;

  set parItems / lo,up,stdDev,mode /;
  parameters p_distPars(factors,parItems);

*
* --- populate parameter with distribution results for the those out of the five potential parameter which can be used
*
  $$ifi not "%name_1%"=="" p_distPars("%name_1%","lo") = %lo_1%;p_distPars("%name_1%","up") = %up_1%;p_distPars("%name_1%","stdDev") = %stdDev_1%;p_distPars("%name_1%","mode") = %mode_1%;
  $$ifi not "%name_2%"=="" p_distPars("%name_2%","lo") = %lo_2%;p_distPars("%name_2%","up") = %up_2%;p_distPars("%name_2%","stdDev") = %stdDev_2%;p_distPars("%name_2%","mode") = %mode_2%;
  $$ifi not "%name_3%"=="" p_distPars("%name_3%","lo") = %lo_3%;p_distPars("%name_3%","up") = %up_3%;p_distPars("%name_3%","stdDev") = %stdDev_3%;p_distPars("%name_3%","mode") = %mode_3%;
  $$ifi not "%name_4%"=="" p_distPars("%name_4%","lo") = %lo_4%;p_distPars("%name_4%","up") = %up_4%;p_distPars("%name_4%","stdDev") = %stdDev_4%;p_distPars("%name_4%","mode") = %mode_4%;
  $$ifi not "%name_5%"=="" p_distPars("%name_5%","lo") = %lo_5%;p_distPars("%name_5%","up") = %up_5%;p_distPars("%name_5%","stdDev") = %stdDev_5%;p_distPars("%name_5%","mode") = %mode_5%;
*
* --- generate the steering file for the SANDIA LHS utility
*

  file lhsSteerFile / "%scrDir%/lhsInput.txt"/;
  put lhsSteerFile;

  put "LHSTITL Run for MANAGE-WB" /;
  put "LHSOBS %ndraws%" /;

  $$onEmbeddedCode Python:
    import time
    import os
    os.environ["seed"] = str(round(round(time.time() * 1000) - 1712728969596)/1000)
  $$offEmbeddedCode

  put "LHSSEED %sysEnv.seed%" /;
  put "lhsrpts data corr hist" /;
  put "lhsopts %pairing% PAIRING"/;
  put "lhspval 2"/;
  put "lhswcol"/;

  if ( card(factors) eq 0, abort "Please specicy at least for one factors lower different from upper limits",factors);
*
* --- output goes to the scratch directory
*
  put "lhsout %scrdir%\lhsout.gms" /;
  put  "DATASET: " /;

  set rFactors /f1*f10000 /;
  set factor_par_s1_s2_s3(rFactors,factors,*,*,*);
  set factor_par(rFactors,factors);
  set s1(*),s2(*),s3(*);
  set l1(*),l2(*),l3(*);
  set l1_s1(*,*),l2_s2(*,*),l3_s3(*,*);

  set s1All(*),s2All(*),s3All(*);


  scalar nFactor / 0 /;
  set curRFactor(rFactors);
*
*  --- helper include (some funny code) which maps the draws of lineared factors to the 1-3 dimensional parameter we are dealing with
*
  $$ifi not "%name_1%"=="" $batinclude 'inc/sensStoch/sens_proc.inc' "%name_1%" "%set1_1%" "%set1_2%" "%set1_3%" %drawMode1_1% %drawMode1_2% %drawMode1_3%
  $$ifi not "%name_2%"=="" $batinclude 'inc/sensStoch/sens_proc.inc' "%name_2%" "%set2_1%" "%set2_2%" "%set2_3%" %drawMode2_1% %drawMode2_2% %drawMode2_3%
  $$ifi not "%name_3%"=="" $batinclude 'inc/sensStoch/sens_proc.inc' "%name_3%" "%set3_1%" "%set3_2%" "%set3_3%" %drawMode3_1% %drawMode3_2% %drawMode3_3%
  $$ifi not "%name_4%"=="" $batinclude 'inc/sensStoch/sens_proc.inc' "%name_4%" "%set4_1%" "%set4_2%" "%set4_3%" %drawMode4_1% %drawMode4_2% %drawMode4_3%
  $$ifi not "%name_5%"=="" $batinclude 'inc/sensStoch/sens_proc.inc' "%name_5%" "%set5_1%" "%set5_2%" "%set5_3%" %drawMode5_1% %drawMode5_2% %drawMode5_3%



  option  factor_par_s1_s2_s3:0:4:1;
  display factor_par_s1_s2_s3;

  curRFactor(rFactors) $ (rFactors.pos le nFactor) = YES;
*
* --- write out distribution and their parameters used by the SANDIA routine
*
  loop(curRFactor(rFactors),
     put rFactors.tl," 1 ";

     loop(factor_par(rFactors,factors),

        if ( distribution(factors,"Trunc. normal"),
          put "Bounded normal ":15,p_distPars(factors,"mode")," ", p_distPars(factors,"stddev")," ",p_distPars(factors,"lo")," ",p_distPars(factors,"up");
        elseif ( distribution(factors,"uniform") ),
          put "Uniform        ":15,p_distPars(factors,"lo")," ",p_distPars(factors,"up");
        else
          put "Triangular     ":15,p_distPars(factors,"lo")," ",p_distPars(factors,"mode")," ",p_distPars(factors,"up");
        );
     );
     put /;
  );

  putclose;

*
* --- let Python read the file and generate the draws
*
 $$include "inc/sensStoch/lhs_py_replace.inc"
*
* --- read the GDX generated by Python
*
 set lhsDraws /d1*d%nDraws%/;
 parameter p_draws(*,*);
 execute_load "%scrdir%/lhs.gdx" p_draws=p_doe;
 set d_d(draws,*);
 d_d(draws,lhsDraws) $ (draws.pos-1 eq lhsDraws.pos) = yes;
 p_draws(draws,curRFactor) = sum(d_d(draws,lhsDraws), p_draws(lhsDraws,curRFactor));
 lhsDraws(draws) = no;
 p_draws(lhsdraws,curRFactor) = 0;
*
* --- calculate the desired mean of distribution for each random factor
*
 p_draws("meanDesired",curRFactor) $ sum(factor_Par(curRFactor,factors), distribution(factors,"uniform"))
    = sum(factor_Par(curRFactor,factors), p_distPars(factors,"lo")+p_distPars(factors,"up"))/2;

 p_draws("meanDesired",curRFactor) $ sum(factor_Par(curRFactor,factors), distribution(factors,"triangular"))
    = sum(factor_Par(curRFactor,factors), p_distPars(factors,"lo")+p_distPars(factors,"up")+p_distPars(factors,"mode"))/3;

$funclibin stolib stodclib
Functions pdfNormal   /stolib.pdfnormal    /
          cdfNormal   /stolib.cdfnormal    /
;
*
* --- calculate the desired mean
*
 p_draws("meanDesired",curRFactor) $ sum(factor_Par(curRFactor,factors), distribution(factors,"Trunc. normal"))
    = sum(factor_Par(curRFactor,factors), p_distPars(factors,"mode") -

            [   pdfNormal(p_distPars(factors,"up"),p_distPars(factors,"mode"),p_distPars(factors,"stdDev"))
              - pdfNormal(p_distPars(factors,"lo"),p_distPars(factors,"mode"),p_distPars(factors,"stdDev"))]
           /[  cdfNormal(p_distPars(factors,"up"),p_distPars(factors,"mode"),p_distPars(factors,"stdDev"))
             - cdfNormal(p_distPars(factors,"lo"),p_distPars(factors,"mode"),p_distPars(factors,"stdDev"))]
                                                                     * p_distPars(factors,"stdDev"));
*
* --- calculate the drawn mean
*
  p_draws("mean",curRFactor) = sum(draws, p_draws(draws,curRFactor))/(card(draws)-1);
*
* --- and scale the draws
*
  p_draws(draws,curRFactor) $ (abs(p_draws("meanDesired",curRFactor)) gt 0.1)
   = p_draws(draws,curRFactor) * p_draws("meanDesired",curRFactor)/p_draws("mean",curRFactor);

  p_draws(draws,curRFactor) $ ( (abs(p_draws("meanDesired",curRFactor)) le 0.1) $ (draws.pos gt 1))
   = p_draws(draws,curRFactor) + (p_draws("meanDesired",curRFactor) - p_draws("mean",curRFactor));

  p_draws("test",curRFactor) = sum(draws, p_draws(draws,curRFactor))/(card(draws)-1);
  p_draws("base",curRFactor) = p_draws("meanDesired",curRFactor);

  option  factor_par_s1_s2_s3:0:4:1;

  parameter p_reportDraws(draws,factors,*,*,*);
  p_reportDraws(draws,factors,s1All,s2All,s3All)
     = sum(factor_par_s1_s2_s3(curRFactor,factors,s1All,s2All,s3All),p_draws(draws,curRFactor));
  option p_reportDraws:2:1:4

  display p_reportDraws;
*
* --- program might terminate here if the user only wanted to check the LHS
*     generation
*
$ifi "%onlyGenerateDraws%" == "ON" $exit
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

 file scenFile / %scrdir%\curScen.gms /;
 scenfile.nd = 10;
 scenFile.ap = 0;
 scenFile.lw = 0;


 loop(draws,
*
*   --- helper code which write out line by line changes to parameters
*
    put scenFile;
    $$ifi not "%name_1%"=="" $$batinclude 'inc/sensStoch/genPut.inc' %name_1% "%set1_1%" "%set1_2%" "%set1_3%" "%applMode_1%"
    $$ifi not "%name_2%"=="" $$batinclude 'inc/sensStoch/genPut.inc' %name_2% "%set2_1%" "%set2_2%" "%set2_3%" "%applMode_2%"
    $$ifi not "%name_3%"=="" $$batinclude 'inc/sensStoch/genPut.inc' %name_3% "%set3_1%" "%set3_2%" "%set3_3%" "%applMode_3%"
    $$ifi not "%name_4%"=="" $$batinclude 'inc/sensStoch/genPut.inc' %name_4% "%set4_1%" "%set4_2%" "%set4_3%" "%applMode_4%"
    $$ifi not "%name_5%"=="" $$batinclude 'inc/sensStoch/genPut.inc' %name_5% "%set5_1%" "%set5_2%" "%set5_3%" "%applMode_5%"
    putClose;
*
*   --- copy as settings for next draw to start
*
    put_utility batch 'shell' / "type %scrdir%\curScen.gms > %curdir%\settings_",draws.tl,".inc";
*
*   --- execute run as a seperate program, no wait
*
    put_utility 'log'          / '%GAMSEXE% %CURDIR%/run.gms'
                         ' --iScen='draws.tl:0' -maxProcDir=255 -optdir=opt -output='draws.tl:0'.lst execerr=100',
                         ' lo=3 --pgmName="'draws.tl:0' (',draws.pos:0:0,' of ',card(draws):0:0,')"  %gamsarg%';

    put_utility 'exec.async'   / '%GAMSEXE% %CURDIR%/run.gms'
                         ' --iScen='draws.tl:0' -maxProcDir=255 -optdir=opt -output='draws.tl:0'.lst execerr=100',
                         ' lo=3 --pgmName="'draws.tl:0' (',draws.pos:0:0,' of ',card(draws):0:0,')"  %gamsarg%';
*
*   --- add to listed for started draws, assign job handle (to later retrive solution) and determine number of
*       running threads (started but not yet solved)
*
    started(draws) = YES;
    p_jobHandles(draws) = JobHandle;
    notSolved(started) = YES $ (jobStatus(p_jobHandles(started)) eq 1);

    $$batinclude 'inc/title.inc' "'Allow output'"
    $$batinclude 'inc/title.inc' card(notSolved):0:0 "' jobs running, '" (card(started)-card(notSolved)):0:0 '" solved, "' (card(draws)-card(started)+card(notSolved)):0:0 '" left, scenario "' draws.pos:0:0 "' of '" card(draws):0:0 "' started '"
    if (%parallelThreads% gt 1,
    $$batinclude 'inc/title.inc' "'Suppress output'"
    );
*
*   --- wait with next start until less than the desired number of parallel threads
*       is currently not active
*
    while ( card(notSolved) ge %parallelThreads%,
        rc=sleep(1);
        notSolved(started) = YES $ (jobStatus(p_jobHandles(started)) eq 1);
    );
 );

* ------------------------------------------------------------------------------
*
*     Wait until all runs are ready
*
* ------------------------------------------------------------------------------

 count = 1;
 while( (card(notSolved) and (count le 240)),
       count = count + 1;
       if ( mod(count,10) eq 0,
          $$batinclude 'inc/title.inc' "'Allow output'"
          $$batinclude 'inc/title.inc' "'Wait until '" card(notSolved):0 "' remaining jobs are finalized'"
          if (%parallelThreads% gt 1,
            $$batinclude 'inc/title.inc' "'Suppress output'"
          );
       );
       rc=sleep(1);
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

  $$ifi "%userOutput%"=="on" $batinclude '%simf%/%add_output_file%.inc' decl

  loop(draws,

        if(execError,execError=0);
        $$batinclude 'inc/title.inc' "'Collect results for '" draws.tl:0:0
        if(execError,execError=0);
        put_utilities 'gdxin' / '%oDir%/%regGtapName%_%outSimName%_',draws.tl:0:0,'.gdx';
        if ( execError eq 0,

           $$iftheni.toGui "%outputGui%"=="on"
              execute_loadpoint p_toGui;
           $$endif.toGui
           $$iftheni.toGui "%userOutput%"=="on"
              execute_loadpoint p_keyInd;
           $$endif.toGui
           if ( execError eq 0,
*             --- remove listing from successful runs
              put_utility 'shell'   / 'rm -f ',draws.tl:0,'.lst';
*             --- and delete GDX
              put_utility 'shell'   / 'rm -f %oDir%/%regGtapName%_%outSimName%_',draws.tl:0,'.gdx';
              put_utility 'shell'   / 'rm -f settings_',draws.tl:0,'.inc';
           );

        );
  );
  $$setglobal GDXOutput
  $$ifi "%outputGui%"=="on"  $$setglobal GDXOutput p_toGui
  $$ifi "%userOutput%"=="on" $$setglobal GDXOutput %GDXOutput% p_keyInd

  $$batinclude 'inc/title.inc' "'Unload merged results'"
  execute_unload "%odir%/%regGtapName%_%outSimName%_%startDraws%.gdx" s_meta,p_reportDraws %GDXoutput%;

  $$batinclude 'inc/title.inc' "'Done'"
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
