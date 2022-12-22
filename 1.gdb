file fn_log
set print pretty
b main
#b EnterProcOutScreenDevice
#b InitFromYMAL
#b fn_parse.cpp:625
#b LoadSharedMemory
#b EnterProcChannel
b HoldChannel
r
