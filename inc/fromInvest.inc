********************************************************************************
$ontext

   MANAGE project

   GAMS file : fromInvest.INC

   @purpose  : Read CSV file from SEALS/INVEST with eco-system service
               feedback from simulated land use change
   @author   : W.Britz
   @date     : 29.05.24
   @since    :
   @refDoc   :
   @seeAlso  : inc/toInvest.inc, section in inc/opt.inc
   @calledBy : run.gms

$offtext
********************************************************************************
  if (years(tt) gt %baseYear%+1,
*
*    --- read CSV file and store values in p_investShocks
*
     EmbeddedCode Connect:
     - CSVReader:
         file: %scrdir%/toManage.csv
         name: p_investShocks
         indexColumns: 1
         valueColumns: "2:lastCol"
     - GAMSWriter:
         symbols:
          - name: p_investShocks
     endEmbeddedCode p_investShocks
*
*    --- map shocks from CSV file back to damage channels of model
*
     $$ifi "%investfeedbacksCropProduction%"=="ON"  dmgSup.fx(actHa,"cropProd",tt)         = p_investShocks("%regGtapNameOri%","cropProd");
     $$ifi "%investfeedbacksCropPollination%"=="ON" dmgSup.fx(actHa,"cropPollination",tt)  = p_investShocks("%regGtapNameOri%","cropPollination");
     $$ifi "%investfeedbacksErosion%"=="ON"         dmgSup.fx(actHa,"erosion",tt)          = p_investShocks("%regGtapNameOri%","erosion");

  );

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
