$global:SERVER="127.0.0.1"
$global:TEST_CASES_FILE=Resolve-Path ".\test_cases.txt"
$global:CLIENTS=@()# For ex 'TEST_CLIENT1','TEST_CLIENT2','TEST_CLIENT3','TEST_CLIENT4','TEST_CLIENT5','TEST_CLIENT6','TEST_CLIENT7','TEST_CLIENT8'
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

function start_test()
{
	$client_executable = Resolve-Path ".\client_executable\client.exe"
	$process_executor = New-Object MultiProcessStarter
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
	Write-Host "Client automation finished , you can press Start button to start another test." -foregroundcolor "Yellow"
	Write-Host ""
}

initialise

Write-Host ""
Write-Host "Client automation is starting : " -foregroundcolor "Yellow"
Write-Host ""

Set-Location $PSScriptRoot

Add-Type -AssemblyName System.Windows.Forms

$FormFunctionalTest = New-Object system.Windows.Forms.Form
$FormFunctionalTest.Text = "OME Functional Test GUI"
$FormFunctionalTest.TopMost = $false
$FormFunctionalTest.FormBorderStyle = [System.Windows.Forms.FormBorderStyle]::FixedSingle
$FormFunctionalTest.MaximizeBox = $false
$FormFunctionalTest.MinimizeBox = $false
$FormFunctionalTest.Width = 377
$FormFunctionalTest.Height = 254

$label_server_address = New-Object system.windows.Forms.Label
$label_server_address.Text = "Server address"
$label_server_address.AutoSize = $true
$label_server_address.Width = 25
$label_server_address.Height = 10
$label_server_address.location = new-object system.drawing.point(21,22)
$label_server_address.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($label_server_address)

$label_test_case_file = New-Object system.windows.Forms.Label
$label_test_case_file.Text = "Test casefile"
$label_test_case_file.AutoSize = $true
$label_test_case_file.Width = 25
$label_test_case_file.Height = 10
$label_test_case_file.location = new-object system.drawing.point(20,62)
$label_test_case_file.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($label_test_case_file)

$label_number_of_clients = New-Object system.windows.Forms.Label
$label_number_of_clients.Text = "Number of clients"
$label_number_of_clients.AutoSize = $true
$label_number_of_clients.Width = 25
$label_number_of_clients.Height = 10
$label_number_of_clients.location = new-object system.drawing.point(21,98)
$label_number_of_clients.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($label_number_of_clients)

$button_start_test = New-Object system.windows.Forms.Button
$button_start_test.Text = "Start test"
$button_start_test.Width = 107
$button_start_test.Height = 37
$button_start_test.Add_Click({
			# BUTTON CLICK EVENT
			$global:SERVER= $textBox_server_address.Text
			$global:TEST_CASES_FILE= (Resolve-Path ($textBox_testcase_file.Text) )
			[int]$num_of_clients = $textBox_num_clients.Text
			for($i = 0 ; $i -lt $num_of_clients; $i++)
			{
				$global:CLIENTS += ("TEST_CLIENT" + ($i+1).ToString() )
			}
			start_test
			
})
$button_start_test.location = new-object system.drawing.point(118,149)
$button_start_test.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($button_start_test)

$textBox_server_address = New-Object system.windows.Forms.TextBox
$textBox_server_address.Text = "127.0.0.1"
$textBox_server_address.Width = 128
$textBox_server_address.Height = 20
$textBox_server_address.location = new-object system.drawing.point(144,21)
$textBox_server_address.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($textBox_server_address)

$textBox_testcase_file = New-Object system.windows.Forms.TextBox
$textBox_testcase_file.Text = ".\test_cases.txt"
$textBox_testcase_file.Width = 128
$textBox_testcase_file.Height = 20
$textBox_testcase_file.location = new-object system.drawing.point(145,61)
$textBox_testcase_file.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($textBox_testcase_file)

$textBox_num_clients = New-Object system.windows.Forms.TextBox
$textBox_num_clients.Text = "8"
$textBox_num_clients.Width = 127
$textBox_num_clients.Height = 20
$textBox_num_clients.location = new-object system.drawing.point(146,100)
$textBox_num_clients.Font = "Microsoft Sans Serif,10"
$FormFunctionalTest.controls.Add($textBox_num_clients)

[void]$FormFunctionalTest.ShowDialog()
$FormFunctionalTest.Dispose()