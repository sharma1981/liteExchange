function initialise()
{
  $source = @"
  using System;
  
    public class FixConstants
    {
        // GENERAL
        public const char FIX_EQUALS = '=';
        public const char FIX_DELIMITER = ((char)1);
        // TAGS
        public const int FIX_TAG_VERSION = 8;
        public const int FIX_TAG_BODY_LENGTH = 9;
        public const int FIX_TAG_BODY_CHECKSUM = 10;
        public const int FIX_TAG_CLIENT_ORDER_ID = 11;
        public const int FIX_TAG_EXEC_ID = 17;
        public const int FIX_TAG_EXEC_INST = 18;
        public const int FIX_TAG_HAND_INST = 21;
        public const int FIX_TAG_SEQUENCE_NUMBER = 34;
        public const int FIX_TAG_MESSAGE_TYPE = 35;
        public const int FIX_TAG_ORDER_QUANTITY = 38;
        public const int FIX_TAG_ORDER_STATUS = 39;
        public const int FIX_TAG_ORDER_TYPE = 40;
        public const int FIX_TAG_ORDER_PRICE = 44;
        public const int FIX_TAG_SECURITY_ID = 48;
        public const int FIX_TAG_SENDER_COMPID = 49;
        public const int FIX_TAG_SENDING_TIME = 52;
        public const int FIX_TAG_ORDER_SIDE = 54;
        public const int FIX_TAG_SYMBOL = 55;
        public const int FIX_TAG_TARGET_COMPID = 56;
        public const int FIX_TAG_TIME_IN_FORCE = 59;
        public const int FIX_TAG_TRANSACTION_TIME = 60;
        public const int FIX_TAG_ENCRYPT_METHOD = 98;
        public const int FIX_TAG_HEARBEAT_INTERVAL = 108;
        public const int FIX_TAG_EXEC_TYPE = 150;
        public const int FIX_TAG_LEAVES_QTY = 151;
        // MESSAGE TYPES
        public const char FIX_MESSAGE_HEARTBEAT = '0';
        public const char FIX_MESSAGE_LOG_ON = 'A';
        public const char FIX_MESSAGE_LOG_OFF = '5';
        public const char FIX_MESSAGE_EXECUTION_REPORT = '8';
        public const char FIX_MESSAGE_NEW_ORDER = 'D';
        public const char FIX_MESSAGE_AMEND_ORDER = 'G';
        public const char FIX_MESSAGE_CANCEL_ORDER = 'F';
        // ORDER STATUS
        public const int FIX_ORDER_STATUS_NEW = 0;
        public const int FIX_ORDER_STATUS_PARTIALLY_FILLED = 1;
        public const int FIX_ORDER_STATUS_FILLED = 2;
        public const int FIX_ORDER_STATUS_DONE_FOR_DAY = 3;
        public const int FIX_ORDER_STATUS_CANCELED = 4;
        public const int FIX_ORDER_STATUS_REPLACED = 5;
        public const int FIX_ORDER_STATUS_PENDING_CANCEL = 6;
        public const int FIX_ORDER_STATUS_STOPPED = 7;
        public const int FIX_ORDER_STATUS_REJECTED = 8;
        // ORDER TYPE
        public const int FIX_ORDER_TYPE_MARKET = 1;
        public const int FIX_ORDER_TYPE_LIMIT = 2;
        // SIDE
        public const int FIX_ORDER_SIDE_BUY = 1;
        public const int FIX_ORDER_SIDE_SELL = 2;
        // TIME IN FORCE
        public const int FIX_ORDER_TIF_DAY = 0;
        // ENCRYPTION METHODS
        public const int FIX_ENCRYPTION_NONE = 0;
    }

    public class FixMessage
    {
        private System.Collections.Generic.Dictionary<int, string> m_tagValuePairs = new System.Collections.Generic.Dictionary<int, string>();
        System.Text.StringBuilder m_builder = new System.Text.StringBuilder();

        private void appendTagValueToBuilder(int tag, string value, bool delimiter = true)
        {
            m_builder.Append(tag.ToString() + FixConstants.FIX_EQUALS + value);
            if (delimiter)
            {
                m_builder.Append(FixConstants.FIX_DELIMITER);
            }
        }

        static public string getCurrentUTCDateTime()
        {
            string datetime = "";
            datetime = System.DateTime.Now.ToUniversalTime().ToString("yyyyMMdd-HH:mm:ss.fff");
            return datetime;
        }

        public static string calculateChecksum(string message)
        {
            string checksum = "";
            int sum = 0;

            foreach (char c in message)
            {
                sum += (int)c;
            }

            sum = sum % 256;
            checksum = string.Format("{0}", sum);
            checksum = checksum.PadLeft(3, '0');
            return checksum;
        }

        public char getMessageType()
        {
            return getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE)[0];
        }

        public bool isAdminMessage()
        {
            var type = getMessageType();
            if (type == FixConstants.FIX_MESSAGE_HEARTBEAT)
            {
                return true;
            }
            if (type == FixConstants.FIX_MESSAGE_LOG_ON)
            {
                return true;
            }
            if (type == FixConstants.FIX_MESSAGE_LOG_OFF)
            {
                return true;
            }
            return false;
        }

        public int calculateBodyLength()
        {
            int bodyLength = 0;

            foreach (var tagValue in m_tagValuePairs)
            {
                //  We exclude header and checksum
                if (tagValue.Key == FixConstants.FIX_TAG_VERSION)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_BODY_LENGTH)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_BODY_CHECKSUM)
                {
                    continue;
                }

                bodyLength += tagValue.Key.ToString().Length + 2 + tagValue.Value.Length; // +2 is because of = and delimiter
            }

            return bodyLength;
        }

        public void setFixVersion(string fixVersion)
        {
            setTag(FixConstants.FIX_TAG_VERSION, fixVersion);
        }

        public bool hasTag(int tag)
        {
            if (m_tagValuePairs.ContainsKey(tag))
            {
                return true;
            }
            return false;
        }

        public void setTag(int tag, string value)
        {
            m_tagValuePairs[tag] = value;
        }

        public void setTag(int tag, char value)
        {
            m_tagValuePairs[tag] = value.ToString();
        }

        public void setTag(int tag, int value)
        {
            m_tagValuePairs[tag] = value.ToString();
        }

        public string getTagValue(int tag)
        {
            return m_tagValuePairs[tag];
        }

        public void loadFromString(string input)
        {
            var tagValuePairs = input.Split(FixConstants.FIX_DELIMITER);

            foreach (var tagValuePair in tagValuePairs)
            {
                if (tagValuePair.Length == 0)
                {
                    continue;
                }
                var tokens = tagValuePair.Split(FixConstants.FIX_EQUALS);
                int tag = System.Convert.ToInt32(tokens[0]);
                string value = tokens[1];
                setTag(tag, value);
            }
        }

        static public System.Collections.Generic.List<FixMessage> loadFromFile(string fileName)
        {
            System.Collections.Generic.List<FixMessage> ret = new System.Collections.Generic.List<FixMessage>();
            if (System.IO.File.Exists(fileName))
            {
                using (System.IO.StreamReader file = new System.IO.StreamReader(fileName))
                {
                    string line;

                    while ((line = file.ReadLine()) != null)
                    {
                        if( line.StartsWith("#") == false )
                        {
                            FixMessage currentMessage = new FixMessage();
                            currentMessage.loadFromString(line);
                            ret.Add(currentMessage);
                        }
                    }
                }
            }
            return ret;
        }

        public string toString(bool sendingAsAMessage, bool updateTransactionTime = false)
        {
            // FIX VERSION
            appendTagValueToBuilder(FixConstants.FIX_TAG_VERSION, getTagValue(FixConstants.FIX_TAG_VERSION));

            // FIX SENDING TIME AND TRANSACTION, have to be before body length calculation ,but not appended for the correct order
            if (sendingAsAMessage)
            {
                var currentUTCDateTime = FixMessage.getCurrentUTCDateTime();
                setTag(FixConstants.FIX_TAG_SENDING_TIME, currentUTCDateTime);
                
                if (updateTransactionTime && isAdminMessage() == false)
                {
                    setTag(FixConstants.FIX_TAG_TRANSACTION_TIME, currentUTCDateTime);
                }
            }

            // FIX BODY LENGTH
            if (sendingAsAMessage)
            {
                var bodyLength = calculateBodyLength().ToString();
                appendTagValueToBuilder(FixConstants.FIX_TAG_BODY_LENGTH, bodyLength);
            }

            // FIX MESSAGE TYPE
            appendTagValueToBuilder(FixConstants.FIX_TAG_MESSAGE_TYPE, getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE));

            // FIX SEQUENCE NUMBER
            appendTagValueToBuilder(FixConstants.FIX_TAG_SEQUENCE_NUMBER, getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));

            // FIX SENDER COMPID
            appendTagValueToBuilder(FixConstants.FIX_TAG_SENDER_COMPID, getTagValue(FixConstants.FIX_TAG_SENDER_COMPID));

            // FIX SENDING TIME
            appendTagValueToBuilder(FixConstants.FIX_TAG_SENDING_TIME, getTagValue(FixConstants.FIX_TAG_SENDING_TIME));

            // FIX TARGET COMPID
            appendTagValueToBuilder(FixConstants.FIX_TAG_TARGET_COMPID, getTagValue(FixConstants.FIX_TAG_TARGET_COMPID));

            foreach (var tagValue in m_tagValuePairs)
            {
                if (tagValue.Key == FixConstants.FIX_TAG_VERSION)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_BODY_LENGTH)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_MESSAGE_TYPE)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_SEQUENCE_NUMBER)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_SENDING_TIME)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_SENDER_COMPID)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_TARGET_COMPID)
                {
                    continue;
                }

                if (tagValue.Key == FixConstants.FIX_TAG_BODY_CHECKSUM)
                {
                    continue;
                }

                appendTagValueToBuilder(tagValue.Key, tagValue.Value);
            }

            // FIX CHECKSUM
            if (sendingAsAMessage)
            {
                var checksum = calculateChecksum(m_builder.ToString());
                appendTagValueToBuilder(FixConstants.FIX_TAG_BODY_CHECKSUM, checksum);            
            }

            return m_builder.ToString();
        }
    }

    public class FixSession
    {
        private System.Net.Sockets.Socket m_socket = new System.Net.Sockets.Socket(System.Net.Sockets.AddressFamily.InterNetwork, System.Net.Sockets.SocketType.Stream, System.Net.Sockets.ProtocolType.Tcp);
        private System.Net.Sockets.Socket m_serverSocket = new System.Net.Sockets.Socket(System.Net.Sockets.AddressFamily.InterNetwork, System.Net.Sockets.SocketType.Stream, System.Net.Sockets.ProtocolType.Tcp);
        private System.Timers.Timer m_heartbeatTimer = new System.Timers.Timer();
        private System.Threading.Mutex m_mutex = new System.Threading.Mutex();

        public bool Connected { get; set; }
        public string TargetAddress { get; set; }
        public int TargetPort { get; set; }
        public string TargetCompid { get; set; }
        public string SenderCompid { get; set; }
        public int HeartbeatInterval { get; set; }
        public int EncryptionMethod { get; set; }
        public int IncomingSequenceNumber { get; set; }
        public int OutgoingSequenceNumber { get; set; }
        public string FixVersion { get; set; }

        public FixSession()
        {
            Connected = false;
            HeartbeatInterval = 30;
            EncryptionMethod = FixConstants.FIX_ENCRYPTION_NONE;
            IncomingSequenceNumber = 1;
            OutgoingSequenceNumber = 1;
            m_socket.NoDelay = true;
        }

        public FixMessage getBaseMessage(char messageType)
        {
            FixMessage message = new FixMessage();
            message.setFixVersion(FixVersion);
            message.setTag(FixConstants.FIX_TAG_MESSAGE_TYPE, messageType);
            // Sequence number will be added during sending
            message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, SenderCompid);
            message.setTag(FixConstants.FIX_TAG_SENDING_TIME, "");
            message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, TargetCompid);
            return message;
        }

        public FixMessage getLogonMessage()
        {
            FixMessage message = getBaseMessage(FixConstants.FIX_MESSAGE_LOG_ON);
            message.setTag(FixConstants.FIX_TAG_ENCRYPT_METHOD, EncryptionMethod);
            message.setTag(FixConstants.FIX_TAG_HEARBEAT_INTERVAL, HeartbeatInterval);
            return message;
        }

        public FixMessage getLogoffMessage()
        {
            FixMessage message = getBaseMessage(FixConstants.FIX_MESSAGE_LOG_OFF);
            return message;
        }

        private FixMessage getHeartbeatMessage()
        {
            FixMessage message = getBaseMessage(FixConstants.FIX_MESSAGE_HEARTBEAT);
            return message;
        }

        private string getSequenceFileName()
        {
            string ret = SenderCompid + "_" + TargetCompid + "_sequence.txt";
            return ret;
        }

        public bool connect()
        {
            bool result = false;
            try
            {
                m_socket.Connect(TargetAddress, TargetPort);

                send(getLogonMessage());
                FixMessage message = recv();
                if (message != null)
                {
                    if (message.hasTag(FixConstants.FIX_TAG_MESSAGE_TYPE))
                    {
                        var value = message.getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE);
                        if (value == FixConstants.FIX_MESSAGE_LOG_ON.ToString())
                        {
                            if (message.hasTag(FixConstants.FIX_TAG_SEQUENCE_NUMBER))
                            {
                                Connected = result = true;
                                IncomingSequenceNumber = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));
                                m_heartbeatTimer.Interval = HeartbeatInterval * 1000;
                                m_heartbeatTimer.Elapsed += new System.Timers.ElapsedEventHandler(heartbeatTimerFunction);
                                m_heartbeatTimer.Start();
                            }
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                System.Console.WriteLine(e.Message);
            }
            return result;
        }

        private void heartbeatTimerFunction(object sender, System.EventArgs e)
        {
            try
            {
                send(getHeartbeatMessage());
            }
            catch { }
        }

        public void send(FixMessage message)
        {
            message.setTag(FixConstants.FIX_TAG_VERSION, FixVersion);
            message.setTag(FixConstants.FIX_TAG_SEQUENCE_NUMBER, OutgoingSequenceNumber);
            message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, SenderCompid);
            message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, TargetCompid);
            var str = message.toString(true, true);
            byte[] bytes = System.Text.Encoding.ASCII.GetBytes(str);
            //////////////////////////////////////////////////////////////
            m_mutex.WaitOne();
            m_socket.Send(bytes);
            OutgoingSequenceNumber += 1;
            m_mutex.ReleaseMutex();
            //////////////////////////////////////////////////////////////
        }

        private string recvString(int length)
        {
            try
            {
                System.Text.Decoder decoder = System.Text.Encoding.UTF8.GetDecoder();
                byte[] buffer = new byte[length];
                Int32 bytes = m_socket.Receive(buffer);
                char[] chars = new char[bytes];
                int charLen = decoder.GetChars(buffer, 0, bytes, chars, 0);
                System.String received = new System.String(chars);
                return received;
            }
            catch
            {
                return null;
            }
        }

        public FixMessage recv()
        {
            string initialBuffer = recvString(20); // Length of 8=FIX.4.2@9=7000@35= so we always get 35=A

            if (initialBuffer == null)
            {
                return null;
            }

            if (initialBuffer.Length == 0)
            {
                return null;
            }

            int allBytes = System.Convert.ToInt32(initialBuffer.Split((char)1)[1].Split('=')[1]);
            int remainingBytes = allBytes - (20 - initialBuffer.IndexOf("35="));
            remainingBytes += 7; // 7 is because of 10=081@

            string restOfBuffer = recvString(remainingBytes);

            FixMessage message = new FixMessage();

            message.loadFromString(initialBuffer + restOfBuffer);
            
            IncomingSequenceNumber = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));

            return message;
        }

        public void disconnect()
        {
            m_heartbeatTimer.Stop();
            send(getLogoffMessage());
            FixMessage logoffResponse = recv();
            m_socket.Close();
            try
            {
                m_serverSocket.Close();
            }
            catch { }
            Connected = false;
            saveSequenceNumberToFile();
        }

        public void restoreSequenceNumberFromFile()
        {
            var fileName = getSequenceFileName();
            if (System.IO.File.Exists(fileName))
            {
                int outgoingSequenceNumber = 1;
                int incomingSequenceNumber = 1;
                string text = System.IO.File.ReadAllText(fileName);

                if (System.Int32.TryParse(text.Split(',')[0], out outgoingSequenceNumber))
                {
                    OutgoingSequenceNumber = outgoingSequenceNumber;
                }

                if (System.Int32.TryParse(text.Split(',')[1], out incomingSequenceNumber))
                {
                    IncomingSequenceNumber = incomingSequenceNumber;
                }
            }
        }

        public void saveSequenceNumberToFile()
        {
            var fileName = getSequenceFileName();
            if (System.IO.File.Exists(fileName))
            {
                System.IO.File.Delete(fileName);
            }
            System.IO.File.WriteAllText(fileName, OutgoingSequenceNumber.ToString() + "," + IncomingSequenceNumber.ToString());
        }


    }

    public class FixClient
    {
        private int m_orderId = 0;
        private FixSession m_session = new FixSession();
        private System.Collections.Generic.List<FixMessage> m_ordersToSend = new System.Collections.Generic.List<FixMessage>();
        private System.Collections.Generic.List<FixMessage> m_executionReports = new System.Collections.Generic.List<FixMessage>();

        public FixSession FixSession { get { return m_session; } }

        public void initialise(string fixVersion, string address, int port, string compId, string targetCompid, int heartbeatInterval = 30, int encryptionMethod = 0)
        {
            m_session.TargetAddress = address;
            m_session.TargetPort = port;
            m_session.SenderCompid = compId;
            m_session.TargetCompid = targetCompid;
            m_session.HeartbeatInterval = heartbeatInterval;
            m_session.EncryptionMethod = encryptionMethod;
            m_session.FixVersion = fixVersion;
            m_session.restoreSequenceNumberFromFile();
        }

        public void setOrders(System.Collections.Generic.List<FixMessage> orders)
        {
            m_ordersToSend = orders;
        }

        public void addExecutionReport(FixMessage execReport)
        {
            m_executionReports.Add(execReport);
        }

        public System.Collections.Generic.List<FixMessage> getOrders()
        {
            return m_ordersToSend;
        }

        public System.Collections.Generic.List<FixMessage> getExecutionReports()
        {
            return m_executionReports;
        }

        public bool connect()
        {
            return m_session.connect();
        }

        public void disconnect()
        {
            m_session.disconnect();
        }

        public void send(FixMessage message)
        {
            m_orderId++;
            message.setTag(FixConstants.FIX_TAG_CLIENT_ORDER_ID, m_orderId);
            m_session.send(message);
        }

        public FixMessage recv()
        {
            return m_session.recv();
        }
    }

    public class FixClientAutomation
    {
        private System.Collections.Generic.List<FixClient> m_fixClients = new System.Collections.Generic.List<FixClient>();
        private System.Collections.Generic.List<System.Threading.Thread> m_fixClientThreads = new System.Collections.Generic.List<System.Threading.Thread>();

        static void fixThreadClient(object client)
        {
            FixClient fixClient = (FixClient)client;
            bool connected = fixClient.connect();
            if (connected)
            {
                Console.WriteLine(String.Format("{0} connected", fixClient.FixSession.SenderCompid));
                // Fire all orders
                var orders = fixClient.getOrders();
                var ordersCount = orders.Count;
                int processedCount = 0;

                foreach (var order in orders)
                {
                    FixMessage actualOrder = order;
                    actualOrder.setTag(FixConstants.FIX_TAG_SENDER_COMPID, fixClient.FixSession.SenderCompid);
                    actualOrder.setTag(FixConstants.FIX_TAG_TARGET_COMPID, fixClient.FixSession.TargetCompid);
                    fixClient.send(actualOrder);
                }

                Console.WriteLine(String.Format("{0} fired all orders", fixClient.FixSession.SenderCompid));

                // Collect exec reports for all
                while (true)
                {
                    FixMessage message = fixClient.recv();

                    if (message == null)
                    {
                        continue;
                    }

                    if (message.getMessageType() == FixConstants.FIX_MESSAGE_HEARTBEAT)
                    {
                        continue;
                    }

                    fixClient.addExecutionReport(message);

                    if (message.hasTag(FixConstants.FIX_TAG_ORDER_STATUS))
                    {
                        string orderStatus = message.getTagValue(FixConstants.FIX_TAG_ORDER_STATUS);
                        
                        if( orderStatus == FixConstants.FIX_ORDER_STATUS_CANCELED.ToString() )
                        {
                            processedCount++;
                            ordersCount--;
                            Console.WriteLine(String.Format("{0} processed a message : {1} of {2}", fixClient.FixSession.SenderCompid, processedCount, orders.Count));
                        }
                        
                        if ( orderStatus == FixConstants.FIX_ORDER_STATUS_FILLED.ToString() )
                        {
                            processedCount++;
                            Console.WriteLine(String.Format("{0} processed a message : {1} of {2}", fixClient.FixSession.SenderCompid, processedCount, orders.Count));
                        }
                    }

                    if (processedCount == ordersCount)
                    {
                        break;
                    }
                }

                fixClient.disconnect();
                Console.WriteLine(String.Format("{0} disconnected", fixClient.FixSession.SenderCompid));
            }
        }

        public void initialise(int numberOfClients, string fixOrdersFile, string fixVersion, string address, int port, string compIdBase, string targetCompid)
        {
            for (int i = 0; i < numberOfClients; i++)
            {
                var orders = FixMessage.loadFromFile(fixOrdersFile);
                string currentCompId = compIdBase + (i + 1).ToString();
                FixClient currentFixClient = new FixClient();
                currentFixClient.initialise(fixVersion, address, port, currentCompId, targetCompid);
                currentFixClient.setOrders(orders);
                m_fixClients.Add(currentFixClient);
            }
        }

        public void start()
        {
            int clientIndex = 0;
            foreach (var fixClient in m_fixClients)
            {
                System.Threading.Thread fixClientThread = new System.Threading.Thread(new System.Threading.ParameterizedThreadStart(fixThreadClient));
                m_fixClientThreads.Add(fixClientThread);
                fixClientThread.Start(fixClient);
                clientIndex++;
            }
        }

        public void join()
        {
            foreach (var clientThread in m_fixClientThreads)
            {
                clientThread.Join();
            }
        }

        public void report(string fileName)
        {
            System.Text.StringBuilder builder = new System.Text.StringBuilder();

            foreach (var fixClient in m_fixClients)
            {
                builder.Append(fixClient.FixSession.SenderCompid);

                builder.Append(System.Environment.NewLine);
                builder.Append(System.Environment.NewLine);

                var execReports = fixClient.getExecutionReports();

                foreach (var execReport in execReports)
                {
                    builder.Append(execReport.toString(false));
                    builder.Append(System.Environment.NewLine);
                }

                builder.Append(System.Environment.NewLine);
                builder.Append(System.Environment.NewLine);
            }

            if (System.IO.File.Exists(fileName))
            {
                System.IO.File.Delete(fileName);
            }

            System.IO.File.WriteAllText(fileName, builder.ToString());
        }
    }
  
