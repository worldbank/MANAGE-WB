********************************************************************************
$ontext

   MANAGE project

   GAMS file : fixEnergy.INC

   @purpose  : Code to fix total Armington demands, factor demands,
               energy efficieny and world market prices
               Part of ccdr.inc, not a standalone-shock file
   @author   : W.Britz
   @date     : 19.10.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

    $$ifi not "%shk_file%"=="ccdr" $$Abort "FixEnergy.inc is part of the ccdr shock file and cannot be used in standalone mode"
*
*   --- declaration of symbols added for power mix and power price fixing
*
    $$ifi not defined energyProj $$abort "Bridge-file is missing energy projections, in file: %system.fn%, line: %system.incline%"

    set aEner(a)                "Activities targeted in energy proj"
        iEner(i)                "Products for which total demand is targeted in energy proj"
        shockedFpNest(fpNest)   "Production factors targeted in energy proj, such as energy products"
        shockedI(i)             "Products for which import or export prices are targeted"
    ;

    $$gdxin %readFile%
      $$load   aEner<energyProj.dim3
      $$load   iEner<energyProj.dim3
      $$load   shockedFpNest<energyProj.dim2
      $$load   shockedI<energyProj.dim3
    $$gdxin

    equation e_xFacAgr(aagr,fpNest,t) "Fix Demand for factors at level for sector aagregates"
             e_lambdaOa(oa,i,kNest,t) "Fix preference shifter to prod shifter";

    variable xFacAgr(fpNest,aagr,t)   "Demand for factors at level for sector aagregates";

*
*   --- the equation is paired with chixFacAgr and activated only if the shift factor is endogenous
*
    e_xFacAgr(aagr,actFpNest,ts(t)) $ (  sum(mapAagr(aagr,a),1) $ (chixFacAgr.range(aagr,actFpNest,t) ne 0)
                                       $ energyProj("%energyScen%",actFpNest,aagr,t))  ..
       xFacAgr(actFpNest,aagr,t)
               /max(%minScale%,sum(flagXFac(actFpNest,a,vp) $ mapAagr(aagr,a),xfac.l(actFpNest,a,vp,t-1))*%scaleMult%)
          =e=   sum(flagXFac(actFpNest,a,vp) $ mapAagr(aagr,a),xfac(actFpNest,a,vp,t))
               /max(%minScale%,sum(flagXFac(actFpNest,a,vp) $ mapAagr(aagr,a),xfac.l(actFpNest,a,vp,t-1))*%scaleMult%);

    e_lambdaOa(oa,i,knest,ts(t)) $ ( energyProj("%energyScen%","Demand",i,ts) $ (lambdaOa.range(oa,i,knest,t) ne 0) $ p_af(oa,i,knest,t)) ..

          lambdaOa(oa,i,knest,t) =E= sum(sameas(actFpNest,i),chixFacF(actFpNest,t));

    set dummyNrgFac / NrgFac /;

    $$iftheni.worldMarketPrices "%WorldMarketPriceProjections%"=="ON"
*
*      --- fix world market price projections to input
*
       pwmRow.fx(iImp(shockedI),tt) $  (energyProj("%energyScen%","pwm",shockedI,tt) $ (years("%enerProjStartYear%") le years(tt)))
         = energyProj("%energyScen%","pwm",shockedI,tt)/energyProj("%energyScen%","pwm",shockedI,t0);

       pweRow.fx(iExp(shockedI),tt) $  (energyProj("%energyScen%","pwm",shockedI,tt) $ (years("%enerProjStartYear%") le years(tt)))
         = energyProj("%energyScen%","pwm",shockedI,tt)/energyProj("%energyScen%","pwm",shockedI,t0);

    $$endif.worldMarketPrices

    alias(a,a_p);
    set largestProducer(a,*) "Largest producer at benchmark";
    largestProducer(a,i)  $ sam(a,i) = sam(a,i) eq smax(a_p,sam(a_p,i));
    largestProducer(a,"") $ sum(i,largestProducer(a,i)) = YES;


  $$else.decl

      $$iftheni.energyDemand "%EnergyDemandProjections%"=="ON"
