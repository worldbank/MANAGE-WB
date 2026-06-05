********************************************************************************
$ontext

   MANAGE project

   GAMS file : fromFrEndReadSheet.INC

   @purpose  : Read one single sheet into a GAMS parameter
   @author   : W.Britz
   @date     : 20.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : inc/fromFrEndExcel.inc

$offtext
********************************************************************************
$onEmbeddedCode Connect:
- PandasExcelReader:
    file:  '%odir%/%shk_file_xslx%.xlsx'
    symbols:
      - name: input
        rowDimension: 7
        columnDimension: 1
        range: %curSheet%!A1
      - name: check
        rowDimension: 6
        columnDimension: 0
        range: %curSheet%!A1
        type : set
- GAMSWriter:
    writeAll: True
$offEmbeddedCode
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