"@

    Add-Type -TypeDefinition $source;
}

function start_test()
{
    $fix_client_automation = New-Object FixClientAutomation
    [int]$numberClients = [Convert]::ToInt32($textBox_numberOfClients.Text)
    [string]$testcaseFile = $textBox_inputFixFile.Text.ToString()
    [string]$fixVersion = $textBox_fixVersion.Text
    [string]$server = $textBox_server_address.Text.ToString()
    [int]$serverPort = [Convert]::ToInt32($textBox_serverPort.Text)
    [string]$baseCompid = $textBox_clientCompPrefix.Text.ToString()
    [string]$serverCompId = $textBox_serverCompId.Text.ToString()
    
    $fix_client_automation.initialise($numberClients, $testcaseFile, $fixVersion, $server, $serverPort, $baseCompid, $serverCompId)

    $sw = [System.Diagnostics.Stopwatch]::startNew()
    $button_start_test.Enabled = $false
    $fix_client_automation.start()
    $fix_client_automation.join()
    $fix_client_automation.report("report.txt")
    
    $sw.Stop()
    Write-Host
    $time_taken = $sw.ElapsedMilliseconds.toString();
    Write-Host "Time : $time_taken milliseconds "
    # Ending
    Write-Host ""
    Write-Host "Client automation finished , you can press Start button to start another test." -foregroundcolor "Yellow"
    Write-Host ""
    $button_start_test.Enabled = $true
    notepad "report.txt"
}

