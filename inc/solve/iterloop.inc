********************************************************************************
$ontext

   MANAGE  project

   GAMS file : ITERLOOP.INC

   @purpose  : Set variables from last year and make guesses for some
               based on real GDP per capita growth in baseline mode.
               Fix variables to zero which are not defined by equations.
   @author   : Manage team
   @date     : 29.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
  $$iftheni.mode "%1"=="fixTaxesLambda"
*
*    --- fix taxes for which all shifters are fixed
*
     tpsb.fx(aact(a),ts) $ (  (tpsb0.range(a,ts) eq 0) $ (tpsbD0.range(ts) eq 0) $ (tpsbD1.range(a,ts) eq 0)
                      $ (tpsbM0.range(ts) eq 0) $ (tpsbM1.range(a,ts) eq 0) $ (tMall.range(ts) eq 0))
      = (tpsb0.l(a,ts) + (tpsbD0.l(ts) + tpsbD1.l(a,ts)))
                 * tpsbM0.l(ts) * tpsbM1.l(a,ts) * (-sign(tpsb.l(a,tt-1)) * (tMall.l(ts)-1)+1);

     tPrd.fx(aact(a),ts) $ (  (tPrd0.range(a,ts) eq 0) $ (tPrdD0.range(ts) eq 0) $ (tPrdD1.range(a,ts) eq 0)
                      $ (tPrdM0.range(ts) eq 0) $ (tPrdM1.range(a,ts) eq 0) $ (tMall.range(ts) eq 0))
      = (tPrd0.l(a,ts) + (tPrdD0.l(ts) + tPrdD1.l(a,ts)))
                 * tPrdM0.l(ts) * tPrdM1.l(a,ts) * (sign(tPrd.l(a,tt-1)) * (tMall.l(ts)-1)+1);

     tPhn.fx(aact(a),ts) $ ((tPhn0.range(a,ts) eq 0) $ (tPhnD0.range(ts)  eq 0) $ (tPhnD1.range(a,ts) eq 0)
                         $  (not sum(mapAagr(aagr,a) $ (tPhnD2.range(aagr,ts) ne 0),1)))
        = tPhn0.l(a,ts) + tPhnD0.l(ts) + tPhnD1.l(a,ts)
                   + sum(mapAagr(aagr,a),tPhnD2.l(aagr,ts));

     tExp.fx(iExp(i),ts) $ ((tExp0.range(i,ts) eq 0) $ (tExpD0.range(ts) eq 0) $ (tExpD1.range(i,ts) eq 0)
                      $ (tExpM0.range(ts) eq 0) $ (tExpM1.range(i,ts) eq 0) $ (tMall.range(ts) eq 0))
          = (tExp0.l(i,ts) + (tExpD0.l(ts) + tExpD1.l(i,ts)))
                 * tExpM0.l(ts) * tExpM1.l(i,ts) * (sign(tExp.l(i,tt-1)) * (tMall.l(ts)-1)+1);

     tImp.fx(iImp(i),ts) $ ((tImp0.range(i,ts) eq 0) $ (tImpD0.range(ts) eq 0) $ (tImpD1.range(i,ts) eq 0)
                      $ (tImpM0.range(ts) eq 0) $ (tImpM1.range(i,ts) eq 0) $ (tMall.range(ts) eq 0))
          = (tImp0.l(i,ts) + (tImpD0.l(ts) + tImpD1.l(i,ts)) )
                 * tImpM0.l(ts) * tImpM1.l(i,ts) * (sign(tImp.l(i,tt-1)) * (tMall.l(ts)-1)+1);

     tVat.fx(iAct(i),n,ts) $ (  (tVat0.range(i,n,ts) eq 0) $ (tVatD0.range(ts) eq 0) $ (tVatD1.range(i,ts) eq 0)
                        $ (tVatD2.range(n,ts) eq 0)  $ (tVatM0.range(ts) eq 0) $ (tVatM1.range(i,ts) eq 0)
                        $ (tVatM2.range(n,ts) eq 0) $ (tMall.range(ts) eq 0))
          = (tVat0.l(i,n,ts) + (tVatD0.l(ts) + tVatD1.l(i,ts)) + tVatD2.l(n,ts))
                 * tVatM0.l(ts) * tVatM1.l(i,ts) * tVatM2.l(n,ts) * (sign(tVat.l(i,n,tt-1)) * (tMall.l(ts)-1)+1);

     tEmi.fx(emiFlag(em,aaIn(aa)),ts) $ (  (tEmi0.range(em,aa,ts) eq 0)
                          $ (tEmiD0.range(ts) eq 0) $ (tEmiD1.range(em,ts) eq 0) $ (tEmiD2.range(aa,ts) eq 0)
                          $ (tEmiM0.range(ts) eq 0) $ (tEmiM1.range(em,ts) eq 0) $ (tEmiM2.range(aa,ts) eq 0))
       = (tEmi0.l(em,aa,ts) + (tEmiD0.l(ts) + tEmiD1.l(em,ts) + tEmiD2.l(aa,ts)) $ (not ema(em)))
                 * tEmiM0.l(ts) * tEmiM1.l(em,ts) * tEmiM2.l(aa,ts);

