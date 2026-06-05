********************************************************************************
$ontext

   MANAGE project

   GAMS file : toGUI.inc

   @purpose  : Generate output for GGIG reporting back-end
   @author   : Wolfgang Britz
   @date     : 18.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : bau.gms

$offtext
********************************************************************************

  $$batinclude "inc/title.inc" '"Post model reporting for backend "'


  p_toGui("%RegGtapName%","V",diagRep,"mod",ts%draws%) = diagnostics(diagRep,ts);
*
* --- import demand at cif
*
  p_toGui("%RegGtapName%","Q",iImp,"imp",ts%draws%) = xm.l(iImp,ts)+mdelst.l(iImp,ts);
  p_toGui("%RegGtapName%","P",iImp,"imp",ts%draws%) = er.l(ts)*pwm.l(iImp,ts);
  p_toGui("%RegGtapName%","V",iImp,"imp",ts%draws%) = p_toGui("%RegGtapName%","Q",iImp,"imp",ts%draws%)*p_toGui("%RegGtapName%","P",iImp,"imp",ts%draws%);
  p_toGui("%RegGtapName%","G",iImp,"imp",ts%draws%) = p_toGui("%RegGtapName%","V",iImp,"imp",ts%draws%)*timp.l(iImp,ts);
  p_toGui("%RegGtapName%","T",iImp,"imp",ts%draws%) = tImp.l(iImp,ts);
*
* --- export demand at fob
*
  p_toGui("%RegGtapName%","Q",iExp,"exp",ts%draws%) = xe.l(iExp,ts);
  p_toGui("%RegGtapName%","P",iExp,"exp",ts%draws%) = p_chiPE(iExp)*pe.l(iExp,ts);
  p_toGui("%RegGtapName%","V",iExp,"exp",ts%draws%) = p_toGui("%RegGtapName%","Q",iExp,"exp",ts%draws%)*p_toGui("%RegGtapName%","P",iExp,"exp",ts%draws%);
  p_toGui("%RegGtapName%","G",iExp,"exp",ts%draws%) = p_toGui("%RegGtapName%","V",iExp,"exp",ts%draws%)*texp.l(iExp,ts);
  p_toGui("%RegGtapName%","T",iExp,"exp",ts%draws%) = tExp.l(iExp,ts);

  p_toGui("%RegGtapName%","V",i,"bot",ts%draws%) = p_toGui("%RegGtapName%","V",i,"exp",ts%draws%)-p_toGui("%RegGtapName%","V",i,"imp",ts%draws%);
  p_toGui("%RegGtapName%","Q",i,"bot",ts%draws%) = p_toGui("%RegGtapName%","Q",i,"exp",ts%draws%)-p_toGui("%RegGtapName%","Q",i,"imp",ts%draws%);
  p_toGui("%RegGtapName%","P",i,"bot",ts%draws%) $ p_toGui("%RegGtapName%","Q",i,"bot",ts%draws%)
   = p_toGui("%RegGtapName%","V",i,"bot",ts%draws%)/p_toGui("%RegGtapName%","Q",i,"bot",ts%draws%);
*
* --- domestic sales
*
  p_toGui("%RegGtapName%","Q",iDom,"dms",ts%draws%) =   xd.l(iDom,ts);
  p_toGui("%RegGtapName%","P",iDom,"dms",ts%draws%) =   p_chiPD(iDom)*pd.l(iDom,ts);
  p_toGui("%RegGtapName%","V",iDom,"dms",ts%draws%)
     = p_toGui("%RegGtapName%","Q",iDom,"dms",ts%draws%)*p_toGui("%RegGtapName%","P",iDom,"dms",ts%draws%)  ;
*
* --- output (domestic sales and exports)
*
  p_toGui("%RegGtapName%",VQG,iAct,"output",ts%draws%) =  p_toGui("%RegGtapName%",VQG,iAct,"dms",ts%draws%) + p_toGui("%RegGtapName%",VQG,iAct,"exp",ts%draws%);
  p_toGui("%RegGtapName%","P",iAct,"output",ts%draws%)  $ p_toGui("%RegGtapName%","Q",iAct,"output",ts%draws%)
   = p_toGui("%RegGtapName%","V",iAct,"output",ts%draws%)  / p_toGui("%RegGtapName%","Q",iAct,"output",ts%draws%);
  p_toGui("%RegGtapName%","T",iAct,"output",ts%draws%)  $ p_toGui("%RegGtapName%","V",iAct,"output",ts%draws%)
   = p_toGui("%RegGtapName%","G",iAct,"output",ts%draws%)  / p_toGui("%RegGtapName%","V",iAct,"output",ts%draws%);
*
* --- market appearances
*
  $$iftheni.IO not "%reducedOutput%"=="on"
     p_toGui("%RegGtapName%",VQG,iAct,"mrkApp",ts%draws%) =  p_toGui("%RegGtapName%",VQG,iAct,"dms",ts%draws%) + p_toGui("%RegGtapName%",VQG,iAct,"imp",ts%draws%);
     p_toGui("%RegGtapName%","P",iAct,"mrkApp",ts%draws%)  $ p_toGui("%RegGtapName%","Q",iAct,"mrkApp",ts%draws%)
      = p_toGui("%RegGtapName%","V",iAct,"mrkApp",ts%draws%)  / p_toGui("%RegGtapName%","Q",iAct,"mrkApp",ts%draws%);
     p_toGui("%RegGtapName%","T",iAct,"mrkApp",ts%draws%)  $ p_toGui("%RegGtapName%","V",iAct,"mrkApp",ts%draws%)
      = p_toGui("%RegGtapName%","G",iAct,"mrkApp",ts%draws%)  / p_toGui("%RegGtapName%","V",iAct,"mrkApp",ts%draws%);
  $$endif.IO
*
* --- margin demand for imports
*
  if ( card(mgEncl),
     p_toGui("%RegGtapName%","Q",iImp,"impMrg",ts%draws%) = (xm.l(iImp,ts)+mdelst.l(iImp,ts)) * tmg.l(iImp,"m",ts);
     p_toGui("%RegGtapName%","P",iImp,"impMrg",ts%draws%) = pmarg.l(iImp,ts);
     p_toGui("%RegGtapName%","V",iImp,"impMrg",ts%draws%) = p_toGui("%RegGtapName%","Q",iImp,"impMrg",ts%draws%)*p_toGui("%RegGtapName%","P",iImp,"impMrg",ts%draws%);
  );
*
* --- import demand at basic prices, inclusive margins
*
  p_toGui("%RegGtapName%","Q",iImp,"impM",ts%draws%) = xm.l(iImp,ts)+mdelst.l(iImp,ts);
  p_toGui("%RegGtapName%","P",iImp,"impM",ts%draws%) = (er.l(ts)*pwm.l(iImp,ts)*(1+tImp.l(iImp,ts)) + pmarg.l(iImp,ts)*tmg.l(iImp,"m",ts));
  p_toGui("%RegGtapName%","V",iImp,"impM",ts%draws%) = p_toGui("%RegGtapName%","Q",iImp,"imp",ts%draws%)*p_toGui("%RegGtapName%","P",iImp,"imp",ts%draws%);
  p_toGui("%RegGtapName%","G",iImp,"impM",ts%draws%) = p_toGui("%RegGtapName%","V",iImp,"imp",ts%draws%)*tVat.l(iImp,"m",ts);
  p_toGui("%RegGtapName%","T",iImp,"impM",ts%draws%) = tVat.l(iImp,"m",ts);
*
* --- margin demand domesitc
*
  if (card(mgencl),
     p_toGui("%RegGtapName%","Q",iDom,"domMrg",ts%draws%) = xd.l(iDom,ts) * tmg.l(iDom,"d",ts);
     p_toGui("%RegGtapName%","P",iDom,"domMrg",ts%draws%) = pmarg.l(iDom,ts);
     p_toGui("%RegGtapName%","V",iDom,"domMrg",ts%draws%) = p_toGui("%RegGtapName%","Q",iDom,"domMrg",ts%draws%)*p_toGui("%RegGtapName%","P",iDom,"domMrg",ts%draws%);
  );
*
* --- domestic demand at agent prices, inclusive margins
*
  p_toGui("%RegGtapName%","Q",iDom,"domM",ts%draws%) = xd.l(iDom,ts);
  p_toGui("%RegGtapName%","P",iDom,"domM",ts%draws%) = p_chiPD(iDom)*pd.l(iDom,ts) + pmarg.l(iDom,ts)*tmg.l(iDom,"d",ts);
  p_toGui("%RegGtapName%","V",iDom,"domM",ts%draws%) = p_toGui("%RegGtapName%","Q",iDom,"domM",ts%draws%)*p_toGui("%RegGtapName%","P",iDom,"domM",ts%draws%);
  p_toGui("%RegGtapName%","G",iDom,"domM",ts%draws%) = p_toGui("%RegGtapName%","V",iDom,"domM",ts%draws%)*tVat.l(iDom,"d",ts);
  p_toGui("%RegGtapName%","T",iDom,"domM",ts%draws%) = tVat.l(iDom,"d",ts);
*
* --- Armington
*
  p_toGui("%RegGtapName%","Q",iAct,"Arm",ts%draws%)  = xat.l(iAct,ts);
  p_toGui("%RegGtapName%","P",iAct,"Arm",ts%draws%)  = pa.l(iAct,ts);
  p_toGui("%RegGtapName%","V",iAct,"Arm",ts%draws%)  = p_toGui("%RegGtapName%","Q",iAct,"Arm",ts%draws%)*p_toGui("%RegGtapName%","P",iAct,"Arm",ts%draws%);
  p_toGui("%RegGtapName%","G",iAct,"Arm",ts%draws%)  = p_toGui("%RegGtapName%","G",iAct,"domm",ts%draws%)+p_toGui("%RegGtapName%","G",iAct,"impM",ts%draws%);
  p_toGui("%RegGtapName%","T",iAct,"Arm",ts%draws%) $ p_toGui("%RegGtapName%","V",iAct,"Arm",ts%draws%)
     = p_toGui("%RegGtapName%","G",iAct,"Arm",ts%draws%)/p_toGui("%RegGtapName%","V",iAct,"Arm",ts%draws%);
*
* --- nest demands
*
  p_toGui("%RegGtapName%","Q",actFpNest(fpPar),aact,ts%draws%) = sum(v,xfac.l(fpPar,aact,v,ts)) ;
  p_toGui("%RegGtapName%","V",actFpNest(fpPar),aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",fpPar,aact,ts%draws%) = sum(v,pxfac.l(fpPar,aact,v,ts)*xfac.l(fpPar,aact,v,ts)) ;
  p_toGui("%RegGtapName%","P",actFpNest(fpPar),aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",fpPar,aact,ts%draws%)
  = p_toGui("%RegGtapName%","V",fpPar,aact,ts%draws%)/p_toGui("%RegGtapName%","Q",fpPar,aact,ts%draws%);
  $$ifi not "%reducedOutput%"=="on" p_toGui("%RegGtapName%","S",actFpNest(fpPar),aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",fpPar,aact,ts%draws%) = sum(v,lambdaxfac.l(fpPar,aact,v,ts)*xfac.l(fpPar,aact,v,ts))/p_toGui("%RegGtapName%","Q",fpPar,aact,ts%draws%);
*
* --- factor demands
*
  p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%) = sum(v,xfac.l(actFp,aact,v,ts)) ;
  p_toGui("%RegGtapName%","V",actFp,aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%)
    = sum(v,pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts)) ;
  p_toGui("%RegGtapName%","P",actFp,aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%)
   = p_toGui("%RegGtapName%","V",actFp,aact,ts%draws%)/p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%);
  $$ifi not "%reducedOutput%"=="on" p_toGui("%RegGtapName%","S",actFp,aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%) = sum(v,lambdaxfac.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts))/p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%);
  p_toGui("%RegGtapName%","G",actFp,aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%)
   = sum((v,ftx)$mapftax(actFp,ftx),tFac.l(aact,actFp,ts) * pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts));
  p_toGui("%RegGtapName%","T",actFp,aact,ts%draws%) $ p_toGui("%RegGtapName%","V",actFp,aact,ts%draws%)
   = sum((v,ftx)$mapftax(actFp,ftx),tFac.l(aact,actFp,ts) * pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts))/p_toGui("%RegGtapName%","V",actFp,aact,ts%draws%);

  p_toGui("%RegGtapName%","Q","yieldLnd",aact,ts%draws%) = yieldLnd.l(aact,ts);
*
* --- aggregate to Va (beware, the VA nest might be differently defined, for instance, comprising energy, and be subject to shifters)
*
  p_toGui("%RegGtapName%",VG,"va-agg",aact,ts%draws%)  = sum(actFp,p_toGui("%RegGtapName%",VG,actFp,aact,ts%draws%));
  p_toGui("%RegGtapName%","Q","va-agg",aact,ts%draws%) = sum(actFp,p_toGui("%RegGtapName%","Q",actFp,aact,ts%draws%)*p_toGui("%RegGtapName%","P",actFp,aact,t0%draws%));
  p_toGui("%RegGtapName%","P","va-agg",aact,ts%draws%) $ p_toGui("%RegGtapName%","Q","va-agg",aact,ts%draws%)
   = p_toGui("%RegGtapName%","V","va-agg",aact,ts%draws%)/p_toGui("%RegGtapName%","Q","va-agg",aact,ts%draws%);
  p_toGui("%RegGtapName%","T","va-agg",aact,ts%draws%) $ p_toGui("%RegGtapName%","G","va-agg",aact,ts%draws%)
   = p_toGui("%RegGtapName%","V","va-agg",aact,ts%draws%)/p_toGui("%RegGtapName%","G","va-agg",aact,ts%draws%);

*
* --- household demand for k-bundles
*
  $$iftheni.IO not "%reducedOutput%"=="on"
     p_toGui("%RegGtapName%","Q",kNest,h,ts%draws%) = xak.l(h,kNest,ts);
     p_toGui("%RegGtapName%","P",kNest,h,ts%draws%) = pxak.l(h,kNest,ts);
     p_toGui("%RegGtapName%","V",kNest,h,ts%draws%) = pxak.l(h,kNest,ts)*xak.l(h,kNest,ts);
  $$endif.IO
