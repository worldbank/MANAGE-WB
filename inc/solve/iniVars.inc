********************************************************************************
$ontext

   MANAGE project

   GAMS file : INIVARS.INC

   @purpose  : Recursively define start point for model variables
   @author   : MANAGE team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms
                                                                                                                                                                  x
$offtext
********************************************************************************
  $$iftheni.arg "%1"=="Prices"

     pwe.l(iExp(i),%tt%) $ (p_etae(i) eq inf) =  min(pwe.l(i,%tt1%)*1.50,max(pwe.l(i,%tt1%)*0.50,p_gammaPWE(i,%tt%)*pwerow.l(i,%tt%)));
     pe.l(iExp(i),%tt%)  =  [er.l(%tt%)*p_chiPWE(i)*pwe.l(i,%tt%) - pmarg.l(i,%tt%)*tmg.l(i,"x",%tt%)]/[(1 + tExp.l(i,%tt%))*p_chiPE(i)];

     pd.l(iExp(i),%tt%) $ (pwerow.l(i,%tt%) ne pwerow.l(i,%tt1%))
       =  min(1.20*pd.l(i,%tt1%),max(pd.l(i,%tt1%)*0.80,
                                 pd.l(i,%tt1%)*(1+[pe.l(iExp,%tt%)/pe.l(iExp,%tt1%)-1]*p_ge(i,%tt%)/(p_ge(i,%tt%)+p_gd(i,%tt%)))));

     $$ondotl
     ps.l(iExp(i),%tt%) $ ((not addCest(i)) $ (p_omegax(i) ne inf))
          =  [                                              p_gd(i,%tt%) *(p_gammaxd(i,%tt%)*pd.l(i,%tt%))**(1+p_omegax(i))
               + sum(sameas(t,%tt%),m_dmgAgg(dmgTrd,'i')) * p_ge(i,%tt%) *(p_gammaxe(i,%tt%)*pe.l(i,%tt%))**(1+p_omegax(i)) ]**(1/(1+p_omegax(i)));

     psInd.l(iExp(i),%tt%) $ ((addCest(i)) $ (p_omegax(i) ne inf))
          =  [                                              p_gd(i,%tt%) *(p_gammaxd(i,%tt%)*pd.l(i,%tt%))**(p_omegax(i))
               + sum(sameas(t,%tt%),m_dmgAgg(dmgTrd,'i')) * p_ge(i,%tt%) *(p_gammaxe(i,%tt%)*pe.l(i,%tt%))**(p_omegax(i)) ]**(1/(p_omegax(i)));
     $$offdotl

     ps.l(iExp(i),%tt%) $ (idom(i) and iexp(i) $ addCest(i) $ (p_omegax(i) ne inf) )
     = [   p_gd(i,%tt%) * p_gammaxd(i,%tt%)**p_omegax(i)*pd.l(i,%tt%)**(p_omegax(i)+1)
         + p_ge(i,%tt%) * p_gammaxe(i,%tt%)**p_omegax(i)*pe.l(i,%tt%)**(p_omegax(i)+1) ]*(1/psIND.l(i,%tt%))**(p_omegax(i));

     pda.l(i,%tt%) = (p_chiPD(i)*pd.l(i,%tt%) + pmarg.l(i,%tt%)*tmg.l(i,"d",%tt%)) *  (1+tVat.l(i,"d",%tt%))/p_chiPDa(i);

     pwm.l(iImp,%tt%) $ (p_omegam(iImp) eq inf) = min(1.50*pwm.l(iImp,%tt1%),max(0.50*pwm.l(iImp,%tt1%),pwmrow.l(iImp,%tt%)));

     pma.l(iImp,%tt%) = (er.l(tt)*pwm.l(iImp,%tt%)*(1+tImp.l(iImp,%tt%)) + pmarg.l(iImp,%tt%)*tmg.l(iImp,"m",%tt%)) *  (1+tVat.l(iImp,"m",%tt%))/p_chiPMa(iImp);

     pa.l(iAct,%tt%)  $ ((not addCest(iAct)) $ (p_sigmam(iAct) ne inf) $ (p_ad(iAct,%tt%) or p_am(iAct,%tt%)))
      =  [    {p_ad(iAct,%tt%)*(pda.l(iAct,%tt%)/p_gammaMd(iAct,%tt%))**(1 - p_sigmam(iAct))} $ p_ad(iact,%tt%)
            + {p_am(iAct,%tt%)*(pma.l(iACt,%tt%)/p_gammaMm(iACt,%tt%))**(1 - p_sigmam(iACt))} $ p_am(iact,%tt%)]**(1/(1 - p_sigmam(iAct)));

     paInd.l(iAct(i),%tt%)  $ (addCest(i) $ (p_sigmam(i) ne inf))
      =  [    p_ad(i,%tt%)*(pda.l(i,%tt%)/p_gammaMd(i,%tt%))**(-p_sigmam(i))
            + p_am(i,%tt%)*(pma.l(i,%tt%)/p_gammaMm(i,%tt%))**(-p_sigmam(i))]**(1/(-p_sigmam(i)));

     pa.l(iAct(i),%tt%) $ (addCest(i) $ (p_sigmam(i) ne inf))
      = [   p_ad(i,%tt%)*(pda.l(i,%tt%)/p_gammaMd(i,%tt%))**(1-p_sigmam(i))
          + p_am(i,%tt%)*(pma.l(i,%tt%)/p_gammaMm(i,%tt%))**(1-p_sigmam(i))] * paInd.l(i,%tt%)**p_sigmam(i);

     paf.l(iAct,aaIn,%tt%) $ sam(iAct,aain)
       = [(1 + tPaf.l(iact,aaIn,%tt%)) * pa.l(iAct,%tt%)
                     +   {sum(emiCommFlag(em,emSrc(iAct),aain),
                                   tEmi.l(em,aaIn,%tt%) * emical.l(%tt%)* chiEmi.l(em,emSrc,aaIn,%tt%)
                                 * p_rhoEmi(em,emSrc,aaIn) * p_phiEmi(em,emSrc,aaIn))/p_chiPA(iAct)} $ hasEmiT ];
     rgdppc.l(%tt%)   = rgdpmp.l(%tt%)/pop.l("ptotl",%tt%);


  $$elseifi.arg  "%1"=="vintages"