*
*    --- set flag for endogenous emission taxes
*
     hasEmit = 1 $ sum(emiFlag(em,aa) $ ((tEmi.range(em,aa,tt) ne 0) or ((tEmi.range(em,aa,tt) eq 0) $ tEmi.l(em,aa,tt))),1);

     tPaf.fx(iAct,aaIn,ts) $ (  (tPaf0.range(iAct,aaIn,ts) eq 0) $ (tMall.range(ts) eq 0)
                            $ (tPafD0.range(ts) eq 0) $ (tPafD1.range(iAct,ts) eq 0) $ (tPafD2.range(aaIn,ts) eq 0)
                            $ (tPafM0.range(ts) eq 0) $ (tPafM1.range(iAct,ts) eq 0) $ (tPafM2.range(aaIn,ts) eq 0) $ sam(iAct,aaIn))
       = (tPaf0.l(iAct,aaIn,ts) + (tPafD0.l(ts) + tPafD1.l(iAct,ts) + tPafD2.l(aaIn,ts)))
                 * tPafM0.l(ts) * tPafM1.l(iAct,ts) * tPafM2.l(aaIn,ts) * (sign(tPaf.l(iAct,aaIn,tt-1)) * (tMall.l(ts)-1)+1);

     tFac.fx(aact,actFp,ts) $  ((tFac0.range(aact,actFp,ts) eq 0) $ (tFacD0.range(ts) eq 0) $ (tFacD1.range(aact,ts) eq 0) $ (tFacD2.range(actFp,ts) eq 0)
                 $ (tFacM0.range(ts) eq 0) $ (tFacM1.range(aact,ts) eq 0) $ (tFacM2.range(actFp,ts) eq 0)
                 $  [    (   [     (tfacMLab.range(ts)       eq 0) $ l(actFp)
                              and  (tfacMLabA.range(aact,ts) eq 0) $ l(actFp) or (not l(actFp))    ]
                              and [ (tfacMCap.range(ts)       eq 0) $ cap(actFp) or (not cap(actFp))]
                              and [ (tfacMlnd.range(ts)       eq 0) $ (lnd(actFp) or natRes(actFp)) or (not (lnd(actFp) or natRes(actFp)))]
                         )
                    ] $ (tMall.range(ts) eq 0) $ sum(v,flagXfac(actFp,aact,v)))
                 = (tFac0.l(aact,actFp,ts) + (tFacD0.l(ts) + tFacD1.l(aact,ts) + tFacD2.l(actFp,ts)))
                 * tFacM0.l(ts) * tFacM1.l(aact,ts) * tFacM2.l(actFp,ts)
                 *   (  (tfacMLab.l(ts)*tfacMLabA.l(aact,ts)) $ l(actFp)
                     +  tfacMCap.l(ts)     $ cap(actFp)
                     +  tfacMlnd.l(ts)     $ (lnd(actFp) or natres(actFp))) * (sign(tFac.l(aact,actFp,tt-1)) * (tMall.l(ts)-1)+1);
*
*    --- fix prod shifters which are exogenous
*
     lambdaxfac.fx(flagXFac(actFpNest,aact,v),ts)  $ ((chixFacF.range(actFpNest,ts) eq 0) $  (chixFacA.range(aact,ts) eq 0) $ (chixFacV.range(v,ts) eq 0)
                                                $ (chixFacFA.range(actFpNest,aact,ts) eq 0)

                                   $ [
                                          ({(chixFacL.range(ts)  eq 0)          $ l(actfpnest)} or (not l(actFpNest)))
                                      and ({(dmgAir.range(actFpnest,aact,ts) eq 0) $ (ifAirPollDmg and l(actFpnest))} or (not ifAirPollDmg))
                                      and ({(sum(chnl,dmgPrd.l(actFpnest,aact,chnl,ts)) eq 0) $ ifdmg or not ifdmg  })
                                      and ({(chixFacK.range(ts)   eq 0) $ cap(actFpnest)                 } or (not cap(actFpNest)))
                                      and ({(chixFacKL.range(ts)  eq 0) $ (cap(actFPnest) or l(actFpnest))} or (not (cap(actfpnest) or l(actfpnest))))
                                      and ({((chixFacFP.range(ts) eq 0) $ fp(actFpnest))                  } or (not fp(actFpNest)))
                                      and ({(chixFacNRG.range(ts) eq 0) $ nrgFac(actFpnest)               } or (not nrgFac(actFpNest)))
                                     ]

                                   $ (   sum(mapaagr(aagr,aact),(chixFacAgr.range(aagr,actFpNest,ts) eq 0))
                                      eq sum(mapaagr(aagr,aact),1))
                                    $ (lambdaxfac.scale(actFpNest,aact,v,ts) ne 999))

                              = chixFacF.l(actFpNest,ts) * chixFacA.l(aact,ts) * chixFacV.l(v,ts)
                                   * chixFacFA.l(actFpNest,aact,ts)
                                   * (1 + (chixFacL.l(ts)) $ l(actFpnest))
                                   * (1 + dmgAir.l(actfpnest,aact,ts) $ ifAirPollDmg $ l(actFpnest))
                                   * prod(chnl,1 + dmgPrd.l(actfpnest,aact,chnl,ts) $ifdmg)
                                   * (1 + chixFacK.l(ts)   $  cap(actFpnest))
                                   * (1 + chixFacKL.l(ts)  $ (cap(actFpnest) or l(actFpnest)))
                                   * (1 + chixFacFP.l(ts)  $ fp(actFpnest))
                                   * prod(mapaagr(aagr,aact),chixFacAgr.l(aagr,actFpNest,ts))
                                   * (1 + chixFacNRG.l(ts) $ nrgFac(actFpnest));

     $$iftheni.twist "%hasTwist%"=="on"