*
* --- production taxes
*
  p_toGui("%RegGtapName%","G",ptx,aact,ts%draws%) = tprd.l(aact,ts)*px.l(aact,ts)*xp.l(aact,ts);
  p_toGui("%RegGtapName%","V",ptx,aact,ts%draws%) = p_toGui("%RegGtapName%","G",ptx,aact,ts%draws%);
  p_toGui("%RegGtapName%","T",ptx,aact,ts%draws%) $ p_toGui("%RegGtapName%","V",ptx,aact,ts%draws%)
    = tprd.l(aact,ts)*px.l(aact,ts)*xp.l(aact,ts)/(px.l(aact,ts)*xp.l(aact,ts));
*
* --- production subsidies
*
  p_toGui("%RegGtapName%","G",psb,aact,ts%draws%) = tpsb.l(aact,ts)*px.l(aact,ts)*xp.l(aact,ts);
  p_toGui("%RegGtapName%","V",psb,aact,ts%draws%) = p_toGui("%RegGtapName%","G",psb,aact,ts%draws%);
  p_toGui("%RegGtapName%","T",psb,aact,ts%draws%) $ p_toGui("%RegGtapName%","G",psb,aact,ts%draws%)
    = p_toGui("%RegGtapName%","G",psb,aact,ts%draws%)/(px.l(aact,ts)*xp.l(aact,ts));
*
* --- production subsidies
*
  p_toGui("%RegGtapName%","G","phn",aact,ts%draws%) = tphn.l(aact,ts)*px.l(aact,ts)*xp.l(aact,ts) ;
  p_toGui("%RegGtapName%","V","phn",aact,ts%draws%) = p_toGui("%RegGtapName%","G","phn",aact,ts%draws%);
  p_toGui("%RegGtapName%","T","phn",aact,ts%draws%) $ p_toGui("%RegGtapName%","G","phn",aact,ts%draws%)
    = p_toGui("%RegGtapName%","G","phn",aact,ts%draws%)/(px.l(aact,ts)*xp.l(aact,ts));
*
* --- factor taxes
*
  p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%) = sum(flagXFac(actFp,aact,v) $  mapftax(actFp,ftx),tFac.l(aact,actFp,ts) * pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts)) ;
  p_toGui("%RegGtapName%","V","ftax","tot",ts%draws%) = sum((ftx,aact),p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%)) + 1.E-10;
  p_toGui("%RegGtapName%","V","usd","tot",ts%draws%)  = usd;
  p_toGui("%RegGtapName%","V",ftx,aact,ts%draws%) $  p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%)
     = sum(flagXFac(actFp,aact,v) $ mapftax(actFp,ftx),pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts)) ;
  p_toGui("%RegGtapName%","T",ftx,aact,ts%draws%) $  p_toGui("%RegGtapName%","V",ftx,aact,ts%draws%)
   = p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%)/ p_toGui("%RegGtapName%","V",ftx,aact,ts%draws%) ;
  p_toGui("%RegGtapName%","V",ftx,aact,ts%draws%) =  p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%);
*
* --- emission taxes on product use
*
  p_toGui("%RegGtapName%","G",ctx,emsrc,ts%draws%)
     = sum(emiFlag(em,aa) $ sam(emsrc,aa), tEmi.l(em,aa,ts)*emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts)) ;

  p_toGui("%RegGtapName%","V",ctx,emsrc,ts%draws%) $ p_toGui("%RegGtapName%","G",ctx,emsrc,ts%draws%)
     = sum(emiFlag(em,aa) $ sam(emsrc,aa), emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts)) ;
  p_toGui("%RegGtapName%","T",ctx,emsrc,ts%draws%) $  p_toGui("%RegGtapName%","V",ctx,emsrc,ts%draws%)
   = p_toGui("%RegGtapName%","G",ctx,emsrc,ts%draws%)/ p_toGui("%RegGtapName%","V",ctx,emsrc,ts%draws%) ;
*
* --- sum of taxes
*
  p_toGui("%RegGtapName%","G","sumtx",aact,ts%draws%) = sum(ftx,p_toGui("%RegGtapName%","G",ftx,aact,ts%draws%))
                                                         + sum(ptx,p_toGui("%RegGtapName%","G",ptx,aact,ts%draws%))
                                                         + sum(psb,p_toGui("%RegGtapName%","G",psb,aact,ts%draws%));
  p_toGui("%RegGtapName%","V","sumtx",aact,ts%draws%) = sum(ftx,p_toGui("%RegGtapName%","V",ftx,aact,ts%draws%))
                                                         + sum(ptx,p_toGui("%RegGtapName%","V",ptx,aact,ts%draws%))
                                                         + sum(psb,p_toGui("%RegGtapName%","V",psb,aact,ts%draws%));

  p_toGui("%RegGtapName%","T","sumtx",aact,ts%draws%) $ p_toGui("%RegGtapName%","V","sumtx",aact,ts%draws%)
       = p_toGui("%RegGtapName%","G","sumtx",aact,ts%draws%)/p_toGui("%RegGtapName%","V","sumtx",aact,ts%draws%);
*
* --- make matrix
*
  $$iftheni.IO not "%reducedOutput%"=="on"
     p_toGui("%RegGtapName%","V",aact,iAct,ts%draws%) $ x.l(aact,iAct,ts)  = chiP.l(aact,iAct,ts)*p.l(aact,iAct,ts)*x.l(aact,iAct,ts) ;
     p_toGui("%RegGtapName%","Q",aact,iAct,ts%draws%) $ x.l(aact,iAct,ts)  = x.l(aact,iAct,ts) ;
     p_toGui("%RegGtapName%","P",aact,iAct,ts%draws%) $ x.l(aact,iAct,ts)  = chiP.l(aact,iAct,ts)*p.l(aact,iAct,ts);
  $$endif.IO
*
* --- other taxes
*
  p_toGui("%RegGtapName%","V",vat,iact,ts%draws%) = (p_chiPD(iAct)*pd.l(iAct,ts) + pmarg.l(iAct,ts)*tmg.l(iAct,"d",ts))*tvat.l(iAct,"d",ts)*xd.l(iAct,ts)
                                                  + (er.l(ts)*pwm.l(iAct,ts)*(1+timp.l(iAct,ts)) + pmarg.l(iAct,ts)*tmg.l(iAct,"m",ts))*tvat.l(iAct,"m",ts)*xm.l(iAct,ts) ;
*
* --- imports and related taxes
*
  p_toGui("%RegGtapName%","V",mtx,iImp,ts%draws%) = (xm.l(iImp,ts) + mdelst.l(iImp,ts))*pwm.l(iImp,ts)*er.l(ts);
  p_toGui("%RegGtapName%","Q",mtx,iImp,ts%draws%) = (xm.l(iImp,ts) + mdelst.l(iImp,ts));
  p_toGui("%RegGtapName%","P",mtx,iImp,ts%draws%) $ p_toGui("%RegGtapName%","Q",mtx,iImp,ts%draws%)
     =  p_toGui("%RegGtapName%","V",mtx,iImp,ts%draws%)/p_toGui("%RegGtapName%","Q",mtx,iImp,ts%draws%);
  p_toGui("%RegGtapName%","T",mtx,iImp,ts%draws%) =  timp.l(iImp,ts);
  p_toGui("%RegGtapName%","G",mtx,iImp,ts%draws%) =  timp.l(iImp,ts) * p_toGui("%RegGtapName%","V",mtx,iImp,ts%draws%);
*
* --- exports and related taxes
*
  p_toGui("%RegGtapName%","V",etx,iExp,ts%draws%) =  xe.l(iExp,ts)*p_chiPE(iExp)*pe.l(iExp,ts);
  p_toGui("%RegGtapName%","Q",etx,iExp,ts%draws%) =  xe.l(iExp,ts);
  p_toGui("%RegGtapName%","P",etx,iExp,ts%draws%)  $ p_toGui("%RegGtapName%","V",etx,iExp,ts%draws%)
       = p_toGui("%RegGtapName%","V",etx,iExp,ts%draws%)/p_toGui("%RegGtapName%","Q",etx,iExp,ts%draws%);
  p_toGui("%RegGtapName%","T",etx,iExp,ts%draws%) =  texp.l(iExp,ts);
  p_toGui("%RegGtapName%","G",etx,iExp,ts%draws%) =  texp.l(iExp,ts)*p_toGui("%RegGtapName%","V",etx,iExp,ts%draws%);
*
* --- sales taxes on Armington demands
*
  p_toGui("%RegGtapName%","V",stx,iAct,ts%draws%) =   sum(ax $ sam(iAct,ax), p_chiPA(iAct)*pa.l(iAct,ts)*xa.l(iAct,ax,ts));
  p_toGui("%RegGtapName%","Q",stx,iAct,ts%draws%) =   sum(ax $ sam(iAct,ax), pa.l(iAct,ts)*xa.l(iAct,ax,ts));
  p_toGui("%RegGtapName%","P",stx,iAct,ts%draws%) $ p_toGui("%RegGtapName%","Q",stx,iAct,ts%draws%)
     = p_toGui("%RegGtapName%","V",stx,iAct,ts%draws%)/p_toGui("%RegGtapName%","Q",stx,iAct,ts%draws%)  ;
  p_toGui("%RegGtapName%","G",stx,iAct,ts%draws%) =   sum(ax $ sam(iAct,ax), tpaf.l(iAct,ax,ts)*p_chiPA(iAct)*pa.l(iAct,ts)*xa.l(iAct,ax,ts))
                   + sum(j $ sam(iAct,j), tpaf.l(iAct,j,ts)*p_chiPA(iAct)*pa.l(iAct,ts)*xa.l(iAct,j,ts));
  p_toGui("%RegGtapName%","T",stx,iAct,ts%draws%) $ p_toGui("%RegGtapName%","V",stx,iAct,ts%draws%)
   =  p_toGui("%RegGtapName%","G",stx,iAct,ts%draws%)/p_toGui("%RegGtapName%","V",stx,iAct,ts%draws%);
*
* --- imports
*
  p_toGui("%RegGtapName%","V",row,iImp,ts%draws%) = (xm.l(iImp,ts) + mdelst.l(iImp,ts))*pwm.l(iImp,ts)*er.l(ts) ;
*
* --- factor income by institutions
*
  p_toGui("%RegGtapName%","V",inst,actFp,ts%draws%)
     = sum((sameas(actFp,actfmp),vf) $ xfmsup.l(actFmp,actFmp,inst,vf,t0),
                   xfmSupUpdateFac.l(actFmp,inst,ts)*xfmSup.l(actFmp,actFmp,inst,vf,ts)
                   *pfmSup.l(actFmp,actFmp,inst,vf,ts)*corrFac.l(actFp,ts))
                 +  facty.l(actFp,ts)*p_chiFac(inst,actFp,ts);


  p_toGui("%RegGtapName%","Q",inst,actFp,ts%draws%) $ p_toGui("%RegGtapName%","V",inst,actFp,ts%draws%)
     = sum((sameas(actFp,actfmp),vf) $ xfmsup.l(actFmp,actFmp,inst,vf,t0),
                   xfmSupUpdateFac.l(actFmp,inst,ts)*xfmSup.l(actFmp,actFmp,inst,vf,ts)
                 + xfmSup.l(actFmp,actFmp,"sumInst",vf,ts)*p_chiFac(inst,actFp,ts));

  p_toGui("%RegGtapName%","P",inst,actFp,ts%draws%) $ p_toGui("%RegGtapName%","Q",inst,actFp,ts%draws%)
     = p_toGui("%RegGtapName%","V",inst,actFp,ts%draws%)/p_toGui("%RegGtapName%","Q",inst,actFp,ts%draws%) ;

  p_toGui("%RegGtapName%","V",inst,"facty",ts%draws%) = sum(actfp,p_toGui("%RegGtapName%","V",inst,actFp,ts%draws%));
  p_toGui("%RegGtapName%","Q",inst,"facty",ts%draws%) = sum(actfp,p_toGui("%RegGtapName%","Q",inst,actFp,ts%draws%));
  option p_toGUi:2:0:1;

  p_toGui("%RegGtapName%","P",inst,"facty",ts%draws%) $ p_toGui("%RegGtapName%","Q",inst,"facty",ts%draws%)
     = p_toGui("%RegGtapName%","V",inst,"facty",ts%draws%)/p_toGui("%RegGtapName%","Q",inst,"facty",ts%draws%);
*
*
* --- income elasticities
*
*  p_toGui("%RegGtapName%","incElas",h,k,ts%draws%)    = incElas.l(h,k,ts);
*
* --- economy wide factor stocks and prices
*
  p_toGui("%RegGtapName%","Q",actFmp,"tot",ts%draws%)
      = sum((inst,vf),xfmSupUpdateFac.l(actFmp,inst,ts)*xfmSup.l(actFmp,actFmp,inst,vf,ts))
     +  sum(vf,xfmSup.l(actFmp,actFmp,"sumInst",vf,ts));
  p_toGui("%RegGtapName%","V",actFmp,"tot",ts%draws%) $ p_toGui("%RegGtapName%","Q",actFmp,"tot",ts%draws%)
          = [     sum((inst,vf),xfmSupUpdateFac.l(actFmp,inst,ts)*pfmSup.l(actFmp,actFmp,inst,vf,ts)*xfmSup.l(actFmp,actFmp,inst,vf,ts))
               +  sum(vf,xfmSup.l(actFmp,actFmp,"sumInst",vf,ts)*pfmSup.l(actFmp,actFmp,"sumInst",vf,ts))];

  p_toGui("%RegGtapName%","P",actFmp,"tot",ts%draws%)  $ p_toGui("%RegGtapName%","Q",actFmp,"tot",ts%draws%)
     = p_toGui("%RegGtapName%","V",actFmp,"tot",ts%draws%)/p_toGui("%RegGtapName%","Q",actFmp,"tot",ts%draws%);


  p_toGui("%RegGtapName%","P",cap,"tot",ts%draws%) $ ifVint   = trent.l(ts);
  p_toGui("%RegGtapName%","Q",cap,"tot",ts%draws%) $ ifVint   = kaps.l(ts);

  p_toGui("%RegGtapName%","V",actFp,"tot",ts%draws%)    = p_toGui("%RegGtapName%","Q",actFp,"tot",ts%draws%) * p_toGui("%RegGtapName%","P",actFp,"tot",ts%draws%);



*
* --- Transfers by enterprises
*
  p_toGui("%RegGtapName%","V",gov,entr,ts%draws%) = sum((inst,instd)$(mapinst(gov,inst) and mapinst(entr,instd)), transfers.l(inst,instd,ts)) ;
  p_toGui("%RegGtapName%","V",inv,entr,ts%draws%) = savEntr.l(entr,ts) ;
