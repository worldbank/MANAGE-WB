********************************************************************************
$ontext

   MANAGE project

   GAMS file : KEYIND.INC

   @purpose  : Store some key indicators from runs
   @author   : W.Britz
   @date     : 07.06.24
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
  $$iftheni.decl "%1"=="decl"

     set keyInd / rGdp,debtGdp,mmuh,set.pollType /;
     parameter p_keyInd(*,*,*%ddraws%);

    $$ifi "%shk_file%"=="output_keyind" $$Abort "Output_keyInd.inc is not a shock, but an additional reporting file"

  $$else.decl

     p_keyInd("%RegGtapName%","rGDP",ts%draws%)    = rgdpmp.l(ts);
     p_keyInd("%RegGtapName%","debtGDP",ts%draws%) = debtGdp.l(ts);
     p_keyInd("%RegGtapName%","mmuh",ts%draws%)    = sum(h,mmuh.l(h,ts)*hpop.l(h,ts))/sum(h,hpop.l(h,ts));
     p_keyInd("%RegGtapName%",polltype,ts%draws%)  = emiGhg.l(pollType,ts);

     $$iftheni.shock "%shk_file%"=="empty"
         parameter p_testGDP(t);
         p_testGdP(ts) = (rgdpmp.l(ts)-p_gdp(ts))/p_gdp(ts);
         p_testGDP(ts) $ (abs(p_testGdp(ts)) le 1.E-4) = 0;
         if (card(p_testGDP), abort "Baseline replication with empty shock failed",p_testGDP);
     $$endif.shock
  $$endif.decl

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