*
*       --- release bounds for active twists
*
        twist.lo(fpChl,fpParTc,aact(a),v,ts) $ (flagXFac(fpChl,a,v) $ tw(fpParTc,a,v,ts) $ ((chitwFA.range(fpParTc,a,ts) ne 0) or (chitwF.range(fpParTc,ts) ne 0)) $ pNest(a,fpChl,fpParTc)) = -inf;
        twist.up(fpChl,fpParTc,aact(a),v,ts) $ (flagXFac(fpChl,a,v) $ tw(fpParTc,a,v,ts) $ ((chitwFA.range(fpParTc,a,ts) ne 0) or (chitwF.range(fpParTc,ts) ne 0)) $ pNest(a,fpChl,fpParTc)) = +inf;

        twist.l(fpChl,flagxfac(fpParTc,aact(a),v),ts) $ ((twist.range(fpChl,fpParTc,a,v,ts)   ne 0)  $ pNEst(a,fpChl,fpPartc) $ twist.l(fpChl,fpParTc,a,v,tt-1)) =  twist.l(fpChl,fpParTc,a,v,tt-1);

     $$endif.twist
*
*    --- remove identical composite prices if vintage specific shifters are used
*
     fpIdent(fpPar,aACt) $ (sum(v $ (chixFacV.range(v,tt) ne 0),1) or (chixFacV.lo(old,tt) ne chixFacV.lo(new,tt))) = NO;


  $$elseif.mode  "%1"=="run"

     dummy.l = 0;

     $$iftheni.endotech "%endotech%"=="on"
         if((tt.pos ge 2),

*           variety progress corrected for exogenous growth
            p_variety(a,tt) $ p_phiRD(a,tt) = (1-p_rhoEn*einn.l(a,tt-1))/(1-p_rhoEn*einn.l(a,t0))*p_variety(a,tt-1);

            einn.l(a,tt)            = einn.l(a,tt-1);
            p_inn(a,tt)             = p_inn(a,tt-1);
         );

         frontBGP.fx(a,tt) = frontBGP.l(a,tt);
         einn.lo(a,tt) = -inf;
         einn.up(a,tt) = inf;
     $$endif.endotech

*    ----------------------------------------------------------------------------------------
*
*     Give a good starting guess for next year
*
*    ----------------------------------------------------------------------------------------
*
*    --- initialization for all years beyond last year or if the user has not indicated
*        that values are already loaded with the ifLoad flag
*
     if (ifCal,
        if ( (not ifBenchMarkCheck),
*
*          --- benchmark run, not a benchmark replication check
*
           pop.l(cohorts,tt) = p_popT(cohorts,tt) ;
*
*          --- use values from tt-1 in baseline construction (with very few exemptions)
*
           p_upDateFac("","") =  p_gdp(%tt%)/p_gdp(%tt1%);
           $$include 'inc/solve/resLastYear.inc'
           rowtransfers.fx(inst,ts(t)) $      ifRowTransfersPop  = rowtransfers.l(inst,t-1) * pop.l("PTOTL",t)/pop.l("PTOTL",t-1);
           rowtransfers.fx(inst,ts(t)) $ (not ifRowTransfersPop) = rowtransfers.l(inst,t-1) * p_gdp(t)/p_gdp(t-1);
        else
           p_upDateFac("","") =  1;
           $$include 'inc/solve/resLastYear.inc'
        );
     else
*
*       --- simulation: update factor is population changes times change in baseline change in per-capita income,
*           corrected for difference in per capita GDP growth in last year between baseline and current scen
*
        p_upDateFac("","") =  pop.l("PTOTL",%tt%)/pop.l("PTOTL",%tt1%) * (1+ggdppc.l(%tt1%)-p_ggdppcT(%tt1%) + ggdppc.l(%tt%));
*       --- use data from last year on demand
        $$ifi not "%startPoint%"=="Baseline" $$include 'inc/solve/resLastYear.inc'
        $$iftheni.forwardLooking "%ForwardLooking%"=="on"
*           --- initialize stocks in forward looking mode
           $$ifi "%startPoint%"=="Baseline"     $$batinclude 'inc/solve/resLastYear.inc' stocks
        $$endif.forwardLooking
     );

*    ----------------------------------------------------------------------------------------
*
*        Zero out non-active transfers between institutions
*        and debt payments  and fix lagged variables
*
*    ----------------------------------------------------------------------------------------

     yf.fx(oa,tt)  $ (yf.l(oa,tt-1) eq 0) = 0 ;
     xfd.fx(oa,tt) $ (yf.l(oa,tt-1) eq 0) = 0 ;
     pf.fx(oa,tt)  $ (yf.l(oa,tt-1) eq 0) = 1 ;

     transfers.fx(inst,instp,ts) $ (chiTr.l(inst,instp,ts) eq 0) = 0 ;
     debtPay.fx(inst,ts)         $ (not (debtPrincipal.l(inst,ts) + debtInterest.l(inst,ts))) = 0 ;

*    --- Fix lagged values used in dynamic equations where also showing up in summations
*        with current year. Otherwise .l is used to avoid confusion

     DebtStk.fx(instfd,tt-1)  = DebtStk.l(instfd,tt-1);
     DebtStkT.fx(tt-1)        = DebtStkT.l(tt-1);
     debtNew.fx(instfd,tt-1)  = debtNew.l(instfd,tt-1);
     debtNew.fx(instfd,t0)    = debtNew.l(instfd,t0);
     interest.fx(tt-1)        = interest.l(tt-1);
     interest.fx(t0)          = interest.l(t0);

     lCovArea.fx(lnd,lcov,tt-1)    = lCovArea.l(lnd,lCov,tt-1);

