********************************************************************************
$ontext

   MANAGE project

   GAMS file : RESCALE.INC

   @purpose  :
   @author   :
   @date     : 12.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************

  $$ifi not "%scaleSam%"=="on" $exit

  if( rescale ne 1,
*
     xfac.l(flagxfac(fpNest,a,v),tScale) $ xfac.l(fpnest,a,v,tScale) = xfac.l(fpnest,a,v,tScale)*rescale;
     xfdRD.l(is,tScale) $  xfdRD.l(is,tScale)                        = xfdRD.l(is,tScale)*rescale;
     p_xRD0(a) $  p_xRD0(a)                                          = p_xRD0(a)*rescale;
     xp.l(a,tScale)                                                  = xp.l(a,tScale)*rescale;
     x.l(a,i,tScale) $ x.l(a,i,tScale)                               = x.l(a,i,tScale)*rescale;
     xs.l(i,tScale)                                                  = xs.l(i,tScale)*rescale;
     $$ifi defined xsExp xsExp.l(ist(i),tScale)                    = xsExp.l(i,tScale)*rescale;
     st.l(i,tScale)                                                  = st.l(i,tScale)*rescale;
     delst.l(i,tScale)                                               = delst.l(i,tScale)*rescale;
     mdelst.l(i,tScale)                                              = mdelst.l(i,tScale)*rescale;
     facty.l(fp,tScale)                                              = facty.l(fp,tScale)*rescale;
     kapy.l(tScale)                                                  = kapy.l(tScale)*rescale;
     yh.l(h,tScale)                                                  = yh.l(h,tScale)*rescale;
     yhDsp.l(h,tScale)                                               = yhDsp.l(h,tScale)*rescale;
     savh.l(h,tScale)                                                = savh.l(h,tScale)*rescale;
     fundT.l(tScale)                                                 = fundT.l(tScale)*rescale;
     fundG.l(tScale)                                                 = fundG.l(tScale)*rescale;
     fundP.l(tScale)                                                 = fundP.l(tScale)*rescale;
     fundF.l(tScale)                                                 = fundF.l(tScale)*rescale;
     totTr.l(inst,tScale)                                            = totTr.l(inst,tScale)*rescale;
     debtStk.l(inst,tScale)                                          = debtStk.l(inst,tScale)*rescale;
     debtStkT.l(tScale)                                              = debtStkT.l(tScale)*rescale;
     debtPay.l(inst,tScale)                                          = debtPay.l(inst,tScale)*rescale;
     debtInterest.l(inst,tScale)                                     = debtInterest.l(inst,tScale)*rescale;
     debtPrincipal.l(inst,tScale)                                    = debtPrincipal.l(inst,tScale)*rescale;
     debtNew.l(inst,tScale)                                          = debtNew.l(inst,tScale)*rescale;
     debtNewT.l(tScale)                                              = debtNewT.l(tScale)*rescale;
     kstocke.l(tScale)                                               = kstocke.l(tScale)*rescale;
     yg.l(g,tScale)                                                  = yg.l(g,tScale)*rescale;
     ygov.l(tScale)                                                  = ygov.l(tScale)*rescale;
     gTax.l(tScale)                                                  = gtax.l(tScale)*rescale;
     ygEmi.l(aa,tScale)                                              = ygEmi.l(aa,tScale)*rescale;
     ygEmiXP.l(a,tScale)                                             = ygEmiXP.l(a,tScale)*rescale;
     ygPit.l(h,tScale)                                               = ygPit.l(h,tScale)*rescale;
     ygCit.l(entr,tScale)                                            = ygCit.l(entr,tScale)*rescale;
     ygPaf.l(i,tScale)                                               = ygPaf.l(i,tScale)*rescale;
     ygtot.l(tScale)                                                 = ygTot.l(tScale)*rescale;
     fiscalBalance.l(tScale)                                         = fiscalBalance.l(tScale)*rescale;
     primaryBalance.l(tScale)                                        = primaryBalance.l(tScale)*rescale;
     govBalance.l(tScale)                                            = govBalance.l(tScale)*rescale;
     rgovBalance.l(tScale)                                           = rgovBalance.l(tScale)*rescale;
     entrY.l(entr,tScale)                                            = entrY.l(entr,tScale)*rescale;
     entrTax.l(entr,tScale)                                          = entrTax.l(entr,tScale)*rescale;
     savEntr.l(entr,tScale)                                          = savEntr.l(entr,tScale)*rescale;
     savf.l(tScale)                                                  = savf.l(tScale)*rescale;
     bot.l(tScale)                                                   = bot.l(tScale)*rescale;
     transfers.l(inst,instp,tScale)                                  = transfers.l(inst,instp,tScale)*rescale;
     rowtransfers.l(inst,tScale)                                     = rowtransfers.l(inst,tScale)*rescale;
     yf.l(oa,tScale)                                                 = yf.l(oa,tScale)*rescale;
     p_yf0(oa,tScale)                                                = p_yf0(oa,tScale)*rescale;
     p_GDP(tScale)                                                   = p_GDP(tScale)*rescale;
     atFmSup.l(fmAll,inst,vf,tScale)                                 = atFmSup.l(fmAll,inst,vf,tScale)*rescale;
     p_gammaFundF(tScale)                                            = p_gammaFundF(tScale)*rescale;
     p_pfIni(h)                                                      = p_pfini(h)*rescale;
     xfd.l(oa,tScale)                                                = xfd.l(oa,tScale)*rescale;
     mmuh.l(h,tScale)                                                = mmuh.l(h,tScale)*rescale;
     xak.l(oa,kNest,tScale) $ xak.l(oa,kNest,tScale)                 = xak.l(oa,kNest,tScale)*rescale;
     xmarg.l(j,tScale)                                               = xmarg.l(j,tScale)*rescale;
     xa.l(i,aa,tScale) $ xa.l(i,aa,tScale)                           = xa.l(i,aa,tScale)*rescale;
     xat.l(i,tScale)                                                 = xat.l(i,tScale)*rescale;
     xd.l(i,tScale)                                                  = xd.l(i,tScale)*rescale;
     xe.l(i,tScale)                                                  = xe.l(i,tScale)*rescale;
     xm.l(i,tScale)                                                  = xm.l(i,tScale)*rescale;
     xfmSup.l(fmAll,fmp,instTot,vf,tScale)                           = xfmSup.l(fmAll,fmp,instTot,vf,tScale)*rescale;
     kaps.l(tScale)                                                  = kaps.l(tScale)*rescale;
     kstock.l(tScale)                                                = kstock.l(tScale)*rescale;
     emiInt.l(polltype,tScale)                                       = emiInt.l(polltype,tScale)/rescale * reEscale;
     emiIntHH.l(h,tScale)                                            = emiIntHH.l(h,tScale)/rescale      * reEscale;
     emiIntSec.l(a,tScale)                                           = emiIntSec.l(a,tScale)/rescale     * reEscale;

     tdelst.l(tScale)                                                = tdelst.l(tScale)*rescale;
     rtdelst.l(tScale)                                               = rtdelst.l(tScale)*rescale;
     totExp.l(tScale)                                                = totExp.l(tScale)*rescale;
     rTotExp.l(tScale)                                               = rTotExp.l(tScale)*rescale;
     totimp.l(tScale)                                                = totimp.l(tScale)*rescale;
     rtotimp.l(tScale)                                               = rtotimp.l(tScale)*rescale;
     oldCap.l(a,tScale)                                              = oldCap.l(a,tScale)*rescale;
     gdpmp.l(tScale)                                                 = gdpmp.l(tScale)*rescale;
     rgdpmp.l(tScale)                                                = rgdpmp.l(tScale)*rescale;
     gdpfc.l(tScale)                                                 = gdpfc.l(tScale)*rescale;
     rgdpfc.l(tScale)                                                = rgdpfc.l(tScale)*rescale;
     walras.l(tScale)                                                = walras.l(tScale)*rescale;
     SectTot.l(tScale)                                               = SectTot.l(tScale)*rescale;
     hPop.l(h,tScale)                                                = hPop.l(h,tScale)*rescale;
     p_hPop0(h)                                                      = p_hPop0(h)*rescale;
     Pop.l(cohorts,tScale)                                           = Pop.l(cohorts,tScale)*rescale;
     p_Pop0(cohorts)                                                 = p_Pop0(cohorts)*rescale;
     p_popT(cohorts,tScale)                                          = p_popT(cohorts,tScale)*rescale;
     PopAge.l(age,tScale)                                            = PopAge.l(age,tScale)*rescale;
     unEmp.l(tScale)                                                 = unEmp.l(tScale)*rescale;
*
*    --- as emission inventories are not scaled with the SAM scaler, we need to scale the factors to keep them unchanged
*
     p_rhoEmi(em,emSrc,aa) $ p_rhoEmi(em,emSrc,aa)                   = p_rhoEmi(em,emSrc,aa)   * reEscale * 1/rescale;
     p_rhoEmiXP(em,a)      $ p_rhoEmiXP(em,a)                        = p_rhoEmiXP(em,a)        * reEscale * 1/rescale;
     p_rhoEmiFac(em,actfp,a)                                         = p_rhoEmiFac(em,actfp,a) * reEscale * 1/rescale;

     tEmi.l(em,aa,tScale)                                            = tEmi.l(em,aa,tScale)*rescale     * 1/reEscale;
     rtEmi.l(em,aa,tScale)                                           = rtEmi.l(em,aa,tScale)*rescale    * 1/reEscale;
     tEmiD0.l(tScale)                                                = tEmiD0.l(tScale)*rescale         * 1/reEscale;
     emiComm.l(emiCommFlag(em,emSrc,aa),tScale)                      = emiComm.l(em,emSrc,aa,tscale)    * reEscale;
     emiFact.l(em,actFp,aact,tScale)                                 = emiFact.l(em,actFp,aact,tscale)  * reEscale;
     emiXP.l(em,aact,tScale)                                         = emiXP.l(em,aact,tscale)          * reEscale;
     emiGHG.l(polltype,tScale)                                       = emiGHG.l(polltype,tScale)        * reEscale;
     emiTot.l(em,tScale)                                             = emiTot.l(em,tScale)              * reEscale;
     emiInv.l(ghgInv,em,tScale)                                      = emiInv.l(ghgInv,em,tScale)       * reEscale;
     dCarbon.l(actHa,lCov,tscale)                                    = dCarbon.l(actHa,lcov,tScale)     * reEscale;
     carbCnt(actLnd,lCov)                                            = carbCnt(actLnd,lCov)/reScale     * reEScale * usd;
     emiSeca.l(em,aa,tScale)                                         = emiSeca.l(em,aa,tScale)          * reEscale;

     p_land(actLnd,a)                                                = p_land(actLnd,a)*rescale;
     p_labor(l,a)                                                    = p_labor(l,a)*rescale;
     $$if defined p_DebtStkF0 p_DebtStkF0                            = p_DebtStkF0*rescale;
     $$if defined p_DebtStkD0 p_DebtStkD0                            = p_DebtStkD0*rescale;
     sam(is,js) $ sam(is,js)                                         = sam(is,js)*rescale;
  );

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
