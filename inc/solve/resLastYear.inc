********************************************************************************
$ontext

   Manage project

   GAMS file : ResLastYear.INC

   @purpose  : Copy result from %tt1% to %tt% in baseline model
   @author   : MANAGE team
   @date     : 09.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : inc\iterloop.inc

$offtext
********************************************************************************
  $$ifi "%1"=="stocks" $goto stocks
*
* --- in first year, make sure that p_updateFac("","") > 1-depr to prevent that negative start values
*     for vintages are generated (hot start point defined below)
*
  p_updateFac("","")  $ (sum(sameas(t1,%tt%), years(t1) eq %baseYear%+1) $ (not ifBenchMarkCheck)) = max(1-depr.l(%tt%)+0.01,p_updateFac("",""));
  p_updateFac("","p") = 1;
  rgdpmp.l(%tt%)      $ (rgdpmp.range(%tt%)                     ne 0) = rgdpmp.l(%tt1%) * p_updateFac("","");
  p_updateFac("","p") $ pgdpmp.l(tt-2) =   (pgdpmp.l(tt-1)/pgdpmp.l(tt-2));
  p_updateFac("","p") $ (pgdpmp.l(tt-2) $ (rgdpmp.l(tt-1)-rgdpmp.l(tt-2) gt 0) $ (rgdpmp.l(tt)-rgdpmp.l(tt-1) gt 0))
   = (pgdpmp.l(tt-1)/pgdpmp.l(tt-2))/(rgdpmp.l(tt-1)/rgdpmp.l(tt-2))*(rgdpmp.l(tt)/rgdpmp.l(tt-1));
  p_updateFac("","v") = p_updateFac("","")*p_updateFac("","p");
*
* --- for normal runs (not forward looking) %tt% = tt and %tt1% = tt-1
*     for forward looking runs, this lastNextYear and sndLastNextYear as results for other years were already simulated
*
*     Update fac is normally equal to expected GDP growth. The changes to volumes and values assume that price stay
*     unchanged and all other variables move with GDP (= balanced growth assumptions). Without "new" shocks in t
*     (changes in taxes, prod shifters, debt stocks, etc.), this should be slightly better than simply using last
*     year's values. But some equations (such as in the demand system depending on GDP per capita) will become
*     infeasible. There are also a few exemptions where quantities are not updated, for instance, land cover,
*     where relative changes are typically smaller than GDP growth.
*
*
  xfac.l(flagxfac(actFpNest,aact,v),%tt%)             $ (xfac.range(actFpNest,aact,v,%tt%)              ne 0) = xfac.l(actFpNest,aact,v,%tt1%);
  oldCap.l(aact,%tt%)                                 $ (oldCap.l(aact,%tt%)                            ne 0) = oldCap.l(aact,%tt1%)           * p_updateFac("","");
  yield.l(actFp,aact,%tt%)                            $ (yield.range(actFp,aact,%tt%)                   ne 0) = yield.l(actFp,aact,%tt1%);
  yieldLnd.l(aact,%tt%)                               $ (yieldLnd.range(aact,%tt%)                      ne 0) = yieldLnd.l(aact,%tt1%);

  pxfac.l(flagxfac(actFpNest,aact,v),%tt%)            $ (pxfac.range(actFpNest,aact,v,%tt%)             ne 0) = pxfac.l(actFpNest,aact,v,%tt1%);
  pxfacInd.l(flagxfac(actFpNest,aact,v),%tt%)         $ ((pxfac.range(actFpNest,aact,v,%tt%)            ne 0)  $ isAdditiveCES(actFpNest)) = pxfacInd.l(actFpNest,aact,v,%tt1%);
  pxfac_pretax.l(flagxfac(actFp,aact,v),%tt%)         $ (pxfac_pretax.range(actFp,aact,v,%tt%)          ne 0) = pxfac_pretax.l(actFp,aact,v,%tt1%);

  lambdaxfac.l(flagxfac(actFpNest,aact,v),%tt%)       $ (lambdaxfac.range(actFpNest,aact,v,%tt%)        ne 0) = lambdaxfac.l(actFpNest,aact,v,%tt1%);

  $$iftheni.twist "%hasTwist%"=="on"
     chitwFA.l(fpParTc,aact,%tt%)                        $ (chitwFA.range(fpParTc,aact,%tt%)               ne 0) = chitwFA.l(fpParTc,aact,%tt1%);
     chitwF.l(fpParTc,%tt%)                              $ (chitwF.range(fpParTc,%tt%)                     ne 0) = chitwF.l(fpParTc,%tt1%);
     twist.l(fpChl,flagxfac(fpParTc,aact,v),%tt%)        $ ((twist.range(fpChl,fpParTc,aact,v,%tt%)        ne 0) $ pNEst(aact,fpChl,fpPartc))      = twist.l(fpChl,fpParTc,aact,v,%tt1%);
     shrXFac.l(fpChlTwist,flagxfac(fpParTc,aact,v),%tt%) $ ((shrXFac.range(fpChlTwist,fpParTc,aact,v,%tt%) ne 0) $ pNEst(aact,fpChlTwist,fpPartc)) = shrxFac.l(fpChlTwist,fpParTc,aact,v,%tt1%);
  $$endif.twist

  px.l(aact,%tt%)                              $ (px.range(aact,%tt%)                    ne 0) = px.l(aact,%tt1%);
  pp.l(aact,%tt%)                              $ (pp.range(aact,%tt%)                    ne 0) = pp.l(aact,%tt1%);
  xp.l(aact,%tt%)                              $ (xp.range(aact,%tt%)                    ne 0) = xp.l(aact,%tt1%)  * p_updateFac("","");
  x.l(aact,iAct,%tt%)                          $ ((x.range(aact,iAct,%tt%)               ne 0) $ sam(aact,iAct))  = x.l(aact,iAct,%tt1%) * p_updateFac("","");
  p.l(aact,iAct,%tt%)                          $ ((p.range(aact,iACt,%tt%)               ne 0) $ sam(aact,iAct))  = p.l(aact,iACt,%tt1%);
  pInd.l(iAct,%tt%)                            $ (pInd.range(iAct,%tt%)                  ne 0) = pInd.l(iAct,%tt1%);
  xs.l(iAct,%tt%)                              $ (xs.range(iAct,%tt%)                    ne 0) = xs.l(iAct,%tt1%)     * p_updateFac("","");
  ps.l(iAct,%tt%)                              $ (ps.range(iAct,%tt%)                    ne 0) = ps.l(iAct,%tt1%);
  psInd.l(iAct,%tt%)                           $ (psInd.range(iAct,%tt%)                 ne 0) = psInd.l(iAct,%tt1%);
  delst.l(ist(i),%tt%)                         $ (delst.range(i,%tt%)                    ne 0) = delst.l(i,%tt1%)  * p_updateFac("","");
  mdelst.l(ist(i),%tt%)                        $ (mdelst.range(i,%tt%)                   ne 0) = mdelst.l(i,%tt1%) * p_updateFac("","");
  ElecMix.l(aElec,iElec,%tt%)                  $ ((ElecMix.range(aElec,iElec,%tt%)       ne 0) $ p_ap(aElec,iElec,%tt%)) = ElecMix.l(aElec,iElec,%tt1%);
  facty.l(actFp,%tt%)                          $ (facty.range(actFp,%tt%)                ne 0) = facty.l(actFp,%tt1%) * p_updateFac("","");
  kapy.l(%tt%)                                 $ (kapy.range(%tt%)                       ne 0) = kapy.l(%tt1%)        * p_updateFac("","");
  corrFac.l(actFp,%tt%)                        $ (corrFac.range(actFp,%tt%)              ne 0) = corrFac.l(actFp,%tt1%);
  yh.l(h,%tt%)                                 $ (yh.range(h,%tt%)                       ne 0) = yh.l(h,%tt1%)        * p_updateFac("","v");
  yhDsp.l(h,%tt%)                              $ (yhDsp.range(h,%tt%)                    ne 0) = yhDsp.l(h,%tt1%)     * p_updateFac("","v");
  gini.l(%tt%)                                 $ (gini.range(%tt%)                       ne 0) = gini.l(%tt1%);
  rorc.l(%tt%)                                 $ (rorc.range(%tt%)                       ne 0) = max(1.E-10,rorc.l(%tt1%));
  rore.l(%tt%)                                 $ (rore.range(%tt%)                       ne 0) = max(1.E-10,rore.l(%tt1%));
  grk.l(%tt%)                                  $ (grk.range(%tt%)                        ne 0) = grk.l(%tt1%);
  aps.l(h,%tt%)                                $ (aps.range(h,%tt%)                      ne 0) = aps.l(h,%tt1%);
  chims.l(%tt%)                                $ (chims.range(%tt%)                      ne 0) = chims.l(%tt1%);

  yg.l(g,%tt%)                                 $ (yg.range(g,%tt%)                       ne 0) = yg.l(g,%tt1%)            * p_updateFac("","v");
  ygov.l(%tt%)                                 $ (ygov.range(%tt%)                       ne 0) = ygov.l(%tt1%)            * p_updateFac("","v");
  gtax.l(%tt%)                                 $ (gtax.range(%tt%)                       ne 0) = gtax.l(%tt1%)            * p_updateFac("","v");
  dgovtax.l(%tt%)                              $ (dgovtax.range(%tt%)                    ne 0) = dgovtax.l(%tt1%)         * p_updateFac("","v");
  govBalance.l(%tt%)                           $ (govBalance.range(%tt%)                 ne 0) = govBalance.l(%tt1%)      * p_updateFac("","v");
  primaryBalance.l(%tt%)                       $ (primaryBalance.range(%tt%)             ne 0) = primaryBalance.l(%tt1%)  * p_updateFac("","v");
  fiscalBalance.l(%tt%)                        $ (fiscalBalance.range(%tt%)              ne 0) = fiscalBalance.l(%tt1%)   * p_updateFac("","v");
  rgovBalance.l(%tt%)                          $ (rgovBalance.range(%tt%)                ne 0) = rgovBalance.l(%tt1%)     * p_updateFac(""," ");
  entrY.l(entr,%tt%)                           $ (entrY.range(entr,%tt%)                 ne 0) = entrY.l(entr,%tt1%)      * p_updateFac("","v");
  entrTax.l(entr,%tt%)                         $ (entrTax.range(entr,%tt%)               ne 0) = entrTax.l(entr,%tt1%)    * p_updateFac("","v");
  savEntr.l(entr,%tt%)                         $ (savEntr.range(entr,%tt%)               ne 0) = savEntr.l(entr,%tt1%)    * p_updateFac("","v");
  apsEntr.l(entr,%tt%)                         $ (apsEntr.range(entr,%tt%)               ne 0) = apsEntr.l(entr,%tt1%);
  savf.l(%tt%)                                 $ (savf.range(%tt%)                       ne 0) = savf.l(%tt1%)  * p_updateFac("","v");
  bot.l(%tt%)                                  $ (bot.range(%tt%)                        ne 0) = bot.l(%tt1%)   * p_updateFac("","v");
  botGdp.l(%tt%)                               $ (botGdp.range(%tt%)                     ne 0) = botGdp.l(%tt1%);
  savfGDP.l(%tt%)                              $ (savfGDP.range(%tt%)                    ne 0) = savfGDP.l(%tt1%);
  transfers.l(inst,instp,%tt%)                 $ (transfers.range(inst,instp,%tt%)       ne 0) = transfers.l(inst,instp,%tt1%)  * p_updateFac("","v");
  rowtransfers.l(inst,%tt%)                    $ (rowtransfers.range(inst,%tt%)          ne 0) = rowtransfers.l(inst,%tt1%)     * p_updateFac("","v");
  yf.l(fd,%tt%)                                $ (yf.range(fd,%tt%)                      ne 0) = yf.l(fd,%tt1%)   * p_updateFac("","v");
  xfd.l(oa,%tt%)                               $ (xfd.range(oa,%tt%)                     ne 0) = xfd.l(oa,%tt1%)  * p_updateFac("","");
  pf.l(oa,%tt%)                                $ (pf.range(oa,%tt%)                      ne 0) = pf.l(oa,%tt1%)   * p_updateFac("","p");
  pmarg.l(iArm,%tt%)                           $ (pmarg.range(iArm,%tt%)                 ne 0) = pmarg.l(iArm,%tt1%)    * p_updateFac("","p");
  pmargInd.l(iArm,%tt%)                        $ (pmargInd.range(iArm,%tt%)              ne 0) = pmargInd.l(iArm,%tt1%) * p_updateFac("","p");
  trentAvg.l(%tt%)                             $ (trentAvg.range(%tt%)                   ne 0) = trentAvg.l(%tt1%);
  brateU.l(%tt%)                               $ (brateU.range(%tt%)                     ne 0) = brateU.l(%tt1%);
  pdelst.l(%tt%)                               $ (pdelst.range(%tt%)                     ne 0) = pdelst.l(%tt1%);
  cpi.l(%tt%)                                  $ (cpi.range(%tt%)                        ne 0) = cpi.l(%tt1%)     * p_updateFac("","p");
  walras.l(%tt%)                               $ (walras.range(%tt%)                     ne 0) = walras.l(%tt1%);

  er.l(%tt%)                                   $ (er.range(%tt%)                         ne 0) = er.l(%tt1%);
  sectShr.l(aagr,%tt%)                         $ (sectShr.range(aagr,%tt%)               ne 0) = sectShr.l(aagr,%tt1%);
  nrgInt.l(%tt%)                               $ (nrgInt.range(%tt%)                     ne 0) = nrgInt.l(%tt1%);