*
* --- tax income flows to goverments or budgetary costs of subsidies
*
  p_toGui("%RegGtapName%","V",gov,psb,ts%draws%)  = sum(aAct,tpsb.l(aAct,ts)*px.l(aAct,ts)*xp.l(aAct,ts))  ;
  p_toGui("%RegGtapName%","V",gov,ptx,ts%draws%)  = sum(aAct,tprd.l(aAct,ts)*px.l(aAct,ts)*xp.l(aAct,ts));
  p_toGui("%RegGtapName%","V",gov,mtx,ts%draws%)  = sum(iAct,(xm.l(iAct,ts) + mdelst.l(iAct,ts))*pwm.l(iAct,ts)*er.l(ts)*timp.l(iAct,ts));
  p_toGui("%RegGtapName%","V",gov,stx,ts%draws%)  = sum((ax,iAct) $ sam(iAct,ax), tpaf.l(iAct,ax,ts)*p_chiPA(iAct)*pa.l(iAct,ts)*xa.l(iAct,ax,ts))
                                                  + sum((iAct,j)  $ sam(iAct,j),  tpaf.l(iAct,j,ts)*p_chiPA(iAct)*pa.l(iAct,ts)*xa.l(iAct,j,ts));
*
* --- factor taxes
*
  p_toGui("%RegGtapName%","V",gov,ftx,ts%draws%)  = sum(flagxFac(actFp,aact,v) $ mapftax(actFp,ftx),pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts));
  p_toGui("%RegGtapName%","Q",gov,ftx,ts%draws%) $  p_toGui("%RegGtapName%","V",gov,ftx,ts%draws%)
    = sum(flagxFac(actFp,aact,v) $ mapftax(actFp,ftx),xfac.l(actFp,aact,v,ts));
  p_toGui("%RegGtapName%","P",gov,ftx,ts%draws%)  $ p_toGui("%RegGtapName%","Q",gov,ftx,ts%draws%)
        = p_toGui("%RegGtapName%","V",gov,ftx,ts%draws%)/p_toGui("%RegGtapName%","Q",gov,ftx,ts%draws%);
  p_toGui("%RegGtapName%","G",gov,ftx,ts%draws%)  = sum(flagxFac(actFp,aact,v) $ mapftax(actfp,ftx),tFac.l(aact,actFp,ts) * pxfac_pretax.l(actFp,aact,v,ts)*xfac.l(actFp,aact,v,ts));
  p_toGui("%RegGtapName%","T",gov,ftx,ts%draws%)  $ p_toGui("%RegGtapName%","V",gov,ftx,ts%draws%)
        = p_toGui("%RegGtapName%","G",gov,ftx,ts%draws%)/p_toGui("%RegGtapName%","V",gov,ftx,ts%draws%);
*
* --- emission taxes
*
*-- HD: value does not make sense for carbon taxes (or equal to Q)

  p_toGui("%RegGtapName%","Q",gov,ctx,ts%draws%)  = sum((em,emsrc,aa) $ sam(emSrc,aa),  emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts)) ;
  p_toGui("%RegGtapName%","G",gov,ctx,ts%draws%) $ p_toGui("%RegGtapName%","Q",gov,ctx,ts%draws%)
    = sum((em,emsrc,aa) $ emiFlag(em,aa), tEmi.l(em,aa,ts)*emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts)) ;

*-- HD: Use quantity to calculate the tax rate as value is not calculated
  p_toGui("%RegGtapName%","T",gov,ctx,ts%draws%)  $ p_toGui("%RegGtapName%","Q",gov,ctx,ts%draws%)
        = p_toGui("%RegGtapName%","G",gov,ctx,ts%draws%)/p_toGui("%RegGtapName%","Q",gov,ctx,ts%draws%);


  p_toGui("%RegGtapName%","V",dtx,h,ts%draws%)        = tpit.l(h,ts) * yh.l(h,ts);
  p_toGui("%RegGtapName%","V",dtx,entr,ts%draws%)     = tcit.l(entr,ts) * entrY.l(entr,ts) ;
  p_toGui("%RegGtapName%","V",dtx,gov,ts%draws%)      = gTax.l(ts) ;
  p_toGui("%RegGtapName%","V",gov,dtx,ts%draws%)      = yg.l("pit",ts) +yg.l("cit",ts) ;
  p_toGui("%RegGtapName%","V",inv,h,ts%draws%)        = savh.l(h,ts) ;
  p_toGui("%RegGtapName%","V",inv,entr,ts%draws%)     = savEntr.l(entr,ts) ;
  p_toGui("%RegGtapName%","V",inv,row,ts%draws%)      = er.l(ts)*savf.l(ts);

  if (ifdelst,
     p_toGui("%RegGtapName%","V","stb",inv,ts%draws%)    = sum(ist, p_chiPS(ist)*ps.l(ist,ts)*delst.l(ist,ts) + er.l(ts)*pwm.l(ist,ts)*(1+timp.l(ist,ts))*mdelst.l(ist,ts)) ;
     p_toGui("%RegGtapName%","V","stb","tot",ts%draws%)  = sum(inv,p_toGui("%RegGtapName%","V","stb",inv,ts%draws%))  ;
     p_toGui("%RegGtapName%","Q","stb","tot",ts%draws%)  = sum(ist, delst.l(ist,ts) + mdelst.l(ist,ts)) ;
     p_toGui("%RegGtapName%","P","stb","tot",ts%draws%) $ p_toGui("%RegGtapName%","Q","stb","tot",ts%draws%)
           = p_toGui("%RegGtapName%","V","stb","tot",ts%draws%)/p_toGui("%RegGtapName%","Q","stb","tot",ts%draws%) ;
  );

*
* --- inst is the paying instutions (on first position) and instp is the receiver (on second position)
*
  p_toGui("%RegGtapName%","V",inst,instp,ts%draws%)    $ (chiTr.l(inst, instp,ts) ne 0) = transfers.l(inst,instp,ts) ;
  p_toGui("%RegGtapName%","V",inst,"govAcc",ts%draws%)      = sum(mapInst(gov,instp),p_toGui("%RegGtapName%","V",inst,instp,ts%draws%));
  p_toGui("%RegGtapName%","V",inst,"entAcc",ts%draws%)      = sum(mapInst(entr,instp),p_toGui("%RegGtapName%","V",inst,instp,ts%draws%));
  p_toGui("%RegGtapName%","V","govAcc",instp,ts%draws%)     = sum(mapInst(gov,inst),p_toGui("%RegGtapName%","V",inst,instp,ts%draws%));
  p_toGui("%RegGtapName%","V","govAcc","entAcc",ts%draws%)  = sum(mapInst(entr,instp),p_toGui("%RegGtapName%","V","govAcc",instp,ts%draws%));
  p_toGui("%RegGtapName%","V","entAcc",instp,ts%draws%)     = sum(mapInst(entr,inst),p_toGui("%RegGtapName%","V",inst,instp,ts%draws%));
  p_toGui("%RegGtapName%","V","entAcc","govAcc",ts%draws%)  = sum(mapInst(gov,instp),p_toGui("%RegGtapName%","V","entAcc",instp,ts%draws%));

  p_toGui("%RegGtapName%","V","rowAcc","entAcc",ts%draws%) = sum(mapInst(row,inst),p_toGui("%RegGtapName%","V",inst,"entAcc",ts%draws%));
  p_toGui("%RegGtapName%","V","rowAcc","govAcc",ts%draws%) = sum(mapInst(row,inst),p_toGui("%RegGtapName%","V",inst,"govAcc",ts%draws%));
  p_toGui("%RegGtapName%","V","rowAcc",h,ts%draws%)        = sum(mapInst(row,inst),p_toGui("%RegGtapName%","V",inst,h,ts%draws%));
  p_toGui("%RegGtapName%","V",inst,dinst,ts%draws%) = chiTr.l(inst, dinst,ts) * totTr.l(dinst,ts) ;
*
* --- transfers from RoW
*
  p_toGui("%RegGtapName%","V",inst,row,ts%draws%)          = er.l(ts)*rowtransfers.l(inst,ts);
  p_toGui("%RegGtapName%","V",inst,"rowAcc",ts%draws%)     = er.l(ts)*rowtransfers.l(inst,ts);
  p_toGui("%RegGtapName%","V","govAcc","rowAcc",ts%draws%) = sum(mapInst(gov,inst),er.l(ts)*rowtransfers.l(inst,ts));
  p_toGui("%RegGtapName%","V","entAcc","rowAcc",ts%draws%) = sum(mapInst(entr,inst),er.l(ts)*rowtransfers.l(inst,ts));
*
* --- Armington demands
*
  p_toGui("%RegGtapName%","V",iAct,aa,ts%draws%) $ xa.l(iAct,aa,ts)   = p_chiPA(iAct)*paf.l(iAct,aa,ts)*xa.l(iAct,aa,ts) ;
  p_toGui("%RegGtapName%","Q",iAct,aa,ts%draws%) $ (xa.l(iAct,aa,ts) gt 1.E-20)    = xa.l(iAct,aa,ts) ;
  p_toGui("%RegGtapName%","P",iAct,aa,ts%draws%) $  p_toGui("%RegGtapName%","Q",iAct,aa,ts%draws%)
      = p_toGui("%RegGtapName%","V",iAct,aa,ts%draws%)/p_toGui("%RegGtapName%","Q",iAct,aa,ts%draws%);
  p_toGui("%RegGtapName%","T",iAct,aa,ts%draws%) $ xa.l(iAct,aa,ts) = tPaf.l(iAct,aa,ts);
  p_toGui("%RegGtapName%","G",iAct,aa,ts%draws%) $ xa.l(iAct,aa,ts) = p_toGui("%RegGtapName%","V",iAct,aa,ts%draws%)/(1+tPaf.l(iAct,aa,ts))*tPaf.l(iAct,aa,ts);

  $$iftheni.nutrients defined nutCont
     p_toGui("%RegGtapName%","Q",nutr,h,ts%draws%) = sum(iAct $ xa.l(iAct,h,ts),        xa.l(iAct,h,ts)*nutCont(iAct,nutr))/hpop.l(h,ts)
                                                   + sum((iAct,gov) $ xa.l(iAct,gov,ts),xa.l(iAct,gov,ts)*nutCont(iAct,nutr))/pop.l("PTOTL",ts);
  $$endif.nutrients

  $$iftheni.IO not "%reducedOutput%"=="on"
    p_toGui("%RegGtapName%","S",iAct,aact,ts%draws%) $ p_toGui("%RegGtapName%","Q",iAct,aact,ts%draws%)
       = sum((sameas(fpNestI,iAct),v),lambdaxfac.l(fpNestI,aact,v,ts)*xfac.l(fpNestI,aact,v,ts))/p_toGui("%RegGtapName%","Q",iAct,aact,ts%draws%);
  $$endif.IO
*
* --- aggregate to ND (beware, the VA nest might be differently defined, for instance, comprising energy, and be subject to shifters)
*
  p_toGui("%RegGtapName%",VG,"nd-agg",aact,ts%draws%) = sum(iAct,p_toGui("%RegGtapName%",VG,iAct,aact,ts%draws%));
  p_toGui("%RegGtapName%","Q","nd-agg",aact,ts%draws%) = sum(iAct $ sam(iact,aact),p_toGui("%RegGtapName%","Q",iAct,aact,ts%draws%)*p_toGui("%RegGtapName%","P",iAct,aact,t0%draws%));
  p_toGui("%RegGtapName%","P","nd-agg",aact,ts%draws%) $ p_toGui("%RegGtapName%","Q","nd-agg",aact,ts%draws%)
   = p_toGui("%RegGtapName%","V","nd-agg",aact,ts%draws%)/p_toGui("%RegGtapName%","Q","nd-agg",aact,ts%draws%);
  p_toGui("%RegGtapName%","T","nd-agg",aact,ts%draws%) $ p_toGui("%RegGtapName%","G","nd-agg",aact,ts%draws%)
   = p_toGui("%RegGtapName%","V","nd-agg",aact,ts%draws%)/p_toGui("%RegGtapName%","G","nd-agg",aact,ts%draws%);
*
* --- stock changes
*
  if ( ifDelst,

    p_toGui("%RegGtapName%","V",ist,"stb",ts%draws%)    = p_chiPS(ist)*ps.l(ist,ts)*delst.l(ist,ts) + er.l(ts)*pwm.l(ist,ts)*(1+timp.l(ist,ts))*mdelst.l(ist,ts) ;
    p_toGui("%RegGtapName%","Q",ist,"stb",ts%draws%)    = delst.l(ist,ts) + mdelst.l(ist,ts) ;
    p_toGui("%RegGtapName%","P",ist,"stb",ts%draws%) $  p_toGui("%RegGtapName%","Q",ist,"stb",ts%draws%)
        = p_toGui("%RegGtapName%","V",ist,"stb",ts%draws%)/p_toGui("%RegGtapName%","Q",ist,"stb",ts%draws%) ;

    p_toGui("%RegGtapName%","V","tot","stb",ts%draws%)    = sum(ist,p_chiPS(ist)*ps.l(ist,ts)*delst.l(ist,ts) + er.l(ts)*pwm.l(ist,ts)*(1+timp.l(ist,ts))*mdelst.l(ist,ts)) ;
    p_toGui("%RegGtapName%","Q","tot","stb",ts%draws%)    = sum(ist,delst.l(ist,ts) + mdelst.l(ist,ts)) ;
    p_toGui("%RegGtapName%","P","tot","stb",ts%draws%) $  p_toGui("%RegGtapName%","Q","tot","stb",ts%draws%)
        = p_toGui("%RegGtapName%","V","tot","stb",ts%draws%)/p_toGui("%RegGtapName%","Q","tot","stb",ts%draws%) ;

  );
*
* --- Export demand
*
  p_toGui("%RegGtapName%","V",iExp,row,ts%draws%)    = er.l(ts)*p_chiPWE(iExp)*pwe.l(iExp,ts)*xe.l(iExp,ts) ;
*
* --- debt flows
*
  p_toGui("%RegGtapName%","V",instfd,debt,ts%draws%) =  debtPay.l(instfd,ts);
  p_toGui("%RegGtapName%","V",debt,instfd,ts%draws%) =  debtNew.l(instfd,ts)  ;
  p_toGui("%RegGtapName%","V",debt,gov,ts%draws%) =  sum(instfd,debtPay.l(instfd,ts))  ;
  p_toGui("%RegGtapName%","V",gov,debt,ts%draws%) =  debtNewT.l(ts)  ;