*
*    --- initialize old capital and capital use of old vintage
*
     $$ondotl
     oldCap.l(aact,%tt%) = sum((cap,vp),xfac.l(cap,aact,vp,%tt1%))*(1-deprA.l(aact,%tt%)*sum(sameas(t,%tt%),m_dmgAgg(dmgDepr,'aact')));
     $$offdotl
     $$ifi "%ForwardLooking%"=="OFF" oldCap.fx(aact,%tt%) = oldCap.l(aact,%tt%);

     rrat1.l(a,%tt%) $ oldCap.l(a,%tt%) = max(0,sum((cap,v),xFac.l(cap,a,v,%tt1%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
     rrat.l(a,%tt%)  $ oldCap.l(a,%tt%)= ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
     xfac.l(cap,aact,old,%tt%) $ ((not ifEnTech) $ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);

     rrat1.l(a,%tt%) $ oldCap.l(a,%tt%)= max(0,sum((cap,v),xFac.l(cap,a,v,%tt%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
     rrat.l(a,%tt%)  $ oldCap.l(a,%tt%)= ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
     xfac.l(cap,aact,old,%tt%) $ ((not ifEnTech) $ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);

     pxfac_preTax.l(cap,aact,old,%tt%) $ oldCap.l(aact,%tt%) = rrat.l(aact,%tt%)**(1/p_invElas(aact)) * trent.l(%tt%);
     pxfac.l(cap,aact,old,%tt%)        $ oldCap.l(aact,%tt%) = pxfac_preTax.l(cap,aact,old,%tt%) * (1 + tFac.l(aact,cap,%tt%));

  $$elseifi.arg  "%1"=="xfac"

     $$iftheni.baseline "%task%"=="baseline"


        labProd.l(ts(t)) $ labProd.l(t-2)
           = max(labProd.l(t-1)-min(0.20,abs(labProd.l(t-1)-labProd.l(t-2))),
             min(labProd.l(t-1)+min(0.20,abs(labProd.l(t-1)-labProd.l(t-2))),
                 labProd.l(t-1) + (labProd.l(t-1)-labProd.l(t-2))/(rgdppc.l(t-1)/rgdppc.l(t-2))*(rgdppc.l(t)/rgdppc.l(t-1))));


        chixFacFA.l(lFpNest(fpNest,aact(a)),ts(t)) $ (sum(flagXFac(fpNest,aact,v),1) $ ifLabProdDiff)
          = -ncpVuPow[-labProd.l(t),-labProd.l(t)*{   p_prodShift("a",a)
                                                    + p_prodShift("b",a) *    [rgdpmp.l(t)/rgdpmp.l(t-1) -1]
                                                    + p_prodShift("c",a) * sqr[rgdpmp.l(t)/rgdpmp.l(t-1) -1] }] $ (p_prodShift("a",a)+ p_prodShift("b",a)+ p_prodShift("c",a))
       + chixFacFA.l(fpNest,a,t-1);

        chixFacF.l(natRes,ts(t)) $ ifNatresProdAsLabor
          = 1 - %NatresProdAsLabor%*ncpVuPow[0,-((labProd.l(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                               + ChixFacL.l(t) $ (not ifLabProdDiff))];
        chixFacF.l(actLnd,ts(t)) $ ifLandProdAsLabor
           = 1 - %LandProdAsLabor%*ncpVuPow[0,-((labProd.l(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                             + ChixFacL.l(t) $ (not ifLabProdDiff))];
        chixFacF.l(cap,ts(t)) $ ifCapProdAsLabor
           = 1 - %capProdAsLabor%*ncpVuPow[0,-((labProd.l(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                             + ChixFacL.l(t) $ (not ifLabProdDiff))];

        lambdaXfac.l(flagxFac(actFpNest,aact,v),ts(t)) $ ((lambdaXfac.range(actFpNest,aact,v,t) ne 0) $ (lambdaXfac.scale(actFpNest,aact,v,t) ne 999))
         =   [  chixFacF.l(actFpNest,t) * chixFacA.l(aact,t) * chixFacV.l(v,t)
              * chixFacFA.l(actFpNest,aact,t)
*       --- air pollution impact on labor productivity
        $$ondotl
                      * (1 + dmgAir(actFpNest,aact,t) $ ifAirPollDmg $ l(actFpNest))
*       --- shifter for all labor types
                      * (1 + (chixFacL.l(t)) $ l(actFpNest))
*       --- climate damages
                      *  m_dmgAgg(dmgPrd,'actFpNest,aact')
        $$offdotl
*       --- shifter for capital
                      * (1 + chixFacK.l(t)   $  cap(actFpNest))
*       --- shifter for capital and labor
                      * (1 + chixFacKL.l(t)  $ (cap(actFpNest) or l(actFpNest)))
*       --- shifter for natural factors (land,labor,capital etc.)
                      * (1 + chixFacFP.l(t)  $ fp(actFpNest))
*       --- shifter for sector groups
                      * sum(mapAagr(aagr,aact),chixFacAgr.l(aagr,actFpNest,t))
*       --- shifter for energy carriers
                      * (1 + chixFacNRG.l(t) $ nrgFac(actFpNest)) ];
     $$endif.baseline

     pxfac.l(flagxFac(fpNestI,a,old),ts(t))    = sum(sameas(fpNestI,i),p_chipa(i)*paf.l(i,a,t));
     toDo(aact,fpPar) = yes;
     while(card(toDo),
        option kill=cur;
        cur(toDo(aact,fpPar)) $ ( (not sum(toDo1(aact,actfPNest), pnest(aact,actfPNest,fpPar)))) = YES;
        curParNest(FpPar)     = sum(cur(aact,fpPar),1);
        curChlNest(actfPNest) = sum(cur(aact,curParNest), pnest(aact,actfPNest,curParNest));

        pxfac.l(flagXfac(curParNest,aact,v),ts(t)) $  (((not fpIdent(curParNest,aact)) or old(v)) $ cur(aact,curParNest)
                                                         $ (not isadditiveCes(curParNest)))
          =  sum(curChlNest $ p_alphaXfac(curChlNest,curParNest,aact,v),p_alphaXfac(curChlNest,curParNest,aact,v)
                      *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))
                      *([  pxfac.l(curChlNest,aact,v,t)   $  (not fpIdent(curChlNest,aact))
                         + pxfac.l(curChlNest,aact,old,t) $       fpIdent(curChlNest,aact) ]
                         /lambdaXfac.l(curChlNest,aact,v,t))**(1 - p_sigmaXFac(curParNest,aact,v)))**(1/(1 - p_sigmaXFac(curParNest,aact,v)))
                      *  1  / (1 $ (not fpTop(curParNest)) + lambdaXfac.l(curParNest,aact,v,t) $ fpTop(curParNest));

        pxfacInd.l(flagXfac(curParNest,aact,v),ts(t)) $  (((not fpIdent(curParNest,aact)) or old(v)) $ cur(aact,curParNest)
                                                              $ (isadditiveCes(curParNest)) )
          =  [sum(curChlNest $ p_alphaXfac(curChlNest,curParNest,aact,v),p_alphaXfac(curChlNest,curParNest,aact,v)
                             *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))
                      *([  pxfac.l(curChlNest,aact,v,t)   $  (not fpIdent(curChlNest,aact))
                         + pxfac.l(curChlNest,aact,old,t) $       fpIdent(curChlNest,aact) ]
                         /lambdaXfac.l(curChlNest,aact,v,t))**(-p_sigmaXFac(curParNest,aact,v)))**(1/(-p_sigmaXFac(curParNest,aact,v)))]  $ p_sigmaXFac(curParNest,aact,v)

              + sum(curChlNest $ p_alphaXfac(curChlNest,curParNest,aact,v),p_alphaXfac(curChlNest,curParNest,aact,v)
                            *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))
                 *([  pxfac.l(curChlNest,aact,v,t)   $  (not fpIdent(curChlNest,aact))
                    + pxfac.l(curChlNest,aact,old,t) $       fpIdent(curChlNest,aact) ]
                    /lambdaXfac.l(curChlNest,aact,v,t))) $ (not p_sigmaXFac(curParNest,aact,v));



        pxfac.l(flagXfac(curParNest,aact,v),ts(t)) $  (((not fpIdent(curParNest,aact)) or old(v)) $ cur(aact,curParNest) $ (isadditiveCes(curParNest)))
        = sum(curChlNest $ p_alphaXfac(curChlNest,curParNest,aact,v),
           p_alphaXfac(curChlNest,curParNest,aact,v)
                        *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))

                *[   pxfac.l(curChlNest,aact,v,t)   $ (not fpIdent(curChlNest,aact))
                   + pxfac.l(curChlNest,aact,old,t) $      fpIdent(curChlNest,aact)]**(1-p_sigmaXFac(curParNest,aact,v))

               /lambdaXfac.l(curChlNest,aact,v,t)**p_sigmaXFac(curParNest,aact,v)) * pxFacInd.l(curParNest,aact,v,t)**p_sigmaXFac(curParNest,aact,v);


         toDo(cur) = no;
      );

      toDo(aact,fpChl) $ sum(fpNest,pnest(aact,fpChl,fpNest))   = yes;
      toDo(aact,"f-top") = no;

      while(card(toDo),
         option kill=cur;
         cur(toDo(aact,fpChl)) $ (sum(actFpNest $ (not toDo(aact,actFpNest)), pnest(aact,fpChl,actFpNest)) eq sum(actFpNest, pnest(aact,fpChl,actFpNest))) = yes;

         curChlNest(actFpNest) = sum(cur(aact,actFpNest),1);
         curParNest(actFpNest) = sum(cur(aact,CurChlNest),pnest(aact,curChlNest,actFpNest));

         xfac.l(flagXfac(curChlNest,aact,v),ts(t)) $ ((not leontief(aact,curChlNest)) $ cur(aact,curChlNest))
           =  sum(curParNest $ p_alphaXfac(curChlNest,curParNest,aact,v),  p_alphaXfac(curChlNest,curParNest,aact,v)
                          *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))
                     *xfac.l(curParNest,aact,v,t)
                       *(lambdaXfac.l(curChlNest,aact,v,t) * (1 $ (not fpTop(curParNest)) + lambdaXfac.l(curParNest,aact,v,t) $ fpTop(curParNest)))
                              **[ (p_sigmaXFac(curParNest,aact,v)-1) $ (not isAdditiveCes(curParNest)) - p_sigmaXFac(curParNest,aact,v) $ isAdditiveCes(curParNest) ]
*
*                       --- if substitution elasticities are identical (and thus the share parameters), use the idential composite price for curParNest
*                          found on old
*
                        * (  [  (pxfac.l(curParNest,aact,v,t)    $ (not fpIdent(curParNest,aact)) + pxFac.l(curParNest,aact,old,t)    $ fpIdent(curParNest,aact)) $ (not isAdditiveCes(curParNest))
                              + (pxfacInd.l(curParNest,aact,v,t) $ (not fpIdent(curParNest,aact)) + pxFacInd.l(curParNest,aact,old,t) $ fpIdent(curParNest,aact)) $      isAdditiveCes(curParNest) ]
*                         --- note that for primary factors, intermediates and
                          /[ pxfac.l(curChlNest,aact,v,t)   $ (not  fpIdent(curChlNest,aact))
                           + pxfac.l(curChlNest,aact,old,t) $       fpIdent(curChlNest,aact)  ])**p_sigmaXFac(curParNest,aact,v));

         xfac.l(flagXfac(curChlNest,aact,v),ts(t)) $ (leontief(aact,curChlNest) $ cur(aact,curChlNest))
           =  sum(curParNest $ p_alphaXfac(curChlNest,curParNest,aact,v),  p_alphaXfac(curChlNest,curParNest,aact,v)
                       *(1+twist.l(curChlNest,curParNest,aact,v,t) $ fpParTc(curParNest))
                       *xfac.l(curParNest,aact,v,t)
                       *(lambdaXfac.l(curChlNest,aact,v,t) * (1 $ (not fpTop(curParNest)) + lambdaXfac.l(curParNest,aact,v,t) $ fpTop(curParNest)))**[-1 $ (not isAdditiveCes(curParNest))]);
           toDo(cur) = no;
     );

  $$else.arg

     if (ifVint,
*
*       --- initialize old capital and capital use of old vintage
*

        rrat1.l(a,%tt%) $ oldCap.l(a,%tt%)= max(0,sum((cap,v),xFac.l(cap,a,v,%tt%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
        rrat.l(a,%tt%)  $ oldCap.l(a,%tt%) = ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
        xfac.l(cap,aact,old,%tt%) $ ((not ifEntech) $ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);

        rrat1.l(a,%tt%) $ oldCap.l(a,%tt%)= max(0,sum((cap,v),xFac.l(cap,a,v,%tt%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
        rrat.l(a,%tt%) $ oldCap.l(a,%tt%) = ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
        xfac.l(cap,aact,old,%tt%) $ ((not ifEntech) $ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);

        pxfac_preTax.l(cap,aact,old,%tt%) $ oldCap.l(aact,%tt%) = rrat.l(aact,%tt%)**(1/p_invElas(aact)) * trent.l(%tt%);

        if (execError,abort rrat.l,pxFac_pretax.l,oldCap.l;);

        pxfac.l(cap,aact,old,%tt%) $ oldCap.l(aact,%tt%) = pxfac_preTax.l(cap,aact,old,%tt%) * (1 + tFac.l(aact,cap,%tt%))
                                  + sum(em $ p_rhoEmiFac(em,cap,aact),tEmi.l(em,aact,%tt%)*emical.l(%tt%)*chiEmiFac.l(em,cap,aact,%tt%)
                                                                      *p_rhoEmiFac(em,cap,aact)*p_phiEmiFac(em,cap,aact)) $ hasEmit;
     );

     if (ifSectSpecInv $ (not ifEntech),
*
*       --- define (proxy) for new investments from capital use of new vintage plus 1% of old one
*
        newInv.l(%tt%) = sum((cap,aact), xfac.l(cap,aact,new,%tt%)+p_oldCapInvShare(aact)*xfac.l(cap,aact,old,%tt%));
        newInv.l(t0)   $ (not newInv.l(t0))   = newInv.l(%tt%);
*
*       --- define investment price, activity specific
*
        invPrice.l(aact,%tt%)  = sum(iAct $ gammaInv.l(iAct,aact,t0),gammaInv.l(iAct,aact,t0) * sum(inv,p_chiPA(iAct)*paf.l(iAct,inv,%tt%))**(1-p_sigmaInv(aact)))**(1/(1-p_sigmaInv(aact)));
*
*       --- investment shares
*
        gammaInv.l(iAct,aact,%tt%) $ p_gammaInv0(iact,aact) = p_gammaInv0(iact,aact) * (invPrice.l(aact,%tt%)/sum(inv,p_chiPA(iact)*paf.l(iact,inv,%tt%)))**p_sigmaInv(aact);
*
*       --- define investment shifter in Leontief bundle from shares
*

        lambdaOa.l(inv,iact,"top",%tt%) $ (p_af(inv,iact,"top",%tt%) and (lambdaoa.range(inv,iact,"top",%tt%) ne 0))
           = [ sum((cap,aact) $ gammaInv.l(iact,aact,t0), (xfac.l(cap,aact,new,%tt%)+p_oldCapInvShare(aact)*xfac.l(cap,aact,old,%tt%)) * gammaInv.l(iact,aact,%tt%) )
                  /newInv.l(%tt%)]/(p_af(inv,iact,"top",t0)*p_lambdaInv0(iact));


        gammaInv.fx(iact,aact,%tt%) $ (not gammaInv.l(iact,aact,t0)) = 0;

     );

*
*    --- define recursively many variables to reduce # and sum of infeasibilities
*
     $$ondotl
     depr.l(t(%tt%)) = sum(flagXFac(cap,aact,v),xFac.l(cap,aact,v,t)*deprA.l(aact,t)*m_dmgAgg(dmgDepr,'aact'))/sum(flagXFac(cap,aact,v),xFac.l(cap,aact,v,t));

     rorc.l(%tt%) = -ncpVuPow(-1.E-10,-[trent.l(%tt%)/sum(inv, pf.l(inv,%tt%)) - depr.l(%tt%)],1.E-7);

     prodInv.l(%tt%)
      = sum(inv,
            + xfd.l(inv,%tt%)
            + p_phiPcInv(inv,%tt%) * sum(ginv, xfd.l(ginv,%tt%))
            - xfdRD.l(inv,%tt%) $ p_chifdRD(inv,%tt%)
            - p_nonProdInv(inv,%tt%))
      ;

     grK.l(%tt%)    = prodInv.l(%tt%)/kstock.l(%tt%) - depr.l(%tt%);

     kstock.l(t(%tt%)) = prod(cap,m_dmgAgg(dmgStk,'cap')) * (1-depr(%tt%))*kstock.l(%tt1%) + prodInv.l(%tt%);
     kaps.l(t(%tt%))  =  prod(cap,m_dmgAgg(dmgSup,'cap')) * ror.l(t)*kstock.l(%tt%);

     xfmSupUpdateFac.l(fmp,instTot,%tt%) $ sum(sameas(fmp,cap),xfmSup.l(fmp,fmp,instTot,oldf,%tt1%))
       =  kaps(%tt%)/kaps.l(t0) /sum(sameas(fmp,cap) $ xfmSup.l(fmp,fmp,instTot,oldf,t0),
               atFmSup(fmp,instTot,oldf,%tt%)/atFmSup.l(fmp,instTot,oldf,t0));

     $$offdotl
     kstocke.l(%tt%) = (1-depr.l(%tt%))*kstock.l(%tt%) + prodInv.l(%tt%);

     rore.l(%tt%) = max[1.E-6,
          $$iftheni.forwardLooking "%ForwardLooking%"=="on"

             sum(tNext $ (years(tNext) ge years(%tt%)), p_exptWgts(tNext)
                     *(rorc.l(tNext) *(kstocke.l(tNext)/kstock.l(tNext))**(epsRorE.l(%tt%))) $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) )))
             /sum(tNext $ (years(tNext) ge years(%tt%)), p_exptWgts(tNext))

           $$else.forwardLooking
                     (rorc.l(%tt%)*(kstocke.l(%tt%)/kstock.l(%tt%))**(epsRorE.l(%tt%)))       $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) ))
           $$endif.forwardLooking
                  +  ((1+rorc.l(%tt%))/(1+interest.l(%tt%)/100) - 1)          $      ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) )
          ];

     xfmSup.l(fmp,fmp,inst,oldf,%tt%) $ (sum(sameas(fmp,cap),1) $ xfmSup.l(fmp,fmp,inst,oldf,%tt%) )
        = xfmSup.l(fmp,fmp,inst,oldf,%tt%) * kaps.l(%tt%)
           / sum(instp, xfmSupUpdateFac.l(fmp,instp,%tt%)*xfmSup.l(fmp,fmp,instp,oldf,%tt%));

     interest.l(%tt%) = min(100,interest.l(%tt%));

     fundPriceD.l(%tt%)
       =  (        p_aFundG(%tt%) * chiFundG.l(%tt%)  * (interest.l(%tt%)*pgdpmp.l(%tt%)/interest.l(t0))**(p_omegaFund(%tt%))
            +   (1-p_aFundG(%tt%) * chiFundG.l(%tt%)) * (rore.l(%tt%)    *pgdpmp.l(%tt%)/rore.l(t0)    )**(p_omegaFund(%tt%)))**(1/(p_omegaFund(%tt%)));

     fundPrice.l(%tt%) = [  fundG.l(%tt%) * (interest.l(%tt%)/(interest.l(t0)))
                           + fundP.l(%tt%) * (rore.l(%tt%)/rore.l(t0))]/fundT.l(%tt%);

     fundF.l(%tt%) = [1.E-12+p_gammaFundF(%tt%) * (interest.l(%tt%)*sum(instf,riskPrem.l(instf,%tt%)))**p_sigmaFInt
                               * pgdpmp.l(%tt%)
        * ( 1 + ncpVuPow(0,-{sigmoid([debtGdp.l(%tt%)-0.5]*4)-0.5}*2,1.E-7))* rgdpmp.l(%tt%)/rgdpmp.l(t0)];

     facty.l(actFp,%tt%) = sum(flagXFac(actFp,a,v), pxfac_preTax.l(actFp,a,old,%tt%)*xfac.l(actFp,a,v,%tt%));

     entrY.l(entr,%tt%) =
                                sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(entr,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0)),
                                 xfmSupUpdateFac.l(actFmp,inst,%tt%)*xfmSup.l(actFmp,actFmp,inst,vf,%tt%)*pfmSup.l(actFmp,actFmp,inst,vf,%tt%)*corrFac.l(actFp,%tt%))
                             +  sum((actFp,mapInst(entr,inst)), facty.l(actFp,%tt%)*p_chiFac(inst,actFp,%tt%))
                             +  sum((mapinst(entr,instp),inst), transfers.l(instp, inst,%tt%))
                             +  sum(mapinst(entr,inst),debtPay.l(inst,%tt%))
                             +  sum((fptop,v,a)$(aact(a)), p_markup(a,%tt%)*xfac.l(fptop,a,v,%tt%)*pxfac.l(fptop,a,v,%tt%))
                             -  sum(f $ p_chifdRD(f,%tt%), xfdRD.l(f,%tt%)*pf.l(f,%tt%)) - sum(i $ p_chifdRD(i,%tt%), xfdRD.l(i,%tt%)*pa.l(i,%tt%));

     savEntr.l(entr,%tt%) = chims.l(%tt%) * entrY.l(entr,%tt%) * (1- tCit.l(entr,%tt%)) * p_aSavEntr(entr,%tt%) * fundPrice.l(%tt%)**p_omegaSavEntr(entr);

     totTr.l(inst,%tt%) $ sum(mapinst(entr,inst),1)
       = sum(mapinst(entr,inst),entrY.l(entr,%tt%) * (1- tCit.l(entr,%tt%)) - savEntr.l(entr,%tt%));

     transfers.l(inst,dinst,%tt%) = chiTr.l(inst,dinst,%tt%) * totTr.l(dinst,%tt%);

     xa.l(iact,aact,%tt%) $ sam(iact,aact)= sum(flagXFac(fpNestI,aact,v) $ sameas(fpNestI,iact),xfac.l(fpNestI,aact,v,%tt%));