*
* --- interest,savings,debt
*
  pindh.l(h,%tt%)                              $ (pindh.range(h,%tt%)                    ne 0) = pindh.l(h,%tt1%) * p_updateFac("","p");
  savh.l(h,%tt%)                               $ (savh.range(h,%tt%)                     ne 0) = savh.l(h,%tt1%)  * p_updateFac("","v");
  fundT.l(%tt%)                                $ (fundT.range(%tt%)                      ne 0) = fundT.l(%tt1%)   * p_updateFac("","v");
  fundG.l(%tt%)                                $ (fundG.range(%tt%)                      ne 0) = fundG.l(%tt1%)   * p_updateFac("","v");
  fundP.l(%tt%)                                $ (fundP.range(%tt%)                      ne 0) = fundP.l(%tt1%)   * p_updateFac("","v");
  fundF.l(%tt%)                                $ (fundF.range(%tt%)                      ne 0) = fundF.l(%tt1%)   * p_updateFac("","v");
  fundPrice.l(%tt%)                            $ (fundPrice.range(%tt%)                  ne 0) = fundPrice.l(%tt1%)  * p_updateFac("","p");
  fundPriceD.l(%tt%)                           $ (fundPriceD.range(%tt%)                 ne 0) = fundPriceD.l(%tt1%) * p_updateFac("","p");
  interest.l(%tt%)                             $ (interest.range(%tt%)                   ne 0) = max(1.E-7,interest.l(%tt1%));
  totTr.l(inst,%tt%)                           $ (totTr.range(inst,%tt%)                 ne 0) = totTr.l(inst,%tt1%)   * p_updateFac("","v");
  debtStk.l(instfd,%tt%)                       $ (debtStk.range(instfd,%tt%)             ne 0) = debtStk.l(instfd,%tt1%)* p_updateFac("","");
  debtStkT.l(%tt%)                             $ (debtStkT.range(%tt%)                   ne 0) = debtStkT.l(%tt1%)* p_updateFac("","");
  debtGDP.l(%tt%)                              $ (debtGDP.range(%tt%)                    ne 0) = debtGDP.l(%tt1%);
  debtPay.l(inst,%tt%)                         $ (debtPay.range(inst,%tt%)               ne 0) = debtPay.l(inst,%tt1%)* p_updateFac("","");
  debtInterest.l(inst,%tt%)                    $ (debtInterest.range(inst,%tt%)          ne 0) = debtInterest.l(inst,%tt1%)* p_updateFac("","");
  debtPrincipal.l(inst,%tt%)                   $ (debtPrincipal.range(inst,%tt%)         ne 0) = debtPrincipal.l(inst,%tt1%)* p_updateFac("","");
  debtNew.l(inst,%tt%)                         $ (debtNew.range(inst,%tt%)               ne 0) = debtNew.l(inst,%tt1%)* p_updateFac("","");
  debtNewT.l(%tt%)                             $ (debtNewT.range(%tt%)                   ne 0) = debtNewT.l(%tt1%)* p_updateFac("","");
  rDebtStk.l(instfd,%tt%)                      $ (rDebtStk.range(instfd,%tt%)            ne 0) = rDebtStk.l(instfd,%tt1%);
  rDebtStkT.l(%tt%)                            $ (rDebtStkT.range(%tt%)                  ne 0) = rDebtStkT.l(%tt1%);