*
*        --- fix the economy wide demand for energy services according to projection
*            (here, the use of an activity indicates that the prod. of the activity producing the output should be targeted
*
         xat.fx(i,%tt%) $ (sum(aEner $ sam(aEner,i),energyProj("%energyScen%","Demand",aEner,%tt%)) $ (years("%enerProjStartYear%") le years(%tt%)))
           = xat.l(i,t0) * sum(aEner $ (sam(aEner,i) $ energyProj("%energyScen%","Demand",aEner,t0)),
                energyProj("%energyScen%","Demand",aEner,%tt%)/energyProj("%energyScen%","Demand",aEner,t0));
*
*        --- shift productivity for energy activities to recover fixed demand forecast
*
         chixFacFa.l("f-top",aEner,%tt%)  $ (energyProj("%energyScen%","Demand",aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))  ) = chixFacFa.l("f-top",aEner,%tt1%);
         chixFacFa.lo("f-top",aEner,%tt%) $ (energyProj("%energyScen%","Demand",aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))  ) = -inf;
         chixFacFa.up("f-top",aEner,%tt%) $ (energyProj("%energyScen%","Demand",aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))  ) = +inf;
*
*        -- if multiple activities produce i, only fix the largest one
*
         chixFacFa.fx("f-top",aEner,%tt%) $ (not largestProducer(aEner,"")) = chixFacFa.l("f-top",aEner,%tt%);
*
*        --- energy demand is targeted directly (not via activity)
*
         xat.fx(iEner,%tt%) $ (energyProj("%energyScen%","Demand",iEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%)))
          = xat.l(iEner,t0) * energyProj("%energyScen%","Demand",iEner,%tt%)/energyProj("%energyScen%","Demand",iEner,t0);
*
*        --- shift productivity for intermediate demand of the product
*
         chixFacF.l(actFpNest,%tt%)  $ (sum(sameas(iEner,actFpNest),energyProj("%energyScen%","Demand",iEner,%tt%)) $ (years("%enerProjStartYear%") le years(%tt%))) =
                 chixFacF.l(actFpNest,%tt1%);
         chixFacF.lo(actFpNest,%tt%)  $ (sum(sameas(iEner,actFpNest),energyProj("%energyScen%","Demand",iEner,%tt%)) $ (years("%enerProjStartYear%") le years(%tt%))) = -inf;
         chixFacF.up(actFpNest,%tt%)  $ (sum(sameas(iEner,actFpNest),energyProj("%energyScen%","Demand",iEner,%tt%)) $ (years("%enerProjStartYear%") le years(%tt%))) = +inf;

*
*        --- and use the same shifter for the preferences in final demand
*
         lambdaOa.l(oa,i,knest,%tt%) $ (energyProj("%energyScen%","Demand",i,%tt%) $ p_af(oa,i,knest,%tt%))
          = lambdaOa.l(oa,i,knest,%tt1%);

         lambdaOa.lo(oa,i,knest,%tt%) $ (energyProj("%energyScen%","Demand",i,%tt%) $ p_af(oa,i,knest,%tt%)) = -inf;
         lambdaOa.up(oa,i,knest,%tt%) $ (energyProj("%energyScen%","Demand",i,%tt%) $ p_af(oa,i,knest,%tt%)) = +inf;
*
      $$endif.energyDemand

      $$iftheni.energyEff "%EnergyEffProjections%"=="ON"
