#/bin/bash
#VARIABLES
PROFILING_TIME_OUT_SECONDS=30
GPROF_OUTPUT_REPORT="ome_gprof_report.txt"
#ENTRY POINT
echo "Profiling with Gprof is starting"
#REMOVE IF A REPORT EXISTS
rm -rf ../bin/${GPROF_OUTPUT_REPORT}
#COPY GMON.OUT TO SERVER EXECUTABLE DIRECTORY
cp ./gmon.out ../bin/
cd ../bin
#FORKING SERVER
echo "Forking server..."
{
    gprof ./ome > ${GPROF_OUTPUT_REPORT}
}&
#FORKING CLIENT AUTOMATION
cd ../test_functional/
echo "Forking client automation..."
{
    ./client_automated_test.sh 127.0.0.1 test_cases_simple.txt 2
}&
# Wait till timeout
sleep $PROFILING_TIME_OUT_SECONDS
# Kill the server
kill $(ps aux | grep ome | awk '{print $2}')
# Kill the client automation
kill $(ps aux | grep client | awk '{print $2}')
# Move gperf report to current directory
cd ..
mv bin/${GPROF_OUTPUT_REPORT} utility_scripts/profiler_linux_gprof/${GPROF_OUTPUT_REPORT}
# Remove initially copied gmon.out
rm -rf bin/gmon.out
#End of the script
echo "Profiling ended. See ${GPROF_OUTPUT_REPORT} report file"