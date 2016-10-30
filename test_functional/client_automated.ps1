$global:SERVER="127.0.0.1"

$global:TEST_CASES_FILE=Resolve-Path ".\test_cases.txt"
$global:CLIENTS='TEST_CLIENT1','TEST_CLIENT2','TEST_CLIENT3','TEST_CLIENT4','TEST_CLIENT5','TEST_CLIENT6','TEST_CLIENT7','TEST_CLIENT8'
$global:CLIENT_DIRECTORY='.\client_executable'
$global:TEMPLATE_FILE=Resolve-Path "${global:CLIENT_DIRECTORY}\template.cfg"

function initialise()
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

                public void add(string processName, string args)
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

#Clear-Host
initialise

Write-Host ""
Write-Host "Client automation is starting : " -foregroundcolor "Yellow"
Write-Host ""

Set-Location $PSScriptRoot

$client_executable = Resolve-Path ".\client_executable\client.exe"
$process_executor = New-Object MultiProcessStarter



#Start tests       
foreach( $client in $global:CLIENTS )
{
    Write-Host "Starting test for $client"
    [string]$current_args = "$global:TEMPLATE_FILE $global:SERVER $client $global:TEST_CASES_FILE"
    $process_executor.add($client_executable, $current_args)
}

$sw = [System.Diagnostics.Stopwatch]::startNew()

$process_executor.execute()
$process_executor.wait_for_all()
$sw.Stop()
Write-Host
$time_taken = $sw.ElapsedMilliseconds.toString();
Write-Host "Time : $time_taken milliseconds "
# Ending
Write-Host ""
Write-Host "Client automation finished , press enter to quit." -foregroundcolor "Yellow"
Read-Host
Write-Host ""