*    ----------------------------------------------------------------------------------------
*
*        Redefine share paramater of old vintage as weighted average from
*        last year old and new vintages
*
*    ----------------------------------------------------------------------------------------

     if (ifVintUpdate $ (not ifBenchMarkCheck) $ (ord(tt) gt 2),
*
*       --- add up factor demands of old and new vintage from t-1 to factor demand of new vintage in t (temporariy on marginals)
*
        xFac.m(flagXFac(actFpNest,aact,old),tt) $ fpNestUpdate(actFpNest,aact)
         = xFac.l(actFpNest,aact,old,tt-1) + xFac.l(actFpNest,aact,new,tt-1);
*
*       --- define average factor prices for old vintage from previous years (temporarily on marginals)
*           (only where subsitution elasticities differ)
*
        pxFac.m(flagXFac(actFpNest,aact,old),tt) $ fpNestUpdate(actFpNest,aact)
           = [    pxFac.l(actFpNest,aact,old,tt-1)*xFac.l(actFpNest,aact,old,tt-1)
              +   pxFac.l(actFpNest,aact,new,tt-1)*xFac.l(actFpNest,aact,new,tt-1)]/xFac.m(actFpNest,aact,old,tt);
*
*       --- define share parameter such that at least year average prices for factors across vintages,
*           the old vintage in the current year would demand the combined demand of old and new from last year.
*           The update is needed only if the substitution elasticities differ
*
        p_alphaXfac(fpChl,fpPar,aact,old) $ ((xFac.m(fpChl,aact,old,tt) gt 0) $ fpParUpdate(fpPar,aact) $ pNest(aact,fpChl,fpPar))
          = max(1.E-10,(xfac.m(fpChl,aact,old,tt)/xfac.m(fpPar,aact,old,tt))
                   * (lambdaxfac.l(fpChl,aact,old,tt-1)*(1 $ (not fpTop(fpPar)) + lambdaXfac.l(fpPar,aact,old,tt-1) $ fpTop(fpPar)))**(1-p_sigmaXFac(fpPar,aact,old))
                   * (pxfac.m(fpChl,aact,old,tt)/pxfac.m(fpPar,aact,old,tt))**p_sigmaxfac(fppar,aact,old)/(1
                   *(1+twist.l(fpChl,fpPar,aact,old,tt-1) $ fpParTC(fpPar))
            ));
        option kill=xFac.m,kill=pxFac.m;

      );
*
*     --- dynamic updates for endotech
*
      $$iftheni.endotech "%endotech%"=="on"
