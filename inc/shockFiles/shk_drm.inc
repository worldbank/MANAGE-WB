********************************************************************************
$ontext

   MANAGE project

   GAMS file : SHK_NONE.INC

   @purpose  : Test for a baseline replication with no shock
   @author   : W.Britz
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

   set randVars / "depr" /;


$elseif.decl "%1"=="stoch"

   $$include 'inc/sensStoch/iniStoch.inc'
   parameter p_depr(t); p_depr(t) = depr.l(t);

   set retPer "Return periods" / T1,T5,T10,T50,T100,Tend /;

   table     p_Drm "Increase in depreciation for disasters with a certain return period"
               dmg     prob
      T1      1.E-10      1
      T5      0.001     0.2
      T10     0.01      0.1
      T50     0.02      0.02
      T100    0.04      0.01
      TEnd    0.04     -1.E-10
  ;
*
*  --- first draw probability from uniform
*
   p_randVar("depr",nodes) = uniform(0,1);
*
*  --- map to damage based on trapeziodal rule (i.e. the damage is proportional to the
*     probality)

   p_randVar("depr",nodes) $ p_randVar("depr",nodes) =  p_depr("%baseYear%")

    + sum(retPer $ ( (p_randVar("depr",nodes) lt p_drm(retPer-1,"prob")) $ (p_randVar("depr",nodes) ge p_drm(retPer,"prob"))),
                     p_drm(retPer-1,"dmg") + (p_drm(retPer,"dmg")     - p_drm(retPer-1,"dmg"))
                                            *(p_drm(retPer-1,"prob")  - p_randVar("depr",nodes))/(p_drm(retPer-1,"prob")-p_drm(retPer,"prob")));
   p_randVar("depr","n1") = p_depr("%baseYear%");
   p_probN("n1")          = 1;

   $$include 'inc/sensStoch/runStoch.inc'

$else.decl

   $$iftheni.draw not "%iScen%" == "base"

      $$ifi not defined p_randVar $abort "%shk_file% is designed to be used with task Stochastic simulation, not  with task %task%"


      depra.fx(a,ts(t))   = depra.l(a,t) * sum(t_nodes(t,nCur) $ draws_nodes("%iScen%",nCur), p_randVar("depr",nCur))/depr.l(t);

   $$endif.draw

$endif.decl
