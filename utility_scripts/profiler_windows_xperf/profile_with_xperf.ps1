param( $TARGET_EXECUTABLE = "../../bin/ome.exe" )
[string]$XPERF_COMMAND="xperf"
[string]$OUTPUT_ETL_NAME="report.etl"
[int]$PROFILE_DURATION_IN_SECONDS=10

function initialise_multiprocess_starter()
{
  $source = @"
            using System;
            using System.Collections.Generic;
            using System.Diagnostics;


            public class MultiProcessStarter
            {
                private System.Collections.Generic.List<string> m_processNames;
                private System.Collections.Generic.List<string> m_processArgs;
				private System.Collections.Generic.List<Process> m_processes;

                public MultiProcessStarter()
                {
                    m_processNames = new System.Collections.Generic.List<string>();
                    m_processArgs = new System.Collections.Generic.List<string>();
					m_processes = new System.Collections.Generic.List<Process>();
                }

                public void add(string processName, string args = "")
                {
                    m_processNames.Add(processName);
                    m_processArgs.Add(args);
                }

                public void execute()
                {
                    for (int i = 0; i < m_processNames.Count; i++)
                    {
                        m_processes.Add(System.Diagnostics.Process.Start(m_processNames[i], m_processArgs[i]));
                    }
                }
				
				public void wait_for_all()
				{
					foreach(var process in m_processes)
                    {
                        process.WaitForExit();
                        process.Close();
                    }
				}

                public void kill_all()
				{
					foreach(var process in m_processes)
                    {
                        process.Kill();
                    }
				}

            }
"@

	Add-Type -TypeDefinition $source;
}

function does_xperf_exist()
{
    [bool]$retval = $false
    try
    {
        Invoke-Expression $XPERF_COMMAND
        $retval = $true
    }
    catch
    {
        $retval = $false
    }
    Clear-Host
    return $retval
}

#Entry point
$xperf_exists = does_xperf_exist

if( $xperf_exists -eq $false )
{
    Write-Error "You need to install Windows Performance Toolkit. You can install it via Windows SDK installer"
    exit -1
}

if (-not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] “Administrator”))
{
    Write-Error "You have to run this script with administrator rights"
    exit -2
}

Clear-Host
initialise_multiprocess_starter
$target_executable_path = Resolve-Path $TARGET_EXECUTABLE
Write-Host "Profiling $target_executable_path"
if( Test-Path "./$OUTPUT_ETL_NAME" )
{
    Rename-Item -Path "./$OUTPUT_ETL_NAME" -NewName "./$OUTPUT_ETL_NAME.old"
}
# Start tracing
$XPERF_FULL_COMMAND="$XPERF_COMMAND -on Base -stackwalk profile"
Invoke-Expression $XPERF_FULL_COMMAND
# Start OME and functional tests
$process_executor = New-Object MultiProcessStarter
#Add your processes by calling $process_executor.add( "" );
[string]$ome_executable= Resolve-Path ..\..\bin\ome.exe
$process_executor.add($ome_executable)
[string]$functional_tester = Resolve-path ..\..\test_functional\client_automated_test.bat
$process_executor.add("cmd.exe", "-c $functional_tester")
$process_executor.execute()
#Wait
Start-Sleep -s $PROFILE_DURATION_IN_SECONDS
#Kill processes
$process_executor.kill_all()
# Stop tracing
$XPERF_FULL_COMMAND="$XPERF_COMMAND -d $OUTPUT_ETL_NAME"
Invoke-Expression $XPERF_FULL_COMMAND
# Now open the trace
Invoke-Expression "./$OUTPUT_ETL_NAME"
# Exit gracefully
exit 0