*
*    -- update debt stocks

     debtPrincipal.l(instfd,%tt%) = rDebtStkT.l(%tt%) * rDebtStk.l(instfd,%tt%) * DebtStk.l(instfd,%tt1%);
*
     DebtStk.l(instfd,%tt%)    = DebtStk.l(instfd,%tt1%) + debtNew.l(instfd,%tt%) - debtPrincipal.l(instfd,%tt%);

     avgDebtInt.l(instfd,%tt%) $ sum(t1 $ [((t1.val lt %tt%.val)) $ tt(t1)], max(0,(debtNew.l(instfd,t1)+1.E-10)
                                   *(1 - rDebtStk.l(instfd,t1)*(%tt%.val-t1.val))))
     = sum(t1 $ [((t1.val lt %tt%.val)) $ tt(t1)], max(0,(debtNew.l(instfd,t1)+1.E-10)
                                   *(1 - rDebtStk.l(instfd,t1)*(%tt%.val-t1.val))) * interest.l(t1)*riskPrem.l(instfd,t1))
                                /  sum(t1 $ [((t1.val lt %tt%.val)) $ tt(t1)], max(0,(debtNew.l(instfd,t1)+1.E-10)
                                   *(1 - rDebtStk.l(instfd,t1)*(%tt%.val-t1.val))));

     avgDebtInt.fx(instfd,%tt%) $ (not sum(t1 $ [((t1.val lt %tt%.val)) $ tt(t1)], max(0,(debtNew.l(instfd,t1)+1.E-10)
                                   *(1 - rDebtStk.l(instfd,t1)*(%tt%.val-t1.val))))) = 0;

     debtInterest.l(instfd,%tt%)
        = ((DebtStk.l(instfd,%tt%)-debtNew.l(instfd,%tt%))*avgDebtInt.l(instfd,%tt%)/100);

     debtPay.l(instfd,%tt%) = debtInterest.l(instfd,%tt%) + debtPrincipal.l(instfd,%tt%);

     yh.l(h,%tt%) =    sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(h,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0)),
                          xfmSupUpdateFac.l(actFmp,inst,%tt%)*xfmSup.l(actFmp,actFmp,inst,vf,%tt%)
                         *pfmSup.l(actFmp,actFmp,inst,vf,%tt%)*corrFac.l(actFp,%tt%))
                     +  sum((actFp,mapInst(h,inst)), facty.l(actFp,%tt%)*p_chiFac(inst,actFp,%tt%))
                     +  sum((mapinst(h,instp),inst), transfers.l(instp, inst,%tt%))
                     +  SUM(mapinst(h,inst),debtPay.l(inst,%tt%));


     gini.l(%tt%) = 2 * sum(h,ord(h) * yh.l(h,%tt%))/(card(h)*sum(h,yh.l(h,%tt%))) - (card(h)+1)/card(h);

     yhDsp.l(h,%tt%) = yh.l(h,%tt%) *(1-tPIT.l(h,%tt%)) - sum(mapinst(h,inst), totTr.l(inst,%tt%));

     pindH.l(h,%tt%)
      =        p_aSav(h,%tt%) * (chims.l(%tt%) $ (p_asav(h,%tt%) gt 0) + (1/chims.l(%tt%)) $ (p_asav(h,%tt%) le 0))   * (fundPrice.l(%tt%)*pgdpmp.l(%tt%)) ** (sign(p_aSav(h,%tt%))*p_omegaSav(h))
           +(1-p_aSav(h,%tt%) * (chims.l(%tt%) $ (p_asav(h,%tt%) gt 0) + (1/chims.l(%tt%)) $ (p_asav(h,%tt%) le 0)) ) * pf.l(h,%tt%)                       ** (sign(p_aSav(h,%tt%))*p_omegaSav(h)) **(1/(sign(p_aSav(h,%tt%))*p_omegaSav(h)));


     savh.l(h,%tt%) = (chims.l(%tt%) $ (p_asav(h,%tt%) gt 0) + (1/chims.l(%tt%)) $ (p_asav(h,%tt%) le 0)) * yhDsp.l(h,%tt%) * p_aSav(h,%tt%)
                          * (fundPrice.l(%tt%)*pgdpmp.l(%tt%)/pindH.l(h,%tt%))**(sign(p_aSav(h,%tt%))*p_omegaSav(h));

     aps.l(h,%tt%) = savh.l(h,%tt%)/yhDsp.l(h,%tt%);

     fundT.l(%tt%) = sum(h,savh.l(h,%tt%))+ sum(entr,saventr.l(entr,%tt%));

     fundG.l(%tt%) = FundT.l(%tt%) * (-1.E-8+p_aFundG(%tt%) * chiFundG.l(%tt%) * (interest.l(%tt%)*pgdpmp.l(%tt%)/[fundPriceD.l(%tt%)*interest.l(t0)])**p_omegaFund(%tt%));
     fundP.l(%tt%) = FundT.l(%tt%) - fundG.l(%tt%);

     debtNew.l(inst,%tt%) =     fundG.l(%tt%)*p_chiDebtNew(inst,%tt%) $ instD(inst)
                              + fundF.l(%tt%)                         $ instF(inst);

     xfd.l(f,%tt%)$ ((ginv(f) or gov(f)) $  xfd.l(f,t0)) = [xfd.l(f,t0)/rgdpmp.l(t0) + p_relExpInc/100*fiscalRespDrv.l(%tt%) $ p_relExpInc]*rgdpmp.l(%tt%);
     yf.l(f,%tt%) $ ((ginv(f) or gov(f)) $  xfd.l(f,t0)) = xfd.l(f,%tt%)*pf.l(f,%tt%);

     primaryBalance.l(%tt%) =    ygov.l(%tt%)
                                 - gtax.l(%tt%)
                                 - sum(mapinst(gov,inst), totTr.l(inst,%tt%))
                                 - yf.l(gov,%tt%)
                                 - sum(ginv,yf.l(ginv,%tt%))
                                 - sum(map_adpInv(gov,adpinv),yf.l(adpinv,%tt%));

     fiscalBalance.l(%tt%) = primaryBalance.l(%tt%) - sum(instfd,debtInterest.l(instfd,%tt%));
     govBalance.l(%tt%)    = fiscalBalance.l(%tt%)  - sum(instfd,debtPrincipal.l(instfd,%tt%));

     savf.l(%tt%)    =  -(bot.l(%tt%)
                               + sum((inst,mapInst(row,instp)),transfers.l(inst,instp,%tt%)- transfers.l(instp,inst,%tt%))
                               - sum(instf, debtPay.l(instf,%tt%)))/er.l(%tt%);


     yf.l(inv,%tt%) = max[yf.l(inv,t0)*0.01,
*         --- part of enterprise and household savings not used for new domestic debt
          fundp.l(%tt%)
*         --- add a positive government deficit (smooth max)
          - ncpvusin(0,-govBalance.l(%tt%)-max(1.E-7,1.E-7*ygov.l(%tt1%)),max(1.E-7,1.E-7*abs(ygov.l(%tt1%))))
*
*         --- difference between total foreign savings and new foreign debt
          +  er.l(%tt%)*savf.l(%tt%) - fundf.l(%tt%)
*         --- nominal total stock changes domestic and foreign
          - tdelst.l(%tt%)
*         --- private adaptive investments
          -  sum(map_adpInv(inv,adpinv),yf.l(adpinv,%tt%))
*         --- R&D investments
          + xfdRD.l(inv,%tt%)*pf.l(inv,%tt%) $ p_chifdRD(inv,%tt%)
          ];


     if (ifSectSpecInv,
        $$ondotl
*
*       --- define average prie of investments, and total demand index
*
        pxak(inv,"top",t(%tt%))
         = sum(iAct $ p_af(inv,iAct,"top",%tt%),
                      p_af(inv,iAct,"top",%tt%)*p_chiPA(iAct)*paf(iAct,inv,tt)
                                    /(sum(sameas(t,%tt%),m_dmgAgg(dmgDem,'inv,iAct,"top"') * (lambdaOa(inv,iAct,"top",tt)*(1+lambdaNRG(%tt%)$e(iAct))))));
        pf(inv,%tt%) = pxak(inv,"top",%tt%);

        xfd.l(inv,%tt%)     = yf.l(inv,%tt%)/pf.l(inv,%tt%);
        xak(inv,"top",%tt%) = xfd(inv,%tt%);
*
*       --- define investment demands
*
        xa(iAct,inv,t(%tt%)) $ xa.l(iAct,inv,t0)
         =  p_af(inv,iACt,"top",%tt%)*xak.l(inv,"top",tt)
                *     1/[sum(sameas(t,%tt%),m_dmgAgg(dmgDem,'inv,iAct,"top"'))*lambdaOa(inv,iAct,"top",tt)*(1+lambdaNRG(%tt%)$e(iAct))];

        $$offdotl
     );

     debtNewT.l(%tt%) = sum(instfd $ p_chiDebtNew(instfd,%tt%),debtNew.l(instfd,%tt%));

     hpop.l(h,%tt%) = p_hpop0(h)/p_pop0("ptotl")*pop.l("ptotl",%tt%);


     yf.l(h,%tt%) = yhDsp.l(h,%tt%) - savh.l(h,%tt%);
     nshrGDP.l(h,%tt%) = yf.l(h,%tt%)/gdpmp.l(%tt%);

     ygPit.l(h,%tt%)  = tPit.l(h,%tt%) * yh.l(h,%tt%);
     yg.l("Pit",%tt%) = sum(h,ygPit.l(h,%tt%));

     ygCit.l(entr,%tt%)  = tCit.l(entr,%tt%) * entry.l(entr,%tt%);
     yg.l("Cit",%tt%) = sum(entr,ygCit.l(entr,%tt%));

     rgdppc.l(%tt%) = rgdpmp.l(%tt%)/pop.l("ptotl",%tt%);
     ggdppc.l(%tt%) = rgdppc.l(%tt%)/rgdppc.l(%tt1%) - 1;

     $$ondotl
     pxak.l(oa,knest,t(%tt%)) $ xakact(oa,knest)
       = {sum(i $ p_af(oa,i,knest,%tt%),
                  p_af(oa,i,knest,%tt%)*(p_chiPA(i)*paf(i,oa,%tt%)
                                 /(m_dmgAgg(dmgDem,'oa,i,knest') * (lambdaOa(oa,i,knest,%tt%)*(1+lambdaNRG(%tt%)$e(i)))))**(1-p_sigmac(oa,knest)))
        + sum(knest1 $ p_afk(oa,knest1,knest,%tt%), p_afk(oa,knest1,knest,%tt%)*pxak(oa,knest1,%tt%)**(1-p_sigmac(oa,knest)))}**(1/(1-p_sigmac(oa,knest)));

     pxak.l(oa,knest,t(%tt%)) $ xakact(oa,knest)
       = {sum(i $ p_af(oa,i,knest,%tt%),
                  p_af(oa,i,knest,%tt%)*(p_chiPA(i)*paf(i,oa,%tt%)
                                 /(m_dmgAgg(dmgDem,'oa,i,knest') * (lambdaOa(oa,i,knest,%tt%)*(1+lambdaNRG(%tt%)$e(i)))))**(1-p_sigmac(oa,knest)))
        + sum(knest1 $ p_afk(oa,knest1,knest,%tt%), p_afk(oa,knest1,knest,%tt%)*pxak(oa,knest1,%tt%)**(1-p_sigmac(oa,knest)))}**(1/(1-p_sigmac(oa,knest)));

     pxak.l(oa,knest,t(%tt%)) $ xakact(oa,knest)
       = {sum(i $ p_af(oa,i,knest,%tt%),
                  p_af(oa,i,knest,%tt%)*(p_chiPA(i)*paf(i,oa,%tt%)
                                 /(m_dmgAgg(dmgDem,'oa,i,knest') * (lambdaOa(oa,i,knest,%tt%)*(1+lambdaNRG(%tt%)$e(i)))))**(1-p_sigmac(oa,knest)))
        + sum(knest1 $ p_afk(oa,knest1,knest,%tt%), p_afk(oa,knest1,knest,%tt%)*pxak(oa,knest1,%tt%)**(1-p_sigmac(oa,knest)))}**(1/(1-p_sigmac(oa,knest)));
     $$offdotl
     for(updateLoop = 1 to 9,
        cdeInt.l(h,topKNest,%tt%) $ akfact(h,topkNest)
          =   p_akf(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)
            * (uh.l(h,%tt%)**(eh.l(h,topKNest,%tt%)*bh.l(h,topKNest,%tt%)))
            * (pxak.l(h,topKNest,%tt%)**(bh.l(h,topKNest,%tt%)))
            * ((yf.l(h,%tt%)/hpop.l(h,%tt%))**(-bh.l(h,topKNest,%tt%)));
        uh.l(h,%tt%) = uh.l(h,%tt%) / sum(akfact(h,topKNest),cdeInt.l(h,topKNest,%tt%)/bh.l(h,topKNest,%tt%));
     );

     if ( execError, abort "Run time error in file: %system.fn%, line: %system.incline%",pxak.l,yf.l,uh.l,cdeint.l,bh.l,eh.l;);

     consScale.l(h,%tt%) = sum(akfact(h,topKNest),cdeInt.l(h,topKNest,%tt%));

     mmuh.l(h,%tt%) = mmuh.l(h,%tt1%) * rgdppc.l(%tt%)/rgdppc.l(%tt1%);

     xak.l(akFact(h,topKNest),%tt%)
      = yf.l(h,%tt%)/pxak.l(h,topKNest,%tt%)*cdeInt.l(h,topKnest,%tt%)/consScale.l(h,%tt%);

     xak.l(oa,kNest,%tt%) $ conscont(oa,kNest)
      =   sum(kNest1 $ p_afk(oa,kNest,kNest1,%tt%),
                   p_afk(oa,kNest,kNest1,%tt%)*xak.l(oa,kNest1,%tt%)*[pxak.l(oa,kNest1,%tt%)/pxak.l(oa,kNest,%tt%)]**p_sigmac(oa,kNest1));

     xak.l(oa,kNest,%tt%) $ conscont(oa,kNest)
      =   sum(kNest1 $ p_afk(oa,kNest,kNest1,%tt%),
                   p_afk(oa,kNest,kNest1,%tt%)*xak.l(oa,kNest1,%tt%)*[pxak.l(oa,kNest1,%tt%)/pxak.l(oa,kNest,%tt%)]**p_sigmac(oa,kNest1));

     xak.l(oa,kNest,%tt%) $ conscont(oa,kNest)
      =   sum(kNest1 $ p_afk(oa,kNest,kNest1,%tt%),
                   p_afk(oa,kNest,kNest1,%tt%)*xak.l(oa,kNest1,%tt%)*[pxak.l(oa,kNest1,%tt%)/pxak.l(oa,kNest,%tt%)]**p_sigmac(oa,kNest1));

     xakshr.l(xakAct(oa,topKNest),%tt%) $ xak.l(oa,topKNest,%tt%) = pxak.l(oa,topKNest,%tt%)*xak.l(oa,topKNest,%tt%)/yf.l(oa,%tt%);

     $$ondotl
     xa.l(iact,oa,%tt%) $ xaact(iact,oa)
      = sum{map_i_knest(iact,knest) $ p_af(oa,iact,knest,%tt%),
                     p_af(oa,iact,knest,%tt%)*xak.l(oa,knest,%tt%)
                *     sum[sameas(t,%tt%),m_dmgAgg(dmgDem,'oa,iact,knest')*lambdaOa(oa,iact,knest,%tt%)*(1+lambdaNRG(%tt%)$e(iact))]**(p_sigmac(oa,knest)-1)
                *     [ pxak.l(oa,knest,%tt%)/(p_chiPA(iact)*paf.l(iact,oa,%tt%))]**p_sigmac(oa,knest)};
     $$offdotl


     pf.l(h,%tt%) = sum(iAct $ sam(iAct,h), xa.l(iAct,h,%tt%)*paf.l(iAct,h,%tt%))/sum(iAct $ sam(iAct,h), xa.l(iAct,h,%tt%)*p_pafini(iACt,h));

     xfd.l(h,%tt%) = yf.l(h,%tt%)/pf.l(h,%tt%);


     for(updateLoop = 1 to 4,

         xat.l(i,tt) = sum(aa $ sam(i,aa), xa.l(i,aa,tt));
         xm.l(iImp(i),%tt%) $ (p_sigmam(i) ne inf)
           = p_am(i,%tt%)*p_gammamm(i,%tt%)**[ -p_sigmam(i) $ addCest(i) + (p_sigmam(i)-1) $ (not addCest(i))]
               * xat.l(i,%tt%)*((paIND.l(i,%tt%) $ addCest(i) + pa.l(i,%tt%) $ (not addCest(i)))/pma.l(i,%tt%))**p_sigmam(i);

         xd.l(iDom(i),%tt%) $ ((p_sigmam(i) ne inf) and (updateLoop lt 4))
           = max(0.50*xd.l(i,%tt1%),min(2.*xd.l(i,%tt1%),
           p_ad(i,%tt%)*p_gammamd(i,%tt%)**[ -p_sigmam(i) $ addCest(i) + (p_sigmam(i)-1) $ (not addCest(i))]
               * xat.l(i,%tt%)*((paIND.l(i,%tt%) $ addCest(i) + pa.l(i,%tt%) $ (not addCest(i)))/pda.l(i,%tt%))**p_sigmam(i)));

         xd.l(iDom(i),%tt%) $ ((p_omegax(i) ne inf) and (updateLoop eq 4))
            = max(0.50*xd.l(i,%tt1%),min(1.5*xd.l(i,%tt1%),
              p_gd(i,%tt%)*(xs.l(i,%tt%) - delst.l(i,%tt%))* p_gammaxd(i,%tt%)**([-1-p_omegax(i)] $ (not addCest(i)) + p_omegax(i) $ addCest(i))
                                         *(pd.l(i,%tt%)/(psIND.l(i,%tt%) $ addCest(i) + ps.l(i,%tt%) $ (not addCest(i))))**p_omegax(i)));

         xe.l(iExp(i),%tt%) $ ((p_omegax(i) ne inf) and (p_etae(i) eq inf))
            =  max(0.50*xe.l(i,%tt1%),min(1.5*xe.l(i,%tt1%),
                p_ge(i,%tt%)*(xs.l(i,%tt%) - delst.l(i,%tt%))* p_gammaxe(i,%tt%)**([-1-p_omegax(i)] $ (not addCest(i)) + p_omegax(i) $ addCest(i))
                                           *(pe.l(i,%tt%)/(psIND.l(i,%tt%) $ addCest(i) + ps.l(i,%tt%) $ (not addCest(i))))**p_omegax(i)));
         xmarg.l(j,%tt%) $ mgencl(j)
            = tmg.l(j,"d",%tt%)*xd.l(j,%tt%) $ iDom(j) + tmg.l(j,"m",%tt%)*xm.l(j,%tt%) $ iImp(j) + tmg.l(j,"x",%tt%)*xe.l(j,%tt%) $ iExp(j);

         xa.l(i,j,%tt%) $ p_amarg(i,j) =(  [p_amarg(i,j)*xmarg.l(j,%tt%)*(pmargInd.l(j,%tt%)/(p_chiPA(i)*pa.l(i,%tt%)))**p_sigmamg(j)] $ p_sigmamg(j)
                                         + [p_amarg(i,j)*xmarg.l(j,%tt%)] $ (not p_sigmamg(j)) );

         if ( updateLoop lt 4,

            xs.scale(i,%tt%)   = max(1.E-10,xs.l(i,%tt%));
            xs.l(iDom(i),%tt%) = xs.l(i,%tt1%) * (xd.l(i,%tt%)+xe.l(i,%tt%))/(xd.l(i,%tt1%)+xe.l(i,%tt1%));

            x.scale(aact,i,%tt%) $ sam(aact,i) = max(1.E-10,x.l(aact,i,%tt%));
            x.l(aact,i,%tt%)     $ sam(aact,i) = x.l(aact,i,%tt%) * xs.l(i,%tt%)/xs.scale(i,%tt%);

            xfac.scale(fptop,aact,v,%tt%) $ xfac.l(fptop,aact,v,%tt%) = max(1.E-10,xfac.l(fptop,aact,v,%tt%));
            xfac.l(fptop,aact,v,%tt%) = xfac.l(fptop,aact,v,%tt%)
                                      * sum(i $ sam(aact,i), x.l(aact,i,%tt%)*p.l(aact,i,%tt1%))
                                       /sum(i $ sam(aact,i),x.scale(aact,i,%tt%)*p.l(aact,i,%tt1%));

            xp.l(aact,%tt%) = sum(flagxFac(fptop,aact,v), xfac.l(fptop,aact,v,%tt%)*pxfac.l(fptop,aact,v,%tt%)*(1+p_markup(aact,%tt%)$ifEnTech))/px.l(aact,%tt%);

            xp.l(a,%tt%) $ (p_omegap(a) eq inf) = sum(i $ p_ap(a,i,%tt%), x.l(a,i,%tt%)*chip.l(a,i,%tt%));

            px.l(aact,%tt%) = sum(flagxFac(fptop,aact,v), xfac.l(fptop,aact,v,%tt%)*pxfac.l(fptop,aact,v,%tt%)*(1+p_markup(aact,%tt%)$ifEnTech))/xp.l(aact,%tt%);

            pp.l(aact(a),%tt%)   = px.l(a,%tt%)*(1 + tPrd.l(a,%tt%) + tpsb.l(a,%tt%) + tphn.l(a,%tt%))
                                 + sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),tEmi.l(em,a,%tt%)*p_phiEmiXP(em,a,%tt%)*chiEmiXP.l(em,a,%tt%)*p_rhoEmiXP(em,a));

            p.l(aact,i,%tt%) $ p_ap(aact,i,%tt%) = p.l(aact,i,%tt1%) * pp.l(aact,%tt%)/pp.l(aact,%tt1%);

            xfac.l(flagXFac(fpChl,aact,v),%tt%) $ (actfPNest(fpChl) $ xfac.scale(fptop,aact,v,%tt%))
               = xfac.l(fpChl,aact,v,%tt%) * xfac.l(fptop,aact,v,%tt%)/xfac.scale(fptop,aact,v,%tt%);
            xa.l(iact,aact,%tt%) $ sam(iact,aact ) = sum((sameas(fpNestI,iact),v),xfac.l(fpNestI,aact,v,%tt%));

            if (ifVint,
*
*              --- initialize old capital and capital use of old vintage
*
               $$ondotl
               oldCap.l(aact,%tt%) = sum((cap,vp),xfac.l(cap,aact,vp,%tt1%))*(1-deprA.l(aact,tt)*sum(sameas(t,tt),m_dmgAgg(dmgDepr,'aact')));
               $$offdotl
               $$ifi "%ForwardLooking%"=="OFF" oldCap.fx(aact,tt) = oldCap.l(aact,tt);

               rrat1.l(a,%tt%) $ oldCap.l(a,%tt%)= max(0,sum((cap,v),xFac.l(cap,a,v,%tt%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
               rrat.l(a,%tt%)  $ oldCap.l(a,%tt%) = ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
               xfac.l(cap,aact,old,%tt%) $ ((not ifEntech)$ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);

               rrat1.l(a,%tt%) $ oldCap.l(a,%tt%)= max(0,sum((cap,v),xFac.l(cap,a,v,%tt%)+5.E-11)/(oldCap.l(a,%tt%)+1.E-10));
               rrat.l(a,%tt%)  $ oldCap.l(a,%tt%) = ncpVuPow(rrat1.l(a,%tt%),1,1.E-7);
               xfac.l(cap,aact,old,%tt%) $ ((not ifEntech) $ oldCap.l(aact,%tt%)) = oldCap.l(aact,%tt%) * rrat.l(aact,%tt%);
               pxfac_preTax.l(cap,aact,old,%tt%) $ oldCap.l(aact,%tt%) = rrat.l(aact,%tt%)**(1/p_invElas(aact)) * trent.l(%tt%);
               pxfac.l(cap,aact,old,%tt%)        $ oldCap.l(aact,%tt%) = pxfac_preTax.l(cap,aact,old,%tt%) * (1 + tFac.l(aact,cap,%tt%));
            );

            xfmSup.l(fma,fmp,"suminst",vf,%tt%) $ (sum((sameas(a,fma),actFp) $ sameas(actFp,fmp),1) $ xfmSup.l(fma,fmp,"suminst",vf,t0))
              = sum((sameas(actFp,fmp),a,vp) $ (sameas(a,fma) $ flagXFac(actFp,a,vp)),xfac.l(actFp,a,vp,%tt%));

            $$iftheni.labVintages "%labVintages%"=="on"

              xfmSup.l(fma,fmp,"suminst",vf,%tt%) $ (sum(sameas(actFmp,l),1) $ atFmSup.l(fmp,"suminst",vf,%tt%))
                 =  xfmSup.l(fma,fmp,"suminst",vf,%tt%) * atFmSup.l(fmp,"suminst",vf,%tt%)
                   /(atFmSup.l(fmp,"suminst",newf,%tt%)+atFmSup.l(fmp,"suminst",oldf,%tt%));
            $$endif.labVintages

            if ( ifSectSpecInv,
               newInv.l(%tt%) = sum((cap,aact), xfac.l(cap,aact,new,%tt%)+p_oldCapInvShare(aact)*xfac.l(cap,aact,old,%tt%));
               lambdaOa.l(inv,iact,"top",%tt%) $ (p_af(inv,iact,"top",%tt%) and (lambdaoa.range(inv,iact,"top",%tt%) ne 0))
                  = [ sum((cap,aact) $ gammaInv.l(iact,aact,t0),
                         (xfac.l(cap,aact,new,%tt%)+p_oldCapInvShare(aact)*xfac.l(cap,aact,old,%tt%)) * gammaInv.l(iact,aact,%tt%) )
                         /newInv.l(%tt%)]/(p_af(inv,iact,"top",t0)*p_lambdaInv0(iact));

               $$ondotl
               xa.l(iAct,inv,%tt%) $ xa.l(iAct,inv,t0)
                  =  p_af(inv,iACt,"top",%tt%)*xak.l(inv,"top",%tt%)
                      *     1/[sum(sameas(t,%tt%),m_dmgAgg(dmgDem,'inv,iAct,"top"'))*lambdaOa(inv,iAct,"top",%tt%)*(1+lambdaNRG(%tt%)$e(iAct))];
               $$offdotl
            );
            $$iftheni.maccs defined emisa

                emisa.l(a,v,%tt%)  $ macc(a,"0")
                       =  [      sum(em      $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs")),
                                     emical.l(%tt%) * p_phiEmiXP(em,a,%tt%)*p_rhoEmiXP(em,a)*xp.l(a,%tt%))

                              +  sum((em,actfp) $ (p_rhoEmiFac(em,actfp,a) and p_emiConv(em,"GHGs")),
                                      emical.l(%tt%) * p_phiEmiFac(em,actFp,a) * p_rhoEmiFac(em,actFp,a)
                                      *sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,%tt%)))
                            ];

                if ( hasGhgPolicy,

                   macDemand1.l(aact(a),%tt%) $ macc(a,"0")
                    = sum[(maccPoints,maccPoints1) $ (   macc(a,maccPoints) $ macc(a,maccPoints1)
                                                           $ (maccPoints.pos gt maccPoints1.pos)
                                                           $ (macc(a,maccPoints1)
                                                              eq smax(maccPoints2 $ (maccPoints.pos gt maccPoints2.pos), macc(a,maccPoints2)))),

                        {-ncpVuPow(0,
                          -[ncpVuPow(tEmiD0.l(%tt%)*1000*rescale/reEscale,p_maccPoints(maccPoints),max(1.E-4,1.E-4*p_maccPoints(maccPoints)))
                                 - p_maccPoints(maccPoints1)],max(1.E-4,1.E-4*macc(a,maccPoints)))}

                         * [ 0.5 * (macc(a,maccPoints)-macc(a,maccPoints1))
                                  + macc(a,maccPoints1)
                           ]];

                   xfac.l("c-macc",a,v,%tt%) $ (flagXFac("c-macc",a,v) $ macc(a,"0"))
                    = max(1.E-10, macDemand1.l(a,%tt%)*(0.001/rescale) * emisa.l(a,v,%tt%)*reEscale/ps.l("c-macc",%tt%));

                );
            $$endif.maccs

         );

         option kill=x.scale,kill=xs.scale,kill=xfac.scale;

     );

     facty.l(actFp,%tt%) = sum(flagXFac(actFp,aact,v), pxfac_preTax.l(actFp,aact,old,%tt%)*xfac.l(actFp,aact,v,%tt%));

     if ( hasEmit,

        emiComm.l(emiCommFlag(em,emSrc,aaIn),%tt%)
          = emical.l(%tt%)*chiEmi.l(em,emSrc,aaIn,%tt%)*p_rhoEmi(em,emSrc,aaIn)*xa.l(emSrc,aaIn,%tt%);

        emiFact.l(em,actFp,aact,%tt%) $ p_rhoEmiFac(em,actFp,aact)
            = emical.l(%tt%)*chiEmiFac.l(em,actFp,aact,%tt%)*p_rhoEmiFac(em,actFp,aact)*sum(flagxFac(actFp,aact,v), xfac.l(actFp,aact,v,%tt%));

        emiXP.l(em,aact,%tt%)  $ p_rhoEmiXP(em,aact)
            = emical.l(%tt%)*chiEmiXP.l(em,aact,%tt%)*p_rhoEmiXP(em,aact)*xp.l(aact,%tt%);

        emiSecA.l(emiFlag(em,aaIn),%tt%)
          = [      sum(emiCommFlag(em,emSrc,aaIn), emiComm.l(em,emSrc,aaIn,%tt%))
                 + sum(sameas(aaIn,aact),
                   +  sum(actFp $ p_rhoEmiFac(em,actFp,aact), emiFact.l(em,actFp,aact,%tt%))
                   +  emiXP.l(em,aact,%tt%) $ p_rhoEmiXP(em,aact)
                   +  emiLUCF.l(em,aact,%tt%) $ (sum(actLnd,-p_rhoEmiLnd(em,actLnd,aact)) $ sameas(em,"co2")))];
     );

     lCovArea.l(actHa,"builtupLand",%tt%) $ (lCovArea.l(actHa,"builtupLand",t0) gt eps)
          = lCovArea.l(actHa,"builtupLand",t0)
               *(rgdpmp.l(%tt%)/rgdpmp.l(t0) * pop.l("ptotl",t0)/pop.l("ptotl",%tt%))**0.33503
               *(pop.l("ptotl",%tt%)/pop.l("ptotl",t0))**(1-0.17634);

     lCovArea.l(actHa,lCov,%tt%) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ lCovMng(lCov))