initialise

Write-Host ""
Write-Host "Client automation is starting : " -foregroundcolor "Yellow"
Write-Host ""

Set-Location $PSScriptRoot

Add-Type -AssemblyName System.Windows.Forms

$form_fix_client_automation = New-Object system.Windows.Forms.Form
$form_fix_client_automation.Text = "FIX Client Automation"
$form_fix_client_automation.TopMost = $false
$form_fix_client_automation.FormBorderStyle = [System.Windows.Forms.FormBorderStyle]::FixedSingle
$form_fix_client_automation.MaximizeBox = $false
$form_fix_client_automation.MinimizeBox = $false
$form_fix_client_automation.Width = 640
$form_fix_client_automation.Height = 387

$textBox_fixVersion = New-Object system.windows.Forms.TextBox
$textBox_fixVersion.Text = "FIX.4.2"
$textBox_fixVersion.Width = 292
$textBox_fixVersion.Height = 20
$textBox_fixVersion.location = new-object system.drawing.point(238,127)
$textBox_fixVersion.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_fixVersion)

$button_start_test = New-Object system.windows.Forms.Button
$button_start_test.Text = "Start test"
$button_start_test.Width = 118
$button_start_test.Height = 26
$button_start_test.Add_Click({
start_test
})
$button_start_test.location = new-object system.drawing.point(254,308)
$button_start_test.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($button_start_test)