* --- Flexible factor markets

  xfmSup.l(fmNestFmp(fmAll,actFmp),instTot,vf,%tt%)  $ (xfmSup.range(fmAll,actFmp,instTot,vf,%tt%)  ne 0) = xfmSup.l(fmAll,actFmp,instTot,vf,%tt1%);

  pfmSup.l(fmNestFmp(fmAll,actFmp),instTot,vf,%tt%)  $ (pfmSup.range(fmAll,actFmp,instTot,vf,%tt%)  ne 0) = pfmSup.l(fmAll,actFmp,instTot,vf,%tt1%);
  pfmSupD.l(fmNestFmp(fmPar,actFmp),instTot,vf,%tt%) $ (pfmSupD.range(fmPar,actFmp,instTot,vf,%tt%) ne 0) = pfmSupD.l(fmPar,actFmp,instTot,vf,%tt1%);

* --- Final demand variables

  xak.l(oa,kNest,%tt%)                         $ (xak.range(oa,kNest,%tt%)               ne 0) = xak.l(oa,kNest,%tt1%)  * p_updateFac("","");
  xakShr.l(oa,topKNest,%tt%)                   $ (xakshr.range(oa,topKNest,%tt%)         ne 0) = xakshr.l(oa,topKNest,%tt1%);
  pxak.l(oa,kNest,%tt%)                        $ (pxak.range(oa,kNest,%tt%)              ne 0) = pxak.l(oa,kNest,%tt1%) * p_updateFac("","p");
  gammainv.l(iArm,aact,%tt%)                   $ (gammaInv.l(iArm,aAct,t0) $ ifSectSpecInv)    = max(0.1*gammaInv.l(iArm,aAct,t0),gammaInv.l(iArm,aAct,%tt1%));
  invPrice.l(aact,%tt%)                        $ ifSectSpecInv                                 = invPrice.l(aact,%tt1%) * p_updateFac("","p");
  unEmp.l(%tt%)                                $ (unemp.range(%tt%) ne 0)                      = unEmp.l(%tt1%) * p_updateFac("","");

* --- Household demand variables

  cdeInt.l(h,topKNest,%tt%)                    $ (cdeInt.range(h,topKNest,%tt%)          ne 0) = cdeInt.l(h,topKNest,%tt1%);
  consScale.l(h,%tt%)                          $ (consScale.range(h,%tt%)                ne 0) = consScale.l(h,%tt1%);
  uh.l(h,%tt%)                                 $ (uh.range(h,%tt%)                       ne 0) = uh.l(h,%tt1%)   * (p_gdp(%tt%)/p_gdp(%tt1%))/(pop.l("PTOTL",%tt%)/pop.l("PTOTL",%tt1%));
  mmuh.l(h,%tt%)                               $ (mmuh.range(h,%tt%)                     ne 0) = mmuh.l(h,%tt1%) * (p_gdp(%tt%)/p_gdp(%tt1%))/(pop.l("PTOTL",%tt%)/pop.l("PTOTL",%tt1%));
  eh.l(h,topKNest,%tt%)                        $ (eh.range(h,topKNest,%tt%)              ne 0) = eh.l(h,topKNest,%tt1%);
  bh.l(h,topKNest,%tt%)                        $ (bh.range(h,topKNest,%tt%)              ne 0) = bh.l(h,topKNest,%tt1%);

* --- Other demand variables

  xmarg.l(iArm,%tt%)                           $ (xmarg.range(iArm,%tt%)                  ne 0) = xmarg.l(iArm,%tt1%)    * p_updateFac("","");
  pmarg.l(iArm,%tt%)                           $ (pmarg.range(iArm,%tt%)                  ne 0) = pmarg.l(iArm,%tt1%)    * p_updateFac("","p");
  pmargInd.l(iArm,%tt%)                        $ (pmargInd.range(iArm,%tt%)               ne 0) = pmargInd.l(iArm,%tt1%) * p_updateFac("","p");
  newInv.l(%tt%)                               $ (newInv.range(%tt%)                      ne 0) = newInv.l(%tt1%) * p_updateFac("","v");

