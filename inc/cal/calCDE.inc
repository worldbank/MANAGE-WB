********************************************************************************
$ontext

   Manage project

   GAMS file : calCDE.inc

   @purpose  : Calibration of CDE demand

   @author   : Manage team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : inc/inical.inc

$offtext
********************************************************************************

  variable
    hpdElas                    "Sum of weighted relative squared differences in elasticties"
  ;

  equations

    e_incelas(h,kNest,t)       "Definition of income elasticity"
    e_prcElas(h,kNest,kNest,t) "Uncompensated demand elasticity"
    e_hpdElas                  "Highest Posterior density function of demand elasticity calibration"
    e_incElasAddup             "Budget share weighted income elasticities add up to unity"
    e_incElasC2(h,kNest,t)     "Restriction on product of income elasticities"
  ;

  parameters
    p_incElas0(h,kNest)        "Targested income Elasticities"
    p_prcElas0(h,kNest)        "Targeted Price Elasticities"
    p_kron(kNest,kNest)        "Kronecker factor"
  ;

  p_incElas0(h,topKNest) = defpar('incElas0');
  p_prcElas0(h,topKNest) = defpar('prcElas0');

  p_incElas0(h,topKNest) $ hohpar(h,"incElas0",topKNest) = hohpar(h,"incElas0",topKNest);
  p_prcElas0(h,topKNest) $ hohpar(h,"prcElas0",topKNest) = hohpar(h,"prcElas0",topKNest);

* ---------------------------------------------------------------------
*
*    Equations used to calibrate the CDE demand system by
*    targeting given income and own price elasticities
*
* ---------------------------------------------------------------------

*
* --- the objective function mimimize squared relative differences
*     to the targeted elasticities, weighted with budget shares
*
  e_hpdElas..
    hpdElas =e= sum((hs(h),topKNest,ts(t)),
               xakShr(h,topKNest,t) * [   sqr((prcElas(h,topKNest,topKNest,t) - p_prcElas0(h,topKNest)))
                                        + sqr((incElas(h,topKNest,t)          - p_incElas0(h,topKNest)))]);
*
* --- uncompensated own price elasticity defined from budget shares, bh parameter
*     income elasticity
*
  e_prcElas(hs(h),topKNest,topKNestp,ts(t)) $ (akfact(h,topKNest) and akfact(h,topKNestp)) ..
   prcElas(h,topKNest,topKNestp,t)
    =e= (1-bh(h,topKNest,t)) * (xakShr(h,topKNestp,t)-p_kron(topKNest,topKNestp))
          - xakShr(h,topKNestp,t)*(bh(h,topKNestp,t) + incElas(h,topKNest,t)
                                   -  sum(topKNestk, xakShr(h,topKNestk,t)*bh(h,topKNestk,t)));
*
* --- Income elasticity
*
  e_incElas(hs(h),topKNest,ts(t)) $ akfact(h,topKNest) ..
   incelas(h,topKNest,t) =e= (  eh(h,topKNest,t)*bh(h,topKNest,t)
                               -sum(topKNestp, xakShr(h,topKNestp,t)*eh(h,topKNestp,t)*bh(h,topKNestp,t)))
            /  sum(topKNestp, xakShr(h,topKNestp,t)*eh(h,topKNestp,t))
            -  (bh(h,topKNest,t) - 1) + sum(topKNestp, xakShr(h,topKNestp,t)*bh(h,topKNestp,t));
*
* --- budget share weigthed income elasticities must add up to one
*
  e_incElasAddup(hs(h)) ..
   1 =e= sum((topKNest,ts(t)), xakShr(h,topKNest,t)*incElas(h,topKNest,t));
*
* --- this prevents sign changes of income elasticities
*
  e_incElasC2(hs(h),topKNest,ts(t)) $ akfact(h,topKNest) ..
    (incElas(h,topKNest,t)-1)*(p_incElas0(h,topKNest)-1) =g= 0;

  model ehcal /
      e_hpdElas,e_prcElas,e_incElas,
      e_incElasAddup,e_incElasC2
  /;
  ehcal.holdFixed = 1;


  uh.l(h,ttCal)          = 1;
*
* --- fix quantities, prices and budget shares during calibration
*
  xak.fx(h,topKNest,t0)     = xak.l(h,topKNest,t0);
  yf.fx(h,t0)        = yf.l(h,t0);
  xakshr.fx(h,topKNest,t0)  = xakshr.l(h,topKNest,t0);
  uh.fx(h,t0)        = uh.l(h,t0);
*
* --- set Kronecker delta and starting values for bh and eh
*
  p_kron(topKNest,topKNest) = 1;

  bh.l(h,topKNest,t0) = min(0.98,(p_incElas0(h,topKNest)-p_prcElas0(h,topKNest))/2);
  eh.l(h,topKNest,t0) = p_incElas0(h,topKNest);
