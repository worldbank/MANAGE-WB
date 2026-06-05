********************************************************************************
$ontext

   MANAGE project

   GAMS file : rebench.inc

   @purpose  : Calculate some parameters (mostly share) parameters in
               sensitivity elasticity
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms
                                                                                                                                                                  x
$offtext
********************************************************************************

  option kill=ttCal;ttCal(tt) $ ((years(tt) le %lastSimYear%) $ (years(tt) ge %BaseYear%)) = yes;

  p_alphaXfac(actFpNest,fppar,a,v) $ (pNest(a,actFpNest,fpPar) $ xfac.l(fpPar,a,old,t0)  $ xfac.l(actFpNest,a,old,t0))
          =     (xfac.l(actFpNest,a,old,t0)*pNest(a,actFpNest,fpPar)/xfac.l(fpPar,a,old,t0))
              * (1 - p_markUpNonComp(actFpNest,a,t0))
              * (pxfac.l(actFpNest,a,old,t0)/(lambdaxfac.l(actFpNest,a,old,t0)*pxfac.l(fpPar,a,old,t0)))**p_sigmaxfac(fppar,a,v);

  leontief(a,fpChl) = (not sum(fpPar $ (pNest(a,fpChl,fpPar) and sum(v,p_sigmaxfac(fpPar,a,v) gt eps)),1));

  if (execError, abort " Execerror encountered after setting share parameters in production nests in file: %system.fn%, line: %system.incline%",p_alphaXfac,xfac.l,pxfac.l);



  atFmSup.fx(actFmp,instTot,vf,ttCal) $ (xfmSup.l(actFmp,actFmp,instTot,vf,ttCal) $  ( (not  ifLabVint) or not(sum(sameas(actFmp,l),1))))
         = xfmSup.l(actFmp,actFmp,instTot,vf,ttCal)/(pfmSup.l(actFmp,actFmp,instTot,vf,ttCal)/pgdpmp.l(ttCal))**p_epsFmp(actFmp,vf);

  if ( not ifVint,

     atFmSup.lo(fmp,instTot,oldf,ttCal) $ sum(sameas(fmp,cap),1) = -inf;
     atFmSup.up(fmp,instTot,oldf,ttCal) $ sum(sameas(fmp,cap),1) = +inf;

  );

  p_gammaFmp(fmNestFmp(fmChild,actFmp),instTot,vf,ttCal) $ (sum(fmNesting(fmChild,fmPar,actFmp), p_omegaFmp(fmPar,vf) ne inf)
                                                                                $  xfmSup.l(fmChild,actFmp,instTot,vf,ttCal)
                                                                                $  ( (not  ifLabVint) or not(sum(sameas(actFmp,l),1))))
      = sum(fmNesting(fmChild,fmPar,actFmp),xfmSup.l(fmChild,actFmp,instTot,vf,ttCal)/xfmSup.l(fmPar,actFmp,instTot,vf,ttCal)
                                        * ( pfmSup.l(fmpar,actFmp,instTot,vf,ttCal)  /pfmSup.l(fmChild,actFmp,instTot,vf,ttCal))**p_omegaFmp(fmPar,vf));


  p_ap(a,i,ttCal) $ (x.l(a,i,ttCal) ne 0) = ( (x.l(a,i,ttCal)/xp.l(a,ttCal))*(pp.l(a,ttCal)/p.l(a,i,ttCal))**p_omegap(a)) $ (p_omegap(a) ne inf)
                                              + (p.l(a,i,ttCal)*x.l(a,i,ttCal)/(pp.l(a,ttCal)*xp.l(a,ttCal)))             $ (p_omegap(a) eq inf);

  p_as(a,i,ttCal) $ (x.l(a,i,ttCal) ne 0) = ((x.l(a,i,ttCal)/xs.l(i,ttCal))*(p.l(a,i,ttCal)/pind.l(i,ttCal))**p_sigmas(i))  $ (p_sigmas(i) ne inf)
                                           + (p.l(a,i,ttCal)*x.l(a,i,ttCal)/(ps.l(i,ttCal)*xs.l(i,ttCal)))                  $ (p_sigmas(i) eq inf);

  if( ifSectSpecInv,

    p_af(oa,i,knest,ttCal) $( (xak.l(oa,knest,ttCal) ne 0) $ mapkNestShr(i,kNest) $ pa.l(i,ttCal) $ (not sum(sameas(inv,oa),1)))
       = ((p_transmat(i,kNest,oa)/((1+tpaf.l(i,oa,ttCal))*pa.l(i,ttCal)))/xak.l(oa,knest,ttCal))
        *(((1+tpaf.l(i,oa,ttCal))*pa.l(i,ttCal))/pxak.l(oa,knest,ttCal))**p_sigmaC(oa,knest);

    p_akf(f,topKNest,ttCal) $ ((xak.l(f,topKNest,ttCal) ne 0) and (not h(f)) and (not inv(f)))
       = (xak.l(f,topKNest,ttCal)/xfd.l(f,ttCal))*(pxak.l(f,topKNest,ttCal)/pf.l(f,ttCal))**p_sigmaf(f);

    p_afk(oa,kNest,kNest1,ttCal)$((xak.l(oa,kNest,ttCal) ne 0) and mapkNestShr(kNest,kNest1) $ (not sum(sameas(inv,oa),1)))
      = (xak.l(oa,kNest,ttCal)/xak.l(oa,kNest1,ttCal))*(pxak.l(oa,kNest,ttCal)/pxak.l(oa,kNest1,ttCal))**p_sigmac(oa,kNest1);

  else

    p_af(oa,i,knest,ttCal)$( (xak.l(oa,knest,ttCal) ne 0) $ mapkNestShr(i,kNest) $ pa.l(i,ttCal))
       = ((p_transmat(i,kNest,oa)/((1+tpaf.l(i,oa,ttCal))*pa.l(i,ttCal)))/xak.l(oa,knest,ttCal))
        *(((1+tpaf.l(i,oa,ttCal))*pa.l(i,ttCal))/pxak.l(oa,knest,ttCal))**p_sigmaC(oa,knest);

    p_akf(f,topKNest,ttCal) $ ((xak.l(f,topKNest,ttCal) ne 0) and (not h(f)))
       = (xak.l(f,topKNest,ttCal)/xfd.l(f,ttCal))*(pxak.l(f,topKNest,ttCal)/pf.l(f,ttCal))**p_sigmaf(f);

    p_afk(oa,kNest,kNest1,ttCal)$((xak.l(oa,kNest,ttCal) ne 0) and mapkNestShr(kNest,kNest1))
      = (xak.l(oa,kNest,ttCal)/xak.l(oa,kNest1,ttCal))*(pxak.l(oa,kNest,ttCal)/pxak.l(oa,kNest1,ttCal))**p_sigmac(oa,kNest1);
  );

  p_ad(idom(i),ttCal) $ p_ad(i,t0) = (xd.l(i,ttCal)/xat.l(i,ttCal))*(pda.l(i,ttCal)/pa.l(i,ttCal))**p_sigmam(i);
  p_am(iimp(i),ttCal) $ p_am(i,t0) = (xm.l(i,ttCal)/xat.l(i,ttCal))*(pma.l(i,ttCal)/pa.l(i,ttCal))**p_sigmam(i);

  p_gd(iAct(i),ttCal) $ p_gd(i,t0)
     = ((xd.l(i,ttCal)/(xs.l(i,ttCal) - delst.l(i,ttCal)))*(ps.l(i,ttCal)/pd.l(i,ttCal))**p_omegax(i))     $ (p_omegax(i) ne inf)
                       + (pd.l(i,ttCal)*xd.l(i,ttCal)/(ps.l(i,ttCal)*(xs.l(i,ttCal) - delst.l(i,ttCal))))  $ (p_omegax(i) eq inf);

  p_ge(iAct(i),ttCal) $ p_ge(i,t0)
      = ((xe.l(i,ttCal)/(xs.l(i,ttCal) - delst.l(i,ttCal)))*(ps.l(i,ttCal)/pe.l(i,ttCal))**p_omegax(i))    $ (p_omegax(i) ne inf)
                       + (pe.l(i,ttCal)*xe.l(i,ttCal)/(ps.l(i,ttCal)*(xs.l(i,ttCal) - delst.l(i,ttCal))))  $ (p_omegax(i) eq inf);

  p_chie(iExp(i),ttCal) $ (p_etae(i) ne inf) = xe.l(i,ttCal)*(pwe.l(i,ttCal)/pweROW.l(i,ttCal))**p_etae(i);

  p_chim(iimp(i),ttCal) $ (p_omegam(i) ne inf) = xm.l(i,ttCal)*(pwmROW.l(i,ttCal)/pwm.l(i,ttCal))**p_omegam(i);

  p_alphaXfac(fpNestI,fppar,aact(a),v) $ (sum(sameas(fpNestI,e),1) $ xfac.l(fppar,a,old,t0))
    = p_alphaXfac(fpNestI,fppar,a,v) * sum(sameas(fpNestI,e),p_chiPA(e))**(-p_sigmaxfac(fppar,a,v));

  $$ontext
  p_gammaFundF(ttCal) $ (FundF.l(ttCal) le 1.E-6) = 0;
  p_gammaFundF(ttCal) $ (FundF.l(ttCal) gt 1.E-6)
       = fundF.l(ttCal)/[ (interest.l(ttCal)*sum(instf,riskPrem.l(instf,ttCal)))**p_sigmaFInt
                          * ( 1 + ncpvusin(0,-{sigmoid([debtGdp.l(ttCal)-0.5]*4)-0.5}*2))];
  $$offtext


  p_aFundG(ttCal) = FundG.l(ttCal)/FundT.l(ttCal) * (fundPrice.l(ttCal)*interest.l(t0)/interest.l(ttCal))**p_omegaFund(ttCal) + 1.E-8;

  p_asav(h,ttCal)  = savh.l(h,ttCal)/yhdsp.l(h,ttCal) * (fundPrice.l(ttCal)/pindh.l(h,ttCal))**(sign(savh.l(h,ttCal))*p_omegaSav(h));

  p_aSavEntr(entr,ttCal) = savEntr.l(entr,ttCal)/(entrY.l(entr,ttCal) * (1- tCit.l(entr,ttCal)) *(fundPrice.l(ttCal)/pgdpmp.l(ttCal))**p_omegaSavEntr(entr));

  rore.l(ttCal) $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) ))
                  = rorc.l(ttCal)*(kstocke.l(ttCal)/kstock.l(ttCal))**(epsRorE.l(ttCal));
  p_gammasf1(ttCal) $ (capAccClosure eq savfEndoResRegSavings)
      = [savf.l(ttCal)/gdpmp.l(ttCal)]
        /(([(1+trent.l(ttCal))/(1+p_retrow(ttCal))]**eta1a.l(ttCal))*([(1+ggdppc.l(ttCal))/(1+p_ggdppcrow(ttCal))]**eta2a.l(ttCal)));

  $$ontext
*    --- attention p_sigmaInv cannot be changed as p_gammaInv0 as the share parameters does not carry a time index

     p_gammaInv0(i,a) $ gammaInv.l(i,a,ttCal)
       = gammaInv.l(i,a,ttCal)/{[(invPrice.l(a,ttCal)/sum(inv,p_chiPA(i)*paf.l(i,inv,ttCal)))**p_sigmaInv(a) ] $ p_sigmaInv(a) + 1 $ (not p_sigmaInv(a))};
   $$offtext

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