*
*        Equations (185)-(190)
*

         p_lambdaEn(a,old,ts(t)) = (  ((1-p_inn(a,t-1)*(1+2*p_rhoEn))*p_lambdaEn(a,old,t-1)**(p_sigmaxfac(fptop,a,old)-1)
                                    +        (1+p_rhoEn)*p_inn(a,t-1)*p_frontier(a,t-1)    **(p_sigmaxfac(fptop,a,old)-1))/(1-p_rhoEn*p_inn(a,t-1))
                                    )**(1/(p_sigmaxfac("f-top",a,"old")-1));

         p_lambdaCr(a,old,ts(t)) = (   ((1-p_inn(a,t0)*(1+2*p_rhoEn))*p_lambdaCr(a,old,t-1)**(p_sigmaxfac(fptop,a,old)-1)
                                    +         (1+p_rhoEn)*p_inn(a,t0)*p_frontCr(a,t-1)     **(p_sigmaxfac(fptop,a,old)-1))/(1-p_rhoEn*p_inn(a,t0))
                                     )**(1/(p_sigmaxfac("f-top",a,"old")-1));

         p_lambdaEnCr(a,old,ts(t)) $ p_frontCr(a,t) = p_lambdaEn(a,old,t)/ p_lambdaCr(a,old,t);
         p_lambdaEnCr(a,new,ts(t)) $ p_frontCr(a,t) = p_frontier(a,t)    / p_frontCr(a,t);

         p_obsol(a,ts(t)) $ einn.l(a,t-1) = (1+2*p_rhoEn)*einn.l(a,t-1);
         p_inn(a,t)       $ einn.l(a,t-1) = einn.l(a,t-1);

      $$endif.endotech


      if (not ifBenchMarkCheck,

*        ----------------------------------------------------------------------------------------
*
*            Closures for government consumption (and consequently gov savings at given
*            tax rates)
*
*        ----------------------------------------------------------------------------------------
*
*        --- Share of government expenditure and investment are solved using .l value for t0. For fiscal responsibility to work
*            rshrGDP needs to be free

         if ( ifFiscalResp,
            tmall.lo(ts)   $ (p_relExpInc ne 100) = -inf;
            tmall.up(ts)   $ (p_relExpInc ne 100) = +inf;
            tmall.l(%tt%)  $ (p_relExpInc ne 100) = tMall.l(%tt1%);
         else
            tmall.fx(ts)  = tmall.l(tt);
         );

         $$iftheni.baseline "%task%"=="baseline"
            $$iftheni.NetExport "%GDPCompNetExports%"=="nominal"
*
*              --- target net exports (= closely related to savf)
*
               botGdp.fx(%tt%)  = GDPComp("%GDPCompScen%","netExp",%tt%);
               capAccClosure    = savfExoRel;
               savfGdp.lo(%tt%) = -inf;
               savfGdp.up(%tt%) = +inf;

            $$endif.NetExport

            $$iftheni.savf not "%GDPCompSavf%"=="Closure rule"
*
*              --- target savf
*
               savfGdp.fx(%tt%)  = GDPComp("%GDPCompScen%","savf",%tt%);

            $$endif.savf

            if ( not ifFiscalResp,

                $$iftheni.govCons "%GDPCompGovCons%"=="nominal"

*                  --- fix gov consumption as share of nominal GDP (if projection is there)
                   nshrGDP.fx(gov,ts) $ GDPComp("%GDPCompScen%","govCons",%tt%) = GDPComp("%GDPCompScen%","govCons",%tt%);
*                  --- release shares in real terms (standard closure)
                   rshrGDP.lo(gov,ts) $ GDPComp("%GDPCompScen%","govCons",%tt%)     = -inf;
                   rshrGDP.up(gov,ts) $ GDPComp("%GDPCompScen%","govCons",%tt%)     = +inf;

                $$elseif.govCons "%GDPCompGovCons%"=="real"
*                  --- fix gov consumption as share of real GDP (if projection is there)
                   rshrGDP.fx(gov,ts) $ GDPComp("%GDPCompScen%","govCons",%tt%) = GDPComp("%GDPCompScen%","govCons",%tt%);
                $$endif.govCons

                $$iftheni.ginv "%GDPCompGovInv%"=="nominal"

*                  --- fix gov investments as share of nomimal GDP (if projection is there)
                   nshrGDP.fx(ginv,ts) $ GDPComp("%GDPCompScen%","ginv",%tt%) = GDPComp("%GDPCompScen%","ginv",%tt%);
*                  --- release shares in real terms (standard closure)
                   rshrGDP.lo(ginv,ts) $ GDPComp("%GDPCompScen%","ginv",%tt%)     = -inf;
                   rshrGDP.up(ginv,ts) $ GDPComp("%GDPCompScen%","ginv",%tt%)     = +inf;

                $$elseif.ginv "%GDPCompGovInv%"=="real"
*                  --- fix gov investment as real share of GDP (if projection is there)
                   rshrGDP.fx(ginv,ts) $ GDPComp("%GDPCompScen%","ginv",%tt%) = GDPComp("%GDPCompScen%","ginv",%tt%);
                $$endif.ginv
            );
         $$endif.baseline

         $$iftheni.sectorShareTarget "%sectorShareTargets%"=="on"

            sectShr.fx(aagr,%tt%) $ (gdpComp("%GDPCompScen%",aagr,%tt%) gt eps) = gdpComp("%GDPCompScen%",aagr,%tt%);
            if(sum(aagr $ (sectShr.range(aagr,%tt%) eq 0),1) eq card(aagr),
*              --- if all sector shares are fixed, release last one to avoid problems
*                  with linear dependencies

               sectShr.lo(aagr,%tt%) $ (aagr.pos eq card(aagr)) = -inf;
               sectShr.up(aagr,%tt%) $ (aagr.pos eq card(aagr)) = +inf;
            );
            chixFacAgr.lo(aagr,"f-top",%tt%) $ (sectShr.range(aagr,%tt%) eq 0) =  -inf;
            chixFacAgr.up(aagr,"f-top",%tt%) $ (sectShr.range(aagr,%tt%) eq 0) =  +inf;

         $$endif.sectorShareTarget
*
*        --- savings (incl. borrowing) close the balance between revenues and taxes
*
         govBalance.lo(ts) = -inf;
         govBalance.up(ts) = +inf;
*
*        --- gov savings share on real GDP in real terms must be endogenous if govBalance is endogenous
*
         rgovBalance.lo(ts) = -inf ;
         rgovBalance.up(ts) = +inf ;

*        ----------------------------------------------------------------------------------------
*
*            Closures for private savings and investments: marginal propensity to save is fixed
*
*        ----------------------------------------------------------------------------------------
*
         chims.fx(tt) = chims.l(tt);

*        ----------------------------------------------------------------------------------------
*
*            Closures for B.O.T.: exchange rate fixed, price index of gdp endogenous
*
*        ----------------------------------------------------------------------------------------

         er.fx(ts) = er.l(ts) ;
         pgdpmp.lo(ts) = -inf ;
         pgdpmp.up(ts) = +inf ;

*       ----------------------------------------------------------------------------------------                                    *
*
*           Specify dynamic assumptions
*
*       ----------------------------------------------------------------------------------------

*
*        --- fix previous year capital rent, investments, depreciation rate and capital stock
*
         xfd.fx(inv,tt-1)   = xfd.l(inv,tt-1);
         xfd.fx(ginv,tt-1)  = xfd.l(ginv,tt-1);
         depr.l(tt)         = depr.l(tt) ;
         kstock.fx(tt-1)    = kstock.l(tt-1) ;

         If( ifChicFacCap,
*
*           --- calculate saving shares by institution on total savings
*
            xfmSupUpdateFac.l(actFmp,inst,%tt%) $ (sum(sameas(actFmp,cap),1)
                                                     $ sum(sameas(inst,h),savh.l(h,%tt1%)) $ xfmSup.l(actFmp,actFmp,inst,oldf,t0))
               =  sum(sameas(h,inst),savh.l(h,%tt1%))
                /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));

            xfmSupUpdateFac.l(actFmp,inst,%tt%) $ (sum(sameas(actFmp,cap),1)
                                                    $ sum(sameas(inst,entr),savEntr.l(entr,%tt1%)) $ xfmSup.l(actFmp,actFmp,inst,oldf,t0))
               =  sum(sameas(entr,inst),savEntr.l(entr,%tt1%))
                /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));

            xfmSupUpdateFac.l(actFmp,inst,%tt%) $ (sum(sameas(actFmp,cap),1)
                                                    $ sum(sameas(inst,gov),max(0,govBalance.l(%tt1%))) $ xfmSup.l(actFmp,actFmp,inst,oldf,t0))
               =  sum(sameas(gov,inst),max(0,govBalance.l(%tt1%)))
                /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));
