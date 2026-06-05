********************************************************************************
$ontext

   MANAGE project

   GAMS file : setBoundsForCns.inc

   @purpose  : Specific bounds for CNS/DNLP and repeated solves
   @author   : W.Britz
   @date     : 31.08.24
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- DNLP (chosen by user or repeate solve after first CNS solve failed)
*
  $$ifi "%ForwardLooking%"=="on" oldCap.lo(a,ts)  =  1.E-10;

  if ( ((mod(iter.pos,2) eq 1) $ (iter.pos le 6)),
*
*    --- remove bounds set below and set some bounds to avoid that CONOPT4
*        takes quite long to find defined variables and equations
*
     interest.lo(ts)   = -inf;
     fundPriceD.lo(ts) = -inf;
     rore.lo(ts)       = -inf;

     rrat.lo(a,ts)     = 0.0001;
     xfac.lo(flagxfac(fpNest,a,v),ts) $  ((not fpNat(fpNest)) $ (xFac.range(fpNest,a,v,ts) ne 0)) = -1.E+10;
  else
*
*    --- trials #2,4,6 etc. or > 6: use a battery of bounds which avoid math trap errors
*
     xfac.lo(flagxfac(fpNest,a,v),ts) $ ((xFac.range(fpNest,a,v,ts) ne 0) $ (not fpNat(fpNest)) $ ( mod(iter.pos,4) eq 0)) = 1.E-14;
     xfac.lo(flagxfac(fpNest,a,v),ts) $ ((xFac.range(fpNest,a,v,ts) ne 0) $ (not fpNat(fpNest)) $ ( mod(iter.pos,4) eq 2)) = -1.E+10;
     xfac.lo(flagxfac(fpNest,a,v),ts) $ ((xFac.range(fpNest,a,v,ts) ne 0) $ (not fpNat(fpNest)) $  (mod(iter.pos,4) eq 3)) = -1.E-10;

     xfac.lo(flagxfac(fpTop,a,v),ts) $ (xFac.range(fpTop,a,v,ts) ne 0)                             = -1.E-10;
     xfac.lo(flagxfac(fpTop,a,v),ts) $ ((xFac.range(fpTop,a,v,ts) ne 0) $ (mod(iter.pos,4) eq 1))  = 1.E-14;
     xfac.lo(flagxfac(fpTop,a,v),ts) $ ((xFac.range(fpTop,a,v,ts) ne 0) $ (mod(iter.pos,4) eq 2))  = -1.E+10;

     pxfac_preTax.lo(flagxfac(fp,a,old),ts) $ (pxfac_preTax.range(fp,a,old,ts) ne 0) = 1.E-8;
     pxfac_preTax.lo(flagxfac(fp,a,old),ts) $ ((pxfac_preTax.range(fp,a,old,ts) ne 0)
       $ ((mod(iter.pos,2) eq 1) or (mod(iter.pos,4) eq 2))) = +1.E-16;

     pxfac.lo(flagxfac(fpNest,a,v),ts) $ ((not fpNat(fpNest)) $ (not fpTop(fpNest)) $ (pxfac.range(fpNest,a,v,ts) ne 0)
        $  (mod(iter.pos,2) eq 0))                            = +1.E-6;
     pxfac.lo(flagxfac(fpNest,a,v),ts) $ ((not fpNat(fpNest)) $ (not fpTop(fpNest)) $ (pxfac.range(fpNest,a,v,ts) ne 0)
       $ ((mod(iter.pos,2) eq 1) or (mod(iter.pos,4) eq 2))) = +1.E-16;

     rrat.lo(a,ts) $ (rrat.range(a,ts) ne 0)  = 0.0001;
     rrat.lo(a,ts) $ ((rrat.range(a,ts) ne 0) $ (mod(iter.pos,4) ne 1))   = -inf;


     xfd.lo(oa,ts) $ ((xfd.range(oa,ts) ne 0) and (not sum(sameas(ginv,oa),1))) = 1.E-6;
     sectTot.lo(ts)  = 1.E-3;
     gdpmp.lo(ts)    = gdpmp.l(tt-1)*0.5;

     pfmSup.lo(fmNestFmp(fmAll,fmp),inst,vf,ts)  $  (pfmSup.range(fmAll,fmp,inst,vf,ts) ne 0)  = -inf;
     pfmSupD.lo(fmNestFmp(fmPar,fmp),inst,vf,ts) $  ((pfmSupD.range(fmPar,fmp,inst,vf,ts) ne 0) $ isAdditiveCet(fmPar))  = -inf;

     pfmSup.lo(fmNestFmp(fmAll,fmp),inst,vf,ts)                          $  (mod(iter.pos,4) eq 3)
      = 1.E-6*pfmSup.l(fmAll,fmp,inst,vf,t0);
     pfmSupD.lo(fmNestFmp(fmPar,fmp),inst,vf,ts) $ (isAdditiveCet(fmPar) $  (mod(iter.pos,4) eq 3))
      = 1.E-6*pfmSup.l(fmPar,fmp,inst,vf,t0);


     pgdpmp.lo(ts) $ (pgdpmp.range(ts) ne 0)                               = max(1.E-3,pgdpmp.l(tt-1)*0.5);

     fundPriceD.lo(ts) $ (fundPriceD.range(ts) ne 0)                                                        = 1.E-10;
     fundPriceD.lo(ts) $ ((fundPriceD.range(ts) ne 0) $ (mod(iter.pos,4) eq 3))                             = -inf;
     fundPriceD.lo(ts) $ ((fundPriceD.range(ts) ne 0) $ ((mod(iter.pos,2) eq 1) or (mod(iter.pos,4) eq 2))) = -inf;

     interest.lo(ts) $ (interest.range(ts) ne 0)                                                            = 1.E-9;
     interest.lo(ts) $ ((interest.range(ts) ne 0) $ (mod(iter.pos,4) eq 3))                                 = -inf;
     interest.lo(ts) $ ((interest.range(ts) ne 0) $ ((mod(iter.pos,2) eq 1) or (mod(iter.pos,4) eq 2)))     = -inf;

     rore.lo(ts)     $ (rore.range(ts) ne 0)                                                                = -inf;
     rore.lo(ts)     $ ((rore.range(ts) ne 0) $ (mod(iter.pos,2) eq 1) or (mod(iter.pos,4) eq 2))           = 1.E-10;

     p.lo(a,i,ts)       $ (p_as(a,i,ts) $ (p.range(a,i,ts) ne 0))      = 1.E-6;
     ps.lo(i,ts)        $ (ps.range(i,ts) ne 0)                        = 1.E-6;
     psExp.lo(i,ts)     $ ((psExp.range(i,ts) ne 0) $ p_alphaSt(i,ts)) = 1.E-6;
  );

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
