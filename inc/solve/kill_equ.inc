********************************************************************************
$ontext

   MANAGE project

   GAMS file : KILL_EQU.INC

   @purpose  : kill all equations
   @author   :
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- include generated include file with kill=e_.. statements
*
  options
    $$include %gams.scrdir%equs.gms
  ;

*
* --- kill bounds which might have been introduced by second solve trials
*
  lambdaxfac.lo(flagXFac(actFpNest,a,v),ts)  = -inf;
  lambdaxfac.up(flagXFac(actFpNest,a,v),ts)  = +inf;
*
* --- kill bounds which might hanve been introduced by second solve trials
*
  option kill=xfac.lo,kill=rrat.lo,kill=pxfac_preTax.lo,kill=pxfac.lo,kill=pa.lo,
        kill=xfd.lo,kill=facty.lo,kill=rore.lo,kill=fundPriceD.lo,kill=xs.lo,kill=xsExp.lo,kill=st.lo,kill=xp.lo,
        kill=ps.lo,kill=p.lo,kill=psExp.lo,kill=psExpRel.lo,kill=psExpRel.up,kill=sectTot.lo,kill=oldcap.lo;

  $$ifi not "%flexFactor%"=="on" option kill=pfmSup.lo,kill=pfmsupD.lo,kill=pgdpmp.lo;

  interest.lo(ts)   = -inf;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