*
*       -- previous year stock of land cover type
*
       =  lCovArea.l(actHa,lCov,%tt1%)
*
*       --- changes in managed categories from changes in xFac (expressed in Mio ha)
*
         + sum((crops,vp) $ flagXFac(actHa,crops,vp),xfac.l(actHa,crops,vp,%tt%)-xfac.l(actHa,crops,vp,%tt1%)) $ sameas(lCov,"cropLand")
         + sum((anim,vp)  $ flagXFac(actHa,anim,vp), xfac.l(actHa,anim,vp,%tt%) -xfac.l(actHa,anim,vp,%tt1%) ) $ sameas(lCov,"pastureLand")
         + sum((frs,vp)   $ flagXFac(actHa,frs,vp),  xfac.l(actHa,frs,vp,%tt%)  -xfac.l(actHa,frs,vp,%tt1%)  ) $ sameas(lCov,"forest");

     lCovArea.l(actHa,lCov,%tt%) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ lCovUnMng(lCov))
*
*      -- previous year stock of land cover type
*
      =  lCovArea.l(actHa,lCov,%tt1%)
*
*      --- add exogenous changes in unmanaged land cover
*
       + p_exolCovAreaChg(actHa,lCov,%tt%)
*
*      --- changes in unmanaged categories driven by changes in other land categories
*          (managed land plus built-up land) times land transition matrix (ha to ha)
*
     - sum(lCovDrv $ (lCovArea.l(actHa,lCovDrv,t0) gt eps),
          (lCovArea.l(actHa,lCovDrv,%tt%) - lCovArea.l(actHa,lCovDrv,%tt1%))*lndTransMat(actHa,lCovDrv,lCov)
       ) $ lCovUnMng(lCov);

     dCarbon.l(actHa,lCov,%tt%) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov))
        = carbCnt(actHa,lCov) * (lCovArea.l(actHa,lCov,%tt%) - lCovArea.l(actHa,lCov,%tt1%));


     emiInv.l(ghgInv,em,%tt%) $ (card(emiFlag) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF")))
       =   sum(emiCommFlag(em,emSrc,aa) $ p_ghgInvAlloc(ghgInv,em,emSrc,aa),
                                          p_ghgInvAlloc(ghgInv,em,emSrc,aa)    * emiComm.l(em,emSrc,aa,%tt%))
         + sum((actFp,a)                $ p_ghgInvAlloc(ghginv,em,actFp,a),
                                          p_ghgInvAlloc(ghginv,em,actFp,a)   * emiFact.l(em,actFp,a,%tt%))
         + sum(a $ (p_rhoEmiXP(em,a)    $ p_ghgInvAlloc(ghginv,em,"emiXP",a)),
                                          p_ghgInvAlloc(ghginv,em,"emiXp",a) * emiXP.l(em,a,%tt%)
                                        + emiLUCF.l(em,a,%tt%) $ (sum(actLnd,-p_rhoEmiLnd(em,actLnd,a)) $ sameas(em,"co2")))
         - sum((actHa,lCov) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov)),
                                              dCarbon.l(actHa,lCov,%tt%)*3.67) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF"));

     emiTot.l(em,%tt%) $ card(emiFlag) = sum(emiFlag(em,aaIn),emiSecA.l(em,aaIn,%tt%))
          $$ifi "%considerCarbon%"=="on"   - sum((actLnd,lCov) $ ((lCovArea.l(actLnd,lCov,t0) gt eps) $ carbCnt(actLnd,lCov)), dCarbon.l(actLnd,lCov,%tt%)*3.67) $ sameas(em,"co2")
      ;

     emiGHG.l(polltype,%tt%) $ card(emiFlag) = sum(em, p_emiConv(em,polltype)*emiTot.l(em,%tt%));
     emiInt.l(polltype,%tt%) = emiGHG.l(polltype,%tt%) / rgdpmp.l(%tt%);

     exposedAirPoll.l(%tt%) $ ifAirPollDmg
        =   sum(tf $ ((years(tf) le years(%tt%)) and (years(%tt%)-years(tf)<5)),emiGhg.l("PMTF",tf));

     dmgAir.l(l,a,%tt%)$ ifAirPollDmg = p_AirPollEff(l,a,%tt%) * log(ExposedAirPoll.l(%tt%)/ExposedAirPoll.l(t0));


     totexp.l(%tt%)  = er.l(%tt%) * sum(iexp(i), p_chiPWE(i)*pwe.l(i,%tt%) * xe.l(i,%tt%));
     rtotexp.l(%tt%) = er.l(%tt%) * sum(iexp(i), p_chiPWE(i)*pwe.l(i,t0) * xe.l(i,%tt%));
     pexp.l(%tt%)    = totexp.l(%tt%)/rtotexp.l(%tt%);

     totimp.l(%tt%)  = er.l(%tt%) * sum(iImp(i), pwm.l(i,%tt%) * (xm.l(i,%tt%) + mdelst.l(i,%tt%)));
     rtotimp.l(%tt%) = er.l(%tt%) * sum(iImp(i), pwm.l(i,t0)   * (xm.l(i,%tt%) + mdelst.l(i,%tt%)));
     pimp.l(%tt%)    = totimp.l(%tt%)/rtotimp.l(%tt%);
     bot.l(%tt%)     = totexp.l(%tt%) - totImp.l(%tt%);

     walras.l(%tt%)  = 0;

     savf.l(%tt%)    =  -(bot.l(%tt%)
                               + sum((inst,mapInst(row,instp)),transfers.l(inst,instp,%tt%)- transfers.l(instp,inst,%tt%))
                               - sum(instf, debtPay.l(instf,%tt%)))/er.l(%tt%);
     yg.l("Exp",%tt%)    = sum(iExp(i), xe.l(i,%tt%)*p_chiPE(i)*pe.l(i,%tt%)*tExp.l(i,%tt%));
     yg.l("Imp",%tt%)    = sum(iImp(i),(xm.l(i,%tt%) + mdelst.l(i,%tt%))*pwm.l(i,%tt%)*er.l(%tt%)*tImp.l(i,%tt%));
     yg.l("VatDom",%tt%) = sum(iDom(i),(p_chiPD(i)*pd.l(i,%tt%) + pmarg.l(i,%tt%)*tmg.l(i,"d",%tt%))*tVat.l(i,"d",%tt%)*xd.l(i,%tt%));
     yg.l("VatImp",%tt%) = sum(iImp(i),(er.l(%tt%)*pwm.l(i,%tt%)*(1+tImp.l(i,%tt%)) + pmarg.l(i,%tt%)*tmg.l(i,"m",%tt%))*tVat.l(i,"m",%tt%)*xm.l(i,%tt%));



     yg.l("FacCap",%tt%) = sum(flagXFac(cap,a,v), tFac.l(a,cap,%tt%) * pxfac_preTax.l(cap,a,old,%tt%)*xfac.l(cap,a,v,%tt%));

     ygPaf.l(iact(i),%tt%) = sum(aa $ sam(i,aa),tPaf.l(i,aa,%tt%)*p_chiPA(i)*pa.l(i,%tt%)*xa.l(i,aa,%tt%));
     yg.l("Paf",%tt%)      = sum(i,ygPaf.l(i,%tt%));
     yg.l("Prd",%tt%)      = sum(aact(a),tPrd.l(a,%tt%)*px.l(a,%tt%)*xp.l(a,%tt%));
     yg.l("Psb",%tt%)      = sum(aact(a),tPsb.l(a,%tt%)*px.l(a,%tt%)*xp.l(a,%tt%));


     yg.l("FacLand",%tt%)   = sum(flagXFac(actLnd,a,v), tFac.l(a,actLnd,%tt%) * pxfac_preTax.l(actLnd,a,old,%tt%)*xfac.l(actLnd,a,v,%tt%));
     yg.l("FacNatres",%tt%) = sum(flagXFac(natres,a,v), tFac.l(a,natres,%tt%) * pxfac_preTax.l(natres,a,old,%tt%)*xfac.l(natres,a,v,%tt%));
     yg.l("FacLab",%tt%)    = sum(flagXFac(l,a,v),      tFac.l(a,l,%tt%)      * pxfac_preTax.l(l,a,old,%tt%)     *xfac.l(l,a,v,%tt%));
     yg.l("FacCap",%tt%)    = sum(flagXFac(cap,a,v),    tFac.l(a,cap,%tt%)    * pxfac_preTax.l(cap,a,old,%tt%)   *xfac.l(cap,a,v,%tt%));

     ygEmi.l(aaIn,%tt%) $ hasEmit
      = sum((em,emSrc) $ (emiFlag(em,aaIn) $ p_rhoEmi(em,emSrc,aaIn)),
                                  tEmi.l(em,aaIn,%tt%)*emical.l(%tt%)*chiEmi.l(em,emSrc,aaIn,%tt%)*p_rhoEmi(em,emSrc,aaIn)*p_phiEmi(em,emSrc,aaIn)*xa.l(emSrc,aaIn,%tt%));

     ygEmiXP.l(a,%tt%) $ hasEmit
      =  sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),tEmi.l(em,a,%tt%)*p_phiEmiXP(em,a,%tt%)*chiEmiXP.l(em,a,%tt%)*p_rhoEmiXP(em,a)*xp.l(a,%tt%))
        +sum((em,actFp)    $ p_rhoEmiFac(em,actFp,a),tEmi.l(em,a,%tt%)*emical.l(%tt%)*chiEmiFac.l(em,actFp,a,%tt%)*p_rhoEmiFac(em,actFp,a)*p_phiEmiFac(em,actFp,a)
                                                *sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,%tt%)));

     yg.l("Emi",%tt%)   $ hasEmit = sum(aa,ygEmi.l(aa,%tt%));
     yg.l("EmiXP",%tt%) $ hasEmit = sum(a,ygEmiXP.l(a,%tt%));

     ygTot.l(%tt%) = sum(g,yg.l(g,%tt%));
     gdpmp.l(%tt%)  = [sum(fd, yf.l(fd,%tt%)) + tdelst.l(%tt%) + totexp.l(%tt%) - totimp.l(%tt%)];

     totTr.l(inst,%tt%) $ sum(mapinst(gov,inst),1)
      = chiTotTr.l(inst,%tt%)*(1 + p_relExpInc/100*fiscalRespDrv.l(%tt%) $ p_relExpInc $ ifFiscalResp)*gdpmp.l(%tt%);
     transfers.l(inst,dinst,%tt%) = chiTr.l(inst,dinst,%tt%) * totTr.l(dinst,%tt%);

     ygov.l(%tt%)
      =     ygTot.l(%tt%)
           + sum((mapinst(gov,instp),inst), transfers.l(instp, inst,%tt%))
           + sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(gov,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0)),
                   xfmSupUpdateFac.l(actFmp,inst,%tt%)*xfmSup.l(actFmp,actFmp,inst,vf,%tt%)*pfmSup.l(actFmp,actFmp,inst,vf,%tt%)*corrFac.l(actFp,%tt%))
           +  sum((actFp,mapInst(gov,inst)), facty.l(actFp,%tt%)*p_chiFac(inst,actFp,%tt%));

     primaryBalance.l(%tt%) =    ygov.l(%tt%)
                                - gtax.l(%tt%)
                                - sum(mapinst(gov,inst), totTr.l(inst,%tt%))
                                - yf.l(gov,%tt%)
                                - sum(ginv,yf.l(ginv,%tt%))
                                - sum(map_adpInv(gov,adpinv),yf.l(adpinv,%tt%));

     debtPay.l(instfd,%tt%) = debtInterest.l(instfd,%tt%) + debtPrincipal.l(instfd,%tt%);
     fiscalBalance.l(%tt%) = primaryBalance.l(%tt%) - sum(instfd,debtInterest.l(instfd,%tt%));
     govBalance.l(%tt%)   = fiscalBalance.l(%tt%)  - sum(instfd,debtPrincipal.l(instfd,%tt%));
     rgovBalance.l(%tt%)  = govBalance.l(%tt%)/pgdpmp.l(%tt%);
     debtNewT.l(%tt%)   = -ncpVuSin(-1.E-6,govBalance.l(%tt%),max(1.E-7,1.E-7*abs(ygov.l(%tt1%))));