*
* --- total value of investments and related quantity and price indicates
*
  p_toGui("%RegGtapName%","V",ginv,gov,ts%draws%) =  yf.l(ginv,ts)  + max(govBalance.l(ts),0);
  p_toGui("%RegGtapName%","Q",ginv,gov,ts%draws%) =  xfd.l(ginv,ts) + max(govBalance.l(ts),0)/sum(inv,pf.l(inv,ts));
  p_toGui("%RegGtapName%","P",ginv,gov,ts%draws%) $  p_toGui("%RegGtapName%","Q",ginv,gov,ts%draws%)
    =  p_toGui("%RegGtapName%","V",ginv,gov,ts%draws%)/ p_toGui("%RegGtapName%","Q",ginv,gov,ts%draws%);

  p_toGui("%RegGtapName%","V",adpinv,gov,ts%draws%) $ map_adpInv(gov,adpinv) =  yf.l(adpinv,ts)  ;
  p_toGui("%RegGtapName%","Q",adpinv,gov,ts%draws%) $ map_adpInv(gov,adpinv) =  sum(i,  p_toGui("%RegGtapName%","Q",i,adpInv,ts%draws%));

  p_toGui("%RegGtapName%","P",adpinv,gov,ts%draws%) $ (map_adpInv(gov,adpinv) $ p_toGui("%RegGtapName%","Q",adpinv,gov,ts%draws%))
   = p_toGui("%RegGtapName%","Q",adpinv,gov,ts%draws%)/p_toGui("%RegGtapName%","Q",adpinv,gov,ts%draws%);

  p_toGui("%RegGtapName%","V",inv,adpinv,ts%draws%) $ map_adpInv(inv,adpinv) =  - yf.l(adpinv,ts)  ;
  p_toGui("%RegGtapName%","Q",inv,adpinv,ts%draws%) $ map_adpInv(inv,adpinv) =  sum(i,  p_toGui("%RegGtapName%","Q",i,adpInv,ts%draws%));
  p_toGui("%RegGtapName%","P",inv,adpinv,ts%draws%) $ (map_adpInv(inv,adpinv) $ p_toGui("%RegGtapName%","Q",inv,adpinv,ts%draws%))
   = p_toGui("%RegGtapName%","Q",inv,adpinv,ts%draws%)/p_toGui("%RegGtapName%","Q",inv,adpinv,ts%draws%);

*
* --- GDP at market prices
*
  p_toGui("%RegGtapName%","Q","ginv","tot",ts%draws%)  = sum(ginv,p_toGui("%RegGtapName%","Q",ginv,gov,ts%draws%));
  p_toGui("%RegGtapName%","V","ginv","tot",ts%draws%)  = sum(ginv,p_toGui("%RegGtapName%","V",ginv,gov,ts%draws%));
  p_toGui("%RegGtapName%","V","pinv","tot",ts%draws%)
    = sum(inv,yf.l(inv,ts)-max(govBalance.l(ts),0));
  p_toGui("%RegGtapName%","Q","pinv","tot",ts%draws%)
    = sum(inv,xfd.l(inv,ts))-max(govBalance.l(ts),0)/sum(inv,pf.l(inv,ts));
  p_toGui("%RegGtapName%","Q","gdpMp","tot",ts%draws%) = rgdpmp.l(ts);
  p_toGui("%RegGtapName%","P","gdpMp","tot",ts%draws%) = pgdpmp.l(ts);
  p_toGui("%RegGtapName%","V","gdpMp","tot",ts%draws%) = gdpmp.l(ts);
  p_toGui("%RegGtapName%","V",iAct,"gdpMp",ts%draws%)     =   sum(oa,xa.l(iAct,oa,ts)*p_chiPA(iAct)*paf.l(iAct,oa,ts))
                                                         - er.l(ts) * sum(iImp(iAct), pwm.l(iImp,ts) * (xm.l(iImp,ts) + mdelst.l(iImp,ts)))
                                                         + er.l(ts) * sum(iexp(iAct), p_chiPWE(iExp)*pwe.l(iExp,ts) * xe.l(iExp,ts));

  p_toGui("%RegGtapName%","Q",iAct,"gdpMp",ts%draws%)     =   sum(oa,xa.l(iAct,oa,ts)*p_chiPA(iAct)*paf.l(iAct,oa,t0))
                                                         - er.l(t0) * sum(iImp(iAct), pwm.l(iImp,t0) * (xm.l(iImp,ts) + mdelst.l(iImp,ts)))
                                                         + er.l(t0) * sum(iexp(iAct), p_chiPWE(iExp)*pwe.l(iExp,t0) * xe.l(iExp,ts));

*
* --- scale to match real GDP
*
  p_toGui("%RegGtapName%","Q",iAct,"gdpMp",ts%draws%) $ p_toGui("%RegGtapName%","Q","gdpMp",iAct,ts%draws%)
     =  p_toGui("%RegGtapName%","Q",iAct,"gdpMp",ts%draws%) * p_toGui("%RegGtapName%","Q","gdpMp","tot",ts%draws%)
                                                          /sum(j,p_toGui("%RegGtapName%","Q",j,"gdpMp",ts%draws%));

  p_toGui("%RegGtapName%","P",iAct,"gdpMp",ts%draws%) $  p_toGui("%RegGtapName%","Q",iAct,"gdpMp",ts%draws%)
                                                       = p_toGui("%RegGtapName%","V",iAct,"gdpMp",ts%draws%)
                                                        /p_toGui("%RegGtapName%","Q",iAct,"gdpMp",ts%draws%);
*
* --- report for endogenous technical change
*
  $$iftheni.endotech "%endoTech%"=="on"

     p_toGui("%RegGtapName%","Q","inn",  aAct,ts%draws%) = p_inn(aAct,ts);
     p_toGui("%RegGtapName%","Q","xrd",  aAct,ts%draws%) = xrd.l(aAct,ts);
     p_toGui("%RegGtapName%","Q","nVar", aAct,ts%draws%) = p_variety(aAct,ts);
     p_toGui("%RegGtapName%","Q","obsol",aAct,ts%draws%) = p_obsol(aAct,ts);
     p_toGui("%RegGtapName%","Q","tpRd", aAct,ts%draws%) = [xp.l(aAct,ts)/xp.l(aAct,t0)]
                                                             /[sum((fpTop,vp),xfac.l(fpTop,aact,vp,ts))
                                                             / sum((fpTop,vp),xfac.l(fpTop,aact,vp,t0))];
*
*    --- aggregate to group of products: first aggregate xp.l (use as weights),
*        aggregate single positions and finally delete aggregate xp.l (not used in reporting part)
*
     p_toGui("%RegGtapName%","Q","xp",agrpp,ts%draws%) = sum(agrpp_a(agrpp,aact),xp.l(aAct,ts));
     p_toGui("%RegGtapName%","Q",aggEndoTech,agrpp,ts%draws%) $ p_toGui("%RegGtapName%","Q","xp",agrpp,ts%draws%)
      = sum(agrpp_a(agrpp,aact),p_toGui("%RegGtapName%","Q",aggEndoTech,aact,ts%draws%)*xp.l(aAct,ts))
       /p_toGui("%RegGtapName%","Q","xp",agrpp,ts%draws%);
     p_toGui("%RegGtapName%","Q","xp",agrpp,ts%draws%) = 0;

  $$endif.endotech
*
* --- skip tech nest when summing up: will imply double counting
*
  isAdd(is) = YES; isAdd(fpPar) = no; isADd(is) $ sum(sameas(is,em),1) = no; isAdd("va-agg") = no;
  isAdd(is)  $ sum(sameas(is,kNest),1)  = no;
  isExl(is)  $ sum(sameas(is,actFmp),1) = yes;

  p_toGui("%RegGtapName%","Q","Tot",is,ts%draws%) $ (not isExl(is))
   = sum(isAdd $ p_toGui("%RegGtapName%","Q",isAdd,is,ts%draws%),p_toGui("%RegGtapName%","Q",isAdd,is,ts%draws%));

  p_toGui("%RegGtapName%",VG,"Tot",is,ts%draws%)  $ ((not isExl(is)) $ p_toGui("%RegGtapName%","Q","Tot",is,ts%draws%))
   = sum(isAdd $ p_toGui("%RegGtapName%",VG,isAdd,is,ts%draws%),p_toGui("%RegGtapName%",VG,isAdd,is,ts%draws%));

  p_toGui("%RegGtapName%","P","Tot",is,ts%draws%) $ p_toGui("%RegGtapName%","Q","Tot",is,ts%draws%)
   = p_toGui("%RegGtapName%","V","Tot",is,ts%draws%)/p_toGui("%RegGtapName%","Q","Tot",is,ts%draws%);

  option kill=aggFlag;
  aggFlag(VQG,is,js) $ p_toGUi("%RegGtapName%","Q",is,js,t0%draws%) = yes;

  p_toGui("%RegGtapName%","G",is,"Tot",ts%draws%)  = sum(aggFlag("G",is,js),p_toGui("%RegGtapName%","G",is,js,ts%draws%));

  p_toGui("%RegGtapName%","V",is,"Tot",ts%draws%)
    = sum(aggFlag("Q",is,js),p_toGui("%RegGtapName%","Q",is,js,ts%draws%)*p_toGui("%RegGtapName%","P",is,js,ts%draws%));

  p_toGui("%RegGtapName%","Q",is,"Tot",ts%draws%) $ (Not p_toGui("%RegGtapName%","Q",is,"Tot",ts%draws%))
    = sum(aggFlag("Q",is,js) $ p_toGui("%RegGtapName%","Q",is,js,ts%draws%),
          p_toGui("%RegGtapName%","Q",is,js,ts%draws%)*p_toGui("%RegGtapName%","P",is,js,t0%draws%));

  p_toGui("%RegGtapName%","P",is,"Tot",ts%draws%) $ p_toGui("%RegGtapName%","Q",is,"Tot",ts%draws%)
   = p_toGui("%RegGtapName%","V",is,"Tot",ts%draws%)/p_toGui("%RegGtapName%","Q",is,"Tot",ts%draws%);
*
* --- aggregate products to group of products and total
*
  option kill=aggFlag;
  toAggIC(stb) $ (not ifDelst) = no;

  aggFlag("Q",cgrp,toAggIC) = sum(cgrp_c(cgrp,i) $ p_toGui("%RegGtapName%","Q",i,toAggIC,t0%draws%),1);
  $$iftheni.IO "%reducedOutput%"=="on"
     aggFlag("Q",cgrp,iAct) = 0;
     aggFlag("Q",cgrp,aa)   = 0;
  $$endif.IO
  aggFlag(VQG,cgrp,toAggIC) =  aggFlag("Q",cgrp,toAggIC);
  p_toGui("%RegGtapName%",aggFlag(VG,cgrp,toAggIC),ts%draws%)
     = sum(cgrp_c(cgrp,i),p_toGui("%RegGtapName%",VG,i,toAggIC,ts%draws%));
  p_toGui("%RegGtapName%",aggFlag("Q",cgrp,toAggIC),ts%draws%)
     = sum(cgrp_c(cgrp,i) $ p_toGui("%RegGtapName%","Q",i,toAggIC,ts%draws%),
          p_toGui("%RegGtapName%","Q",i,toAggIC,ts%draws%)*p_toGui("%RegGtapName%","P",i,toAggIC,t0%draws%));

  p_toGui("%RegGtapName%","P",cgrp,toAggIC,ts%draws%) $ p_toGui("%RegGtapName%","Q",cgrp,toAggIC,ts%draws%)
    = p_toGui("%RegGtapName%","V",cgrp,toAGGIC,ts%draws%)/ p_toGui("%RegGtapName%","Q",cgrp,toAggIC,ts%draws%);


  $$iftheni.IO not "%reducedOutput%"=="on"
     p_toGui("%RegGtapName%","S",cgrp,aAct,ts%draws%) $ p_toGui("%RegGtapName%","Q",cgrp,aAct,ts%draws%)
      = sum(cgrp_c(cgrp,iAct) $ SAM(iAct,aact),
             p_toGui("%RegGtapName%","Q",iAct,aact,ts%draws%)*p_toGui("%RegGtapName%","S",iAct,aact,ts%draws%))/ p_toGui("%RegGtapName%","Q",cgrp,aact,ts%draws%);
  $$endif.io

  p_toGui("%RegGtapName%","T",cgrp,toAggIC,ts%draws%) $ p_toGui("%RegGtapName%","V",cgrp,toAggIC,ts%draws%)
    = p_toGui("%RegGtapName%","G",cgrp,toAGGIC,ts%draws%)/ p_toGui("%RegGtapName%","V",cgrp,toAggIC,ts%draws%);
*
* --- aggregate products to group of products and total
*
  option kill=aggFlag;
  aggFlag("Q",toAggIr,cgrp) = sum(cgrp_c(cgrp,iAct) $ p_toGui("%RegGtapName%","Q",toAggIR,iAct,t0%draws%),1);
  aggFlag(VQG,toAggIr,cgrp) = aggFlag("Q",toAggIr,cgrp);

  p_toGui("%RegGtapName%",aggFlag(VG,toAggIR,cgrp),ts%draws%)
    = sum(cgrp_c(cgrp,iAct) $ p_toGui("%RegGtapName%",VG,toAggIR,iAct,ts%draws%),p_toGui("%RegGtapName%",VG,toAggIR,iAct,ts%draws%));

  p_toGui("%RegGtapName%",aggFlag("Q",toAggIR,cgrp),ts%draws%)
    = sum(cgrp_c(cgrp,iAct) $ p_toGui("%RegGtapName%","Q",toAggIR,iAct,ts%draws%), p_toGui("%RegGtapName%","Q",toAggIR,iAct,ts%draws%)
                                                                                  *p_toGui("%RegGtapName%","P",toAggIR,iAct,t0%draws%));

  p_toGui("%RegGtapName%","P",toAggIR,cgrp,ts%draws%) $ p_toGui("%RegGtapName%","Q",toAggIR,cgrp,ts%draws%)
    = p_toGui("%RegGtapName%","V",toAggIR,cgrp,ts%draws%)/ p_toGui("%RegGtapName%","Q",toAggIR,cgrp,ts%draws%);
  p_toGui("%RegGtapName%","T",toAggIR,cgrp,ts%draws%) $ p_toGui("%RegGtapName%","V",toAggIR,cgrp,ts%draws%)
    = p_toGui("%RegGtapName%","G",toAggIR,cgrp,ts%draws%)/ p_toGui("%RegGtapName%","V",toAggIR,cgrp,ts%draws%);

  p_toGui("%RegGtapName%","Q","gdp-cont",aact,ts%draws%)
   = p_toGui("%RegGtapName%","Q",aact,"c-sum",ts%draws%)-p_toGui("%RegGtapName%","Q","nd-agg",aact,ts%draws%);
  p_toGui("%RegGtapName%","P","gdp-cont",aact,ts%draws%) = 1;
  p_toGui("%RegGtapName%","V","gdp-cont",aact,ts%draws%) = p_toGui("%RegGtapName%","Q","gdp-cont",aact,ts%draws%);

