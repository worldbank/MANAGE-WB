********************************************************************************
$ontext

   MANAGE project

   GAMS file : SAMCALC.INC

   @purpose  : Construct a SAM for each simulated year
   @author   : MANAGE team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  p_samT(i,a,ts) = 0 ;

  p_samT(i,a,ts)   = sum((sameas(i,fpNestI),v),xfac.l(fpNestI,a,v,ts)*pxfac.l(fpNestI,a,v,ts));
  p_samT(fp,a,ts) = sum(v,pxfac_pretax.l(fp,a,v,ts)*xfac.l(fp,a,v,ts)) ;
  p_samT(ptx,a,ts)   $ (ptx.pos eq 1) = (tprd.l(a,ts)+tphn.l(a,ts) $ (tphn.l(a,ts) gt 0))*px.l(a,ts)*xp.l(a,ts) ;
  p_samT(psb,a,ts)   $ (psb.pos eq 1) = (tpsb.l(a,ts)+tphn.l(a,ts) $ (tphn.l(a,ts) lt 0))*px.l(a,ts)*xp.l(a,ts) ;
  p_samT(ftx,a,ts)     = sum((v,fp)$mapftax(fp,ftx),tFac.l(a,fp,ts) * pxfac_pretax.l(fp,a,v,ts)*xfac.l(fp,a,v,ts)) ;
  p_samT(a,i,ts)   = chip.l(a,i,ts)*p.l(a,i,ts)*x.l(a,i,ts) ;

  p_samT(i,j,ts)   = p_chiPA(i)*pa.l(i,ts)*xa.l(i,j,ts) ;

  p_samT(vat,i,ts) $ (vat.pos eq 1)
     = (p_chiPD(i)*pd.l(i,ts) + pmarg.l(i,ts)*tmg.l(i,"d",ts))*tvat.l(i,"d",ts)*xd.l(i,ts)
                + (er.l(ts)*pwm.l(i,ts)*(1+timp.l(i,ts)) + pmarg.l(i,ts)*tmg.l(i,"m",ts))*tvat.l(i,"m",ts)*xm.l(i,ts) ;

  p_samT(mtx,i,ts)   $ (mtx.pos eq 1) = (xm.l(i,ts) + mdelst.l(i,ts))*pwm.l(i,ts)*er.l(ts)*timp.l(i,ts) ;
  p_samT(etx,i,ts)   $ (etx.pos eq 1) =  xe.l(i,ts)*p_chiPE(i)*pe.l(i,ts)*texp.l(i,ts) ;
  p_samT(stx,i,ts)   $ (stx.pos eq 1) =   sum((ax), tpaf.l(i,ax,ts)*p_chiPA(i)*pa.l(i,ts)*xa.l(i,ax,ts))
                   + sum((j), tpaf.l(i,j,ts)*p_chiPA(i)*pa.l(i,ts)*xa.l(i,j,ts));
  p_samT(row,i,ts) = (xm.l(i,ts) + mdelst.l(i,ts))*pwm.l(i,ts)*er.l(ts) ;

  p_samT(h,fp,ts)    = sum((a,v), p_chiFac(h,fp,ts)*pxfac_preTax.l(fp,a,v,ts)*xfac.l(fp,a,v,ts)) ;

  p_samT(gov,entr,ts) = sum((inst,instd)$(mapinst(gov,inst) and mapinst(entr,instd)), transfers.l(inst,instd,ts)) ;
  p_samT(inst,fp,ts)  = p_chiFac(inst,fp,ts)*sum((v,a),xfac.l(fp,a,v,ts)*pxfac_pretax.l(fp,a,v,ts)) ;
  p_samT(gov,psb,ts) $ (psb.pos eq 1) = sum(a,(tpsb.l(a,ts)+tphn.l(a,ts) $ (tphn.l(a,ts) lt 0))*px.l(a,ts)*xp.l(a,ts))  ;
  p_samT(gov,ptx,ts) $ (ptx.pos eq 1) = sum(a,(tprd.l(a,ts)+tphn.l(a,ts) $ (tphn.l(a,ts) gt 0))*px.l(a,ts)*xp.l(a,ts));
  p_samT(gov,mtx,ts) $ (mtx.pos eq 1) = sum(i,(xm.l(i,ts) + mdelst.l(i,ts))*pwm.l(i,ts)*er.l(ts)*timp.l(i,ts));
  p_samT(gov,stx,ts) $ (stx.pos eq 1) = sum((ax,i), tpaf.l(i,ax,ts)*p_chiPA(i)*pa.l(i,ts)*xa.l(i,ax,ts))
                   + sum((j,i), tpaf.l(i,j,ts)*p_chiPA(i)*pa.l(i,ts)*xa.l(i,j,ts));
  p_samT(gov,ftx,ts)  = sum((a,v,fp)$mapftax(fp,ftx),tFac.l(a,fp,ts) * pxfac_pretax.l(fp,a,v,ts)*xfac.l(fp,a,v,ts));
  p_samT(gov,etx,ts) $ (etx.pos eq 1) =  sum(i,xe.l(i,ts)*p_chiPE(i)*pe.l(i,ts)*texp.l(i,ts)) ;
  p_samT(gov,vat,ts) $ (vat.pos eq 1)
   =  sum(i,(p_chiPD(i)*pd.l(i,ts) + pmarg.l(i,ts)*tmg.l(i,"d",ts))*tvat.l(i,"d",ts)*xd.l(i,ts)
                + (er.l(ts)*pwm.l(i,ts)*(1+timp.l(i,ts)) + pmarg.l(i,ts)*tmg.l(i,"m",ts))*tvat.l(i,"m",ts)*xm.l(i,ts)) ;