*
*           --- For updating capital income flows, weight with depreciation rate and capital income in %tt1%
*
            xfmSupUpdateFac.l(actFmp,inst,%tt%) $ (sum(sameas(actFmp,cap),1) $ xfmSup.l(actFmp,actFmp,inst,oldf,t0))
             = (1-depr.l(%tt1%))  * sum(vf,xfmSupUpdateFac.l(actFmp,inst,%tt1%)
                                        *xfmSup.l(actFmp,actFmp,inst,vf,%tt1%)
                                        *pfmSup.l(actFmp,actFmp,inst,vf,%tt1%)*sum(sameas(actFmp,cap),corrFac.l(cap,%tt1%)))
               + depr.l(%tt1%)    * xfmSupUpdateFac.l(actFmp,inst,%tt%) * sum(inv,yf.l(inv,%tt1%));
*
*           --- scale to match capital income in %tt1%
*
            xfmSupUpdateFac.fx(actFmp,inst,%tt%) $ (sum(sameas(actFmp,cap),1) $ xfmSup.l(actFmp,actFmp,inst,oldf,t0))
              =
*
*             --- estimated update factor (expected returns according to share on new investments relative to last year's capital income)
*
              xfmSupUpdateFac.l(actFmp,inst,%tt%) * sum(sameas(actFmp,cap),facty.l(cap,%tt1%))/sum(instp,xfmSupUpdateFac.l(actFmp,instp,%tt%));
*
*           --- sams for chiFac
*
            p_chiFac(inst,cap,%tt%) $ (sum(sameas(inst,h),savh.l(h,%tt1%)) $ p_chiFac(inst,cap,t0))
               =  sum(sameas(h,inst),savh.l(h,%tt1%))
                 /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));

            p_chiFac(inst,cap,%tt%) $ (sum(sameas(inst,entr),savEntr.l(entr,%tt1%)) $ p_chiFac(inst,cap,t0))
               =  sum(sameas(entr,inst),savEntr.l(entr,%tt1%))
                /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));

            p_chiFac(inst,cap,%tt%) $ (sum(sameas(inst,gov),max(0,govBalance.l(%tt1%))) $ p_chiFac(inst,cap,t0))
               =  sum(sameas(gov,inst),max(0,govBalance.l(%tt1%)))
                /(sum(entr,saventr.l(entr,%tt1%))+sum(hp,savh.l(hp,%tt1%))+savf.l(%tt1%)+max(0,govBalance.l(%tt1%)));
*
*           --- For updating capital income flows, weight with depreciation rate and capital income in %tt1%
*
            p_chiFac(inst,cap,%tt%) $ (p_chiFac(inst,cap,t0))
             = (1-depr.l(%tt1%))  * p_chiFac(inst,cap,%tt1%) * facty.l(cap,%tt1%)
               + depr.l(%tt1%)    * p_chiFac(inst,cap,%tt%)   * sum(inv,yf.l(inv,%tt1%))/facty.l(cap,%tt1%);