*
     DebtStkT.l(%tt%)    = sum(instfd,DebtStk.l( instfd,%tt%));

     botGdp.l(%tt%)  = bot.l(%tt%)/gdpmp.l(%tt%);
     debtGDP.l(%tt%)     = DebtStkT.l(%tt%) / gdpmp.l(%tt%);

     ror.l(%tt%) = ror.l(t0)*p_chiRor(%tt%)*([(trent.l(%tt%)/(trent.l(t0)*pgdpmp.l(%tt%)))**epsRor.l(%tt%)] $ epsRor.l(t0) + 1 $ (not epsRor.l(t0)));

     GdpCompShr.l(nipa,%tt%)   =  sum(mapn(fd,nipa), xfd.l(fd,%tt%))/rgdpmp.l(%tt%);
     GdpCompShr.l("Expo",%tt%) =  rtotexp.l(%tt%)/rgdpmp.l(%tt%);
     GdpCompShr.l("Impo",%tt%) = -rtotimp.l(%tt%)/rgdpmp.l(%tt%);
     nshrGDP.l(oa,%tt%)   = yf.l(oa,%tt%)/gdpmp.l(%tt%);
     rshrGDP.l(oa,%tt%)    = xfd.l(oa,%tt%)/rgdpmp.l(%tt%);

     sectTot.l(%tt%) = sum(flagXFac(fp,a,vp),pxfac_preTax.l(fp,a,old,%tt%)*xfac.l(fp,a,vp,%tt%));

     sectShr.l(aagr,%tt%) $ sum(mapAagr(aagr,a),1)
       = sum(flagXFac(fp,a,vp) $ mapAagr(aagr,a),pxfac_preTax.l(fp,a,old,%tt%)*xfac.l(fp,a,vp,%tt%))/SectTot.l(%tt%);

     depRate.l(cohorts,ts) = pop.l(cohorts,ts)/pop.l("P1564",ts);
     popAge.l(age,%tt%) $ (ord(age) eq 1) = brateU.l(%tt%) * pop.l("P1564",%tt1%);
     Loop(age $ (age.pos gt 1),
          popAge.l(age,%tt%) = popAge.l(age-1,%tt1%)*drate.l(age,%tt%);
     );

     unemp.l(%tt%) = (pop.l("P1564",%tt%) - sum((sameas(l,actFmp),vf),xfmSup.l(actFmp,actFmp,"sumInst",vf,%tt%)));

     gdpfc.l(%tt%)  = sum(flagXFac(actFp,a,v),pxfac.l(actFp,a,old,%tt%)*xfac.l(actFp,a,v,%tt%));
     rgdpfc.l(%tt%) = sum(flagXFac(actFp,a,v),pxfac.l(actFp,a,old,t0)*xfac.l(actFp,a,v,%tt%));
     pgdpfc.l(%tt%) = gdpfc.l(%tt%)/rgdpfc.l(%tt%);
     nrgInt.l(ts)   = sum(e,xat.l(e,ts))/rgdpmp.l(ts);
     elyInt.l(ts)   = sum(ielec,xat.l(ielec,ts))/rgdpmp.l(ts);
     cpi.l(ts)      = sum((i,h) $ sam(i,h), xa.l(i,h,ts)*paf.l(i,h,ts)) /  sum((i,h) $ sam(i,h), xa.l(i,h,ts)*p_pafini(i,h));

     trentAvg.l(%tt%) = sum(cap, facty.l(cap,%tt%))/sum(flagXFac(cap,aact,v), xfac.l(cap,aact,v,%tt%));
     kapy.l(%tt%)     = sum(cap,facty.l(cap,%tt%));

     yieldLnd.l(aact,%tt%) $ sum(flagXFac(actHa,aact,v),xfac.l(actHa,aact,v,%tt%))
          = sum(iAct $ p_as(aact,iAct,%tt%),xs.l(iAct,%tt%))/sum(flagXFac(actHa,aact,v),xfac.l(actHa,aact,v,%tt%));
  $$endif.arg

  $$if not errorFree $$abort "Compilation error after end of 'iterloop.inc', check input data."
  if (execError, abort "Some execution error at end of iterloop.inc");
