********************************************************************************
$ontext

   CAPRI project

   GAMS file : READIEC.GMS

   @purpose  :
   @author   :
   @date     : 23.08.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************

 parameter p_shocks;



* -------------------------------------------------------------------------------------------------------------
*
*   read human health
*
* -------------------------------------------------------------------------------------------------------------

*execute 'csv2GDX "human health/ETH_humanHealth_CG_shock.csv" id=p_humanHealth index=1,2,3 values=4..lastCol useHeader=y'
*execute 'csv2GDX "human health/ETH_humanHealth_CR_shock.csv" id=p_humanHealth index=1,2,3 values=4..lastCol useHeader=y'
*execute 'csv2GDX "human health/ETH_humanHealth_SR_shock.csv" id=p_humanHealth index=1,2,3 values=4..lastCol useHeader=y'

set years,clim_scen,scen_grp;
$gdxin ETH_humanHealth_CG_shock
   $$load years<p_humanHealth.dim1
   $$load clim_Scen<p_humanHealth.dim2
   $$load scen_grp<p_humanHealth.dim3
$gdxIn

parameter p_humanHealth(*,*,*);

execute_load "ETH_humanHealth_CG_shock" p_humanHealth;
p_shocks(years,clim_scen,scen_grp,"CG","health","") = p_humanHealth(years,clim_scen,scen_grp);

execute_load "ETH_humanHealth_CR_shock" p_humanHealth;
p_shocks(years,clim_scen,scen_grp,"CR","health","") = p_humanHealth(years,clim_scen,scen_grp);

execute_load "ETH_humanHealth_SR_shock" p_humanHealth;
p_shocks(years,clim_scen,scen_grp,"SR","health","") = p_humanHealth(years,clim_scen,scen_grp);


* -------------------------------------------------------------------------------------------------------------
*
*   hydrop power
*
* -------------------------------------------------------------------------------------------------------------


* execute 'csv2GDX "hydroPower/ETH_hydropower_CG_irrPriority_generation.csv" id=p_hydrop index=1,2 values=3 useHeader=y'
* execute 'csv2GDX "hydroPower/ETH_hydropower_SR_irrPriority_generation.csv" id=p_hydrop index=1,2 values=3 useHeader=y'


*  execute 'csv2GDX "hydroPower/ETH_hydropower_CG_HydroPriority_generation.csv" id=p_hydrop index=1,2 values=3 useHeader=y'
*  execute 'csv2GDX "hydroPower/ETH_hydropower_SR_HydPriority_generation.csv" id=p_hydrop index=1,2 values=3 useHeader=y'


$onmulti
set years,SSP;
$gdxin  ETH_hydropower_CG_irrPriority_generation
   $$load years<p_hydrop.dim1
   $$load SSP<p_hydrop.dim2
$gdxIn
$offmulti

parameter p_hydrop(*,*);

execute_load "ETH_hydropower_CG_irrPriority_generation" p_hydrop;
p_shocks(years,ssp,"","CG","hydro","irr") = p_hydrop(years,SSP);

execute_load "ETH_hydropower_SR_irrPriority_generation" p_hydrop;
p_shocks(years,ssp,"","SR","hydro","irr") = p_hydrop(years,SSP);

execute_load "ETH_hydropower_CG_HydroPriority_generation" p_hydrop;
p_shocks(years,ssp,"","CG","hydro","pow") = p_hydrop(years,SSP);

execute_load "ETH_hydropower_SR_HydPriority_generation" p_hydrop;
p_shocks(years,ssp,"","SR","hydro","pow") = p_hydrop(years,SSP);


* -------------------------------------------------------------------------------------------------------------
*
*   inland flooding
*
* -------------------------------------------------------------------------------------------------------------


*execute 'csv2GDX "inland flooding/ETH_inland_flooding_capital_loss.csv"      id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "inland flooding/ETH_inland_flooding_capital_lossAdapt.csv" id=p_flood_capital index=1,2 values=3 useHeader=y'