*
*           --- scale to match capital income in %tt1%
*
            p_chiFac(inst,cap,%tt%) $ p_chiFac(inst,cap,t0)
              = p_chiFac(inst,cap,%tt%) * facty.l(cap,%tt1%)
                 /sum(instp,p_chiFac(instp,cap,%tt%)*facty.l(cap,%tt1%));
         );

         if(ifCal eq 1,

*           --- Baseline scenario --> gdp per capita growth is fixed, labor productivity is endogenous

            rgdpmp.fx(ts) = p_GDP(ts);


            GdpCompShr.l(nipa,ts)   =  sum(mapn(fd,nipa), xfd.l(fd,ts))/rgdpmp.l(ts);
            GdpCompShr.l("Expo",ts) =   rtotexp.l(ts)/rgdpmp.l(ts);
            GdpCompShr.l("Impo",ts) =  -rtotimp.l(ts)/rgdpmp.l(ts);

            $$iftheni.endotech "%endotech%"=="on"
               frontBGP.up(a,ts) $ p_phiRD(a,ts) = +inf;
               frontBGP.lo(a,ts) $ p_phiRD(a,ts) = -inf;
               einn.fx(a,ts)     $ p_phiRD(a,ts) = p_inn(a,t0);
            $$endif.endotech

*           --- adaptation investments in the baseline (level fixed, shares adjusts)

            xfd.fx(adpinv,ts)       = xfd.l(adpinv,t0);
            adpInvShr.lo(adpinv,ts) = -inf ;
            adpInvShr.up(adpinv,ts) = +inf ;

*           --- labor stock shifter follows population size in 15-64 year cohort
            atFmSup.fx(actFmp,instTot,oldf,ts(t)) $ sum(sameas(actFmp,l),1) = atFmSup.l(actFmp,instTot,oldf,t-1)*(p_popT("P1564",t)/p_popT("P1564",t-1));

            $$iftheni.labVintages "%labVintages%"=="on"

*              --- labor stock change for the new vintage follows the change in <15 year cohort (new entrants)

               atFmSup.fx(actFmp,instTot,newf,%tt%) $ sum(sameas(actFmp,l),1)
                = atFmSup.l(actFmp,instTot,newf,%tt1%)*(p_popT("PLT15",%tt%)/p_popT("PLT15",%tt1%));

*              --- the change in the old is the residual

               atFmSup.fx(actFmp,instTot,oldf,%tt%) $ sum(sameas(actFmp,l),1)
                 =   (atFmSup.l(actFmp,instTot,oldf,%tt1%)+atFmSup.l(actFmp,instTot,newf,%tt1%))*(p_popT("P1564",%tt%)/p_popT("P1564",%tt1%))
                   - atFmSup.l(actFmp,instTot,newf,%tt%);

*              --- recalibrate the share parameter of the old vintage such that it would
*                  return that the combined stock of new and old in %tt1% at the average price t.1

               xfmSup.l(fmNestFmp(fmAll,actFmp),instTot,oldf,%tt%) $ sum(sameas(actFmp,l),1)
                 =  xfmSup.l(fmAll,actFmp,instTot,oldf,%tt1%)+xfmSup.l(fmAll,actFmp,instTot,newf,%tt1%);

               pfmSup.l(fmNestFmp(fmAll,actFmp),instTot,oldf,%tt%) $ (sum(sameas(actFmp,l),1) $ xfmSup.l(fmAll,actFmp,instTot,oldf,%tt%))
                 =  (  xfmSup.l(fmAll,actFmp,instTot,oldf,%tt1%)*pfmSup.l(fmAll,actFmp,instTot,oldf,%tt1%)
                     + xfmSup.l(fmAll,actFmp,instTot,newf,%tt1%)*pfmSup.l(fmAll,actFmp,instTot,newf,%tt1%))
                   /xfmSup.l(fmAll,actFmp,instTot,oldf,%tt%);

               $$ondotl
               p_gammaFmp(fmNestFmp(fmChild,actFmp),instTot,oldf,%tt%) $ (sum(fmNesting(fmChild,fmPar,actfmp), p_omegaFmp(fmPar,oldf) ne inf)
                                                                       $  xfmSup.l(fmChild,actFmp,instTot,oldf,tt) $ sum(sameas(actFmp,l),1) )
                  = sum(fmNesting(fmChild,fmPar,actFmp),
                                          (xfmSup.l(fmChild,actFmp,instTot,oldf,%tt%)/xfmSup.l(fmPar,actFmp,instTot,oldf,%tt%))
                                        * (pfmSup.l(fmpar,actFmp,instTot,oldf,%tt%)  /m_pfmSup(fmChild,actFmp,instTot,oldf,%tt%))**p_omegaFmp(fmPar,oldf));
               $$offdotl

               xfmSup.l(fmNestFmp(fmAll,actFmp),instTot,vf,%tt%)
                $ (sum(sameas(actFmp,l),1) $ atFmSup.l(actFmp,instTot,vf,%tt1%) $ xfmSup.l(fmAll,actFmp,instTot,vf,t0))
                   = xfmSup.l(fmAll,actFmp,instTot,vf,%tt1%) * atFmSup.l(actFmp,instTot,vf,%tt%)/atFmSup.l(actFmp,instTot,vf,%tt1%);

            $$endif.labVintages

            $$iftheni.IncomeDepCDE "%IncomeDepCDE%"=="on"
*
*              --- update expansion parameter based on income change
*                  and regression coefficients
*
               eh.fx(h,topKNest,%tt%) $ akfact(h,topKNest)
                = min(max(2.0,eh.l(h,topKNest,t0)),max(min(1.E-4,eh.l(h,topKNest,t0)),
                                               eh.l(h,topKNest,t0) + regParIncomeCDE("incP","logGDP",topkNest)
                                                        * [log(xfd.l(h,%tt%)/hpop.l(h,%tt%))-log(xfd.l(h,t0)/hpop.l(h,t0))]));

               bh.fx(h,topKNest,%tt%) $ akfact(h,topkNest)
                = min(0.9999,max(min(1.E-4,bh.l(h,topKNest,t0)),
                                           bh.l(h,topKNest,t0) + regParIncomeCDE("subP","logGDP",topkNest)
                                                        * [log(xfd.l(h,%tt%)/hpop.l(h,%tt%))-log(xfd.l(h,t0)/hpop.l(h,t0))]));

               p_akf(h,topKNest,%tt%) $ akFact(h,topkNest)
                  = (xakshr.l(h,topKNest,%tt%)/bh.l(h,topKNest,%tt%))
                  *(((yf.l(h,%tt%)/hpop.l(h,%tt%))/pxak.l(h,topKNest,%tt%))**(bh.l(h,topKNest,%tt%)))
                     * (uh.l(h,%tt%)**(-eh.l(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)))
                         /sum(topKNestK$akfact(h,topKNestK), xakshr.l(h,topKNestK,%tt%)/bh.l(h,topKNestK,%tt%));
*
*              --- recalculate intermediate expression in CDE demand system
*

              incelas.l(h,topKNest,%tt%) $ akfact(h,topKNest)
                = (eh.l(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)
                     -sum(topKNestK, xakshr.l(h,topKNestK,%tt%)*eh.l(h,topKNestK,%tt%)*bh.l(h,topKNestK,%tt%)))
                     /  sum(topKNestk, xakshr.l(h,topKNestk,%tt%)*eh.l(h,topKNestk,%tt%))
                     -  (bh.l(h,topKNest,%tt%) - 1) + sum(topKNestK, xakshr.l(h,topKNestK,%tt%)*bh.l(h,topKNestK,%tt%));

              cdeInt.l(h,topKNest,%tt%) $ akfact(h,topKNest)
                =   p_akf(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)
                  * (uh.l(h,%tt%)**(eh.l(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)))
                  * (pxak.l(h,topKNest,%tt%)**(bh.l(h,topKNest,%tt%)))
                  * ((yf.l(h,%tt%)/hpop.l(h,%tt%))**(-bh.l(h,topKNest,%tt%)));
              consScale.l(h,%tt%) = sum(akfact(h,topkNest),cdeInt.l(h,topKNest,%tt%));


            $$endif.IncomeDepCDE

         else

*           --- Dynamic scenarios with pre-calibrated productivity trends

            ggdppc.lo(ts) = -inf ;
            ggdppc.up(ts) = +inf ;
*
*           --- udpate distribution of interest and principal from government to institution based on distribution of new debt
*
         );

      else
*
*       --- benchmark test
*
        ifFiscalResp  = 0;
        capAccClosure = savfExoRel;

        xfd.fx(adpinv,ts)       = xfd.l(adpinv,t0);