*
* --- fix parameters for unobserved demands to zero
*
  bh.fx(h,topKNest,ttCal)  $ (not akfact(h,topKNest)) = 0;
  eh.fx(h,topKNest,ttCal)  $ (not akfact(h,topKNest)) = 0;
  p_akf(h,topKNest,ttCal)  $ (not akfact(h,topKNest)) = 0;
*
* --- derive starting values for elasticities
*
  incelas.l(h,topKNest,t0) $ akfact(h,topKNest)
    = (eh.l(h,topKNest,t0)*bh.l(h,topKNest,t0)-sum(topKNestk, xakshr.l(h,topKNestk,t0)*eh.l(h,topKNestk,t0)*bh.l(h,topKNestk,t0)))
           /  sum(topKNestk, xakshr.l(h,topKNestk,t0)*eh.l(h,topKNestk,t0))
           -  (bh.l(h,topKNest,t0) - 1) + sum(topKNestk, xakshr.l(h,topKNestk,t0)*bh.l(h,topKNestk,t0));

  prcElas.l(h,topKNest,topKNestp,t0) $ ( akfact(h,topKNest) and akfact(h,topKNestp))
   = (1-bh.l(h,topKNest,t0))*(xakshr.l(h,topKNestp,t0)-p_kron(topKNest,topKNestP))
          -   xakshr.l(h,topKNestp,t0)*(bh.l(h,topKNestp,t0) + incElas.l(h,topKNest,t0)
          -   sum(topKNestk, xakshr.l(h,topKNestk,t0)*bh.l(h,topKNestk,t0)));

  option kill=ts;
  ts(t0) = yes;

  alias(h,hp);

  loop(hp,

     hs(hp) = yes;

     $$batinclude "inc/title.inc" '"Demand system calibration for "' hp.tl
*
*    --- bounds on expansion and substiution effects. Reflect max/min values found
*        in GTAP data base for single products
*
     eh.lo(hp,topKNest,t0) = 1.E-6;
     eh.up(hp,topKNest,t0) = 2.00;
     bh.lo(hp,topKNest,t0) = 1.E-4;
     bh.up(hp,topKNest,t0) = 0.9999;
     bh.fx(hp,topKNest,t0) $ (not akfact(hp,topKNest)) = 0;
*
*    --- as a default, no output to listing file.
*
     ehcal.solprint = 2;
     ehcal.iterlim  = 1000;
     ehcal.limcol   = 0;
     ehcal.limrow   = 0;
*
*    --- starting value for objective
*
     hpdElas.l = sum((hs(h),topKNest,ts(t)),
           xakshr.l(h,topKNest,t)* [   sqr((prcElas.l(h,topKNest,topKNest,t) - p_prcElas0(h,topKNest))/(p_prcElas0(h,topKNest)+1e-6))
                                     + sqr((incElas.l(h,topKNest,t)          - p_incElas0(h,topKNest))/(p_incElas0(h,topKNest)+1e-6))]);

     option NLP=Conopt4;
     solve ehcal minimizing hpdElas using nlp;
     if( (ehcal.modelStat > 2 OR ehcal.solveStat > 1),
*
*        --- if infeasible, switch solprint on and repeat solve
*
         ehcal.solprint = 1;
         solve ehcal minimizing hpdElas using nlp;
     );
     abort$(ehcal.modelStat > 2 OR ehcal.solveStat > 1) "Infeasible Solution for ehcal, in file: %system.fn%, line: %system.incline%";

     hs(hp) = no;
  );

  bh.fx(h,topKNest,ttCal) = bh.l(h,topKNest,t0);
  eh.fx(h,topKNest,ttCal) = eh.l(h,topKNest,t0);
  ts(ttCal) = no;
*
* --- derive constant terms of CDE and set intermediate variables used in model
*
  p_akf(h,topKNest,ttCal)$akFact(h,topKNest)
     = (xakshr.l(h,topKNest,t0)/bh.l(h,topKNest,t0))
         *(((yf.l(h,t0)/hpop.l(h,t0))/pxak.l(h,topKNest,t0))**(bh.l(h,topKNest,t0)))
          * (uh.l(h,t0)**(-eh.l(h,topKNest,t0)*bh.l(h,topKNest,t0)))
            /sum(akfact(h,topKNestk), xakshr.l(h,topKNestk,t0)/bh.l(h,topKNestk,t0));

  cdeInt.l(akFact(h,topKNest),ttCal) = p_akf(h,topKNest,t0)*bh.l(h,topKNest,t0)
                   * (uh.l(h,t0)**(eh.l(h,topKNest,t0)*bh.l(h,topKNest,t0)))
                   * (pxak.l(h,topKNest,t0)**(bh.l(h,topKNest,t0)))
                   * ((yf.l(h,t0)/hpop.l(h,t0))**(-bh.l(h,topKNest,t0)));

  consScale.l(h,ttCal) = sum(akfact(h,topKNest),cdeInt.l(h,topKNest,ttCal));

  option kill=prcElas.l;
  option kill=incElas.l;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execError, abort " Execerror encountered after initialiation of CDE demand system in file: %system.fn%, line: %system.incline%");
