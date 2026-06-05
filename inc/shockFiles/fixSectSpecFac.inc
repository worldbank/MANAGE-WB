********************************************************************************
$ontext

   MANAGEWB project

   GAMS file : SECTSPECFAC.GMS

   @purpose  : Example file to target a factor demand in a sector by
               swapping with factor tax. Needs to be adjusted to specific
               SAM (labels f-capital  and a-windBl might provoke errors)
   @author   : W.Britz
   @date     : 22.11.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

     parameter xFacTarget(fp,a,t) "Pre-determined factor use in activity";

     set aShocked(a) /a-windBl /;
     xFacTarget("f-capital",aShocked,tt) = sum(v, xFac.l("f-capital",ashocked,v,t0))*(1+0.05*(tt.val-t0.val));

     $$setglobal modelVariant "facTarget"

     Model cge_%modelVariant%
     /
        cge_
        $$iftheni.baseline "%task%"=="Baseline"
           -e_rgdpmp
           +e_rgdpmp.ChixFacL
        $$endif.baseline
        -e_xfmSup
        +e_xfmSup
        -e_pk
        +e_pk
     /;


$$else.decl

     tFac0.lo(a,fp,tt) $ xFacTarget(fp,a,tt) = -inf;
     tFac0.up(a,fp,tt) $ xFacTarget(fp,a,tt) = +inf;

     xfmSup.fx(fma,fmp,vf,tt) $ sum( (a,sameas(fp,fmp)) $ sameas(a,fma),xfacTarget(fp,a,tt))
        = sum( (a,sameas(fp,fmp)) $ sameas(a,fma),xfacTarget(fp,a,tt));

     fmNestFmp(fma,fmp) $ sum( (a,sameas(fp,fmp)) $ sameas(a,fma),xfacTarget(fp,a,tt)) = YES;


$$endif.decl
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
