********************************************************************************
$onText
 inc/sensStoch/lhs_py_replace.inc

 Replace in run_stoch.gms / run_sens.gms the five lines:
   $if exist "%scrdir%\lhsout.gms" execute "del ...
   put_utility batch 'msglog' / "inc/sensstoch/lhs.exe ...
   execute "inc\sensstoch\lhs.exe ...
   put_utility batch 'msglog' / "%GAMSEXE% inc/sensStoch/lhs_to_gdx.gms ...
   execute                      "%GAMSEXE% inc/sensStoch/lhs_to_gdx.gms ...

 with:
   $include "inc/sensStoch/lhs_py_replace.inc"

 Why: lhs.exe is blocked by Windows Device Guard.
      This recreates its output (lhs.gdx with p_doe) via embedded Python.

 How nFactor reaches Python:
   run_stoch.gms writes lhsInput.txt with one DATASET line per factor
   Python counts those lines -> nFactor
   No execute_unload, no gamstransfer read, no temp file needed

 How lhs.gdx is written:
   gamstransfer m.write() writes directly to %scrdir%/lhs.gdx
   No intermediate lhsgams.gms, no cmd copy, no execute needed

 GAMS scanner rules learned for embedded Python:
   - Use chr(10) chr(92) chr(47) instead of \n \\ /
   - Use single-quoted strings to avoid scanner issues with double-quoted \X
   - r'%macro%' is safe if macro value does not end with \
   - %macro% in Python code is substituted at compile time (not gams.get)
   - gams.get() retrieves GAMS symbols but returns ECSymbol not a number
   - $if not exist is compile-time: never use to check runtime-created files
   - No leading _ in filenames passed to execute_unload or file/put
$offText
********************************************************************************

EmbeddedCode Python:
import os
import numpy as np
import pandas as pd
from scipy.stats import qmc
from scipy.stats import truncnorm
from scipy.stats import triang
from gams import transfer as gt
scrdir  = r'%scrdir%'.replace(chr(92), chr(47))

# Read nFactor by counting lines in the DATASET section of lhsInput.txt
with open(os.path.join(os.getcwd(),scrdir+'/lhsInput.txt')) as f:
    content = f.read()

parts = content.split('DATASET:')
if len(parts) < 2:
    raise RuntimeError('DATASET section not found in lhsInput.txt')

nFactor = sum(1 for ln in parts[1].split(chr(10)) if ln.strip())
if nFactor == 0:
    raise RuntimeError('nFactor=0: check name_1..name_5 and lo/up bounds.')

nDraws = %nDraws%

try:
    seed_val = int(round(abs(float(os.environ.get('seed', '42'))))) % (2**31 - 1)
except Exception:
    seed_val = 42

sample = qmc.LatinHypercube(d=nFactor, seed=seed_val).random(n=nDraws)

#
# --- read the distribution
#
k = 0
for ln in parts[1].split(chr(10)):
    if ln:
#
#      --- read inform in steering file: name of function, mode, stddev, lo,up
#
       function = ln[15:35].strip()

       if function == 'Bounded normal':
          mode     = float(ln[30:42].strip())
          stddev   = float(ln[43:55].strip())
          lo       = float(ln[56:68].strip())
          up       = float(ln[69:81].strip())
          lo = (lo - mode) / stddev
          up = (up - mode) / stddev
          print('[LHS-Py] factor {} function {} mode {} stddev {} lo {} up {}'.format((k+1),function,mode,stddev,lo,up))
       elif function == 'Triangular':
          lo       = float(ln[30:42].strip())
          mode     = float(ln[43:55].strip())
          up       = float(ln[56:68].strip())
          c        = (mode-lo)/(up-lo)
          print('[LHS-Py] factor {} function {} lo {} mode {} up {}'.format((k+1),function,lo,mode,up))
       elif function == 'Uniform':
          lo       = float(ln[30:42].strip())
          up       = float(ln[43:55].strip())
          print('[LHS-Py] factor {} function {} lo {} up {}'.format(k,function,lo,up))

       for d in range(nDraws):
          if function == 'Bounded normal':
              sample[d,k] = truncnorm.ppf(sample[d,k],lo,up,mode,stddev)
          elif function == 'Triangular':
              sample[d,k] = triang.ppf(sample[d,k],c)*(up-lo) + lo
          elif function == 'Uniform':
              sample[d,k] = lo + sample[d,k] * (up-lo)
       k+=1


# Write lhs.gdx directly via gamstransfer
lhs_gdx = scrdir + '/lhs.gdx'

recs = [('d{}'.format(d+1), 'f{}'.format(k+1), float(sample[d,k]))
        for d in range(nDraws) for k in range(nFactor)]
df = pd.DataFrame(recs, columns=['i', 'j', 'value'])

m = gt.Container()
gt.Parameter(m, 'p_doe', ['*', '*'], records=df)
m.write(lhs_gdx)

print('[LHS-Py] {} draws x {} factors | seed={}'.format(nDraws, nFactor, seed_val))
EndEmbeddedCode
