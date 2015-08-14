

set_requestfile_path("$(AREA_DETECTOR)", "ADApp/Db")

set_pass0_restoreFile("auto_settings.sav", "P=$(PREFIX)")
set_pass1_restoreFile("auto_settings.sav", "P=$(PREFIX)")
# save settings every thirty seconds
doAfterIocInit("create_monitor_set('auto_settings.req',30,'P=$(PREFIX)')")

epicsEnvSet("BUILT_SETTINGS", "built_settings.req")
epicsEnvSet("BUILT_POSITIONS", "built_positions.req")
autosaveBuild("$(BUILT_SETTINGS)", "_settings.req", 1)