*
* --- carbon taxes as defined in the model equations
*
  p_samT(ctx,emsrc,ts) $ (sameas(ctx,"co2EqCarbTax"))
   = sum((em,aa), tEmi.l(em,aa,ts)*emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts)) ;

  p_samT(ctx,a,ts)     $ (sameas(ctx,"co2EqCarbTax"))
   = sum(emiFlag(em,a)  $ p_rhoEmiXP(em,a),       tEmi.l(em,a,ts)*emical.l(ts)*p_phiEmiXP(em,a,ts)    *chiEmiXP.l(em,a,ts)       *p_rhoEmiXP(em,a)*xp.l(a,ts))
     +sum((em,actFp)    $ p_rhoEmiFac(em,actFp,a),tEmi.l(em,a,ts)*emical.l(ts)*p_phiEmiFac(em,actFp,a)*chiEmiFac.l(em,actFp,a,ts)*p_rhoEmiFac(em,actFp,a)
                                                  *sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,ts)));

  p_samT(gov,ctx,ts) $ (sameas(ctx,"co2EqCarbTax"))
      = sum((em,emsrc,aa),  tEmi.l(em,aa,ts)*emical.l(ts)*chiEmi.l(em,emSrc,aa,ts)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,ts))
      + sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),       tEmi.l(em,a,ts)*p_phiEmiXP(em,a,ts)*chiEmiXP.l(em,a,ts)*p_rhoEmiXP(em,a)*xp.l(a,ts))
      + sum((em,actFp,a)  $ p_rhoEmiFac(em,actFp,a),tEmi.l(em,a,ts)*emical.l(ts)*p_phiEmiFac(em,actFp,a)*chiEmiFac.l(em,actFp,a,ts)*p_rhoEmiFac(em,actFp,a)
                                                   *sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,ts)));

  p_samT(dtx,h,ts)   $ (dtx.pos eq 1) = tpit.l(h,ts) * yh.l(h,ts);
  p_samT(dtx,entr,ts)$ (dtx.pos eq 1) = tcit.l(entr,ts) * entrY.l(entr,ts) ;
  p_samT(dtx,gov,ts) $ (dtx.pos eq 1) = gTax.l(ts) ;
  p_samT(gov,dtx,ts) $ (dtx.pos eq 1) = yg.l("pit",ts) +yg.l("cit",ts) + gTax.l(ts);