$onmulti
set years,SSP;
$gdxin  ETH_inland_flooding_capital_loss
   $$load years<p_flood_capital.dim1
   $$load SSP<p_flood_capital.dim2
$gdxIn
$offmulti

parameter p_flood_capital(*,*)

execute_load "ETH_inland_flooding_capital_lossAdapt" p_flood_capital;
p_shocks(years,ssp,"","","inland_flood","adapt") = p_flood_capital(years,SSP);

execute_load "ETH_inland_flooding_capital_loss" p_flood_capital;
p_shocks(years,ssp,"","","inland_flood","no_adapt") = p_flood_capital(years,SSP);

* -------------------------------------------------------------------------------------------------------------
*
*   ruban flooding
*
* -------------------------------------------------------------------------------------------------------------


*execute 'csv2GDX "urban flooding/ETH_urbanflooding_CG_cost.csv"        id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "urban flooding/ETH_urbanflooding_CG_cost_pctCap.csv" id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "urban flooding/ETH_urbanflooding_CR_cost.csv"        id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "urban flooding/ETH_urbanflooding_CR_cost_pctCap.csv" id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "urban flooding/ETH_urbanflooding_SR_cost.csv"        id=p_flood_capital index=1,2 values=3 useHeader=y'
*execute 'csv2GDX "urban flooding/ETH_urbanflooding_SR_cost_pctCap.csv" id=p_flood_capital index=1,2 values=3 useHeader=y'


$onmulti
set years,SSP;
$gdxin  ETH_urbanflooding_CG_cost
   $$load years<p_flood_capital.dim1
   $$load SSP<p_flood_capital.dim2
$gdxIn
$offmulti

execute_load "ETH_urbanFlooding_CG_cost" p_flood_capital;
p_shocks(years,ssp,"","CG","urban_flood","cost") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_CG_cost_pctCap" p_flood_capital;
p_shocks(years,ssp,"","CG","urban_flood","cost_pctCap") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_CR_cost" p_flood_capital;
p_shocks(years,ssp,"","CR","urban_flood","cost") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_CR_cost_pctCap" p_flood_capital;
p_shocks(years,ssp,"","CR","urban_flood","cost") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_SR_cost_pctCap" p_flood_capital;
p_shocks(years,ssp,"","SR","urban_flood","cost_pctCap") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_SR_cost" p_flood_capital;
p_shocks(years,ssp,"","SR","urban_flood","cost") = p_flood_capital(years,SSP);

execute_load "ETH_urbanFlooding_CG_cost_pctCap" p_flood_capital;
p_shocks(years,ssp,"","CG","urban_flood","cost_pctCap") = p_flood_capital(years,SSP);



* -------------------------------------------------------------------------------------------------------------
*
*   read labor heat stress
*
* -------------------------------------------------------------------------------------------------------------

* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CG_agr_shock_.csv"      id=p_lab_heat_stress_ag index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CG_ind_shock_.csv"      id=p_lab_heat_stress_ind index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CG_ser_shock_.csv"      id=p_lab_heat_stress_ser index=1,2,3 values=4 useHeader=y'

* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CR_agr_shock_.csv"      id=p_lab_heat_stress_ag index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CR_ind_shock_.csv"      id=p_lab_heat_stress_ind index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_CR_ser_shock_.csv"      id=p_lab_heat_stress_ser index=1,2,3 values=4 useHeader=y'

* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_SR_agr_shock_.csv"      id=p_lab_heat_stress_ag index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_SR_ind_shock_.csv"      id=p_lab_heat_stress_ind index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "labor heat stress/ETH_laborheatstress_SR_ser_shock_.csv"      id=p_lab_heat_stress_ser index=1,2,3 values=4 useHeader=y'

*$exit
*
*
* --- combine
*

 parameter p_lab_heat_stress_ag(*,*,*),p_lab_heat_stress_ind(*,*,*),p_lab_heat_stress_ser(*,*,*);