$textBox_server_address = New-Object system.windows.Forms.TextBox
$textBox_server_address.Text = "127.0.0.1"
$textBox_server_address.Width = 292
$textBox_server_address.Height = 20
$textBox_server_address.location = new-object system.drawing.point(239,27)
$textBox_server_address.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_server_address)

$textBox_serverPort = New-Object system.windows.Forms.TextBox
$textBox_serverPort.Text = "5001"
$textBox_serverPort.Width = 292
$textBox_serverPort.Height = 20
$textBox_serverPort.location = new-object system.drawing.point(238,64)
$textBox_serverPort.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_serverPort)

$label_note_client_compids = New-Object system.windows.Forms.Label
$label_note_client_compids.Text = "Note : If no of clients is 2 , their compids will be : <client_compid_base>1 and <client_compid_base>2"
$label_note_client_compids.AutoSize = $true
$label_note_client_compids.Width = 25
$label_note_client_compids.Height = 10
$label_note_client_compids.location = new-object system.drawing.point(7,268)
$label_note_client_compids.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_note_client_compids)

$textBox_serverCompId = New-Object system.windows.Forms.TextBox
$textBox_serverCompId.Text = "OME"
$textBox_serverCompId.Width = 292
$textBox_serverCompId.Height = 20
$textBox_serverCompId.location = new-object system.drawing.point(240,96)
$textBox_serverCompId.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_serverCompId)

