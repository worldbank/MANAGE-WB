********************************************************************************
$ontext

   MANAGE project

   GAMS file : prepSolve.INC

   @purpose  : Define years in current solve and run iterloop in run mode
               (variables from t-1 to t, closures etc.)

   @author   : MANAGE team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms
                                                                                                                                                                  x
$offtext
********************************************************************************
*
  isShortRun = 0;
*
*
*   --- cleanse in the first and every third year the memory by setting solvelink = 0
*
    option solvelink=5;
    if ( (mod(tt.pos,4) eq 0), option solveLink=0;);
    if ( (tt.pos eq 2), option solveLink=0;);
*
* --- set dynamic simulation year
*
  option kill=ts;
*
  $$iftheni.forwardLooking "%ForwardLooking%"=="on"
*
*     --- define next years to form expectation
*
     option kill=tNext;
     tNext(t) = yes $ ((years(t) le years(tt)+futureYears)
                    $ (years(t)  ge years(tt)) $ (years(t) gt %BaseYear%) $ (t.pos gt 1));
*
*    --- related expectation weights
*
     option kill=p_exptWgts;
     p_exptWgts(tNext) = 1 - abs((years(tNext) - years(tt)))*p_expWgt;
     p_exptWgts(tNext) = p_exptWgts(tNext)/sum(tNext1,p_exptWgts(tNext1));
*
*    --- forward looking mode, result up tt + futureYears -1 are already known
*        only set values tt + futureYear anew in resLastYear.inc
*
     tNextFirst(tt)  = yes;
     tNextLast(t)    = yes $ (years(t) eq years(tt)+futureYears);
     tNextSndLast(t) = yes $ (years(t) eq years(tt)+futureYears-1);
     ts(tNext) = YES;
*
*    --- first simulation year: recursively set startpoint for the
*        simulation horizon tNest stored on ts
*
     if ( (ord(tt) eq 2) $ ifCal,
        hasEmit = 0;
        $$setglobal tt  t
        $$setglobal tt1 t-1

        loop(t $ ts(t),
           $$batinclude 'inc/solve/iterloop.inc' run
           $$batinclude 'inc/solve/iterloop.inc' final
        );
     else
*
*       --- solution from previous solves exist for
*           the first n-1 years. Only construct
*           start point for the newly added year tNestLast
*
        $$setglobal tt  tNextLast
        $$setglobal tt1 tNextSndLast
        $$batinclude 'inc/solve/iterloop.inc' run
        $$batinclude 'inc/solve/iterloop.inc' final
     );

  $$else.forwardLooking
*
*    --- normal myopic call, no forward looking behavior
*
     option kill=p_exptWgts;
     option kill=st;
     option kill=tNextFirst;
*
*    --- only put the current simulation year in the model
*        equations
*
     ts(tt)   = yes ;
     tNext(t) = yes $ ts(t);
     $$setglobal tt  tt
     $$setglobal tt1 tt-1
*
*     --- initialize variables and apply closures
*
      $$batinclude 'inc/solve/iterloop.inc' run
  $$endif.forwardLooking
*
* --- unchanged model (no additional equations, no redirections)
*     if not otherwise stated in shock files
*
  $$ifi not setglobal modelVariant $setglobal modelVariant

  $$ifi setglobal iterlim cge_%modelVariant%.iterlim = %iterlim%;
*
* --- for the benchmark check, we don't allow the solver to remove
*     infeasibilities to check the start point of the model
*
  if ( ifBenchmarkCheck,  cge_%modelVariant%.iterlim = 0;);
*  if ( sameas(tt,"2018"), cge_%modelVariant%.iterlim = 0;);
*
* --- other model properties: fixed variables become constants in equations,
*     report infeasible > 1.E-5, accept quite some math trap errors
*
  cge_%modelVariant%.holdfixed = 1;
  cge_%modelVariant%.tolinfrep = 1e-5;
  cge_%modelVariant%.domLim    = 1000;
*
* --- deleted list of successful option file if empty
*
  $$ifi not defined optrec option kill=optRec;
  option kill=e_walras;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
