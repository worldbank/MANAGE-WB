********************************************************************************
$ontext

   MANAGE project

   GAMS file : runShortRunShock.inc

   @purpose  : Short run mode: xfmSup partly fixed, updated elasticities

   @author   :
   @date     : 31.08.24
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- fix primary factor allocation to activities if short-run elasticity is set to eps
*     (exemption: vintage mechanism is on for capital. Dont' restrict the anyhow
*      limited option to adjust capital use9
*
  xfmSup.fx(fma,actFmp,"suminst",vf,tt) $ ((sum(sameas(actFmp,fmAll),p_omegaFmpShort(fmAll,vf)) le eps)
                                             $  xfmSup.l(fma,actFmp,"suminst",vf,t0)
                                             $ (not (ifVint $ sum(sameas(actFmp,cap),1))))
     = xfmSup.l(fma,actFmp,"suminst",vf,tt);
*
* --- replace long with short-run transformation and factor supply elasticities in model equations
*
  p_omegaFmp(fmAll,vf) $ ((sum(sameas(actFmp,fmAll),p_omegaFmpShort(fmAll,vf)) gt eps)) = p_omegaFmpShort(fmAll,vf);
  p_epsFmp(fmAll,vf)                                                                    = p_epsFmpShort(fmAll,vf);
*
  $$ondotl
*
* --- And update share parameters  and constant terms to reflect updated elasticities
*     Calibration wil recover current quantities at current price
*
  p_gammaFmp(fmNestFmp(fmChild,actFmp),instTot,vf,tt) $ (sum(fmNesting(fmChild,fmPar,actFmp)
                                                               $ (p_omegaFmp(actFmp,vf) gt eps), p_omegaFmp(fmPar,vf) ne inf)
                                                               $  xfmSup.l(fmChild,actFmp,instTot,vf,tt))
   = sum(fmNesting(fmChild,fmPar,actFmp),xfmSup.l(fmChild,actFmp,instTot,vf,tt)/xfmSup.l(fmPar,actFmp,instTot,vf,tt)
                                     * ((    pfmSupD.l(fmPar,actFmp,instTot,vf,tt) $ isAdditiveCet(fmPar)
                                           + pfmSup.l(fmPar,actFmp,instTot,vf,tt)  $ (not isAdditiveCet(fmPar)))
                                      /m_pfmSup(fmChild,actFmp,instTot,vf,tt))**p_omegaFmp(fmPar,vf));
  $$offdotl
*
  atFmSup.fx(actFmp,instTot,vf,tt) $ (xfmSup.l(actFmp,actFmp,instTot,vf,tt) $ (atFmSup.range(actFmp,instTot,vf,tt) eq 0) $ (p_omegaFmp(actFmp,vf) ne 0))
       = xfmSup.l(actFmp,actFmp,instTot,vf,tt)/(pfmSup.l(actFmp,actFmp,instTot,vf,tt)/pgdpmp.l(tt))**p_epsFmp(actFmp,vf);
*
* --- these elasticities work against the long-run equlibrium price determined in first solve
*     (scale field is used in short run equations for adjustments at extensive margins)
*
  pxfac_pretax.scale(actFp,a,old,tt) = pxfac_pretax.l(actFp,a,old,tt);
*
* -- make sure that long-run adjustments in share parameters are based on first solve
*    by storing them on "LastT" from which they are re-loaded later
*
  $$setglobal lastT "'lastT'"
  xfac.l(flagxfac(fpNest,a,v),"lastT")                   = max(1.E-14,xfac.l(fpnest,a,v,tt));
  pxfac.l(flagxfac(fpNest,a,v),"lastT")                  = pxfac.l(fpnest,a,v,tt);
  lambdaxfac.l(flagxfac(fpChl,aact,old),"lastT")         = lambdaxfac.l(fpChl,aact,old,tt);
  twist.l(fpChl,fpPar,aact,old,"lastT") $ (fpParTC(fpPar) $ fpParUpdate(fpPar,aact)) = twist.l(fpChl,fpPar,aact,old,tt);
  isShortRun = 1;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
*
* --- run the shock file provided by user. An indicator flag on shockMode
*     informs the shock file to introduce the unexpected short run shocks
*
  $$batinclude '%simf%/%shk_file%.inc' run