*
* --- aggregate activities to group of activities and total
*
  option kill=aggFlag;
  aggFlag("V",toAggaR,agrpp) = sum(agrpp_a(agrpp,aact) $ p_toGui("%RegGtapName%","V",toAggAR,aact,t0%draws%),1);
  aggFlag(VQG,toAggaR,agrpp) = aggFlag("V",toAggaR,agrpp);
  $$iftheni.IO "%reducedOutput%"=="on"
     aggFlag(VQG,cgrp,agrpp) = 0;
  $$endif.IO

  p_toGui("%RegGtapName%",aggFlag(VG,toAggaR,agrpp),ts%draws%)
    = sum(agrpp_a(agrpp,aact),p_toGui("%RegGtapName%",VG,toAggAR,aact,ts%draws%));

  p_toGui("%RegGtapName%",aggFlag("Q",toAggaR,agrpp),ts%draws%)
    = sum(agrpp_a(agrpp,aact),p_toGui("%RegGtapName%","Q",toAggAR,aact,ts%draws%)*p_toGui("%RegGtapName%","P",toAggAR,aact,t0%draws%));

  p_toGui("%RegGtapName%","S",toAggaR,agrpp,ts%draws%) $ p_toGui("%RegGtapName%","Q",toAggaR,agrpp,ts%draws%)
    =  sum(agrpp_a(agrpp,aact),
              p_toGui("%RegGtapName%","Q",toAggAR,aact,ts%draws%)*p_toGui("%RegGtapName%","S",toAggaR,aact,ts%draws%))/p_toGui("%RegGtapName%","Q",toAggaR,agrpp,ts%draws%);

  p_toGui("%RegGtapName%","P",toAggaR,agrpp,ts%draws%) $ p_toGui("%RegGtapName%","Q",toAggaR,agrpp,ts%draws%)
    = p_toGui("%RegGtapName%","V",toAggaR,agrpp,ts%draws%)/ p_toGui("%RegGtapName%","Q",toAggaR,agrpp,ts%draws%);

  p_toGui("%RegGtapName%","T",toAggaR,agrpp,ts%draws%) $ p_toGui("%RegGtapName%","V",toAggaR,agrpp,ts%draws%)
    = p_toGui("%RegGtapName%","G",toAggaR,agrpp,ts%draws%)/ p_toGui("%RegGtapName%","V",toAggaR,agrpp,ts%draws%);

  option kill=aggFlag;
  aggFlag("Q",agrp,toAggAC) = sum(agrp_a(agrp,aact) $ p_toGui("%RegGtapName%","Q",aact,toAggAC,t0%draws%),1);
  aggFlag(VQG,agrp,toAggAC) = aggFlag("Q",agrp,toAggAC);
  p_toGui("%RegGtapName%",aggFlag(VG,agrp,toAggAC),ts%draws%)
    = sum(agrp_a(agrp,aact),p_toGui("%RegGtapName%",VG,aact,toAggAC,ts%draws%));

  p_toGui("%RegGtapName%",aggFlag("Q",agrp,toAggAC),ts%draws%)
    = sum(agrp_a(agrp,aact),p_toGui("%RegGtapName%","Q",aact,toAggAC,ts%draws%)*p_toGui("%RegGtapName%","P",aact,toAggAC,t0%draws%));

  p_toGui("%RegGtapName%","P",agrp,toAggAC,ts%draws%) $ p_toGui("%RegGtapName%","Q",agrp,toAggAC,ts%draws%)
    = p_toGui("%RegGtapName%","V",agrp,toAggAC,ts%draws%)/ p_toGui("%RegGtapName%","Q",agrp,toAggAC,ts%draws%);

  p_toGui("%RegGtapName%","T",agrp,toAggAC,ts%draws%) $ p_toGui("%RegGtapName%","G",agrp,toAggAC,ts%draws%)
    = p_toGui("%RegGtapName%","G",agrp,toAggAC,ts%draws%)/ p_toGui("%RegGtapName%","V",agrp,toAggAC,ts%draws%);
*
* --- GDP at factor cost
*
  p_toGui("%RegGtapName%","Q","gdpFC","tot",ts%draws%) = rgdpfc.l(ts);
  p_toGui("%RegGtapName%","P","gdpFC","tot",ts%draws%) = pgdpfc.l(ts);
  p_toGui("%RegGtapName%","V","gdpFC","tot",ts%draws%) = gdpfc.l(ts);
*
* --- exports
*
  p_toGui("%RegGtapName%","V","exp","tot",ts%draws%)   = totExp.l(ts);
  p_toGui("%RegGtapName%","Q","exp","tot",ts%draws%)   = rtotExp.l(ts);
  p_toGui("%RegGtapName%","P","exp","tot",ts%draws%)   = pExp.l(ts);
*
* --- imports
*
  p_toGui("%RegGtapName%","V","imp","tot",ts%draws%)   = totImp.l(ts);
  p_toGui("%RegGtapName%","Q","imp","tot",ts%draws%)   = rtotImp.l(ts);
  p_toGui("%RegGtapName%","P","imp","tot",ts%draws%)   = pimp.l(ts);
*
*
  p_toGui("%RegGtapName%","V","bot","tot",ts%draws%)   = bot.l(ts);
  p_toGui("%RegGtapName%","Q","bot","tot",ts%draws%)   = rtotexp.l(ts)- rtotimp.l(ts);
*
* --- Money metric, utility
*
  p_toGui("%RegGtapName%","V","pop",h,ts%draws%)         = hpop.l(h,ts);
  p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)    = sum(h,hpop.l(h,ts));
  p_toGui("%RegGtapName%","V","years","tot",ts%draws%)   = years(ts);
  p_toGui("%RegGtapName%","V","mmuh",h,ts%draws%)        = mmuh.l(h,ts);
  p_toGui("%RegGtapName%","V","uh",h,ts%draws%)          = uh.l(h,ts);


* --- household and government consumption, investments
*
  p_toGui("%RegGtapName%","V","yf","tot",ts%draws%)     = sum(h,yf.l(h,ts));
  p_toGui("%RegGtapName%","P","yf","tot",ts%draws%)     = sum(h,yf.l(h,ts)*pf.l(h,ts))/sum(h,yf.l(h,ts));
  p_toGui("%RegGtapName%","Q","yf","tot",ts%draws%)     = p_toGui("%RegGtapName%","V","yf","tot",ts%draws%)/p_toGui("%RegGtapName%","P","yf","tot",ts%draws%);

  p_toGui("%RegGtapName%","V","yf",h,ts%draws%)         = yf.l(h,ts);
  p_toGui("%RegGtapName%","P","yf",h,ts%draws%)         = pf.l(h,ts);
  p_toGui("%RegGtapName%","Q","yf",h,ts%draws%)         = p_toGui("%RegGtapName%","V","yf",h,ts%draws%)/p_toGui("%RegGtapName%","P","yf",h,ts%draws%);
  p_toGui("%RegGtapName%","V","govExp","tot",ts%draws%) = sum(gov,yf.l(gov,ts));
  p_toGui("%RegGtapName%","P","govExp","tot",ts%draws%) = sum(gov,pf.l(gov,ts));
  p_toGui("%RegGtapName%","Q","govExp","tot",ts%draws%) = p_toGui("%RegGtapName%","V","govExp","tot",ts%draws%)/p_toGui("%RegGtapName%","P","govExp","tot",ts%draws%);

  p_toGui("%RegGtapName%","V","ygov","tot",ts%draws%)   = ygov.l(ts);
  p_toGui("%RegGtapName%","P","ygov","tot",ts%draws%)   = sum(gov,pf.l(gov,ts));
  p_toGui("%RegGtapName%","Q","ygov","tot",ts%draws%)   = p_toGui("%RegGtapName%","V","ygov","tot",ts%draws%)/p_toGui("%RegGtapName%","P","ygov","tot",ts%draws%);
  p_toGui("%RegGtapName%","V","yinv","tot",ts%draws%)   = p_toGui("%RegGtapName%","V","ginv","tot",ts%draws%)+p_toGui("%RegGtapName%","V","pinv","tot",ts%draws%);
  p_toGui("%RegGtapName%","Q","yinv","tot",ts%draws%)   = p_toGui("%RegGtapName%","Q","ginv","tot",ts%draws%)+p_toGui("%RegGtapName%","Q","pinv","tot",ts%draws%);
  p_toGui("%RegGtapName%","P","yinv","tot",ts%draws%)   = p_toGui("%RegGtapName%","V","yinv","tot",ts%draws%)/p_toGui("%RegGtapName%","Q","yinv","tot",ts%draws%);
  p_toGui("%RegGtapName%","V","tdelst","tot",ts%draws%)  = tdelst.l(ts);
*
* --- savings
*
* p_toGui("%RegGtapName%","V","savg","tot",ts%draws%)   = savg.l(ts);
  p_toGui("%RegGtapName%","V","savh","tot",ts%draws%)   = sum(h,savh.l(h,ts));
  p_toGui("%RegGtapName%","V","savf","tot",ts%draws%)         = er.l(ts)*(savf.l(ts)-fundf.l(ts));
  p_toGui("%RegGtapName%","V","fundf","tot",ts%draws%)        = er.l(ts)*fundf.l(ts);
  p_toGui("%RegGtapName%","V","fDebtPay","tot",ts%draws%)     = sum(instf, debtPay.l(instf,ts));
  p_toGui("%RegGtapName%","V","fNetTrans","tot",ts%draws%)   = -p_toGui("%RegGtapName%","V","bot","tot",ts%draws%)
                                                               -p_toGui("%RegGtapName%","V","savf","tot",ts%draws%)
                                                               -p_toGui("%RegGtapName%","V","fundf","tot",ts%draws%)
                                                               +p_toGui("%RegGtapName%","V","fdebtPay","tot",ts%draws%);

  p_toGui("%RegGtapName%","V","save","tot",ts%draws%)   = sum(entr,savEntr.l(entr,ts));
  p_toGui("%RegGtapName%","V","sav","tot",ts%draws%)    =        + p_toGui("%RegGtapName%","V","savh","tot",ts%draws%)
                                                          + p_toGui("%RegGtapName%","V","save","tot",ts%draws%)
                                                          + p_toGui("%RegGtapName%","V","savf","tot",ts%draws%);
*
* --- household accounts
*
  p_toGui("%RegGtapName%","V","income",h,ts%draws%)           = yh.l(h,ts);
  p_toGui("%RegGtapName%","V","transY",h,ts%draws%)           = sum((inst,instp)$mapinst(h,instp), transfers.l(instp, inst,ts));
  p_toGui("%RegGtapName%","V","debtPay",h,ts%draws%)          = SUM(mapinst(h,inst),debtPay.l(inst,ts));
  p_toGui("%RegGtapName%","V","princPay",h,ts%draws%)         = SUM(mapinst(h,inst),debtPrincipal.l(inst,ts));
  p_toGui("%RegGtapName%","V","intPay",h,ts%draws%)           = SUM(mapinst(h,inst),debtInterest.l(inst,ts));
  p_toGui("%RegGtapName%","V","savh",h,ts%draws%)             = savh.l(h,ts);
  p_toGui("%RegGtapName%","V","consum",h,ts%draws%)           = yf.l(h,ts);
  p_toGui("%RegGtapName%","V","transS",h,ts%draws%)           = sum(inst$mapinst(h,inst), totTr.l(inst,ts));
  p_toGui("%RegGtapName%","V","tax",h,ts%draws%)              = yh.l(h,ts)*tPIT.l(h,ts);
  p_toGui("%RegGtapName%","V","borrow",h,ts%draws%)           = SUM(mapInst(h,inst),debtNew.l(inst,ts));
*
* --- enterprise account
*
  p_toGui("%RegGtapName%","V","entrY","entAcc",ts%draws%)     = sum(entr,entry.l(entr,ts));
  p_toGui("%RegGtapName%","V","transY","entAcc",ts%draws%)    = sum((entr,instp,inst)$mapinst(entr,instp), transfers.l(instp, inst,ts));
  p_toGui("%RegGtapName%","V","transS","entAcc",ts%draws%)    = SUM(mapInst(entr,inst),tottr.l(inst,ts));
  p_toGui("%RegGtapName%","V","facty","entAcc",ts%draws%)     = sum(mapInst(entr,inst),p_toGui("%RegGtapName%","V",inst,"factY",ts%draws%));
  p_toGui("%RegGtapName%","V","debtPay","entAcc",ts%draws%)   = SUM(mapinst(entr,inst),debtPay.l(inst,ts));
  p_toGui("%RegGtapName%","V","princPay","entAcc",ts%draws%)  = SUM(mapinst(entr,inst),debtPrincipal.l(inst,ts));
  p_toGui("%RegGtapName%","V","intPay","entAcc",ts%draws%)    = SUM(mapinst(entr,inst),debtInterest.l(inst,ts));
  p_toGui("%RegGtapName%","V","debtPay","rowAcc",ts%draws%)   = SUM(mapInst(row,inst),debtPay.l(inst,ts));
  p_toGui("%RegGtapName%","V","princPay","rowAcc",ts%draws%)  = SUM(mapinst(row,inst),debtPrincipal.l(inst,ts));
  p_toGui("%RegGtapName%","V","intPay","rowAcc",ts%draws%)    = SUM(mapinst(row,inst),debtInterest.l(inst,ts));
  p_toGui("%RegGtapName%","V","save","entAcc",ts%draws%)      = sum(entr,savEntr.l(entr,ts));
  p_toGui("%RegGtapName%","V","tax","entAcc",ts%draws%)       = sum(entr,entry.l(entr,ts)*tCit.l(entr,ts));
  p_toGui("%RegGtapName%","V","borrow","entAcc",ts%draws%)    = SUM(mapInst(entr,inst),debtNew.l(inst,ts));
