********************************************************************************
$ontext

   MANAGE project

   GAMS file : BalGrowth.inc

   @purpose  : Test framework for balanced growth in baseline mode
   @author   : W.Britz
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  $$ifi not "%task%"=="Baseline" $$abort "%Shk_file% is designed to work in baseline mode, only"

*
*     --- don't fix real GDP
*
  ifCal = 0;
  $$iftheni.decl "%1"=="decl"
     $$setglobal modelVariant "BG"

     Parameter
       p_kstock_gr(t) "Growth factor of the capital stock"
       p_pop_gr(t)    "Growth factor of population"
       p_tech_gr(t)   "Necessary growth factor in labor productivity"
     ;

*
*    --- calculate growth factor of total population
*
     p_pop_gr(tt) $ (not t0(tt)) = pop.l("ptotl",tt)/pop.l("ptotl",tt-1)-1;
*
*    --- copy from first year to benchmark where above calculation is not possible
*
     p_pop_gr(t0) = sum(tt $ (tt.pos eq 2),p_pop_gr(tt));

     Model cge_%modelVariant%
     /
          cge_
          -e_rgdpmp
          -e_rshrGdpGov
          +e_rgdpmp
     /
     ;

     p_kstock_gr(t) = [kstocke.l(t0)/kstock.l(t0)]-1;
     p_tech_gr(tt)  =  [1+p_kstock_gr(t0)] / (1 + p_pop_gr(t0)) - 1;
     option p_tech_Gr:7,p_pop_gr:7,p_kstock_gr:7;
     display p_tech_gr,p_pop_gr,p_kstock_gr;

  $$else.decl

*
*    --- don't fix real GDP
*
     ifCal = 0;
*
*    --- no principal, interest rate is set in base.inc such that fits the balanced growth assumption
*
     rDebtStk.fx(instfd,tt)   $ (not t0(tt))   = 0;
*
*    --- labor stock grows with population growth rates
*
     atFmSup.fx(fmp,instTot,oldf,tt)  $ ((not t0(tt))  $ sum(sameas(l,fmp),1)) = atFmSup.l(fmp,instTot,oldf,tt-1)*(1+p_pop_gr(tt));
*
*    --- labor producticity growth closes the gap to the captial stock growth rates
*
     chixfacl.fx(tt)          $ (not t0(tt))  = (1+chixfacl.l(tt-1)) * (1+p_kstock_gr(tt))/(1 + p_pop_gr(tt)) - 1;
*
*    --- other primary factors (land, natural ressources) grow with the same rate as the capital stock
*
     atFmSup.fx(fmp,instTot,oldf,tt)  $ ((not t0(tt))  $ sum(sameas(lnd,fmp),1))    = atFmSup.l(fmp,instTot,oldf,tt-1)*(1+p_kstock_gr(tt));
     $$iftheni.baseline "%task%"=="baseline"
         p_gammaFmp(fma,fmp,instTot,oldf,tt) $ ((not t0(tt))  $ sum(sameas(natres,fmp),1)) = p_gammaFmp(fma,fmp,instTot,oldf,tt-1)*(1+p_kstock_gr(tt));
     $$endif.baseline
*
*    --- foreign savings, foreign transfers and gov. consumption/savings grow with the same rate as the
*        capital stock (foreign savings are fixed in relation to real GDP as default closure)
*
     rowtransfers.fx(inst,tt) $ (not t0(tt)) = rowtransfers.l(inst,tt-1)*(1+p_kstock_gr(tt));

     xfd.fx(ginv,tt)          $ (not t0(tt)) = xfd.l(ginv,tt-1)*(1+p_kstock_gr(tt));

     xfd.fx(gov,tt)           $ (not t0(tt)) = xfd.l(gov,tt-1)*(1+p_kstock_gr(tt));
*
*    --- don't fix shares of the two fixed variables above
*
     rShrGdp.lo(gov,tt)= -inf;
     rShrGdp.up(gov,tt)= +inf;

     rShrGdp.lo(ginv,tt)= -inf;
     rShrGdp.up(ginv,tt)= +inf;

     $$iftheni.baseline "%task%"=="Simulation"
        er.fx(ts(t)) $ (t.pos eq 2) = er.l(t)*1.1;
     $$endif.baseline
  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