$textBox_inputFixFile = New-Object system.windows.Forms.TextBox
$textBox_inputFixFile.Text = "test_cases_simple.txt"
$textBox_inputFixFile.Width = 292
$textBox_inputFixFile.Height = 20
$textBox_inputFixFile.location = new-object system.drawing.point(237,161)
$textBox_inputFixFile.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_inputFixFile)

$textBox_numberOfClients = New-Object system.windows.Forms.TextBox
$textBox_numberOfClients.Text = "1"
$textBox_numberOfClients.Width = 292
$textBox_numberOfClients.Height = 20
$textBox_numberOfClients.location = new-object system.drawing.point(239,194)
$textBox_numberOfClients.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_numberOfClients)

$textBox_clientCompPrefix = New-Object system.windows.Forms.TextBox
$textBox_clientCompPrefix.Text = "TEST_CLIENT"
$textBox_clientCompPrefix.Width = 292
$textBox_clientCompPrefix.Height = 20
$textBox_clientCompPrefix.location = new-object system.drawing.point(238,230)
$textBox_clientCompPrefix.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_clientCompPrefix)

$textBox_fixVersion = New-Object system.windows.Forms.TextBox
$textBox_fixVersion.Text = "FIX.4.2"
$textBox_fixVersion.Width = 292
$textBox_fixVersion.Height = 20
$textBox_fixVersion.location = new-object system.drawing.point(238,127)
$textBox_fixVersion.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($textBox_fixVersion)