*
* --- government account
*
  p_toGui("%RegGtapName%","V","transS","govAcc",ts%draws%)   = SUM(mapInst(gov,inst),tottr.l(inst,ts));
  p_toGui("%RegGtapName%","V","transY","govAcc",ts%draws%)   = sum((mapInst(gov,instp),inst), transfers.l(instp, inst,ts));
  p_toGui("%RegGtapName%","V","transY","rowAcc",ts%draws%)   = sum((mapInst(row,instp),inst), transfers.l(instp, inst,ts));
  p_toGui("%RegGtapName%","V","transs","rowAcc",ts%draws%)   = sum(inst,er.l(ts)*rowtransfers.l(inst,ts));
  p_toGui("%RegGtapName%","V","consum","govAcc",ts%draws%)    = sum(gov,yf.l(gov,ts));
  p_toGui("%RegGtapName%","Q","consum","govAcc",ts%draws%)    = sum(gov,xfd.l(gov,ts));
  p_toGui("%RegGtapName%","P","consum","govAcc",ts%draws%) $ p_toGui("%RegGtapName%","Q","consum","govAcc",ts%draws%)
   = (p_toGui("%RegGtapName%","V","consum","govAcc",ts%draws%)/p_toGui("%RegGtapName%","Q","consum","govAcc",ts%draws%));

  p_toGui("%RegGtapName%","V","invest","govAcc",ts%draws%)   = SUM(ginv,yf.l(ginv,ts));
  p_toGui("%RegGtapName%","Q","invest","govAcc",ts%draws%)   = SUM(ginv,xfd.l(ginv,ts));
  p_toGui("%RegGtapName%","P","invest","govAcc",ts%draws%) $ p_toGui("%RegGtapName%","Q","invest","govAcc",ts%draws%)
   = (p_toGui("%RegGtapName%","V","invest","govAcc",ts%draws%)/p_toGui("%RegGtapName%","Q","invest","govAcc",ts%draws%));

  p_toGui("%RegGtapName%","V","debPay","govAcc",ts%draws%)   = sum(inst,debtPay.l(inst,ts));
  p_toGui("%RegGtapName%","V","princPay","govAcc",ts%draws%) = sum(inst,debtPrincipal.l(inst,ts));
  p_toGui("%RegGtapName%","V","intPay","govAcc",ts%draws%)   = sum(inst,debtinterest.l(inst,ts));
* p_toGui("%RegGtapName%","V","saving","govAcc",ts%draws%)   = savg.l(ts);
  p_toGui("%RegGtapName%","V","adpInv","govAcc",ts%draws%)   = sum(map_adpInv(gov,adpinv),yf.l(adpinv,ts));

  p_toGui("%RegGtapName%","V","taxRev","govAcc",ts%draws%)   = sum(g, yg.l(g,ts));
*
* -- calculate price index for gov revenues from consumption and investments
*
  p_toGui("%RegGtapName%","P","taxRev","govAcc",ts%draws%) $ (  p_toGui("%RegGtapName%","Q","consum","govAcc",ts%draws%)
                                                        +  p_toGui("%RegGtapName%","Q","invest","govAcc",ts%draws%))
       =   (    p_toGui("%RegGtapName%","V","consum","govAcc",ts%draws%)
             +  p_toGui("%RegGtapName%","V","invest","govAcc",ts%draws%))
       /   (    p_toGui("%RegGtapName%","Q","consum","govAcc",ts%draws%)
             +  p_toGui("%RegGtapName%","Q","invest","govAcc",ts%draws%));
  p_toGui("%RegGtapName%","Q","taxRev","govAcc",ts%draws%) $ p_toGui("%RegGtapName%","P","taxRev","govAcc",ts%draws%)
    = p_toGui("%RegGtapName%","V","taxRev","govAcc",ts%draws%)/p_toGui("%RegGtapName%","P","taxRev","govAcc",ts%draws%);


  p_toGui("%RegGtapName%","V","factY","govAcc",ts%draws%)     = sum(mapInst(gov,inst),p_toGui("%RegGtapName%","V",inst,"factY",ts%draws%));
  p_toGui("%RegGtapName%","V","factY","rowAcc",ts%draws%)     = sum(instf,p_toGui("%RegGtapName%","V",instf,"factY",ts%draws%));
  p_toGui("%RegGtapName%","V","borrow","govAcc",ts%draws%)      = SUM(inst,debtNew.l(inst,ts));
  p_toGui("%RegGtapName%","V","fiscalBal","govAcc",ts%draws%)   = fiscalBalance.l(ts);
  p_toGui("%RegGtapName%","V","primaryBal","govAcc",ts%draws%)  = primaryBalance.l(ts);
  p_toGui("%RegGtapName%","V","borrow","rowAcc",ts%draws%)   = SUM(mapInst(row,inst),debtNew.l(inst,ts));
  p_toGui("%RegGtapName%","V","savf","rowAcc",ts%draws%)     = p_toGui("%RegGtapName%","V","savf","tot",ts%draws%);
  p_toGui("%RegGtapName%","V","bot","rowAcc",ts%draws%)      = -bot.l(ts);
*
* --- debt
*
  p_toGui("%RegGtapName%","V","debtStkD","tot",ts%draws%)   = sum(instd,debtStk.l(instd,ts));
  p_toGui("%RegGtapName%","V","debtStkF","tot",ts%draws%)   = sum(instf,debtStk.l(instf,ts));
  p_toGui("%RegGtapName%","V","debtStkT","tot",ts%draws%)   = debtStkT.l(ts);
  p_toGui("%RegGtapName%","V","debtGDP","tot",ts%draws%)    = debtGDP.l(ts);
  p_toGui("%RegGtapName%","V","debtPay","tot",ts%draws%)    = sum(inst,debtPay.l(inst,ts));
  p_toGui("%RegGtapName%","V","debtNewT","tot",ts%draws%)   = debtNewT.l(ts);
  p_toGui("%RegGtapName%","V","rorc","tot",ts%draws%)       = rorc.l(ts);
  p_toGui("%RegGtapName%","V","fundPrice","tot",ts%draws%)  = fundPrice.l(ts);
  p_toGui("%RegGtapName%","V","interest","tot",ts%draws%)   = interest.l(ts);
  p_toGui("%RegGtapName%","V","debtIntF","tot",ts%draws%)   = interest.l(ts)*sum(instf,riskPrem.l(instf,ts));
  p_toGui("%RegGtapName%","V","debtIntD","tot",ts%draws%)   = interest.l(ts)*sum(instd,riskPrem.l(instd,ts));
  p_toGui("%RegGtapName%","V","rDebtStkD","tot",ts%draws%)  = sum(instd,rDebtStk.l(instd,ts));
  p_toGui("%RegGtapName%","V","rDebtStkF","tot",ts%draws%)  = sum(instf,rDebtStk.l(instf,ts));
*
* --- tax flows to government account
*
  p_toGui("%RegGtapName%","V",g,"govAcc",ts%draws%)   = yg.l(g,ts);
*
* --- Capital stock and depreciation
*
  p_toGui("%RegGtapName%","V","Kstock","tot",ts%draws%)    = kstock.l(ts);
  p_toGui("%RegGtapName%","V","depr","tot",ts%draws%)      = kstock.l(ts)*depr.l(ts) ;
  p_toGui("%RegGtapName%","V","deprRate","tot",ts%draws%)  = kstock.l(ts)*depr.l(ts)/p_toGui("%RegGtapName%","V","Kstock","tot",ts%draws%);
  p_toGui("%RegGtapName%","V","trentAvg","tot",ts%draws%)  = trentAvg.l(ts);

  $$ondotl
  p_toGui("%RegGtapName%","Q","depr",a,ts%draws%) = sum((cap,v,t) $ sameas(t,ts),xfac.l(cap,a,v,ts)*deprA.l(a,ts)*m_dmgAgg(dmgDepr,'a')) ;
  p_toGui("%RegGtapName%","V","depr",a,ts%draws%) = p_toGui("%RegGtapName%","q","depr",a,ts%draws%)*sum(inv, pf(inv,ts));
  p_toGui("%RegGtapName%","V","depr","sum",ts%draws%) = sum(a,p_toGui("%RegGtapName%","V","depr",a,ts%draws%));

  p_toGui("%RegGtapName%","Q","depr",a,ts%draws%) = p_toGui("%RegGtapName%","Q","depr",a,ts%draws%)
    * p_toGui("%RegGtapName%","V","depr","tot",ts%draws%)/p_toGui("%RegGtapName%","V","depr","sum",ts%draws%);
  p_toGui("%RegGtapName%","V","depr",a,ts%draws%) = p_toGui("%RegGtapName%","V","depr",a,ts%draws%)
    * p_toGui("%RegGtapName%","V","depr","tot",ts%draws%)/p_toGui("%RegGtapName%","V","depr","sum",ts%draws%);
  p_toGui("%RegGtapName%","V","depr","sum",ts%draws%) = 0;

  $$offdotl
  p_toGui("%RegGtapName%",VG,"depr",agrpp,ts%draws%) = sum(agrpp_a(agrpp,a),p_toGui("%RegGtapName%",VG,"depr",a,ts%draws%));
  p_toGui("%RegGtapName%","Q","depr",agrpp,ts%draws%) = sum(agrpp_a(agrpp,a),p_toGui("%RegGtapName%","Q","depr",a,ts%draws%)*p_toGui("%RegGtapName%","P","depr",a,t0%draws%));
*
*
*
  p_toGui("%RegGtapName%","V","facty","lnd",ts%draws%) = sum(lnd,   p_toGui("%RegGtapName%","V",lnd,"tot",ts%draws%));
  p_toGui("%RegGtapName%","V","facty","cap",ts%draws%) = sum(cap,   p_toGui("%RegGtapName%","V",cap,"tot",ts%draws%));
  p_toGui("%RegGtapName%","V","facty","lab",ts%draws%) = sum(l,     p_toGui("%RegGtapName%","V",l,"tot",ts%draws%));
  p_toGui("%RegGtapName%","V","facty","nr",ts%draws%)  = sum(natres,p_toGui("%RegGtapName%","V",natres,"tot",ts%draws%));
*
* --- population
*
  p_toGui("%RegGtapName%","V",cohorts,"tot",ts%draws%)   = pop.l(cohorts,ts);
  p_toGui("%RegGtapName%","V","pop","tot",ts%draws%)     = pop.l("PTOTL",ts);
*
* --- air emissions
*
  $$iftheni.IO not "%reducedOutput%"=="on"
     p_toGui("%RegGtapName%",emiCommFlag(em,emsrc,aa),ts%draws%)                         = emiComm.l(em,emSrc,aa,ts);
     p_toGui("%RegGtapName%",em,actFp,a,ts%draws%)               $ p_rhoEmiFac(em,actFp,a) = emiFact.l(em,actFp,a,ts);
  $$endif.IO

  p_toGui("%RegGtapName%",em,"LUCF",a,ts%draws%)                                    = emiLucf.l(em,a,ts);
  p_toGui("%RegGtapName%",em,"act",aa,ts%draws%)               $ emiFlag(em,aa)     = emiSecA.l(em,aa,ts);
  p_toGui("%RegGtapName%",pollType,"act",aa,ts%draws%)                              = sum(emiFlag(em,aa),p_emiConv(em,polltype)*emiSecA.l(em,aa,ts));
  p_toGui("%RegGtapName%",em,"tot","tot",ts%draws%)                                 = emiTot.l(em,ts);
  p_toGui("%RegGtapName%",em,ghgInv,"tot",ts%draws%)                                = emiInv.l(ghgInv,em,ts);
  p_toGui("%RegGtapName%",pollType,ghgInv,"tot",ts%draws%)                          = sum(em, p_emiConv(em,polltype)*emiInv.l(ghgInv,em,ts));
*
* --- ghg emissions
*
  p_toGui("%RegGtapName%","Q",pollType,"tot",ts%draws%)   = emiGHG.l(pollType,ts);
  p_toGui("%RegGtapName%","Q",pollType,"int",ts%draws%)   = emiInt.l(pollType,ts);
  p_toGui("%RegGtapName%","Q","ghgs","tax",ts%draws%)     = tEmiD0.l(ts);
*
* --- land use change and carbon stock changes
*
  $$ifthen.aez defined aez


     p_toGui(raez,"Q",lCov,"ha",ts%draws%)   = sum(raez_aez(raez,actHa),lCovArea.l(actHa,lCov,ts));
     p_toGui(raez,"Q",lCov,"c",ts%draws%)    = sum(raez_aez(raez,actHa),lCovArea.l(actHa,lCov,ts)*carbCnt(actHa,lCov))*samScale;
     p_toGui(raez,"Q",lCov,"dc",ts%draws%)   = sum(raez_aez(raez,actHa),dCarbon.l(actHa,lCov,ts));
     p_toGui(raez,"Q","tot","ha",ts%draws%)  = sum((raez_aez(raez,actHa),lCov),lCovArea.l(actHa,lCov,ts));
     p_toGui(raez,"Q","tot","dc",ts%draws%)  = sum((raez_aez(raez,actHa),lCov),dCarbon.l(actHa,lCov,ts));
     p_toGui(raez,"Q","tot","c",ts%draws%)   = sum((raez_aez(raez,actHa),lCov),lCovArea.l(actHa,lCov,ts)*carbCnt(actHa,lCov))*samScale;

  $$endif.aez

  p_toGui("%RegGtapName%","Q",lCov,"ha",ts%draws%)  = sum(actHa,lCovArea.l(actHa,lCov,ts));
  p_toGui("%RegGtapName%","Q","tot","ha",ts%draws%) = sum((actHa,lCov),lCovArea.l(actHa,lCov,ts));
  p_toGui("%RegGtapName%","Q",lCov,"dc",ts%draws%)  = sum(actHa,dCarbon.l(actHa,lCov,ts));
  p_toGui("%RegGtapName%","Q","tot","dc",ts%draws%) = sum((actHa,lCov),dCarbon.l(actHa,lCov,ts));