*
*       --- benchmark test for calibration: fix some variables to t0 = t-1
*
        kstock.fx(tt)         = kstock.l(t0);

        rgdpmp.fx(tt)         = rgdpmp.l(t0);
        er.fx(tt)             = er.l(tt);
        xfd.fx(ginv,tt)       = xfd.l(ginv,t0);
        xfd.fx(gov,tt)        = xfd.l(gov,t0);
        rgdppc.lo(tt)         = -inf;
        rgdppc.up(tt)         = +inf;
        rgdppc.l(tt)          = rgdppc.l(t0);

        ggdppc.lo(tt) = -inf ;
        ggdppc.up(tt) = +inf ;
*
*       --- increase last year capital use to make e_xpold feasible
*
        xfac.l(cap,a,v,t0) $ (not ifEntech) = xfac.l(cap,a,v,t0)/(1-deprA.l(a,tt));

        oldCap.l(a,tt) $ ifVint = sum((cap,vp) $ flagXFac(cap,a,vp),xfac.l(cap,a,vp,tt-1))*(1-deprA.l(a,tt)) ;

        pop.l(cohorts,t0) = pop.l(cohorts,tt);
        popage.l(age-1,t0) $ drate.l(age,tt) = popAge.l(age,tt)/drate.l(age,tt);
        rgdppc.l(t0)    = rgdpmp.l(t0)/pop.l("ptotl",t0);
        hPop.l(h,tt)    = hPop.l(h,t0);
        p_hpop0(h)      = hpop.l(h,tt);
        p_pop0("ptotl") = pop.l("ptotl",tt);
        rgdppc.l(tt)    = rgdpmp.l(tt)/pop.l("ptotl",tt);
        ggdppc.l(tt)    = rgdppc.l(tt)/rgdppc.l(t0)-1;

        DebtStk.l(instfd,tt) = DebtStk.l(instfd,tt-1) + debtNew.l(instfd,tt) - rDebtStkT.l(tt) * rDebtStk.l(instfd,tt) * DebtStk.l(instfd,tt-1);
        DebtStkT.l(tt)    = sum(instfd,DebtStk.l(instfd,tt) );
        debtGDP.l(tt)     = DebtStkT.l(tt) / gdpmp.l(tt);

        brateU.fx(tt)     = sum(age $(ord(age) eq 1),popAge.l(age,tt))/pop.l("P1564",t0);

        debtPay.fx(instfd,tt) = debtPay.l(instfd,t0);
        debtInterest.fx(instfd,tt)  = debtInterest.l(instfd,t0);
        debtPrincipal.fx(instfd,tt) = debtPrincipal.l(instfd,t0);
*
*       --- as we are updating debtToGdp, we have to recalculate gammaFundF/gammasf2 to remove the resulting infeasibilities
*           (the model will stop after the benchmark check, so this no impact on any simulation)
*
        p_gammaFundF(tt) $ FundF.l(tt) = fundF.l(tt)/[(interest.l(tt)*sum(instf,riskPrem.l(instf,tt)))**p_sigmaFInt * ( 1 + ncpvusin(0,-{sigmoid([debtGdp.l(tt)-0.5]*4)-0.5}*2))];

        p_gammasf2(tt)    = savf.l(tt)/[exp(-p_omegasavf(tt)*er.l(tt)/pgdpmp.l(tt)*debtGDP.l(tt))
                            *depr.l(tt)*kstock.l(tt)*exp(p_rhosavf(tt)*(rore.l(tt)-rorg.l(tt)))];

        p_gammasf1(tt) $ (capAccClosure eq savfEndoResRegSavings)
           = savf.l(tt)/([((1+trent.l(tt))/(1+p_retrow(tt))) ** eta1a.l(tt)]
                         *[((1+ggdppc.l(tt))/(1+p_ggdppcrow(tt)))** eta2a.l(tt)]) ;

        rorg.fx(tt) = rore.l(tt);

     );

     if( (capAccClosure eq savfEndoERFix),

       er.fx(ts)     = er.l(ts);
       pgdpmp.fx(ts) = pgdpmp.l(ts);

     elseif( capAccClosure eq savfExoRel),
       savfGDP.fx(ts) = savfGDP.l(ts);
     );

  $$else.mode
*
*    ---- initialiaze variables on demand (mode final)
*
*    ---- initalize prices (pe/pd, pf => ps => pa )
*
     $$ifi "%iniAddVars%"=="on" $batinclude 'inc/solve/iniVars.inc' prices
*
*    ---- Initialize vintages
*
     iniVint = 0;
     $$ifi "%task%"=="baseline" iniVint $ ((tt.pos eq 2) $ ifVint $ (not ifEnTech) $ (not ifBenchMarkCheck)) = YES;
     $$ifi "%iniXfac%"=="on"    iniVint $ ifVint = YES;
     if (iniVint,
        $$batinclude 'inc/solve/iniVars.inc' vintages
     );
*
*    ---- factor demand
*
     $$ifi "%iniXfac%"=="on" $batinclude 'inc/solve/iniVars.inc' xFac

     if (ifVint $ (not ifEnTech),
        $$ondotl
        oldCap.l(aact,%tt%) = sum((cap,vp),xfac.l(cap,aact,vp,%tt1%))*(1-deprA.l(aact,tt)*sum(sameas(t,tt),m_dmgAgg(dmgDepr,'aact')));
        $$offdotl
        $$ifi "%ForwardLooking%"=="OFF" oldCap.fx(aact,%tt%) = oldCap.l(aact,%tt%);
     );
*
*    --- other variables (demands etc.)
*
     $$ifi "%iniAddVars%"=="on" $include 'inc/solve/iniVars.inc'

  $$endif.mode

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
