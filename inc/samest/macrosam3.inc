********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : MACROSAM3.INC

   @purpose  : Define MacroSam from cleansed protoSAM
               Include file to produce an aggregated SAM, MACSAM3. Aggregates defined from
               subsets in the Excel file. Sets are defined in the main program and then
               loaded fom excel. Parameters are also defined in the main program.
               Specification of MACMAP3 below must be changed if the amac3 aggregates
               in the excel file are changed.
   @author   :
   @date     : 07.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :  macTotal_eg_agg.inc

$offtext
********************************************************************************

* --- Aggregate from proto SAM

  macSam(acmnt,acmntp,"fromProto") =
   sum((ac,acp) $ (macmap3(acmnt,ac) and macmap3(acmntp,acp)),SAM(ac,acp,"cleansed")) ;

* --- Computing and checking totals

  macSam('TOTAL3',ACMNT,"fromProto")  = sum(ACMNT2P, macSam(ACMNT2P,ACMNT,"fromProto"));
  macSam(ACMNT,'TOTAL3',"fromProto") = sum(ACMNT2P, macSam(ACMNT,ACMNT2P,"fromProto"));
  samBalChk3(ACMNT) = macSam('TOTAL3',ACMNT,"fromProto") - macSam(ACMNT,'TOTAL3',"fromProto");


* -- Define non-zero macro-sam elements based on aggregated proto-sam
*
  macmnz(acm,acmp) $ macSam(acm,acmp,"fromProto") = yes ;

* --- Non-zero elements in macSam3. Will count as non-zero if there are non-zero
*     elements in the aggregation, even if the aggregate is zero.
 macmnz(acmnt,acmntp) =
  sum((ac,acp)$(macMap3(acmnt,ac) and macMap3(acmntp,acp) and SAM(ac,acp,"cleansed")), 1) ;

* --- add in total row and column if they have non-zero elements
 macmNz("total3",acmnt) = sum(acmnt2p $ macSam(acmnt2p,acmnt,"fromProto"), 1) ;
 macmNZ(acmnt,"total3") = sum(acmnt2p $ macSam(acmnt,acmnt2p,"fromProto"), 1) ;
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