*
* --- aggregate over households
*
  p_toGui("%RegGtapName%",VQG,aggH3,"htot",ts%draws%)   =  sum(h,p_toGui("%RegGtapName%",VQG,aggH3,h,ts%draws%));
  $$ifi defined nutr  p_toGui("%RegGtapName%",VQG,nutr,"htot",ts%draws%)   =  sum(h,p_toGui("%RegGtapName%",VQG,nutr,h,ts%draws%)*hPop.l(h,ts))/pop.l("ptotl",ts);

  option kill=aggFlag;
  aggFlag(VQG,"htot",aggH4) $ sum(h,p_toGui("%RegGtapName%","Q",h,aggH4,t0%draws%)+p_toGui("%RegGtapName%","V",h,aggH4,t0%draws%)) = YES;
  p_toGui("%RegGtapName%",aggFlag(VQG,"htot",aggH4),ts%draws%)   =  sum(h,p_toGui("%RegGtapName%",VQG,h,aggH4,ts%draws%));

  p_toGui("%RegGtapName%","P","htot",aggH4,ts%draws%) $ p_toGui("%RegGtapName%","Q","htot",aggH4,ts%draws%)
      = p_toGui("%RegGtapName%","V","htot",aggH4,ts%draws%)/p_toGui("%RegGtapName%","Q","htot",aggH4,ts%draws%);
  p_toGui("%RegGtapName%","T","htot",aggH4,ts%draws%) $ p_toGui("%RegGtapName%","V","htot",aggH4,ts%draws%)
      = p_toGui("%RegGtapName%","G","htot",aggH4,ts%draws%)/p_toGui("%RegGtapName%","V","htot",aggH4,ts%draws%);

  p_toGui("%RegGtapName%","P",aggH3,"htot",ts%draws%) $ p_toGui("%RegGtapName%","Q",aggH3,"htot",ts%draws%)
      = p_toGui("%RegGtapName%","V",aggH3,"htot",ts%draws%)/p_toGui("%RegGtapName%","Q",aggH3,"htot",ts%draws%);
  p_toGui("%RegGtapName%","T",aggH3,"htot",ts%draws%) $ p_toGui("%RegGtapName%","V",aggH3,"htot",ts%draws%)
      = p_toGui("%RegGtapName%","G",aggH3,"htot",ts%draws%)/p_toGui("%RegGtapName%","V",aggH3,"htot",ts%draws%);

*
* --- two items where simple aggregation does not make sense or results are overwritten
*
  p_toGui("%RegGtapName%","V","uh","htot",ts%draws%)     = sum(h,uh.l(h,ts)*hpop.l(h,ts))/sum(h,hpop.l(h,ts));
  p_toGui("%RegGtapName%","V","mmuh","htot",ts%draws%)   = sum(h,mmuh.l(h,ts)*hpop.l(h,ts))/sum(h,hpop.l(h,ts));
  p_toGui("%RegGtapName%","V","gini","htot",ts%draws%)   = gini.l(ts);
*
* --- delete detail (on demand, can be imported for stochatics to avoid very large GDX containers)
*
  $$ifi not setglobal reducedOutput $$setglobal reducedOutput off
  $$iftheni.IO "%reducedOutput%"=="on"

     fpNestToDel(fpNestI) = YES;
     fpNestToDel(actFp)        = YES;
     fpNestToDel(fpChl) = YES;
*
*    --- delete cost information (intermediate, factors, groups of them)
*
     p_toGui("%RegGtapName%",VQGTP,fpNestToDel,a,ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,fpNestToDel,agrpp,ts%draws%) = 0;
     p_toGui("%RegGtapName%",VQGTP,cgrp,a,ts%draws%)            = 0;
     p_toGui("%RegGtapName%",VQGTP,cgrp,agrpp,ts%draws%)        = 0;
*
*    --- delete details for market balances and armington demand
*
     p_toGui("%RegGtapName%",VQGTP,iAct,"output",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,iAct,"domM",ts%draws%)       = 0;
     p_toGui("%RegGtapName%",VQGTP,iAct,"domMrg",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,iAct,"impm",ts%draws%)       = 0;
     p_toGui("%RegGtapName%",VQGTP,iAct,"impMrg",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,iAct,aa,ts%draws%)           = 0;

     p_toGui("%RegGtapName%",VQGTP,cGrp,"output",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,cGrp,"domM",ts%draws%)       = 0;
     p_toGui("%RegGtapName%",VQGTP,cGrp,"domMrg",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,cGrp,"impm",ts%draws%)       = 0;
     p_toGui("%RegGtapName%",VQGTP,cGrp,"impMrg",ts%draws%)     = 0;
     p_toGui("%RegGtapName%",VQGTP,cGrp,aa,ts%draws%)           = 0;
  $$endif.IO

  $$iftheni.stoch not "%nDraws%"==""
    $$iftheni.stochSim "%task%"=="Stochastic simulation"
       p_toGui("%RegGtapName%","V","prob","tot",ts%draws%)  = p_prob("%iScen%",ts);
    $$else.stochSim
       p_toGui("%RegGtapName%","V","prob","tot",ts%draws%)  = 1/%nDraws%;
    $$endif.stochSim
  $$else.stoch

    p_toGui("%RegGtapName%","V","prob","tot",ts%draws%)  = 1;

  $$endif.stoch

* -------------------------------------------------------------------------------------
*
*     SDG indicators
*
* -------------------------------------------------------------------------------------

*
* --- SDG1: no poverty: income of poorest household
*
  p_toGui("%RegGtapName%","Q","SDG1","tot",ts%draws%)
    = smin(h, p_toGui("%RegGtapName%","V","income",h,ts%draws%)/p_toGui("%RegGtapName%","V","pop",h,ts%draws%))*1000;
*
* --- SDG2: zero hunger: lowest calorie uptake of any households
*
  p_toGui("%RegGtapName%","Q","SDG2.1","tot",ts%draws%)
    = smin(h, p_toGui("%RegGtapName%","Q","cal",h,ts%draws%));
*
  p_toGui("%RegGtapName%","Q","SDG2.2","tot",ts%draws%)
    = smin(h, p_toGui("%RegGtapName%","Q","prot",h,ts%draws%));
*
* --- SDG3: Health: Exposed air polluation
*
  p_toGui("%RegGtapName%","Q","SDG3.1","tot",ts%draws%)
    = exposedAirPoll.l(ts);
*
* --- SDG3: Health: Domestic demand for health services on total demand
*
  p_toGui("%RegGtapName%","Q","SDG3.3","tot",ts%draws%)
    = sum(sameas(i,"c-hht"),p_toGui("%RegGtapName%","Q",i,"dms",ts%draws%)/p_toGui("%RegGtapName%","Q","c-sum","dms",ts%draws%))*100;
*
* --- SDG3: Health: Domestic demand for health services per capita
*
  p_toGui("%RegGtapName%","Q","SDG3.2","tot",ts%draws%)
    = sum(sameas(i,"c-hht"),p_toGui("%RegGtapName%","Q",i,"dms",ts%draws%))/p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG4: Quality education: Domestic demand for educational services per capita
*
  p_toGui("%RegGtapName%","Q","SDG4.1","tot",ts%draws%)
    = sum(sameas(i,"c-edu"),p_toGui("%RegGtapName%","Q",i,"dms",ts%draws%))/p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG4: Quality education: Domestic demand for educational on total demand
*
  p_toGui("%RegGtapName%","Q","SDG4.2","tot",ts%draws%)
    = sum(sameas(i,"c-edu"),p_toGui("%RegGtapName%","Q",i,"dms",ts%draws%)/p_toGui("%RegGtapName%","Q","c-sum","dms",ts%draws%))*100;
*
* --- SDG5: Gender equality: Wage gap
*
  p_toGui("%RegGtapName%","Q","SDG5.1","tot",ts%draws%)
   $ (p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%))
    = ([ (p_toGui("%RegGtapName%","V","f-f_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-f_nsk","tot",ts%draws%))
        /(p_toGui("%RegGtapName%","Q","f-f_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","Q","f-f_nsk","tot",ts%draws%))]
     / [ (p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-m_nsk","tot",ts%draws%))
        /(p_toGui("%RegGtapName%","Q","f-m_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","Q","f-m_nsk","tot",ts%draws%))]-1)*100;
*
* --- SDG5: Gender equality: Share of female wages on total
*
  p_toGui("%RegGtapName%","Q","SDG5.2","tot",ts%draws%)
   $ (p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%))
    = (p_toGui("%RegGtapName%","V","f-f_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-f_nsk","tot",ts%draws%))
     /( p_toGui("%RegGtapName%","V","f-m_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-m_nsk","tot",ts%draws%)
       +p_toGui("%RegGtapName%","V","f-f_skl","tot",ts%draws%)+p_toGui("%RegGtapName%","V","f-f_nsk","tot",ts%draws%))*100;
*
* --- SDG6: Clean water and sanitation: Household per capital consumption of water services
*
  p_toGui("%RegGtapName%","Q","SDG6","tot",ts%draws%)
    = sum((h,sameas(i,"c-wtr")),p_toGui("%RegGtapName%","Q",i,h,ts%draws%))
    / p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG7: Affordable and clean energy: Energy price index for household energy (excluding private transport)
*
  p_toGui("%RegGtapName%","Q","SDG7.1","tot",ts%draws%) $ sum((h,sameas(kNest,"Housener")),p_toGui("%RegGtapName%","Q",kNest,h,ts%draws%))
    = sum((h,sameas(kNest,"HousEner")),p_toGui("%RegGtapName%","V",kNest,h,ts%draws%))
    / sum((h,sameas(kNest,"HousEner")),p_toGui("%RegGtapName%","Q",kNest,h,ts%draws%));
*
* --- SDG7: GHG emission intensity of power production
*
  p_toGui("%RegGtapName%","Q","SDG7.2","tot",ts%draws%) $ sum((em,emSrc,a) $ p_ghgInvAlloc("ElecHeat",em,emSrc,a),xp.l(a,ts))
    = p_toGui("%RegGtapName%","Ghgs","elecHeat","tot",ts%draws%)
    / sum((em,emSrc,a) $ p_ghgInvAlloc("ElecHeat",em,emSrc,a),xp.l(a,ts));
*
* --- SDG8: Decent work and economic growth: Real GDP per capita
*
  p_toGui("%RegGtapName%","Q","SDG8","tot",ts%draws%) = p_toGui("%RegGtapName%","Q","gdpMp","tot",ts%draws%)
    / p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG9: Industry, Innovation and Infrastructure: Construction demand by government by capita
*
  p_toGui("%RegGtapName%","Q","SDG9.1","tot",ts%draws%) = sum(sameas(i,"c-cns"),p_toGui("%RegGtapName%","Q",i,gov,ts%draws%))
    / p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG9: Industry, Innovation and Infrastructure: Economy-wide demand for communication and information
*
  p_toGui("%RegGtapName%","Q","SDG9.2","tot",ts%draws%)
     = sum(sameas(i,"c-cmn"),p_toGui("%RegGtapName%","Q",i,"mrkApp",ts%draws%))
       / p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG9: Industry, Innovation and Infrastructure: Contribution of manufacturing to Value Added
*
  p_toGui("%RegGtapName%","Q","SDG9.3","tot",ts%draws%)
     = p_toGui("%RegGtapName%","V","VA-agg","a-man",ts%draws%)/p_toGui("%RegGtapName%","V","VA-agg","a-sum",ts%draws%)*100;
*
* --- SDG10: Reduced inequality: gini index
*
  p_toGui("%RegGtapName%","Q","SDG10.1","tot",ts%draws%) = p_toGui("%RegGtapName%","V","gini","htot",ts%draws%);
*
* --- SDG10: Reduced inequality: contribution of labor to value added
*
  p_toGui("%RegGtapName%","Q","SDG10.2","tot",ts%draws%)
     = sum(l,p_toGui("%RegGtapName%","V",l,"a-sum",ts%draws%))/p_toGui("%RegGtapName%","V","VA-agg","a-sum",ts%draws%)*100;
*
* --- SDG11: Sustainable cities and communities: GHG emissions per capita
*
  p_toGui("%RegGtapName%","Q","SDG11.1","tot",ts%draws%) = emiGhg.l("GHGs",ts)/p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG11: Sustainable cities and communities: Built-up area per capita
*
  p_toGui("%RegGtapName%","Q","SDG11.2","tot",ts%draws%)
   = p_toGui("%RegGtapName%","Q","builtUpLand","ha",ts%draws%)/p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG12: Responsible consumption and production: GHG emission intensity of GDP
*
  p_toGui("%RegGtapName%","Q","SDG12.1","tot",ts%draws%) = p_toGui("%RegGtapName%","Q","GHGs","int",ts%draws%);
*
* --- SDG12: Responsible consumption and production: Fossil energy consumption of economy per capita
*
  p_toGui("%RegGtapName%","Q","SDG12.2","tot",ts%draws%)
     = [   sum(sameas(i,"c-gas"),p_toGui("%RegGtapName%","Q",i,"mrkApp",ts%draws%))
         + sum(sameas(i,"c-oil"),p_toGui("%RegGtapName%","Q",i,"mrkApp",ts%draws%))
         + sum(sameas(i,"c-coa"),p_toGui("%RegGtapName%","Q",i,"mrkApp",ts%draws%))
         + sum(sameas(i,"c-p_c"),p_toGui("%RegGtapName%","Q",i,"imp",ts%draws%))
         ]/ p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;
*
* --- SDG12: Responsible consumption and production: Irrigation water use per unit of irrigated activities
*
  p_toGui("%RegGtapName%","Q","SDG12.3","tot",ts%draws%) $ sum(sameas(actFp,"f-water"),p_toGui("%RegGtapName%","Q",actFp,"a-sum",ts%draws%))
     = sum(sameas(actFp,"f-water"),p_toGui("%RegGtapName%","Q",actFp,"a-sum",ts%draws%))
     / sum((sameas(actFp,"f-water"),a) $ p_toGui("%RegGtapName%","Q",actFp,a,ts%draws%),p_toGui("%RegGtapName%","Q","tot",a,ts%draws%));
*
* --- SDG13: Climate action: GHG emissions
*
  p_toGui("%RegGtapName%","Q","SDG13","tot",ts%draws%) = emiGhg.l("GHGs",ts);
*
* --- SDG14: Life below water: Natural resource use in fisheries
*
  p_toGui("%RegGtapName%","Q","SDG14","tot",ts%draws%)
   = sum((sameas(a,"a-fsh"),natres),p_toGui("%RegGtapName%","Q",natres,a,ts%draws%));
*
* --- SDG15: Life on land: loss of natural vegetation
*
  p_toGui("%RegGtapName%","Q","SDG15","tot",ts%draws%)
   = sum(lCovUnMng,p_toGui("%RegGtapName%","Q",lcovUnMng,"ha",ts%draws%)-p_toGui("%RegGtapName%","Q",lcovUnMng,"ha",t0%draws%));
*
* --- SDG16: Debt over GDP
*
  p_toGui("%RegGtapName%","Q","SDG16.1","tot",ts%draws%)
   = debtGdp.l(ts);
*
* --- SDG16: Public adminstration over population
*
  p_toGui("%RegGtapName%","Q","SDG16.2","tot",ts%draws%)
   = sum(sameas(i,"c-osg"),p_toGui("%RegGtapName%","Q",i,gov,ts%draws%))/p_toGui("%RegGtapName%","V","pop","htot",ts%draws%)*1000;


  $$ifi not "%iScen%"=="" $exit

