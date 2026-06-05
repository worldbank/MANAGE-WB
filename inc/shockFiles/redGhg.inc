********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : REDGHG.INC

   @purpose  :
   @author   :
   @date     : 14.08.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

*
*  --- Change pairing of model such that emiGHG can define tEmi
*

   model cge_redGHG
   /
      cge_
    -  e_emighg
       e_emighg
   /
   ;

   $$setglobal modelVariant redGHG

*
*  --- store GHG emissions from baseline
*
   parameter p_ghgs(t);
   p_ghgs(t) = emiGHG.l("GHGs",t);



$else.decl

   $$ifi not setglobal redGHG $setglobal redGHG 0.5


   emiGHG.fx("GHGs",ts(t)) = p_ghgs(t) * (1 - %redGHG%*t.pos/(card(t)-2));
   tEmiD0.lo(ts(t)) = -inf;
   tEmiD0.up(ts(t)) = +inf;
   tEmiD0.l(%tt%)   = tEmiD0.l(%tt1%);
   tEmiD0.l(ts(t)) $ (Not tEmiD0.l(t-1)) = 0.01;

   tEmi.l(emiFlag(em,aa),ts(t)) $ (not tEmi.l(em,aa,t))
     = (tEmi0.l(em,aa,t) + (tEmiD0.l(t) + tEmiD1.l(em,t) + tEmiD2.l(aa,t)))
                             * tEmiM0.l(t) * tEmiM1.l(em,t) * tEmiM2.l(aa,t);

   rtEmi.l(emiFlag(em,aa),ts(t)) $ (not rtEmi.l(em,aa,t)) = tEmi.l(em,aa,t)/pgdpmp.l(t);

   ygEmi.l(aa,ts(t)) $ (not ygEmi.l(aa,t))
    = sum((em,emSrc) $ (emiFlag(em,aa) $ p_rhoEmi(em,emSrc,aa)),
                                tEmi.l(em,aa,t)*emical.l(t)*chiEmi.l(em,emSrc,aa,t)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,t));

   ygEmiXP.l(a,ts(t)) $ (not ygEmiXP.l(a,t))
    = sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),tEmi.l(em,a,t)*p_phiEmiXP(em,a,t)*chiEmiXP.l(em,a,t)*p_rhoEmiXP(em,a)*xp.l(a,t));

   yg.l("Emi",ts(t))   $ (not yg.l("Emi",t))   = sum(aa,ygEmi.l(aa,t));
   yg.l("EmiXP",ts(t)) $ (not yg.l("EmiXp",t)) = sum(a,ygEmiXP.l(a,t));


$endif.decl
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