* --- Trade variables

  xa.l(iArm,aaIn,%tt%)                         $ ((xa.range(iArm,aaIn,%tt%)               ne 0) $ sam(iArm,aaIn)) = xa.l(iArm,aaIn,%tt1%)   * p_updateFac("","");
  xat.l(iArm,%tt%)                             $ (xat.range(iArm,%tt%)                    ne 0) = xat.l(iArm,%tt1%)   * p_updateFac("","");
  pa.l(iArm,%tt%)                              $ (pa.range(iArm,%tt%)                     ne 0) = pa.l(iArm,%tt1%)    * p_updateFac("","p");
  paInd.l(iArm,%tt%)                           $ (paInd.range(iArm,%tt%)                  ne 0) = paInd.l(iArm,%tt1%) * p_updateFac("","p");
  paf.l(iArm,aaIn,%tt%)                        $ ((paf.range(iArm,aaIn,%tt%)              ne 0) $ sam(iArm,aaIn))  = paf.l(iArm,aaIn,%tt1%)  * p_updateFac("","p");

  xd.l(iDom,%tt%)                              $ (xd.range(iDom,%tt%)                     ne 0) = xd.l(iDom,%tt1%)  * p_updateFac("","");
  pd.l(iDom,%tt%)                              $ (pd.range(iDom,%tt%)                     ne 0) = pd.l(iDom,%tt1%)  * p_updateFac("","p");
  pda.l(iDom,%tt%)                             $ (pda.range(iDom,%tt%)                    ne 0) = pda.l(iDom,%tt1%) * p_updateFac("","p");
  xe.l(iExp,%tt%)                              $ (xe.range(iExp,%tt%)                     ne 0) = xe.l(iExp,%tt1%)  * p_updateFac("","");
  pe.l(iExp,%tt%)                              $ (pe.range(iExp,%tt%)                     ne 0) = pe.l(iExp,%tt1%);
  pwe.l(iExp,%tt%)                             $ (pwe.range(iExp,%tt%)                    ne 0) = pwe.l(iExp,%tt1%);

  xm.l(iImp,%tt%)                              $ (xm.range(iImp,%tt%)                     ne 0) = xm.l(iImp,%tt1%)   * p_updateFac("","");
  pwm.l(iImp,%tt%)                             $ (pwm.range(iImp,%tt%)                    ne 0) = pwm.l(iImp,%tt1%);
  pma.l(iImp,%tt%)                             $ (pma.range(iImp,%tt%)                    ne 0) = pma.l(iImp,%tt1%)  * p_updateFac("","p");


  rrat.l(aact,%tt%)                            $ (rrat.range(aact,%tt%)                   ne 0) = rrat.l(aact,%tt1%);
  rrat1.l(aact,%tt%)                           $ (rrat1.range(aact,%tt%)                  ne 0) = rrat1.l(aact,%tt1%);
  trent.l(%tt%)                                $ (trent.range(%tt%)                       ne 0) = trent.l(%tt1%);
  trentAvg.l(%tt%)                             $ (trentAvg.range(%tt%)                    ne 0) = trentAvg.l(%tt1%);

  kaps.l(%tt%)                                 $ (kaps.range(%tt%)                        ne 0) = kaps.l(%tt1%) * p_updateFac("","");
  ror.l(%tt%)                                  $ (ror.range(%tt%)                         ne 0) = max(1.E-10,ror.l(%tt1%));
  devRor.l(%tt%)                               $ (devRor.range(%tt%)                      ne 0) = devRor.l(%tt1%);
  rorc.l(%tt%)                                 $ (rorc.range(%tt%)                        ne 0) = rorc.l(%tt1%);
  prodInv.l(%tt%)                              $ (prodInv.range(%tt%)                     ne 0) = prodInv.l(%tt1%) * p_updateFac("","v");
  kstock.l(%tt%)                               $ (kstock.range(%tt%)                      ne 0) = kstock.l(%tt1%);
  kstocke.l(%tt%)                              $ (kstocke.range(%tt%)                     ne 0) = kstocke.l(%tt1%);
  depr.l(%tt%)                                 $ (depr.range(%tt%)                        ne 0) = depr.l(%tt1%);
  deprA.l(aact,%tt%)                           $ (deprA.range(aact,%tt%)                  ne 0) = deprA.l(aact,%tt1%);

* --- Demographics

  deprate.l(cohorts,%tt%)                      $ (deprate.range(cohorts,%tt%)            ne 0) = deprate.l(cohorts,%tt1%);
  brateU.l(%tt%)                               $ (brateU.range(%tt%)                     ne 0) = brateU.l(%tt1%);
  drate.l(age,%tt%)                            $ (drate.range(age,%tt%)                  ne 0) = drate.l(age,%tt1%);
  hpop.l(h,%tt%)                                                                               = hpop.l(h,%tt1%) * pop.l("PTOTL",%tt%)/pop.l("PTOTL",%tt1%);

* --- Emission variables

  if(card(EmiFlag),

     emiComm.l(emiCommFlag(em,emSrc,aaIn),%tt%)   $ ((emiComm.range(em,emSrc,aaIn,%tt%)     ne 0) $ emiCommFlag(em,emSrc,aaIn)) = emiComm.l(em,emSrc,aaIn,%tt1%) * p_updateFac("","");
     emiFact.l(em,actFp,aact,%tt%)                $ ((emiFact.range(em,actFp,aact,%tt%)     ne 0) $ emiFlag(em,aact))           = emiFact.l(em,actFp,aact,%tt1%) * p_updateFac("","");
     emiXP.l(em,aact,%tt%)                        $ ((emiXP.range(em,aact,%tt%)             ne 0) $ emiFlag(em,aact))           = emiXP.l(em,aact,%tt1%)         * p_updateFac("","");
     emiLUCF.l(em,aact,%tt%)                      $ ((emiLUCF.range(em,aact,%tt%)           ne 0) $ emiFlag(em,aact))           = emiLUCF.l(em,aact,%tt1%)       * p_updateFac("","");
     emiSecA.l(em,aaIn,%tt%)                      $ ((emiSecA.range(em,aaIn,%tt%)           ne 0) $ emiFlag(em,aaIn))           = emiSecA.l(em,aaIn,%tt1%)       * p_updateFac("","");
     emiTot.l(em,%tt%)                            $ (emiTot.range(em,%tt%)                  ne 0) = emiTot.l(em,%tt1%)           * p_updateFac("","");
     emiOth.l(em,%tt%)                            $ (emiOth.range(em,%tt%)                  ne 0) = emiOth.l(em,%tt1%)           * p_updateFac("","");
     emiGHG.l(polltype,%tt%)                      $ (emiGHG.range(polltype,%tt%)            ne 0) = emiGHG.l(polltype,%tt1%)     * p_updateFac("","");
     emiInv.l(ghgInv,em,%tt%)                     $ (emiInv.range(ghgInv,em,%tt%)           ne 0) = emiInv.l(ghgInv,em,%tt1%) * p_updateFac("","");
     emiInt.l(polltype,%tt%)                      $ (emiInt.range(polltype,%tt%)            ne 0) = emiInt.l(polltype,%tt1%);
     emiIntSec.l(aact,%tt%)                       $ (emiIntSec.range(aact,%tt%)             ne 0) = emiIntSec.l(aact,%tt1%);
     emiIntHH.l(h,%tt%)                           $ (emiIntHH.range(h,%tt%)                 ne 0) = emiIntHH.l(h,%tt1%);
     emical.l(%tt%)                               $ (emical.range(%tt%)                     ne 0) = emical.l(%tt1%);

     chiEmi.l(em,emSrc,aaIn,%tt%)                 $ ((chiEmi.range(em,emSrc,aaIn,%tt%)      ne 0) $ emiCommFlag(em,emSrc,aaIN)) = chiEmi.l(em,emSrc,aaIn,%tt1%);
     chiEmiFac.l(em,actFp,aact,%tt%)              $ ((chiEmiFac.range(em,actFp,aact,%tt%)   ne 0) $ emiFlag(em,aact))           = chiEmiFac.l(em,actFp,aact,%tt1%);
     chiEmiXP.l(em,aact,%tt%)                     $ ((chiEmiXP.range(em,aact,%tt%)          ne 0) $ emiFlag(em,aact))           = chiEmiXP.l(em,aact,%tt1%);
     chiEmiAA.l(aact,%tt%)                        $ (chiEmiAA.range(aact,%tt%)              ne 0)                               = chiEmiAA.l(aact,%tt1%);

  );

  lCovArea.l(actLnd,lCov,%tt%)                 $ (lCovArea.range(actLnd,lCov,%tt%)       ne 0) = lCovArea.l(actLnd,lCov,%tt1%);
  dCarbon.l(actLnd,lCov,%tt%)                  $ (dCarbon.range(actLnd,lCov,%tt%)        ne 0) = dCarbon.l(actLnd,lCov,%tt1%);


  elyInt.l(%tt%)                               $ (elyInt.range(%tt%)                     ne 0) = elyInt.l(%tt1%);
  nrgInt.l(%tt%)                               $ (nrgInt.range(%tt%)                     ne 0) = nrgInt.l(%tt1%);
  ExposedAirpoll.l(%tt%)                       $ (ExposedAirpoll.range(%tt%)             ne 0) = ExposedAirpoll.l(%tt1%);