*
* --- generate XML file with list of products, activities etc.
*
  xml.lw=0;
  xml.pw=32000;
  put xml;
  put '<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>'
  put "<generated>" /;
*
* --- first loop over SAM rows to generate what is called "product"
*     in backend
*

  for( XMLIter = 1 to 2,
*
*    ---- loop over columns and rows of SAM, store them as products
*         and activities
*
     if ( XMLIter eq 1,
        put "<product>" /;
     else
        put "<activity>" /;
     );
*
*    --- first show column / row totals
*
     put "   <key>tot</key>" /;
     put "   <itemName>Sum</itemName>" /;
     put "   <sel>tot</sel>" /;
     put "   <aggreg>yes</aggreg>" /;

     if ( XMLIter eq 1,
        put "</product>" /;
        put "<product>" /;
     else
        put "</activity>" /;
        put "<activity>" /;
     );
*
*    --- sum over households
*
     put "   <key>htot</key>" /;
     put "   <itemName>all households</itemName>" /;
     put "   <sel>h,htot,inst</sel>" /;
     if ( XMLIter eq 1,
        put "</product>" /;
        put "<product>" /;
     else
        put "</activity>" /;
        put "<activity>" /;
     );
*
*    --- stock changes
*
     put "   <key>stb</key>" /;
     put "   <itemName>Stock changes</itemName>" /;
     put "   <sel>stb</sel>" /;

     if ( XMLIter eq 1,
        put "</product>" /;
     else
        put "</activity>" /;
     );
*
*    --- Next show aggregate group of products/activities
*
     loop(cgrp,
        if ( XMLIter eq 1,
           put "<product>" /;
        else
           put "<activity>" /;
        );
        put "   <key>",cgrp.tl,"</key>" /;
        put "   <itemName>",cgrp.te(cgrp),"</itemName>" /;
        put "   <sel>all,prod";
        if ( not sameas(cgrp,"c-sum"), put ",prodAgg");
        put "</sel>" /;
        put "   <aggreg>yes</aggreg>" /;
        if ( XMLIter eq 1,
           put "</product>" /;
        else
           put "</activity>" /;
        );
        loop(cgrp_c(cgrp,i) $ (not sameas(cgrp,"c-sum")),
            if ( XMLIter eq 1,
               put "<product>" /;
            else
               put "<activity>" /;
            );

            put "   <key>",i.tl,"</key>" /;
            put "   <itemName>",is.te(i),"</itemName>" /;
            put "   <sel>all,prod</sel>" /;
            if ( XMLIter eq 1,
               put "</product>" /;
            else
               put "</activity>" /;
            );
        );

     );
     loop(agrp,
        if ( XMLIter eq 1,
           put "<product>" /;
        else
           put "<activity>" /;
        );
        put "   <key>",agrp.tl,"</key>" /;
        put "   <itemName>",agrp.te(agrp),"</itemName>" /;
        put "   <sel>all,act,actSum",
        if ( not sameas(agrp,"a-sum"), put ",actAgg");
        put "</sel>" /;
        put "   <aggreg>yes</aggreg>" /;
        if ( XMLIter eq 1,
           put "</product>" /;
        else
           put "</activity>" /;
        );
        loop(agrp_a(agrp,a) $ (not sameas(agrp,"a-sum")),
            if ( XMLIter eq 1,
               put "<product>" /;
            else
               put "<activity>" /;
            );

            put "   <key>",a.tl,"</key>" /;
            put "   <itemName>",is.te(a),"</itemName>" /;
            put "   <sel>all,act,aa</sel>" /;
            if ( XMLIter eq 1,
               put "</product>" /;
            else
               put "</activity>" /;
            );
        );
     );

     if ( XMLIter eq 1,
        put "<product>" /;
     else
        put "<activity>" /;
     );
     put "   <key>va-agg</key>" /;
     put "   <sel>all,actFp,va,prod</sel>" /;
     put "   <itemName>Value added</itemName>" /;
     put "   <aggreg>yes</aggreg>" /;
     if ( XMLIter eq 1,
        put "</product>" /;
     else
        put "</activity>" /;
     );

     loop(fp,
         if ( XMLIter eq 1,
            put "<product>" /;
         else
            put "<activity>" /;
         );

         put "   <key>",fp.tl,"</key>" /;
         put "   <itemName>",is.te(fp),"</itemName>" /;
         put "   <sel>all,fp</sel>";

         if ( XMLIter eq 1,
            put "</product>" /;
         else
            put "</activity>" /;
         );
     );

     if ( XMLIter eq 1,
        put "<product>" /;
     else
        put "<activity>" /;
     );
     put "   <key>sumTx</key>" /;
     put "   <sel>sumTx</sel>";
     put "   <itemName>Taxes</itemName>" /;
     put "   <aggreg>yes</aggreg>" /;
     if ( XMLIter eq 1,
        put "</product>" /;
     else
        put "</activity>" /;
     );


     loop(is $ (ftx(is) or ptx(is) or psb(is)),
         if ( XMLIter eq 1,
            put "<product>" /;
         else
            put "<activity>" /;
         );

         put "   <key>",is.tl,"</key>" /;
         put "   <itemName>",is.te(is),"</itemName>" /;
         if ( sum(sameas(ptx,is),1),
            put "<sel>all,ptx,tax</sel>" /;)
         if ( sum(sameas(psb,is),1),
            put "<sel>all,psb,tax</sel>" /;)
         if ( sum(sameas(ftx,is),1),
            put "<sel>all,ftx,tax</sel>" /;)

         if ( XMLIter eq 1,
            put "</product>" /;
         else
            put "</activity>" /;
         );
     );
*
*    --- next sam items in order
*

     loop(is $ (      (not sum(sameas(a,is),1))
                  and (not sum(sameas(i,is),1))
                  and (not sum(sameas(fp,is),1))
                  and (not sum(sameas(ftx,is),1))
                  and (not sum(sameas(stx,is),1))
                  and (not sum(sameas(psb,is),1))
                  ),
        if ( XMLIter eq 1,
           put "<product>" /;
        else
           put "<activity>" /;
        );
       put "   <key>",is.tl,"</key>" /;
       put "   <itemName>",is.te(is),"</itemName>" /;
       put "   <sel>all";
       if ( sum(sameas(aa,is),1),
           put ",aa";);
       if ( sum(sameas(fp,is),1),
          put ",fp";)
       if ( sum(sameas(h,is),1),
          put ",h";)
       if ( sum(sameas(ptx,is),1),
          put ",ptx,tax";)
       if ( sum(sameas(psb,is),1),
          put ",psb,tax";)
       if ( sum(sameas(ftx,is),1),
          put ",ftx,tax";)
       if ( sum(sameas(vat,is),1),
          put ",vat,tax";)
       if ( sum(sameas(mtx,is),1),
          put ",mtx,tax";)
       if ( sum(sameas(etx,is),1),
          put ",etx,tax";)
       if ( sum(sameas(stx,is),1),
          put ",stx,tax";)
       if ( sum(sameas(dtx,is),1),
          put ",dtx,tax";)
       if ( sum(sameas(ctx,is),1),
          put ",ctx,tax";)
       if ( sum(sameas(row,is),1),
          put ",row";)
       if ( sum(sameas(fd,is),1),
          put ",fd";)
       if ( sum(sameas(inv,is),1),
          put ",inv,inst";)
       if ( sum(sameas(gov,is),1),
          put ",gov,inst";)
       put "</sel>" /;
       if ( XMLIter eq 1,
          put "</product>" /;
       else
          put "</activity>" /;
       );
     );

*
*    --- additional items stored in column (= column dimension)
*
     if ( XMLIter eq 2,
        put "<activity>" /;
        put "   <key>Int</key>" /;
        put "   <itemName>Itensity per GDP</itemName>" /;
        put "   <sel>int</sel>";
        put "</activity>" /;

        put "<activity>" /;
        put "   <key>tot</key>" /;
        put "   <itemName>Sum</itemName>" /;
        put "   <sel>tot</sel>";
        put "</activity>" /;
     );
*
*    --- additional items stored in row (= product dimension)
*
     if ( XMLIter eq 1,

        loop(fppar,
           put "<product>" /;
           put "   <key>",fppar.tl,"</key>" /;
           put "   <itemName>",fppar.te(fppar),"</itemName>" /;
           put "   <sel>fppar</sel>" /;
           put "</product>" /;
        );

        put "<product>" /;
        put "   <key>Tot</key>" /;
        put "   <itemName>Sum</itemName>" /;
        put "   <sel>emsrc</sel>";
        put "</product>" /;

        put "<product>" /;
        put "   <key>Act</key>" /;
        put "   <itemName>Sum from activity</itemName>" /;
        put "   <sel>emsrc</sel>";
        put "</product>" /;

        put "<product>" /;
        put "   <key>phn</key>" /;
        put "   <itemName>Phantom tax</itemName>" /;
        put "   <sel>phn</sel>";
        put "</product>" /;

        put "<product>" /;
        put "   <key>LUCF</key>" /;
        put "   <itemName>Land Use Change</itemName>" /;
        put "   <sel>emsrc</sel>";
        put "</product>" /;
        loop(emsrc,
           put "<product>" /;
           put "   <key>",emsrc.tl,"</key>" /;
           put "   <itemName>",emsrc.te(emsrc),"</itemName>" /;
           put "   <sel>emsrc</sel>";
           put "</product>" /;
        );
        loop(pollType,
           put "<product>" /;
           put "   <key>",pollType.tl,"</key>" /;
           put "   <itemName>",pollType.te(pollType),"</itemName>" /;
           put "   <sel>pollType</sel>";
           put "</product>" /;
        );
        loop(ghgInv,
           put "<product>" /;
           put "   <key>",ghgInv.tl,"</key>" /;
           put "   <itemName>",ghgInv.te(ghgInv),"</itemName>" /;
           put "   <sel>ghgInvUsr</sel>";
           put "</product>" /;
        );
        loop(cohorts,
           put "<product>" /;
           put "   <key>",cohorts.tl,"</key>" /;
           put "   <itemName>",cohorts.te(cohorts),"</itemName>" /;
           put "   <sel>cohorts</sel>";
           put "</product>" /;
        );
        loop(lCov,
           put "<product>" /;
           put "   <key>",lCov.tl,"</key>" /;
           put "   <itemName>",lCov.te(lCov),"</itemName>" /;
           put "   <sel>lCov</sel>";
           put "</product>" /;
        );
     );
  );
*
*    --- output emissions in dim5 (where also V,Q,P etc. is stored)
*
  put "<dim5>" /;
  put "   <key>V</key>" /;
  put "   <itemName>Value</itemName>" /;
  put "   <sel>V,VQP,VQPTG,all</sel>";
  put "</dim5>" /;

  put "<dim5>" /;
  put "   <key>Q</key>" /;
  put "   <itemName>Quantity</itemName>" /;
  put "   <sel>Q,VQP,VQPTG,all</sel>";
  put "</dim5>" /;

  put "<dim5>" /;
  put "   <key>P</key>" /;
  put "   <itemName>Price</itemName>" /;
  put "   <sel>P,VQP,VQPTG,all</sel>";
  put "</dim5>" /;

  put "<dim5>" /;
  put "   <key>G</key>" /;
  put "   <itemName>Tax income</itemName>" /;
  put "   <sel>G,VQPTG,all</sel>";
  put "</dim5>" /;

  put "<dim5>" /;
  put "   <key>T</key>" /;
  put "   <itemName>Tax rate</itemName>" /;
  put "   <sel>T,VQPTG,all</sel>";
  put "</dim5>" /;

  put "<dim5>" /;
  put "   <key>S</key>" /;
  put "   <itemName>Tech shifter</itemName>" /;
  put "   <sel>S,VQPTGS,all</sel>";
  put "</dim5>" /;

*
*    --- output emissions
*
  loop(em,
     put "<dim5>" /;
     put "   <key>",em.tl,"</key>" /;
     put "   <itemName>",em.te(em),"</itemName>" /;
     put "   <sel>em</sel>";
     put "</dim5>" /;
  );
  loop(pollType,
     put "<dim5>" /;
     put "   <key>",polltype.tl,"</key>" /;
     put "   <itemName>",polltype.te(polltype),"</itemName>" /;
     put "   <sel>em</sel>";
     put "</dim5>" /;
  );
*
* --- output k-bundles
*
  loop(topKnest,
     put "<product>" /;
     put "   <key>",topkNest.tl,"</key>" /;
     put "   <itemName>",topKNest.te(topKNest),"</itemName>" /;
     put "   <sel>k</sel>";
     put "</product>" /;
  );
*
* --- output nutrients
*
  $$iftheni.nutr defined nutr
   loop(nutr,
      put "<product>" /;
      put "   <key>",nutr.tl,"</key>" /;
      put "   <itemName>",nutr.te(nutr),"</itemName>" /;
      put "   <sel>nutr</sel>";
      put "</product>" /;
   );
  $$endif.nutr
*
* --- output case specific activity aggregates
*
  loop(aagr,
     put "<activity>" /;
     put "   <key>",aagr.tl,"</key>" /;
     put "   <itemName>",aagr.te(aagr),"</itemName>" /;
     put "   <sel>aagr</sel>" /;
     put "</activity>" /;
  );

  put "<activity>" /;
  put "   <key>bot</key>" /;
  put "   <itemName>BOT</itemName>" /;
  put "   <sel>inst</sel>" /;
  put "</activity>" /;

  $$iftheni.aez defined raez

*
*    --- put the aezIds to the XML file for mapping
*
      put "<region>" /;
        put "   <key>%RegGtapName%</key>" /;
        put "   <itemName>%RegGtapName%</itemName>" / ;
        put "   <sel>all</sel>" /;
      put "</region>" /;

     loop(raez,
        put "<region>" /;
        put "   <key>", raez.tl,"</key>" /;
        put "   <itemName>", raez.te(raez) "</itemName>" / ;
        put "   <sel>raez</sel>" /;
        put "</region>" /;
     );

  $$endif.aez

  put "</generated>";

  putclose;

  embeddedCode Python:
     x = open("gui/generated.xml" )
     s =x.read().replace("&", "&amp;" )
     x.close()
     x = open("gui/generated.xml","w")
     x.write(s)
     x.close
  endEmbeddedCode

  execute 'cp gui/generated.xml gui/xml/%regGtapNameOri%.xml';

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");