*
*        --- shift energy efficiency
*
         lambdaxFac.scale("ener",aEner,v,%tt%) $ (energyProj("%energyScen%","nrgFac",aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))
            $ xfac.l("ener",aEner,old,t0))    = 999;
         lambdaxFac.fx("ener",aEner,v,%tt%)   $ (energyProj("%energyScen%","nrgFac",aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))
            $ xfac.l("ener",aEner,old,t0))
             = energyProj("%energyScen%","nrgFac",aEner,%tt%);

      $$endif.energyEff

      $$iftheni.energyAct "%EnergyActProjections%"=="ON"
*
*        --- fix factor demands according to projection (can be, for instance, total energy demand
*                                                        or individual intermediate demands)
*
         xfac.fx(shockedFpNest,aEner,v,%tt%) $ (energyProj("%energyScen%",shockedFpNest,aEner,%tt%) $ (years("%enerProjStartYear%") le years(%tt%))
              $  xfac.l(shockedFpNest,aEner,v,t0))
            = xfac.l(shockedFpNest,aEner,v,t0) * energyProj("%energyScen%",shockedFpNest,aEner,%tt%)
                                                 /energyProj("%energyScen%",shockedFpNest,aEner,t0);
*
*        --- render prod shifters endogenous for energy related nests with fixed demands
*
         lambdaxFac.scale(shockedFpNest,aEner,v,%tt%) $ (xfac.range(shockedFpNest,aEner,v,%tt%) eq 0) = 999;
         lambdaxFac.lo(shockedFpNest,aEner,v,%tt%)    $ (xfac.range(shockedFpNest,aEner,v,%tt%) eq 0)= -inf;
         lambdaxFac.up(shockedFpNest,aEner,v,%tt%)    $ (xfac.range(shockedFpNest,aEner,v,%tt%) eq 0)= +inf;
         lambdaxFac.l(shockedFpNest,aEner,v,%tt%)     $ (xfac.range(shockedFpNest,aEner,v,%tt%) eq 0)= lambdaxFac.l(shockedFpNest,aEner,v,%tt%-1);
*
*        --- intermediate, factor and composite demand for sector aggregates is targeted
*
         xfacAgr.fx(actFpNest,aagr,%tt%) $ (energyProj("%energyScen%",actFpNest,aagr,%tt%) $ (years("%enerProjStartYear%") le years(%tt%)))
          = sum(flagXFac(actFpNest,a,vp) $ mapAagr(aagr,a),
                 xfac.l(actFpNest,a,vp,t0)
                      * energyProj("%energyScen%",actFpNest,aagr,%tt%)/energyProj("%energyScen%",actFpNest,aagr,t0));
*
*        --- shift productivity for intermediate demand of the product
*
         chixFacAgr.l(aagr,actFpNest,%tt%)  $  (xfacAgr.range(actFpNest,aagr,%tt%) eq 0)  =  chixFacAgr.l(aagr,actFpNest,%tt1%);
         chixFacAgr.lo(aagr,actFpNest,%tt%)  $ (xfacAgr.range(actFpNest,aagr,%tt%) eq 0) =  -inf;
         chixFacAgr.up(aagr,actFpNest,%tt%)  $ (xfacAgr.range(actFpNest,aagr,%tt%) eq 0) =  +inf;

      $$endif.energyAct

      $$iftheni.worldMarketPrices "%WorldMarketPriceProjections%"=="ON"
*
*        --- fix world market price projections to input
*
         pwmRow.fx(iImp(shockedI),%tt%) $  (energyProj("%energyScen%","pwm",shockedI,%tt%) $ (years("%enerProjStartYear%") le years(%tt%)))
           = energyProj("%energyScen%","pwm",shockedI,%tt%)/energyProj("%energyScen%","pwm",shockedI,t0);

         pweRow.fx(iExp(shockedI),%tt%) $  (energyProj("%energyScen%","pwm",shockedI,%tt%) $ (years("%enerProjStartYear%") le years(%tt%)))
           =  energyProj("%energyScen%","pwm",shockedI,%tt%)/energyProj("%energyScen%","pwm",shockedI,t0);

      $$endif.worldMarketPrices

  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