* --- Macro variables

  tdelst.l(%tt%)                               $ (tdelst.range(%tt%)                     ne 0) = tdelst.l(%tt1%)  * p_updateFac("","");
  tdelst.l(%tt%)                               $ (tdelst.range(%tt%)                     ne 0) = tdelst.l(%tt1%)  * p_updateFac("","");
  rtdelst.l(%tt%)                              $ (rtdelst.range(%tt%)                    ne 0) = rtdelst.l(%tt1%) * p_updateFac("","");
  pdelst.l(%tt%)                               $ (pdelst.range(%tt%)                     ne 0) = pdelst.l(%tt1%);

  totExp.l(%tt%)                               $ (totExp.range(%tt%)                     ne 0) = totExp.l(%tt1%)  * p_updateFac("","");
  rTotExp.l(%tt%)                              $ (rTotExp.range(%tt%)                    ne 0) = rTotExp.l(%tt1%) * p_updateFac("","");
  pexp.l(%tt%)                                 $ (pexp.range(%tt%)                       ne 0) = pexp.l(%tt1%);
  totimp.l(%tt%)                               $ (totimp.range(%tt%)                     ne 0) = totimp.l(%tt1%)  * p_updateFac("","");
  rtotimp.l(%tt%)                              $ (rtotimp.range(%tt%)                    ne 0) = rtotimp.l(%tt1%) * p_updateFac("","");
  pimp.l(%tt%)                                 $ (pimp.range(%tt%)                       ne 0) = pimp.l(%tt1%);

  pgdpmp.l(%tt%)                               $ (pgdpmp.range(%tt%)                     ne 0) = pgdpmp.l(%tt1%) * p_updateFac("","p");
  gdpmp.l(%tt%)                                $ (gdpmp.range(%tt%)                      ne 0) = gdpmp.l(%tt1%)  * p_updateFac("","v");
  rgdpmp.l(%tt%)                               $ (rgdpmp.range(%tt%)                     ne 0) = rgdpmp.l(%tt1%) * p_updateFac("","");

  rgdppc.l(%tt%)                               $ (rgdppc.range(%tt%)                     ne 0) = rgdppc.l(%tt1%) * (p_gdp(%tt%)/p_gdp(%tt1%))/(pop.l("PTOTL",%tt%)/pop.l("PTOTL",%tt1%));
  ggdppc.l(%tt%)                               $ (ggdppc.range(%tt%)                     ne 0) = rgdppc.l(%tt%)/rgdppc.l(%tt1%) - 1;

  pgdpfc.l(%tt%)                               $ (pgdpfc.range(%tt%)                     ne 0) = pgdpfc.l(%tt1%);
  gdpfc.l(%tt%)                                $ (gdpfc.range(%tt%)                      ne 0) = gdpfc.l(%tt1%) * p_updateFac("","");
  rgdpfc.l(%tt%)                               $ (rgdpfc.range(%tt%)                     ne 0) = rgdpfc.l(%tt1%) * p_updateFac("","");
  fiscalRespDrv1.l(%tt%)                       $ (fiscalRespDrv1.range(%tt%)             ne 0) = fiscalRespDrv1.l(%tt1%);
  fiscalRespDrv.l(%tt%)                        $ (fiscalRespDrv.range(%tt%)              ne 0) = fiscalRespDrv.l(%tt1%);
  rshrGDP.l(f,%tt%)                            $ (rshrGDP.range(f,%tt%)                  ne 0) = rshrGDP.l(f,%tt1%);
  rshrGDP.l(oa,%tt%)                           $ (rshrGDP.range(oa,%tt%)                 ne 0) = rshrGDP.l(oa,%tt1%);
  nshrGDP.l(f,%tt%)                            $ (nshrGDP.range(f,%tt%)                  ne 0) = nshrGDP.l(f,%tt1%);
  GdpCompShr.l(nipa,%tt%)                      $ (GdpCompShr.range(nipa,%tt%)            ne 0) = GdpCompShr.l(nipa,%tt1%);
  cpi.l(%tt%)                                  $ (cpi.range(%tt%)                        ne 0) = cpi.l(%tt1%) * p_updateFac("","p");

* --- Productivity variables

  lambdaOa.l(oa,iArm,topKNest,%tt%)            $ (lambdaOa.range(oa,iArm,topKNest,%tt%)         ne 0) = lambdaOa.l(oa,iArm,topKNest,%tt1%);
  lambdaNRG.l(%tt%)                            $ (lambdaNRG.range(%tt%)                  ne 0) = lambdaNRG.l(%tt1%);
  labProd.l(%tt%)                              $ (labProd.range(%tt%)                    ne 0) = labprod.l(%tt1%);

  chixFacF.l(actFpNest,%tt%)                   $ (chixFacF.range(actFpNest,%tt%)         ne 0)  = chixFacF.l(actFpNest,%tt1%);
  chixFacA.l(aact,%tt%)                        $ (chixFacA.range(aact,%tt%)              ne 0)  = chixFacA.l(aact,%tt1%);
  chixFacV.l(v,%tt%)                           $ (chixFacV.range(v,%tt%)                 ne 0)  = chixFacV.l(v,%tt1%);
  chixFacFA.l(actFpNest,aact,%tt%)             $ (chixFacFA.range(actFpNest,aact,%tt%)   ne 0)  = chixFacFA.l(actFpNest,aact,%tt1%);
  chixFacL.l(%tt%)                             $ (chixFacL.range(%tt%)                   ne 0)  = chixFacL.l(%tt1%);
  chixFacK.l(%tt%)                             $ (chixFacK.range(%tt%)                   ne 0)  = chixFacK.l(%tt1%);
  chixFacKL.l(%tt%)                            $ (chixFacKL.range(%tt%)                  ne 0)  = chixFacKL.l(%tt1%);
  chixFacFP.l(%tt%)                            $ (chixFacFP.range(%tt%)                  ne 0)  = chixFacFP.l(%tt1%);
  chixFacNRG.l(%tt%)                           $ (chixFacNRG.range(%tt%)                 ne 0)  = chixFacNRG.l(%tt1%);
  chixFacAgr.l(aagr,actFpNest,%tt%)            $ (chixFacAgr.range(aagr,actFpNest,%tt%)  ne 0)  = chixFacAgr.l(aagr,actFpNest,%tt1%);