* p_samT(inv,gov,ts)  = savg.l(ts) - sum(adpinv$map_adpinv(gov,adpinv),yf.l(adpinv,ts))  ;
* p_samT(inv,gov,ts)  = govBalance.l(ts) ;
  p_samT(inv,row,ts)  = er.l(ts)*savf.l(ts) - fundf.l(ts);
  p_samT(stb,inv,ts)  = sum(i, p_chiPS(i)*ps.l(i,ts)*delst.l(i,ts) + er.l(ts)*pwm.l(i,ts)*(1+timp.l(i,ts))*mdelst.l(i,ts)) ;

  p_samT(inst,instp,ts)$(chiTr.l(inst, instp,ts) ne 0) = transfers.l(inst,instp,ts) ;
  p_samT(inst,dinst,ts) = chiTr.l(inst, dinst,ts) * totTr.l(dinst,ts) ;
  p_samT(inst,row,ts) = er.l(ts)*rowtransfers.l(inst,ts);

  p_samT(i,fd,ts)     = p_chiPA(i)*paf.l(i,fd,ts)*xa.l(i,fd,ts) ;
  p_samT(i,stb,ts)    = p_chiPS(i)*ps.l(i,ts)*delst.l(i,ts) + er.l(ts)*pwm.l(i,ts)*(1+timp.l(i,ts))*mdelst.l(i,ts) ;
  p_samT(i,row,ts)    = er.l(ts)*p_chiPWE(i)*pwe.l(i,ts)*xe.l(i,ts) ;
  p_samT(instfd,debt,ts) = debtPay.l(instfd,ts) ;
  p_samT(debt,inst,ts)   = debtnew.l(inst,ts) ;
  p_samT(inv,h,ts)    =  savh.l(h,ts)       - sum(sameas(h,inst),debtnew.l(inst,ts));
  p_samT(inv,entr,ts) =  savEntr.l(entr,ts) - sum(sameas(entr,inst),debtnew.l(inst,ts)); ;
  p_samT(debt,gov,ts) =  sum(instfd,debtPay.l(instfd,ts))  ;
  p_samT(gov,debt,ts) =  sum(instfd,debtNew.l(instfd,ts))  ;
  p_samT(i,adpInv,ts) = p_chiPA(i)*paf.l(i,adpInv,ts)*xa.l(i,adpinv,ts)  ;
  p_samT(i,gInv,ts)   = p_chiPA(i)*paf.l(i,gInv,ts)*xa.l(i,ginv,ts)  ;
  p_samT(ginv,gov,ts) =  yf.l(ginv,ts)  ;
  p_samT(adpinv,gov,ts)$map_adpInv(gov,adpinv) =  yf.l(adpinv,ts)  ;
  p_samT(inv,adpinv,ts)$map_adpInv(inv,adpinv) =  - yf.l(adpinv,ts)  ;
  p_samT(inv,gov,ts) =  max(0,govBalance.l(ts))  ;
*
* --- calculate absolute error in SAM balance and delete if <1.E-6
*
  p_samTBal("Level",is,ts)= sum(js,p_samT(is,js,ts))-sum(js,p_samT(js,is,ts));
  p_samTBal("Level",is,ts) $ (abs(p_samTBal("Level",is,ts))<1E-5) = 0;
*
* --- convert the remaining errors into a relative difference, deleted imbalances < 1/1E6
*
  option kill=p_samTDiff;
  p_samTDiff(is,js,ts(t0)) = p_samT(is,js,t0)- sam(is,js) ;
  p_samTDiff(is,js,ts(t0)) $(abs(  p_samTDiff(is,js,t0)) < 1E-8)= 0 ;
  if (card(p_samTDiff), display "Warning: Original and derived SAM for t0 not equal",p_samTDiff;);

  p_samTBal("Perc",is,ts) $  p_samTBal("Level",is,ts) = 2*p_samTBal("Level",is,ts)/(abs(sum(js,p_samT(is,js,ts)))+abs(sum(js,p_samT(js,is,ts))));
  p_samTBal("Perc",is,ts) $ (abs(p_samTBal("Perc",is,ts))<1E-4) = 0;
  p_samTBal("Level",is,ts) $ (abs(p_samTBal("Perc",is,ts))<1E-4) = 0;
  p_samTBal("Perc",is,ts) = p_samTBal("Perc",is,ts)*100;
  if (card(p_samTBal), abort "final SAM not balanced, errors in %",p_samTBal);

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