$label_server_port = New-Object system.windows.Forms.Label
$label_server_port.Text = "Server port"
$label_server_port.AutoSize = $true
$label_server_port.Width = 25
$label_server_port.Height = 10
$label_server_port.location = new-object system.drawing.point(5,65)
$label_server_port.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_server_port)

$label_server_compid = New-Object system.windows.Forms.Label
$label_server_compid.Text = "Server compid"
$label_server_compid.AutoSize = $true
$label_server_compid.Width = 25
$label_server_compid.Height = 10
$label_server_compid.location = new-object system.drawing.point(5,95)
$label_server_compid.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_server_compid)

$label_fix_version = New-Object system.windows.Forms.Label
$label_fix_version.Text = "FIX Version"
$label_fix_version.AutoSize = $true
$label_fix_version.Width = 25
$label_fix_version.Height = 10
$label_fix_version.location = new-object system.drawing.point(6,128)
$label_fix_version.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_fix_version)

$label_input_fix_file = New-Object system.windows.Forms.Label
$label_input_fix_file.Text = "Input FIX file"
$label_input_fix_file.AutoSize = $true
$label_input_fix_file.Width = 25
$label_input_fix_file.Height = 10
$label_input_fix_file.location = new-object system.drawing.point(7,162)
$label_input_fix_file.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_input_fix_file)

$label_number_of_clients = New-Object system.windows.Forms.Label
$label_number_of_clients.Text = "Number of concurrent clients"
$label_number_of_clients.AutoSize = $true
$label_number_of_clients.Width = 25
$label_number_of_clients.Height = 10
$label_number_of_clients.location = new-object system.drawing.point(7,194)
$label_number_of_clients.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_number_of_clients)

$label_client_compid_base = New-Object system.windows.Forms.Label
$label_client_compid_base.Text = "Client compid base"
$label_client_compid_base.AutoSize = $true
$label_client_compid_base.Width = 25
$label_client_compid_base.Height = 10
$label_client_compid_base.location = new-object system.drawing.point(6,229)
$label_client_compid_base.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_client_compid_base)

$label_server = New-Object system.windows.Forms.Label
$label_server.Text = "Server"
$label_server.AutoSize = $true
$label_server.Width = 25
$label_server.Height = 10
$label_server.location = new-object system.drawing.point(5,27)
$label_server.Font = "Microsoft Sans Serif,10"
$form_fix_client_automation.controls.Add($label_server)

[void]$form_fix_client_automation.ShowDialog()
$form_fix_client_automation.Dispose()
#########################################################