* --- Policy variables

  tFac.l(aact,actFp,%tt%)                      $ (tFac.range(aact,actFp,%tt%)            ne 0) = tFac.l(aact,actFp,%tt1%);
  rtFac.l(aact,actFp,%tt%)                     $ (rtFac.range(aact,actFp,%tt%)           ne 0) = rtFac.l(aact,actFp,%tt1%);
  tFac0.l(aact,actFp,%tt%)                     $ (tFac0.range(aact,actFp,%tt%)           ne 0) = tFac0.l(aact,actFp,%tt1%);
  tFacD0.l(%tt%)                               $ (tFacD0.range(%tt%)                     ne 0) = tFacD0.l(%tt1%);
  tFacD1.l(aact,%tt%)                          $ (tFacD1.range(aact,%tt%)                ne 0) = tFacD1.l(aact,%tt1%);
  tFacD2.l(actFp,%tt%)                         $ (tFacD2.range(actFp,%tt%)               ne 0) = tFacD2.l(actFp,%tt1%);
  tFacM0.l(%tt%)                               $ (tFacM0.range(%tt%)                     ne 0) = tFacM0.l(%tt1%);
  tFacM1.l(aact,%tt%)                          $ (tFacM1.range(aact,%tt%)                ne 0) = tFacM1.l(aact,%tt1%);
  tFacM2.l(actFp,%tt%)                         $ (tFacM2.range(actFp,%tt%)               ne 0) = tFacM2.l(actFp,%tt1%);
  tfacMLab.l(%tt%)                             $ (tfacMLab.range(%tt%)                   ne 0) = tfacMLab.l(%tt1%);
  tfacMLabA.l(aact,%tt%)                       $ (tfacMLabA.range(aact,%tt%)             ne 0) = tfacMLabA.l(aact,%tt1%);
  tfacMCap.l(%tt%)                             $ (tfacMCap.range(%tt%)                   ne 0) = tfacMCap.l(%tt1%);
  tfacMlnd.l(%tt%)                             $ (tfacMlnd.range(%tt%)                   ne 0) = tfacMlnd.l(%tt1%);

  tpsb.l(aact,%tt%)                            $ (tpsb.range(aact,%tt%)                  ne 0) = tpsb.l(aact,%tt1%);
  tpsb0.l(aact,%tt%)                           $ (tpsb0.range(aact,%tt%)                 ne 0) = tpsb0.l(aact,%tt1%);
  tpsbD0.l(%tt%)                               $ (tpsbD0.range(%tt%)                     ne 0) = tpsbD0.l(%tt1%);
  tpsbD1.l(aact,%tt%)                          $ (tpsbD1.range(aact,%tt%)                ne 0) = tpsbD1.l(aact,%tt1%);
  tpsbM0.l(%tt%)                               $ (tpsbM0.range(%tt%)                     ne 0) = tpsbM0.l(%tt1%);
  tpsbM1.l(aact,%tt%)                          $ (tpsbM1.range(aact,%tt%)                ne 0) = tpsbM1.l(aact,%tt1%);

  tPrd.l(aact,%tt%)                            $ (tPrd.range(aact,%tt%)                  ne 0) = tPrd.l(aact,%tt1%);
  tPrd0.l(aact,%tt%)                           $ (tPrd0.range(aact,%tt%)                 ne 0) = tPrd0.l(aact,%tt1%);
  tPrdD0.l(%tt%)                               $ (tPrdD0.range(%tt%)                     ne 0) = tPrdD0.l(%tt1%);
  tPrdD1.l(aact,%tt%)                          $ (tPrdD1.range(aact,%tt%)                ne 0) = tPrdD1.l(aact,%tt1%);
  tPrdM0.l(%tt%)                               $ (tPrdM0.range(%tt%)                     ne 0) = tPrdM0.l(%tt1%);
  tPrdM1.l(aact,%tt%)                          $ (tPrdM1.range(aact,%tt%)                ne 0) = tPrdM1.l(aact,%tt1%);

  tPhn.l(aact,%tt%)                            $ (tPhn.range(aact,%tt%)                  ne 0) = tPhn.l(aact,%tt1%);
  tPhn0.l(aact,%tt%)                           $ (tPhn0.range(aact,%tt%)                 ne 0) = tPhn0.l(aact,%tt1%);
  tPhnD0.l(%tt%)                               $ (tPhnD0.range(%tt%)                     ne 0) = tPhnD0.l(%tt1%);
  tPhnD1.l(aact,%tt%)                          $ (tPhnD1.range(aact,%tt%)                ne 0) = tPhnD1.l(aact,%tt1%);
  tPhnD2.l(aagr,%tt%)                          $ (tPhnD2.range(aagr,%tt%)                ne 0) = tPhnD2.l(aagr,%tt1%);

  tExp.l(iExp,%tt%)                            $ (tExp.range(iExp,%tt%)                  ne 0) = tExp.l(iExp,%tt1%);
  tExp0.l(iExp,%tt%)                           $ (tExp0.range(iExp,%tt%)                 ne 0) = tExp0.l(iExp,%tt1%);
  tExpD0.l(%tt%)                               $ (tExpD0.range(%tt%)                     ne 0) = tExpD0.l(%tt1%);
  tExpD1.l(iExp,%tt%)                          $ (tExpD1.range(iExp,%tt%)                ne 0) = tExpD1.l(iExp,%tt1%);
  TExpM0.l(%tt%)                               $ (TExpM0.range(%tt%)                     ne 0) = TExpM0.l(%tt1%);
  tExpM1.l(iExp,%tt%)                          $ (tExpM1.range(iExp,%tt%)                ne 0) = tExpM1.l(iExp,%tt1%);

  tVat.l(iArm,n,%tt%)                          $ (tVat.range(iArm,n,%tt%)                ne 0) = tVat.l(iArm,n,%tt1%);
  tVat0.l(iArm,n,%tt%)                         $ (tVat0.range(iArm,n,%tt%)               ne 0) = tVat0.l(iArm,n,%tt1%);
  tVatD0.l(%tt%)                               $ (tVatD0.range(%tt%)                     ne 0) = tVatD0.l(%tt1%);
  tVatD1.l(iArm,%tt%)                          $ (tVatD1.range(iArm,%tt%)                ne 0) = tVatD1.l(iArm,%tt1%);
  tVatD2.l(n,%tt%)                             $ (tVatD2.range(n,%tt%)                   ne 0) = tVatD2.l(n,%tt1%);
  tVatM0.l(%tt%)                               $ (tVatM0.range(%tt%)                     ne 0) = tVatM0.l(%tt1%);
  tVatM1.l(iArm,%tt%)                          $ (tVatM1.range(iArm,%tt%)                ne 0) = tVatM1.l(iArm,%tt1%);
  tVatM2.l(n,%tt%)                             $ (tVatM2.range(n,%tt%)                   ne 0) = tVatM2.l(n,%tt1%);

  tImp.l(iImp,%tt%)                            $ (tImp.range(iImp,%tt%)                  ne 0) = tImp.l(iImp,%tt1%);
  tImp0.l(iImp,%tt%)                           $ (tImp0.range(iImp,%tt%)                 ne 0) = tImp0.l(iImp,%tt1%);
  tImpD0.l(%tt%)                               $ (tImpD0.range(%tt%)                     ne 0) = tImpD0.l(%tt1%);
  tImpD1.l(iImp,%tt%)                          $ (tImpD1.range(iImp,%tt%)                ne 0) = tImpD1.l(iImp,%tt1%);
  tImpM0.l(%tt%)                               $ (tImpM0.range(%tt%)                     ne 0) = tImpM0.l(%tt1%);
  tImpM1.l(iImp,%tt%)                          $ (tImpM1.range(iImp,%tt%)                ne 0) = tImpM1.l(iImp,%tt1%);

  if(card(emiFlag),

     tEmi.l(emiFlag(em,aaIn),%tt%)                $ (tEmi.range(em,aaIn,%tt%)               ne 0) = tEmi.l(em,aaIn,%tt1%);
     rtEmi.l(emiFlag(em,aaIn),%tt%)               $ (rtEmi.range(em,aaIn,%tt%)              ne 0) = rtEmi.l(em,aaIn,%tt1%);
     tEmi0.l(emiFlag(em,aaIn),%tt%)               $ (tEmi0.range(em,aaIn,%tt%)              ne 0) = tEmi0.l(em,aaIn,%tt1%);
     tEmiD0.l(%tt%)                               $ (tEmiD0.range(%tt%)                     ne 0) = tEmiD0.l(%tt1%);
     tEmiD1.l(em,%tt%)                            $ (tEmiD1.range(em,%tt%)                  ne 0) = tEmiD1.l(em,%tt1%);
     tEmiD2.l(aact,%tt%)                          $ (tEmiD2.range(aact,%tt%)                ne 0) = tEmiD2.l(aact,%tt1%);
     tEmiM0.l(%tt%)                               $ (tEmiM0.range(%tt%)                     ne 0) = tEmiM0.l(%tt1%);
     tEmiM1.l(em,%tt%)                            $ (tEmiM1.range(em,%tt%)                  ne 0) = tEmiM1.l(em,%tt1%);
     tEmiM2.l(aact,%tt%)                          $ (tEmiM2.range(aact,%tt%)                ne 0) = tEmiM2.l(aact,%tt1%);

  );

  tPIT.l(h,%tt%)                               $ (tPIT.range(h,%tt%)                     ne 0) = tPIT.l(h,%tt1%);
  tPIT0.l(h,%tt%)                              $ (tPIT0.range(h,%tt%)                    ne 0) = tPIT0.l(h,%tt1%);
  tPITD0.l(%tt%)                               $ (tPITD0.range(%tt%)                     ne 0) = tPITD0.l(%tt1%);
  tPITD1.l(h,%tt%)                             $ (tPITD1.range(h,%tt%)                   ne 0) = tPITD1.l(h,%tt1%);
  tPITM0.l(%tt%)                               $ (tPITM0.range(%tt%)                     ne 0) = tPITM0.l(%tt1%);
  tPITM1.l(h,%tt%)                             $ (tPITM1.range(h,%tt%)                   ne 0) = tPITM1.l(h,%tt1%);

  tCIT.l(entr,%tt%)                            $ (tCIT.range(entr,%tt%)                  ne 0) = tCIT.l(entr,%tt1%);
  tCIT0.l(entr,%tt%)                           $ (tCIT0.range(entr,%tt%)                 ne 0) = tCIT0.l(entr,%tt1%);
  tCITD0.l(%tt%)                               $ (tCITD0.range(%tt%)                     ne 0) = tCITD0.l(%tt1%);
  tCITD1.l(entr,%tt%)                          $ (tCITD1.range(entr,%tt%)                ne 0) = tCITD1.l(entr,%tt1%);
  tCITM0.l(%tt%)                               $ (tCITM0.range(%tt%)                     ne 0) = tCITM0.l(%tt1%);
  tCITM1.l(entr,%tt%)                          $ (tCITM1.range(entr,%tt%)                ne 0) = tCITM1.l(entr,%tt1%);

  tPhn.l(aact,%tt%)                            $ (tPhn.range(aact,%tt%)                  ne 0) = tPhn.l(aact,%tt1%);
  tPhn0.l(aact,%tt%)                           $ (tPhn0.range(aact,%tt%)                 ne 0) = tPhn0.l(aact,%tt1%);
  tPhnD0.l(%tt%)                               $ (tPhnD0.range(%tt%)                     ne 0) = tPhnD0.l(%tt1%);
  tPhnD1.l(aact,%tt%)                          $ (tPhnD1.range(aact,%tt%)                ne 0) = tPhnD1.l(aact,%tt1%);
  tPhnD2.l(aagr,%tt%)                          $ (tPhnD2.range(aagr,%tt%)                ne 0) = tPhnD2.l(aagr,%tt1%);

  tDr.l(%tt%)                                  $ (tDr.range(%tt%)                        ne 0) = tDr.l(%tt1%);

  tmAll.l(%tt%)                                $ (tmAll.range(%tt%)                      ne 0) = tmAll.l(%tt1%);

  tPaf.l(iExp,aaIn,%tt%)                       $ ((tPaf.range(iExp,aaIn,%tt%)            ne 0) $ sam(iExp,aaIn)) = tPaf.l(iExp,aaIn,%tt1%);
  tPaf0.l(iExp,aaIn,%tt%)                      $ ((tPaf0.range(iExp,aaIn,%tt%)           ne 0) $ sam(iExp,aaIn)) = tPaf0.l(iExp,aaIn,%tt1%);
  tPafD0.l(%tt%)                               $ (tPafD0.range(%tt%)                     ne 0) = tPafD0.l(%tt1%);
  tPafD1.l(iExp,%tt%)                          $ (tPafD1.range(iExp,%tt%)                ne 0) = tPafD1.l(iExp,%tt1%);
  tPafD2.l(aaIn,%tt%)                          $ (tPafD2.range(aaIn,%tt%)                ne 0) = tPafD2.l(aaIn,%tt1%);
  tPafM0.l(%tt%)                               $ (tPafM0.range(%tt%)                     ne 0) = tPafM0.l(%tt1%);
  tPafM1.l(iExp,%tt%)                          $ (tPafM1.range(iExp,%tt%)                ne 0) = tPafM1.l(iExp,%tt1%);
  tPafM2.l(aaIn,%tt%)                          $ (tPafM2.range(aaIn,%tt%)                ne 0) = tPafM2.l(aaIn,%tt1%);
  ygPaf.l(iExp,%tt%)                           $ (ygPaf.range(iExp,%tt%)                 ne 0) = ygPaf.l(iExp,%tt1%)   * p_updateFac("","v");
  ygPit.l(h,%tt%)                              $ (ygPit.range(h,%tt%)                    ne 0) = ygPit.l(h,%tt1%)      * p_updateFac("","v");
  ygCit.l(entr,%tt%)                           $ (ygCit.range(entr,%tt%)                 ne 0) = ygCit.l(entr,%tt1%)   * p_updateFac("","v");
  ygEmi.l(aaIn,%tt%)                           $ (ygEmi.range(aaIn,%tt%)                 ne 0) = ygEmi.l(aaIn,%tt1%)   * p_updateFac("","v");
  ygEmiXP.l(aact,%tt%)                         $ (ygEmiXP.range(aact,%tt%)               ne 0) = ygEmiXP.l(aact,%tt1%) * p_updateFac("","v");
  ygTot.l(%tt%)                                $ (ygTot.range(%tt%)                      ne 0) = ygTot.l(%tt1%) * p_updateFac("","v");

