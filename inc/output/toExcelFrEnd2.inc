********************************************************************************
$ontext

   MANAGE project

   GAMS file : toExcelFrEnd.INC

   @purpose  : Output selected variables as time series to an EXCEL sheet
   @author   : W.Britz
   @date     : 19.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

* ------------------------------------------------------------------
*
*   Define steering blocks for Python based export
*   of some variables which I used might want to use in
*   closures swap in counterfactuals
*
* ------------------------------------------------------------------
*
* ---- number of years output, used to calculate # of exogenous ariables in each block
*
  scalar nTrun; nTrun = Card(t);
*
* --- delete previous output (before copying). This
*     might fail if the workbook is open in EXCEL
*
  $$call  "rm  %odir%/%regGtapName%_%simName%.xlsx"
*
* ---- define output block for EXCEL
*
  parameter p_excelOutput(*,*,*,*,*,*,*,*) "Output to excel: block,varname,vartext,endo_exo,dim1...3,data";
*
  singleton Set curBlock(blocks);

*
* --- keep only the first year in the records read from EXCEL, we need soley this to get the time series
*
*
  input(blocks,vars,endo_exo,dim1,dim2,dim3,varNames,t) $ ((input(blocks,vars,endo_exo,dim1,dim2,dim3,varNames,t)) $ (not t0(t))) = 0;

  embeddedCode Python:
    nTrun      = list(gams.get('ntRun',keyFormat=KeyFormat.FLAT))[0]
    t0         = list(gams.get('tt',keyFormat=KeyFormat.FLAT))[0]
    input      = list(gams.get('input',keyFormat=KeyFormat.FLAT))
    from gams_connect.connectdatabase import ConnectDatabase
  pauseEmbeddedCode

  loop(blocks,

      $$batinclude "inc/title.inc" '"Export "' blocks.tl '" to EXCEL "'

      option kill=p_excelOutput;
      curBlock(blocks) =yes;

      continueEmbeddedCode:

#        --- GAMS record for output symbol exported to EXCEL
         records = []
#        --- Get the current block
         curBlock = list(gams.get('curBlock',keyFormat=KeyFormat.FLAT))[0]

#        --- get input records as read from EXCEL
         oldVarName = ""
         for record in input:

#            --- solely take the ones in the current block
             if record[0] == curBlock:

#              --- keep the record with the # of exogenous instances required
               if record[1] == 'exo':
                  records.append(record)
               else:
#                 --- find the variable name
                  varName = record[1]
#                 --- and get the related variable
#                     get.gams is computing time intensive, only load when variable name changes
                  if not varName == oldVarName:
                     varRecs = list(gams.get(varName,keyFormat=KeyFormat.FLAT))

                  for varRec in varRecs:
                      nDim  = len(varRec) - 1;
#
#                     --- filter the right instances
                      keep = True
                      if nDim > 1:
                         keep = (varRec[0:nDim-1] == record[3:3+nDim-1])

                      if keep:
                         val    = varRec[nDim][0] + 1.E-20
                         outRec = (record[0],record[1],record[2],record[3],record[4],record[5],record[6],varRec[nDim-1],val)
                         records.append(outRec)
               oldVarName = varName
         gams.set('p_excelOutput',records)

#        --- set output sheet
         range = str(curBlock)+'!A1'

#        --- read p_excelOutput in ConnectDataBase
         cdb = ConnectDatabase(gams._system_directory, gams)
         cdb.exec_task( {'GAMSReader':{'symbols': [{'name': 'p_excelOutput'}]}})

#        --- and output to the desired sheet
         cdb.exec_task( {'PandasExcelWriter':
                           {
                             'file':r"%odir%/%regGtapName%_%simName%.xlsx",
                             'symbols': [{'name': 'p_excelOutput','range':range,
                                          'toExcelArguments':{"merge_cells":False}
                                         }]
                            }
                         })
      pauseEmbeddedCode
*$offtext

  );

  $$ontext
     - PythonCode:
          code: |
             df = connect.container.data
             for column in df:
                column_length = max(df[column].astype(str).map(len).max(), len(column))
                col_idx = df.columns.get_loc(column)
                writer.sheets['FromModel'].set_column(col_idx, col_idx, column_length)
  $$offtext
*
* --- avoid any hiccups in follow programs if something goes wrong with the EXCEL export
*
  if ( execError, execError = 0;);
  $$if not errorfree $abort Compilation error after file: %system.fn%
