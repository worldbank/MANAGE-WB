********************************************************************************
$ontext

   MANAGE-WB

   MANAGE file : REPORTDECL.INC

   @purpose  : Declaration of sets and parameters used for different types
               of post-model reports
   @author   : MANAGE team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  set diagRep "Diagnostic indicators about model solve" /
     prep
     modStatus
     solStatus
     iterUsd
     "solve"
     resTot
     resUsd
     resGen
     nSolves
     "Walras % of GDP"
     numEqu
     numInfes
     sumInfes
     numRedef
     post
     final
     totSec
     heapSize
   /;

  parameter diagnostics(diagRep, t) "diagnostic measures about model solve"
*
* --- for GGIG backend
*
  parameter p_toGui(*,*,*,*,*%ddraws%);
  option kill=diagnostics;

  set dummyReportLabels "Add labels used in p_toGui which might not be known to UELs" / cal,prot,f-m_skl,f-f_skl,f-m_nsk,f-f_nsk,c-care,a-care /;


  file xml / "gui/generated.xml" /;
  scalar XMLiter;
  $$onmulti
  set is / va-agg /;
  $$offmulti
  set cgrp /c-sum "All products and services", c-agr "Agricultural products",c-man "Manufacturing products",c-srv "Service ouputs", c-careEcon "Care economy"/;
*
* --- aggregate to group of products for toGui
*
  srv(a)   $ sameas(a,"a-care")         = no;
  iSrv(i)  $ sameas(i,"c-care")         = no;


  set cgrp_c(cgrp,i); cgrp_c("c-srv",i)      $ isrv(i)            = YES;
                      cgrp_c("c-agr",i)      $ iagr(i)            = YES;
                      cgrp_c("c-man",i)      $ iman(i)            = YES;
                      cgrp_c("c-careEcon",i) $ sameas("c-care",i) = YES;
                      cgrp_c("c-sum",i)                           = YES;

  set toAggIC(*) "Columns to aggregate for products" / set.stb,set.row,tot,va-agg,dom,domM,imp,exp,bot,dms,output,mrkApp,impM,arm,domMrg,impMrg,gdpMp /;
      toAggIC(gInv) = YES;
      toAggIC(aaIn) = YES;

  set toAggIR(*) "Rows to aggregate for products" / set.a,set.row,tot /;
  toAggIR(vat) = yes;
  toAggIR(mtx) = yes;
  toAggIR(stx) = yes;
  toAggIR(etx) = yes;

  set agrp /a-sum "All sectors",a-agr "Agriculture" , a-man "Manufacturing",a-srv "Services",a-careEcon "Care economy" /;

  set agrp_a(agrp,a);  agrp_a("a-srv",a)      $ srv(a)             = YES;
                       agrp_a("a-agr",a)      $ agr(a)             = YES;
                       agrp_a("a-man",a)      $ man(a)             = YES;
                       agrp_a("a-careEcon",a) $ sameas("a-care",a) = YES;
                       agrp_a("a-sum",a)                           = YES;

  set aggFlag(*,*,*);

  set agrpp;
      agrpp(agrp) = YES;
      agrpp(aAgr) = YES;
  set agrpp_a(*,a);
  agrpp_a(agrp,a) $ agrp_a(agrp,a)  = YES;
  agrpp_a(aAgr,a) $ mapAagr(aAgr,a) = YES;

  set toAggAR(*) "Rows to aggregate for activities" / set.i,phn,tot,sumTx,va-agg,nd-agg,gdp-cont /;
  toAggAr(actFp) = YES;
  toAggAr(fpPar) $ actFpNest(fpPar) = YES;
  toAggAR(cgrp) = YES;
  toAggAR(ptx) = YES;
  toAggAR(psb) = YES;
  toAggAR(ftx) = YES;

  set toAggAC(*)  "Columns to aggregate" / set.i,tot /;
  toAggAC(cgrp) = YES;
  set VQG / V,Q,G /;
  set VG  / V,Q /;
  set VQGTP / V,Q,G,T,P /;

  set fpNestToDel(fpNest);

  set isAdd(is);
  set isExl(is);
  set aggH3(*),aggH4(*);

  $$iftheni.task "%task%"=="Baseline"
     alias(dimRep3,*);
     alias(dimRep4,*);
  $$else.task
     set dimRep1,dimRep2,dimRep3,dimRep4;
     $$GDXIN "%odir%/%BaUFile%"
       $$load  dimRep1<p_toGui.dim1
       $$load  dimRep2<p_toGui.dim2
       $$load  dimRep3<p_toGui.dim3
       $$load  dimRep4<p_toGui.dim4
     $$GDXIN
  $$endif.task
  alias(uel,*);

  aggH3(dimRep3)    = YES;
  aggH3(tf)     = no;
  aggH3(fpPar)  = no;
  aggH3(a)      = no;
  aggH3(age)    = no;
  aggH3(agrpp)  = no;
  aggH3(agrp)   = no;
  aggH3(cGTAP)   = no;
  aggH3(aGTAP)   = no;
  aggH3(regGTAP)   = no;
  aggH3(allMetaItems) = no;
  $$ifi defined nCur aggh(nCur) = no;
  aggH3(emSrc) = no;
  aggH3(em) = no;
  aggH3(a0) = no;
  aggH3(i0) = no;
  aggH3(fp) = no;

  aggH4(dimRep4)    = YES;
  aggH4(tf)     = no;
  aggH4(fpPar)  = no;
  aggH4(a)      = no;
  aggH4(age)    = no;
  aggH4(agrpp)  = no;
  aggH4(agrp)   = no;
  aggH4(cGTAP)   = no;
  aggH4(aGTAP)   = no;
  aggH4(regGTAP)   = no;
  aggH4(allMetaItems) = no;
  $$ifi defined nCur aggh(nCur) = no;
  aggH4(emSrc) = no;
  aggH4(em) = no;
  aggH4(a0) = no;
  aggH4(i0) = no;
  aggH4(fp) = no;

  $$ifi "%endoTech%"=="on" set aggEndoTech / inn,xrd,nVar,obSol,tpRd /;


  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