* --- Exogenous variables

  tmg.l(iExp,n,%tt%)                           $ (tmg.range(iExp,n,%tt%)                 ne 0) = tmg.l(iExp,n,%tt1%);
  er.l(%tt%)                                   $ (er.range(%tt%)                         ne 0) = er.l(%tt1%);
  pwmROW.l(iExp,%tt%)                          $ (pwmRow.range(iExp,%tt%)                ne 0) = pwmROW.l(iExp,%tt1%);
  pweROW.l(iExp,%tt%)                          $ (pweRow.range(iExp,%tt%)                ne 0) = pweROW.l(iExp,%tt1%);

* ---  CC damages

  if(ifdmg,

    dmgPrd.l(actFpNest,aact,chnl,%tt%)           $ (dmgPrd.range(actFpNest,aact,chnl,%tt%) ne 0) = dmgPrd.l(actFpNest,aact,chnl,%tt1%);
    dmgYld.l(actFpNest,aact,chnl,%tt%)           $ (dmgYld.range(actFpNest,aact,chnl,%tt%) ne 0) = dmgYld.l(actFpNest,aact,chnl,%tt1%);
    dmgSup.l(actFpNest,chnl,%tt%)                $ (dmgSup.range(actFpNest,chnl,%tt%)      ne 0) = dmgSup.l(actFpNest,chnl,%tt1%);
    dmgStk.l(age,chnl,%tt%)                      $ (dmgStk.range(age,chnl,%tt%)            ne 0) = dmgStk.l(age,chnl,%tt1%);
    dmgStk.l(cap,chnl,%tt%)                      $ (dmgStk.range(cap,chnl,%tt%)            ne 0) = dmgStk.l(cap,chnl,%tt1%);
    dmgDem.l(h,iExp,knest,chnl,%tt%)             $ (dmgDem.range(h,iExp,knest,chnl,%tt%)   ne 0) = dmgDem.l(h,iExp,knest,chnl,%tt1%);
    dmgTrd.l(iExp,chnl,%tt%)                     $ (dmgTrd.range(iExp,chnl,%tt%)           ne 0) = dmgTrd.l(iExp,chnl,%tt1%);

  );