$onmulti
set years,clim_scen,scen_grp;
$gdxin ETH_laborheatstress_SR_agr_shock_
   $$load years<p_lab_heat_stress_ag.dim1
   $$load clim_Scen<p_lab_heat_stress_ag.dim2
   $$load scen_grp<p_lab_heat_stress_ag.dim3
$gdxIn
$offmulti

execute_load "ETH_laborheatstress_SR_agr_shock_" p_lab_heat_stress_ag;
p_shocks(years,clim_scen,scen_grp,"SR","lab","agr") = p_lab_heat_stress_ag(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_SR_ind_shock_" p_lab_heat_stress_ind;
p_shocks(years,clim_scen,scen_grp,"SR","lab","ind") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_SR_ser_shock_" p_lab_heat_stress_ser;
p_shocks(years,clim_scen,scen_grp,"SR","lab","ser") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);


execute_load "ETH_laborheatstress_CR_agr_shock_" p_lab_heat_stress_ag;
p_shocks(years,clim_scen,scen_grp,"CR","lab","agr") = p_lab_heat_stress_ag(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_CR_ind_shock_" p_lab_heat_stress_ind;
p_shocks(years,clim_scen,scen_grp,"CR","lab","ind") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_CR_ser_shock_" p_lab_heat_stress_ser;
p_shocks(years,clim_scen,scen_grp,"CR","lab","ser") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);


execute_load "ETH_laborheatstress_CG_agr_shock_" p_lab_heat_stress_ag;
p_shocks(years,clim_scen,scen_grp,"CG","lab","agr") = p_lab_heat_stress_ag(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_CG_ind_shock_" p_lab_heat_stress_ind;
p_shocks(years,clim_scen,scen_grp,"CG","lab","ind") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);

execute_load "ETH_laborheatstress_CG_ser_shock_" p_lab_heat_stress_ser;
p_shocks(years,clim_scen,scen_grp,"CG","lab","ser") = p_lab_heat_stress_ind(years,clim_scen,scen_grp);

* -------------------------------------------------------------------------------------------------------------
*
*   read livestock
*
* -------------------------------------------------------------------------------------------------------------

* execute 'csv2GDX "livestock/ETH_livestock_CG_shock_.csv"  id=p_livestock index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "livestock/ETH_livestock_CR_shock_.csv"  id=p_livestock index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "livestock/ETH_livestock_CRM_shock_.csv"  id=p_livestock index=1,2,3 values=4 useHeader=y'

 parameter p_livestock(*,*,*);

$onmulti
set years,clim_scen,scen_grp;
$gdxin ETH_livestock_CG_shock_
   $$load years<p_livestock.dim1
   $$load clim_Scen<p_livestock.dim2
   $$load scen_grp<p_livestock.dim3
$gdxIn
$offmulti

execute_load "ETH_livestock_CG_shock_" p_livestock;
p_shocks(years,clim_scen,scen_grp,"CG","livestock","") = p_livestock(years,clim_scen,scen_grp);

execute_load "ETH_livestock_CR_shock_" p_livestock;
p_shocks(years,clim_scen,scen_grp,"CR","livestock","") = p_livestock(years,clim_scen,scen_grp);

execute_load "ETH_livestock_CRM_shock_" p_livestock;
p_shocks(years,clim_scen,scen_grp,"CRM","livestock","") = p_livestock(years,clim_scen,scen_grp);


* -------------------------------------------------------------------------------------------------------------
*
*   read road bridges
*
* -------------------------------------------------------------------------------------------------------------

* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CG_delay_cost.csv"        id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CG_delay_pctLabor.csv"    id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CG_dmg_Cost.csv"          id=p_roadbridge index=1,2,3 values=4 useHeader=y'
  execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CG_dmg_cost_pctcap.csv"   id=p_roadbridgePCT index=1,2,3 values=4,5 useHeader=y'

* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CR_delay_cost.csv"        id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CR_delay_pctLabor.csv"    id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CR_dmg_Cost.csv"          id=p_roadbridge index=1,2,3 values=4 useHeader=y'
  execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_CR_dmg_cost_pctcap.csv"   id=p_roadbridgePCT index=1,2,3 values=4,5 useHeader=y'

* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_SR_delay_cost.csv"        id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_SR_delay_pctLabor.csv"    id=p_roadbridge index=1,2,3 values=4 useHeader=y'
* execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_SR_dmg_Cost.csv"          id=p_roadbridge index=1,2,3 values=4 useHeader=y'
  execute 'csv2GDX "Roads and bridges/ETH_roadandbridge_SR_dmg_cost_pctcap.csv"   id=p_roadbridgePCT index=1,2,3 values=4,5 useHeader=y'

$onmulti
set years,clim_scen,scen_grp;
$gdxin ETH_roadandbridge_CG_delay_cost
   $$load years<p_roadbridge.dim1
   $$load clim_Scen<p_roadbridge.dim2
   $$load scen_grp<p_roadbridge.dim3
$gdxIn
$offmulti


parameter p_roadBridge(*,*,*);
set dummy / incremental_cost_pctcapital/;
parameter p_roadBridgePCT(*,*,*,*);

execute_load "ETH_roadandbridge_CG_delay_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CG","roadBridge","incremental_delay_milhrs") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CG_delay_pctLabor" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CG","roadBridge","delay_pctLabor") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CG_dmg_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CG","roadBridge","dmg_Cost") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CG_dmg_cost_pctCap" p_roadbridgePCT;
p_shocks(years,clim_scen,scen_grp,"CG","roadBridge","dmg_Cost_pctCap") = p_roadbridgePCT(years,clim_scen,scen_grp,"incremental_cost_pctcapital");

execute_load "ETH_roadandbridge_CR_delay_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CR","roadBridge","incremental_delay_milhrs") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CR_delay_pctLabor" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CR","roadBridge","delay_pctLabor") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CR_dmg_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"CR","roadBridge","dmg_Cost") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_CR_dmg_cost_pctCap" p_roadbridgePCT;
p_shocks(years,clim_scen,scen_grp,"CR","roadBridge","dmg_Cost_pctCap") = p_roadbridgePCT(years,clim_scen,scen_grp,"incremental_cost_pctcapital");

execute_load "ETH_roadandbridge_SR_delay_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"SR","roadBridge","incremental_delay_milhrs") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_SR_delay_pctLabor" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"SR","roadBridge","delay_pctLabor") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_SR_dmg_cost" p_roadbridge;
p_shocks(years,clim_scen,scen_grp,"SR","roadBridge","dmg_Cost") = p_roadbridge(years,clim_scen,scen_grp);
execute_load "ETH_roadandbridge_SR_dmg_cost_pctCap" p_roadbridgePCT;
p_shocks(years,clim_scen,scen_grp,"SR","roadBridge","dmg_Cost_pctCap") = p_roadbridgePCT(years,clim_scen,scen_grp,"incremental_cost_pctcapital");


* -------------------------------------------------------------------------------------------------------------
*
*   read WASH
*
* -------------------------------------------------------------------------------------------------------------

 execute 'csv2GDX "WASH/ETH_wash_CG_shock.csv"        id=p_wash index=1,2,3 values=4 useHeader=y'
 execute 'csv2GDX "WASH/ETH_wash_SR_shock.csv"        id=p_wash index=1,2,3 values=4 useHeader=y'



$onmulti
set years,clim_scen,scen_grp;
$gdxin ETH_wash_CG_shock
   $$load years<p_wash.dim1
   $$load clim_Scen<p_wash.dim2
   $$load scen_grp<p_wash.dim3
$gdxIn
$offmulti


parameter p_wash(*,*,*);

execute_load "ETH_wash_CG_shock" p_wash;
p_shocks(years,clim_scen,scen_grp,"CG","wash","pct_change_supply") = p_wash(years,clim_scen,scen_grp);

execute_load "ETH_wash_SR_shock" p_wash;
p_shocks(years,clim_scen,scen_grp,"SR","wash","pct_change_supply") = p_wash(years,clim_scen,scen_grp);

* --- output summary results

execute_unload "shocks" p_shocks;



$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
