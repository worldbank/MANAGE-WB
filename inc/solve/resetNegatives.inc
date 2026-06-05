********************************************************************************
$ontext

   Manage project

   GAMS file :  resetNegatives.inc


   @purpose  : Reset negative, very small or large values to a share of
               last year's solution to avoid numerical problems with
               math over/underflows and to help the solver with a providing
               reasonable start point for further solution trials

               The bounds and reset values are heuristically determined.

   @author   : W.Britz
   @date     : 10.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  if ( dummy.l eq 0,

     $$iftheni.twist "%hasTwist%"=="on"
     twist.l(fpChl,fpPar,a,v,ts)       $ (twist.l(fpChl,fpPar,a,v,ts)        lt -0.999) = twist.l(fpChl,fpPar,a,v,tt-1)*0.5;
     shrxfac.l(fpChl,fpParTC,a,v,ts)   $ (shrxfac.l(fpChl,fpParTC,a,v,ts)    le 0)     = shrxfac.l(fpChl,fpParTC,a,v,tt-1) *0.5;
     $$endif.twist
     lambdaxfac.l(flagxFac(fpNest,a,v),ts)       $ (lambdaxfac.l(fpNest,a,v,ts)        le 1.E-3) = lambdaxfac.l(fpNest,a,v,tt-1)*0.2;
     lambdaxfac.l(flagxFac(fpNest,a,v),ts)       $ (lambdaxfac.l(fpNest,a,v,ts)        gt 1.E+3) = lambdaxfac.l(fpNest,a,v,tt-1)*5.0;

     yf.l(oa,ts)                       $ (yf.l(oa,ts)                        le 0)     = yf.l(oa,tt-1)*0.5;
     pf.l(oa,ts)                       $ (pf.l(oa,ts)                        le 1.E-3) = max(1.E-3,pf.l(oa,tt-1)*0.5);
     uh.l(h,ts)                        $ (uh.l(h,ts)                         le 0)     = uh.l(h,tt-1)*0.5;
     mmuh.l(h,ts)                      $ (mmuh.l(h,ts)                       le 0)     = mmuh.l(h,tt-1)*0.5;
     oldCap.l(a,ts)                    $ (oldCap.l(a,ts)                     le 0.1*oldCap.l(a,tt-1)) = oldcap.l(a,tt-1)*0.5;

     xfac.l(flagxFac(fpNest,a,new),ts)   $ (xfac.l(fpNest,a,new,ts)          le 1.E-14) = 1.E-14;
     xfac.l(flagxFac(fpNest,a,old),ts)   $ (xfac.l(fpNest,a,old,ts)          le 1.E-10) = max(1.E-10,xfac.l(fpNest,a,old,tt-1)*0.4);
     pxfac.l(flagxFac(fpNest,a,v),ts)    $ (pxfac.l(fpNest,a,v,ts)           le 1.E-2*pxFac.l(fpNest,a,v,t0))    = max(1.E-2*pxfac.l(fpNest,a,v,t0),pxfac.l(fpNest,a,v,tt-1)*0.5);
     pxfac.l(flagxFac(fpNest,a,v),ts)    $ (pxfac.l(fpNest,a,v,ts)           gt 50*pxfac.l(fpNest,a,v,t0))       = min(50*pxfac.l(fpNest,a,v,t0),pxfac.l(fpNest,a,v,tt-1)*2.0);
     pxfacInd.l(flagxFac(fpNest,a,v),ts) $ (pxfacInd.l(fpNest,a,v,ts)        le 1.E-2*pxfacInd.l(fpNest,a,v,t0)) = max(1.E-2*pxfacInd.l(fpNest,a,v,t0),pxfacInd.l(fpNest,a,v,tt-1)*0.5);
     pxfacInd.l(flagxFac(fpNest,a,v),ts) $ (pxfacInd.l(fpNest,a,v,ts)        gt 50*pxfacInd.l(fpNest,a,v,t0))    = min(50*pxfacInd.l(fpNest,a,v,t0),pxfacInd.l(fpNest,a,v,tt-1)*2.0);
     pxfac_pretax.l(flagxFac(fp,a,v),ts) $ (pxfac_pretax.l(fp,a,v,ts)        le 1.E-2*pxfac_pretax.l(fp,a,v,t0)) = max(1.E-2*pxfac_pretax.l(fp,a,v,t0),pxfac_pretax.l(fp,a,v,tt-1)*0.5);
     pxfac_pretax.l(flagxFac(fp,a,v),ts) $ (pxfac_pretax.l(fp,a,v,ts)        gt 50*pxfac_pretax.l(fp,a,v,t0))    = min(50*pxfac_pretax.l(fp,a,v,t0),pxfac_pretax.l(fp,a,v,tt-1)*2.0);

     xp.l(a,ts)                        $ (xp.l(a,ts)                         le 1.E-8) = max(1.E-8,xp.l(a,tt-1)*0.5);
     pp.l(a,ts)                        $ (pp.l(a,ts)                         le 1.E-3) = max(1.E-3,pp.l(a,tt-1)*0.5);
     x.l(a,i,ts)                       $ (x.l(a,i,ts)                        le 0)     = x.l(a,i,tt-1)*0.5;
     p.l(a,i,ts)                       $ ((p.l(a,i,ts)                       le 1.E-3) $ p_as(a,i,ts)) = max(1.E-3,p.l(a,i,tt-1)*0.5);
     xa.l(i,aa,ts)                     $ (xa.l(i,aa,ts)                      le 0)     = xa.l(i,aa,tt-1)*0.5;
     pmargInd.l(i,ts)                  $ (pmargInd.l(i,ts)                   le 1.E-3) = max(1.E-3,pmargInd.l(i,tt-1)*0.5);
     pmarg.l(i,ts)                     $ (pmarg.l(i,ts)                      le 1.E-3) = max(1.E-3,pmarg.l(i,tt-1)*0.5);
     xmarg.l(i,ts)                     $ (xmarg.l(i,ts)                      le 0)     = xmarg.l(i,tt-1)*0.5;
     paf.l(i,aa,ts)                    $ ((paf.l(i,aa,ts)                     le 1.E-3) $  sam(i,aa)) = max(1.E-3,paf.l(i,aa,tt-1)*0.5);
     cdeint.l(h,topKNest,ts)                  $ (cdeint.l(h,topKNest,ts)                   le 0)     = cdeint.l(h,topKNest,tt-1)*0.5;
     consScale.l(h,ts)                 $ (consScale.l(h,ts)                  le 0)     = consScale.l(h,tt-1)*0.5;

     xakShr.l(oa,kNest,ts)             $ (xakShr.l(oa,kNest,ts)              le 0)     = xakShr.l(oa,kNest,tt-1)*0.5;
     xak.l(oa,kNest,ts)                $ (xak.l(oa,kNest,ts)                 le 0)     = xak.l(oa,kNest,tt-1)*0.5;
     pxak.l(oa,kNest,ts)               $ (pxak.l(oa,kNest,ts)                le 1.E-3) = max(1.E-3,pxak.l(oa,kNest,tt-1)*0.5);
     lambdaOa.l(oa,i,topKNest,ts)             $ ((lambdaOa.l(oa,i,topKNest,ts)              le 1.E-3) $ lambdaOa.l(oa,i,topKNest,t0))  = max(1.E-6,lambdaOa.l(oa,i,topKNest,tt-1)*0.5);
     lambdaOa.l(oa,i,topKNest,ts)             $ ((lambdaOa.l(oa,i,topKNest,ts)              gt 50)    $ lambdaOa.l(oa,i,topKNest,t0))  = min(50,lambdaOa.l(oa,i,topKNest,tt-1)*2);

     ygov.l(ts)                        $ (ygov.l(ts)                         le 0)     = max(ygov.l(t0)*0.1,ygov.l(tt-1) * 0.5);

     xs.l(i,ts)                        $ (xs.l(i,ts)                         le 0)     = xs.l(i,tt-1)*0.5;
     xsExp.l(i,ts)                     $ ((xsExp.l(i,ts)                      le 0) $  p_alphaSt(i,ts))    = xs.l(i,tt-1)*0.9;
     ps.l(i,ts)                        $ (ps.l(i,ts)                         le 1.E-3*ps.l(i,t0)) = max(1.E-3*ps.l(i,t0),ps.l(i,tt-1)*0.5);
     ps.l(i,ts)                        $ (ps.l(i,ts)                         gt 50*ps.l(i,t0))    = min(50*ps.l(i,t0),ps.l(i,tt-1)*2);
     px.l(a,ts)                        $ (px.l(a,ts)                         le 1.E-3) = max(1.E-3,px.l(a,tt-1)*0.5);
     psExp.l(i,ts)                     $ ((psExp.l(i,ts)le 1.E-6)     $ p_alphaSt(i,ts)) = psExp.l(i,tt-1)*0.9;
     st.l(i,ts)                        $ ((st.l(i,ts)le 1.E-6)        $ p_alphaSt(i,ts)) = st.l(i,tt-1)*0.9;
     pd.l(i,ts)                        $ (pd.l(i,ts)                         le 1.E-3) = max(1.E-3,pd.l(i,tt-1)*0.5);
     pd.l(i,ts)                        $ (pd.l(i,ts)                         gt 50*pd.l(i,t0))     = min(50*pd.l(i,t0),pd.l(i,tt-1)*2);
     pe.l(i,ts)                        $ (pe.l(i,ts)                         le 1.E-3) = max(1.E-3,pe.l(i,tt-1)*0.5);
     pe.l(i,ts)                        $ (pe.l(i,ts)                         gt 50*pe.l(i,t0))     = min(50*pe.l(i,t0),pe.l(i,tt-1)*2);
     pwm.l(i,ts)                       $ (pwm.l(i,ts)                        le 1.E-3) = max(1.E-3,pwm.l(i,tt-1)*0.5);
     pwe.l(i,ts)                       $ (pwe.l(i,ts)                        le 1.E-3) = max(1.E-3,pwe.l(i,tt-1)*0.5);
     pda.l(i,ts)                       $ (pda.l(i,ts)                        le 1.E-2*pda.l(i,t0))  = max(1.E-2*pda.l(i,t0),pda.l(i,tt-1)*0.5);
     pda.l(i,ts)                       $ (pda.l(i,ts)                        ge 40*pda.l(i,t0))     = min(40*pda.l(i,t0),pda.l(i,tt-1)*2);
     pma.l(i,ts)                       $ (pma.l(i,ts)                        le 1.E-3) = max(1.E-3,pma.l(i,tt-1)*0.5);
     pa.l(i,ts)                        $ (pa.l(i,ts)                         le 1.E-2*pa.l(i,t0)) = max(1.E-2*pa.l(i,t0),pa.l(i,tt-1)*0.5);
     pa.l(i,ts)                        $ (pa.l(i,ts)                         ge 40*pa.l(i,t0))    = min(40*pa.l(i,t0),pa.l(i,tt-1)*2);
     xd.l(i,ts)                        $ (xd.l(i,ts)                         le 0)     = xd.l(i,tt-1)*0.5;
     xm.l(i,ts)                        $ (xm.l(i,ts)                         le 0)     = xm.l(i,tt-1)*0.5;
     xat.l(i,ts)                       $ (xat.l(i,ts)                        le 0)     = xat.l(i,tt-1)*0.5;
     rrat.l(a,ts)                      = max(1.E-3,min(1.,rrat.l(a,ts)));
     rrat1.l(a,ts)                     = max(1.E-3,min(1.,rrat1.l(a,ts)));
     kstock.l(ts)                      $ (kstock.l(ts)                       le 1.E-6) = kstock.l(tt-1)*0.9;
     kstockE.l(ts)                     $ (kstockE.l(ts)                      le 1.E-6) = kstockE.l(tt-1)*0.9;

     gammainv.l(iAct,aact,%tt%)       $ (gammaInv.l(iAct,aAct,t0) $ ifSectSpecInv )                 = max(0.1*gammaInv.l(iAct,aAct,t0),0.1*gammaInv.l(iAct,aAct,%tt1%),gammaInv.l(iAct,aAct,%tt%));
     invPrice.l(aAct,%tt%)            $ ((invPrice.l(aact,%tt%) le 1.E-3) $ ifSectSpecInv)          = max(1.E-3,invPrice.l(aAct,%tt1%));
     newInv.l(t)                      $ ((newInv.l(%tt%) le 0.1*newInv.l(%tt1%)) $ ifSectSpecInv)   = newInv.l(%tt1%);

     pfmSup.l(fmNestFmp(fmAll,fmp),instTot,vf,ts)  $ ((pfmSup.l(fmAll,fmp,instTot,vf,ts)  le 1.E-3*pfmSup.l(fmAll,fmp,instTot,vf,t0))  $ pfmSup.l(fmAll,fmp,instTot,vf,t0)) = max(1.E-3*pfmSup.l(fmAll,fmp,instTot,vf,t0),pfmSup.l(fmAll,fmp,instTot,vf,tt-1)*0.5);
     pfmSupD.l(fmNestFmp(fmPar,fmp),instTot,vf,ts) $ ((pfmSupD.l(fmPar,fmp,instTot,vf,ts) le 1.E-3*pfmSupd.l(fmPar,fmp,instTot,vf,t0)) $ pfmSupD.l(fmPar,fmp,instTot,vf,t0) $ isAdditiveCet(fmPar))  = max(1.E-3*pfmSupD.l(fmPar,fmp,instTot,vf,t0),pfmSupD.l(fmPar,fmp,instTot,vf,tt-1)*0.5);
     xfmSup.l(fmNestFmp(fmAll,fmp),instTot,vf,ts)  $ ((xfmSup.l(fmAll,fmp,instTot,vf,ts)  le 1.E-10) $ xfmSup.l(fmAll,fmp,instTot,vf,t0))= max(1.E-10,xfmSup.l(fmAll,fmp,instTot,vf,tt-1));

     yhDsp.l(h,ts)                     $ (yhDsp.l(h,ts)                      le 0)     = yhDsp.l(h,tt-1)*0.75;
     yh.l(h,ts)                        $ (yh.l(h,ts)                         le 0)     = yh.l(h,tt-1)*0.75;
     trent.l(ts)                       $ (trent.l(ts)                        le 0)     = max(1.E-10,trent.l(tt-1));
     interest.l(ts)                    $ (interest.l(ts)                     le 1.E-9)    = max(1.E-9,interest.l(tt-1));
     rorc.l(ts)                        $ (rorc.l(ts)                         le 0)     = max(1.E-10,rorc.l(tt-1));
     rore.l(ts)                        $ (rore.l(ts)                         le 0)     = max(1.E-10,rore.l(tt-1));
     fundPriceD.l(ts)                  $ (fundPriceD.l(ts)                   le 1.E-3)    = max(1.E-3,fundPriceD.l(tt-1)*0.01);
     fundPrice.l(ts)                   $ (fundPrice.l(ts)                    le 1.E-3)    = max(1.E-3,fundPrice.l(tt-1)*0.01);
     cpi.l(ts)                         $ (cpi.l(ts)                          le 0)     = cpi.l(tt-1);
     kapy.l(ts)                        $ (kapy.l(ts)                         le 0)     = kapy.l(tt-1);
     savf.l(ts)                        $ (savf.l(ts)                         le 0)     = savf.l(tt-1);
     entry.l(entr,ts)                  $ (entry.l(entr,ts)                   le 0)     = entry.l(entr,tt-1);
     gdpmp.l(ts)                       $ (gdpmp.l(ts)                        le 0)     = gdpmp.l(tt-1);
     rgdpmp.l(ts)                      $ (rgdpmp.l(ts)                       le 0)     = rgdpmp.l(tt-1);
     pgdpmp.l(ts)                      $ (pgdpmp.l(ts)                       le 1.E-3) = max(1.E-3,pgdpmp.l(tt-1));
     gdpfc.l(ts)                       $ (gdpfc.l(ts)                        le 1.E-6) = gdpfc.l(tt-1);
     rgdpfc.l(ts)                      $ (rgdpfc.l(ts)                       le 1.E-6) = rgdpfc.l(tt-1);
     pgdpfc.l(ts)                      $ (pgdpfc.l(ts)                       le 1.E-3) = max(1.E-3,pgdpfc.l(tt-1));
     sectTot.l(ts)                     $ (sectTot.l(ts)                      le 0)     = sectTot.l(tt-1);
     psind.l(i,ts)                     $ (psind.l(i,ts)                      le 1.E-3) = max(1.E-3,psind.l(i,tt-1));
     psind.l(i,ts)                     $ (psind.l(i,ts)                      gt 50*psInd.l(i,t0))  = min(50*psInd.l(i,t0),psind.l(i,tt-1)*2);
     paind.l(i,ts)                     $ (paind.l(i,ts)                      le 1.E-3*paind.l(i,t0)) = max(1.E-3*paind.l(i,tt-1),paind.l(i,tt-1)*0.5);
     paind.l(i,ts)                     $ (paind.l(i,ts)                      ge 50*paInd.l(i,t0)) =    min(50*paind.l(i,t0),paind.l(i,tt-1)*2);
     pind.l(i,ts)                      $ (pind.l(i,ts)                       le 1.E-3) = max(1.E-3,pind.l(i,tt-1));
     pindh.l(h,ts)                     $ (pindh.l(h,ts)                      le 1.E-3) = max(1.E-3,pindh.l(h,tt-1));
     savh.l(h,ts)                      $ (savh.l(h,ts)                       le 0)     = savh.l(h,tt-1) ;
     avgDebtInt.l(inst,ts)             $ (avgDebtInt.l(inst,ts)              le 0)     = avgDebtInt.l(inst,tt-1);
  );

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