* ---  other

  adpInvShr.l(oa,%tt%)                      $ (adpInvShr.range(oa,%tt%)            ne 0) = adpInvShr.l(oa,%tt1%);
  adpInvShft.l(oa,%tt%)                     $ (adpInvShft.range(oa,%tt%)           ne 0) = adpInvShft.l(oa,%tt1%);
  chiP.l(aact,iExp,%tt%)                       $ ((chiP.range(aact,iExp,%tt%)            ne 0) $ sam(aact,iExp)) = chiP.l(aact,iExp,%tt1%);
  SectTot.l(%tt%)                              $ (SectTot.range(%tt%)                    ne 0) = SectTot.l(%tt1%) * p_updateFac("","");
  SectShr.l(aagr,%tt%)                         $ (SectShr.range(aagr,%tt%)               ne 0) = SectShr.l(aagr,%tt1%);

  If(ifVint $ (years(%tt%) eq %baseYear%+1) $ (not ifEnTech),

*    --- May have to jump start some variables in the first simulation year

     $$ondotl
     oldCap.l(aact,%tt%) $ sum(flagXFac(cap,aact,vp),1)
       = max(1.E-12,sum((cap,vp) $ flagXFac(cap,aact,vp),xfac.l(cap,aact,vp,%tt1%))*(1-deprA.l(aact,%tt%)*sum(sameas(t,%tt%),m_dmgAgg(dmgDepr,'aact'))));
     $$offdotl

     xfac.l(cap,aact,old,%tt%) = oldCap.l(aact,%tt%);

     xfac.l(flagxfac(actFpNest,aact,old),%tt%) $ oldCap.l(aact,%tt%) = max(1.E-16,xfac.l(actFpNest,aact,old,%tt1%) * sum(cap,xfac.l(cap,aact,old,%tt%)/xfac.l(cap,aact,old,%tt1%)));
     xfac.l(flagxfac(actFpNest,aact,new),%tt%) = max(1.E-16,sum(v,xfac.l(actFpNest,aact,v,%tt1%))*p_updateFac("","") - xfac.l(actFpNest,aact,old,%tt%));

  );

  $$iftheni.endoTech "%endotech%"=="on"

     oldCap.l(aact,%tt%) $ (sum(flagXFac(cap,aact,old),1) $ (not oldCap.l(aact,%tt%))) = sum(cap,xfac.l(cap,aact,old,%tt1%));

     xfdRD.l(f,%tt%) $ p_chiFdRD(f,%tt%) = xfdRD.l(f,%tt1%);
     yfdRD.l(%tt%) = yfdRD.l(%tt1%);
     pfdrd.l(%tt%) = pfdrd.l(%tt1%);
     einn.l(a,%tt%) $ p_markUp(a,%tt%) = einn.l(a,%tt1%);
     xRD.l(a,%tt%)  $ p_markUp(a,%tt%) = xRD.l(a,%tt1%);

  $$endif.endoTech

  flagXfac(fpnest,a,new) $ (not sum(cap $ flagXFac(cap,a,new),1)) = 0;


  If( (years(%tt%) eq %baseYear%+1),

     dCarbon.l(actHa,lCov,%tt%) $  ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov))
        = carbCnt(actHa,lCov) * (lCovArea.l(actHa,lCov,%tt%) - lCovArea.l(actHa,lCov,t0));

     emiInv.l(ghgInv,em,%tt%) $ (card(emiFlag) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF")))
       =   sum(emiCommFlag(em,emSrc,aa) $ p_ghgInvAlloc(ghgInv,em,emSrc,aa),
                                          p_ghgInvAlloc(ghgInv,em,emSrc,aa)    * emiComm.l(em,emSrc,aa,%tt%))
         + sum((actFp,a)                $ p_ghgInvAlloc(ghginv,em,actFp,a),
                                          p_ghgInvAlloc(ghginv,em,actFp,a)   * emiFact.l(em,actFp,a,%tt%))
         + sum(a $ (p_rhoEmiXP(em,a)      $ p_ghgInvAlloc(ghginv,em,"emiXP",a)),
                                            p_ghgInvAlloc(ghginv,em,"emiXp",a) * emiXP.l(em,a,%tt%)
                                        + emiLUCF.l(em,a,%tt%) $ (sum(actLnd,-p_rhoEmiLnd(em,actLnd,a)) $ sameas(em,"co2")))
         - sum((actHa,lCov) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov)),
                                              dCarbon.l(actHa,lCov,%tt%)*3.67) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF"));
  );

*
* --- this is also an entry for calling the code fom outside
*
  $$label stocks
  $$iftheni.forwardLooking "%ForwardLooking%"=="on"

*
*    --- update stocks and stock changes
*
     psExp.l(i,t1) $ (p_alphaSt(i,t1) $ ts(t1))
      = sum(tNext $ (years(tNext) ge years(t1)), p_exptWgts(tNext) * ps.l(i,tNext))
       /sum(tNext $ (years(tNext) ge years(t1)), p_exptWgts(tNext));

     psExpRel.l(i,t1) $ (p_alphaSt(i,t1) $ tNext(t1) $ psExp.l(i,t1)) = max(-0.9,min(+0.9,[(psExp.l(i,t1)-ps.l(i,t1))/psExp.l(i,t1)]));
     if(execError, abort psExp.l,psExpRel.l,tNExt);

     xsExp.l(i,t1) $ (p_alphaSt(i,t1) $ ts(t1))
       = max(1.E-8,sum(tNext $ (years(tNext) ge years(t1)), p_exptWgts(tNext) * xs.l(i,tNext))
                  /sum(tNext $ (years(tNext) ge years(t1)), p_exptWgts(tNext)));

     loop(ttt $ tNext(ttt),

        loop(ts(t1),
           st.l(i,t1) $ p_alphaSt(i,t1) = max(1.E-8,st.l(i,t1-1) + delst.l(i,t1));
        );

        delst.l(i,ts(t1)) $ p_alphaSt(i,t1) =
          max(-st.l(i,t1-1)+1.E-8,st.l(i,t1-1)
                * [ (-ncpvusin(-1.E-6,-psExpRel.l(i,t1),1.E-7))**p_rhoPDelSt(i) - (-ncpvusin(-1.E-6,psExpRel.l(i,t1),1.E-7))**p_rhoPDelSt(i) ]
                      $ sum(tNext, years(tNext) gt years(t1))
          +   xsExp.l(i,t1) * {(2*p_alphaSt(i,t1)/((st.l(i,t1)+st.l(i,t1-1))/xsExp.l(i,t1)))**p_rhoXDelSt(i) -1});

        loop(ts(t1),
           st.l(i,t1) $ p_alphaSt(i,t1) = max(1.E-8,st.l(i,t1-1) + delst.l(i,t1));
        );

        tdelst.l(ts(t1))  = sum(ist(i), p_chiPS(i)*ps.l(i,t1) *delst.l(i,t1) + er.l(t1)*pwm.l(i,t1) $iImp(i)*(1+tImp.l(i,t1))*mdelst.l(i,t1));
        rtdelst.l(ts(t1)) = sum(ist(i), p_chiPS(i)*ps.l(i,t0) *delst.l(i,t1) + er.l(t0)*pwm.l(i,t0) $iImp(i)*(1+tImp.l(i,t1))*mdelst.l(i,t1));
*        pdelst.l(ts(t1))  = 1;
*        pdelst.l(ts(t1)) $  rtdelst.l(t1)= tdelst.l(t1)/rtdelst.l(t1);

        xe.l(i,ts(t1)) $ (p_alphaSt(i,t1) $ (p_omegax(i) ne inf) $ xs.l(i,t1)) = p_ge(i,t1) *(xs.l(i,t1) - delst.l(i,t1))
                            * p_gammaxe(i,t1)**([-1-p_omegax(i)] $ (not addCest(i)) + p_omegax(i) $ addCest(i))
                            *(pe.l(i,t1)/(psIND.l(i,t1) $ addCest(i) + ps.l(i,t1) $ (not addCest(i))))**p_omegax(i);
     );

  $$endif.forwardLooking

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
