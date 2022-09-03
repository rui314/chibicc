pvs-studio-analyzer trace -- make
pvs-studio-analyzer analyze -f ./strace_out
plog-converter -a GA:1,2 -t tasklist -o report.tasks ./PVS-Studio.log 
rm strace_out PVS-Studio.log 