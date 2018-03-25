function initialise()
{
  $source = @"
  using System;
  
     public class FixConstants
    {
        // GENERAL
        public const char FIX_EQUALS = '=';
        public const char FIX_DELIMITER = ((char)1);
        // VERSIONS
        public const string FIX_VERSION_4_0 = "FIX.4.0";
        public const string FIX_VERSION_4_1 = "FIX.4.1";
        public const string FIX_VERSION_4_2 = "FIX.4.2";
        public const string FIX_VERSION_4_3 = "FIX.4.3";
        public const string FIX_VERSION_4_4 = "FIX.4.4";
        public const string FIX_VERSION_5_0 = "FIX.5.0";
        // TAGS
        public const int FIX_AVERAGE_PRICE = 6;
        public const int FIX_TAG_VERSION = 8;
        public const int FIX_TAG_BODY_LENGTH = 9;
        public const int FIX_TAG_BODY_CHECKSUM = 10;
        public const int FIX_TAG_CLIENT_ORDER_ID = 11;
        public const int FIX_TAG_CUMULATIVE_QUANTITY = 14;
        public const int FIX_TAG_EXEC_ID = 17;
        public const int FIX_TAG_EXEC_INST = 18;
        public const int FIX_TAG_EXEC_TRANSTYPE = 20;
        public const int FIX_TAG_HAND_INST = 21;
        public const int FIX_TAG_LAST_PRICE = 31;
        public const int FIX_TAG_LAST_QUANTITY = 32;
        public const int FIX_TAG_SEQUENCE_NUMBER = 34;
        public const int FIX_TAG_MESSAGE_TYPE = 35;
        public const int FIX_TAG_ORDER_ID = 37;
        public const int FIX_TAG_ORDER_QUANTITY = 38;
        public const int FIX_TAG_ORDER_STATUS = 39;
        public const int FIX_TAG_ORDER_TYPE = 40;
        public const int FIX_TAG_ORIG_CLIENT_ORDER_ID = 41;
        public const int FIX_TAG_ORDER_PRICE = 44;
        public const int FIX_TAG_SECURITY_ID = 48;
        public const int FIX_TAG_SENDER_COMPID = 49;
        public const int FIX_TAG_SENDER_SUBID = 50;
        public const int FIX_TAG_SENDING_TIME = 52;
        public const int FIX_TAG_ORDER_SIDE = 54;
        public const int FIX_TAG_SYMBOL = 55;
        public const int FIX_TAG_TARGET_COMPID = 56;
        public const int FIX_TAG_TARGET_SUBID = 57;
        public const int FIX_TAG_TIME_IN_FORCE = 59;
        public const int FIX_TAG_TRANSACTION_TIME = 60;
        public const int FIX_TAG_ENCRYPT_METHOD = 98;
        public const int FIX_TAG_HEARTBEAT_INTERVAL = 108;
        public const int FIX_TAG_TEST_REQ_ID = 112;
        public const int FIX_TAG_EXEC_TYPE = 150;
        public const int FIX_TAG_LEAVES_QTY = 151;
        public const int FIX_TAG_USERNAME = 553;
        public const int FIX_TAG_PASSWORD = 554;
        public const int FIX_TAG_USER_REQUEST_ID = 923;
        public const int FIX_TAG_USER_PASSWORD = 924;
        // MESSAGE TYPES
        public const string FIX_MESSAGE_HEARTBEAT = "0";
        public const string FIX_MESSAGE_TEST_REQUEST = "1";
        public const string FIX_MESSAGE_LOG_ON = "A";
        public const string FIX_MESSAGE_LOG_OFF = "5";
        public const string FIX_MESSAGE_ADMIN_REJECT = "3";
        public const string FIX_MESSAGE_USER_LOGON = "BE";
        public const string FIX_MESSAGE_USER_RESPONSE = "BF";
        public const string FIX_MESSAGE_EXECUTION_REPORT = "8";
        public const string FIX_MESSAGE_NEW_ORDER = "D";
        public const string FIX_MESSAGE_AMEND_ORDER = "G";
        public const string FIX_MESSAGE_CANCEL_ORDER = "F";
        public const string FIX_MESSAGE_BUSINESS_REJECT = "j";
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
    
    // Not using enum to be consistent with Python interface
    public class FixTime
    {
        public const int FIX_SECONDS = 1;
        public const int FIX_MILLISECONDS = 2;
        public const int FIX_MICROSECONDS = 3;
        
        static public string getCurrentUTCDateTimeSeconds()
        {
            string datetime = "";
            datetime = System.DateTime.Now.ToString("yyyyMMdd-HH:mm:ss");
            return datetime;
        }
        
        static public string getCurrentUTCDateTimeMilliseconds()
        {
            string datetime = "";
            datetime = System.DateTime.Now.ToString("yyyyMMdd-HH:mm:ss.fff");
            return datetime;
        }
        
        static public string getCurrentUTCDateTimeMicroseconds()
        {
            string datetime = "";
            datetime = System.DateTime.Now.ToString("yyyyMMdd-HH:mm:ss.ffffff");
            return datetime;
        }
    }
    
    // Not using .Net FW tuples in order to support most C# / .Net FW versions
    public class FixTagValuePair
    {
        public int Tag {get; set;}
        public string Value {get; set;}
    }
 
    public class FixMessage
    {
        private System.Collections.Generic.List<FixTagValuePair> m_tagValuePairs = new System.Collections.Generic.List<FixTagValuePair>();
        private int m_timePrecision = FixTime.FIX_MILLISECONDS;
        
        System.Text.StringBuilder m_builder = new System.Text.StringBuilder();
 
        private void appendTagValueToBuilder(int tag, string value = "")
        {
            if( value.Length == 0)
            {
                value = getTagValue(tag);
            }
            m_builder.Append(tag.ToString() + FixConstants.FIX_EQUALS + value);
            m_builder.Append(FixConstants.FIX_DELIMITER);
        }
        
        public void setTimePrecision(int precision)
        {
            m_timePrecision = precision;
        }
        
        public string getCurrentUTCDateTime()
        {
            if( m_timePrecision == FixTime.FIX_MICROSECONDS)
            {
                return FixTime.getCurrentUTCDateTimeMicroseconds();
            }
            
            if( m_timePrecision == FixTime.FIX_MILLISECONDS)
            {
                return FixTime.getCurrentUTCDateTimeMilliseconds();
            }
            
            return FixTime.getCurrentUTCDateTimeSeconds();
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
 
        public string getMessageType()
        {
            return getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE);
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
        
        static public bool isBodyTag(int tag)
        {
            if( tag == FixConstants.FIX_TAG_VERSION )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_BODY_LENGTH )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_MESSAGE_TYPE )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_SEQUENCE_NUMBER )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_SENDING_TIME )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_SENDER_COMPID )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_SENDER_SUBID )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_TARGET_COMPID )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_TARGET_SUBID )
            {
                return false;
            }
            if( tag == FixConstants.FIX_TAG_BODY_CHECKSUM )
            {
                return false;
            }
            return true;
        }
 
        public int calculateBodyLength()
        {
            int bodyLength = 0;
 
            foreach (var tagValue in m_tagValuePairs)
            {
                //  We exclude header and checksum
                if (tagValue.Tag == FixConstants.FIX_TAG_VERSION)
                {
                    continue;
                }
 
                if (tagValue.Tag == FixConstants.FIX_TAG_BODY_LENGTH)
                {
                    continue;
                }
 
                if (tagValue.Tag == FixConstants.FIX_TAG_BODY_CHECKSUM)
                {
                    continue;
                }
 
                bodyLength += tagValue.Tag.ToString().Length + 2 + tagValue.Value.Length; // +2 is because of = and delimiter
            }
 
            return bodyLength;
        }
 
        public void setFixVersion(string fixVersion)
        {
            setTag(FixConstants.FIX_TAG_VERSION, fixVersion);
        }
 
        public bool hasTag(int tag)
        {
            foreach (var tagValue in m_tagValuePairs)
            {
                if(tagValue.Tag == tag)
                {
                    return true;
                }
            }
            return false;
        }
 
        public void setTag(int tag, string value)
        {
            var pair = new FixTagValuePair();
            pair.Tag = tag;
            pair.Value = value;
            m_tagValuePairs.Add(pair);
        }
 
        public void setTag(int tag, char value)
        {
            setTag(tag, value.ToString());
        }
 
        public void setTag(int tag, int value)
        {
            setTag(tag, value.ToString());
        }
        
        public void setTag(int tag, double value)
        {
            setTag(tag, value.ToString());
        }
 
        public string getTagValue(int tag, int index=1)
        {
            var count = 0;
            foreach (var tagValue in m_tagValuePairs)
            {
                if(tag == tagValue.Tag)
                {
                    count++;
                    if(index == count)
                    {
                        return tagValue.Value;
                    }
                }
            }
            return null;
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
 
        public string toString(bool sendingAsAMessage = false, bool updateTransactionTime = false)
        {
            m_builder = new System.Text.StringBuilder();
            // FIX VERSION
            appendTagValueToBuilder(FixConstants.FIX_TAG_VERSION);
 
            // FIX SENDING TIME AND TRANSACTION, have to be before body length calculation ,but not appended for the correct order
            if (sendingAsAMessage)
            {
                var currentUTCDateTime = getCurrentUTCDateTime();
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
            else
            {
                if( hasTag(FixConstants.FIX_TAG_BODY_LENGTH))
                {
                    appendTagValueToBuilder(FixConstants.FIX_TAG_BODY_LENGTH);
                }
            }
 
            // FIX MESSAGE TYPE
            appendTagValueToBuilder(FixConstants.FIX_TAG_MESSAGE_TYPE);
 
            // FIX SEQUENCE NUMBER
            appendTagValueToBuilder(FixConstants.FIX_TAG_SEQUENCE_NUMBER);
 
            // FIX SENDER COMPID
            appendTagValueToBuilder(FixConstants.FIX_TAG_SENDER_COMPID);
            
            // FIX SENDER SUBID
            if( hasTag(FixConstants.FIX_TAG_SENDER_SUBID) )
            {
                appendTagValueToBuilder(FixConstants.FIX_TAG_SENDER_SUBID);
            }           
 
            // FIX SENDING TIME
            appendTagValueToBuilder(FixConstants.FIX_TAG_SENDING_TIME);
 
            // FIX TARGET COMPID
            appendTagValueToBuilder(FixConstants.FIX_TAG_TARGET_COMPID);
            
            // FIX TARGET SUBID
            if( hasTag(FixConstants.FIX_TAG_TARGET_SUBID) )
            {
                appendTagValueToBuilder(FixConstants.FIX_TAG_TARGET_SUBID);
            }       
 
            foreach (var tagValue in m_tagValuePairs)
            {
                if( isBodyTag( tagValue.Tag ) == false )
                {
                    continue;
                }
                appendTagValueToBuilder(tagValue.Tag);
            }
 
            // FIX CHECKSUM
            if (sendingAsAMessage)
            {
                var checksum = calculateChecksum(m_builder.ToString());
                appendTagValueToBuilder(FixConstants.FIX_TAG_BODY_CHECKSUM, checksum);           
            }
            else
            {
                if( hasTag(FixConstants.FIX_TAG_BODY_CHECKSUM))
                {
                    appendTagValueToBuilder(FixConstants.FIX_TAG_BODY_CHECKSUM);
                }
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
        public int TimePrecision {get; set;}
        public int NetworkTimeoutInSeconds{get; set;}
        public bool RestoreSequenceNumbersFromFile {get; set;}
        public string TargetAddress { get; set; }
        public int TargetPort { get; set; }
        public string TargetCompid { get; set; }
        public string TargetSubid { get; set; }
        public string SenderCompid { get; set; }
        public string SenderSubid { get; set; }
        public int HeartbeatInterval { get; set; }
        public int EncryptionMethod { get; set; }
        public int IncomingSequenceNumber { get; set; }
        public int OutgoingSequenceNumber { get; set; }
        public string FixVersion { get; set; }
 
        public FixSession()
        {
            Connected = false;
            TimePrecision = FixTime.FIX_MILLISECONDS;
            HeartbeatInterval = 30;
            EncryptionMethod = FixConstants.FIX_ENCRYPTION_NONE;
            NetworkTimeoutInSeconds = 0;
            IncomingSequenceNumber = 1;
            OutgoingSequenceNumber = 1;
            m_socket.NoDelay = true;
            enableSocketBlocking();
        }
 
        public FixMessage getBaseMessage(string messageType)
        {
            FixMessage message = new FixMessage();
            message.setFixVersion(FixVersion);
            message.setTimePrecision(TimePrecision);
            message.setTag(FixConstants.FIX_TAG_MESSAGE_TYPE, messageType);
            // Sequence number will be added during sending
            message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, SenderCompid);
            
            if( SenderSubid != null )
            {
                if(SenderSubid.Length > 0)
                {
                    message.setTag(FixConstants.FIX_TAG_SENDER_SUBID, SenderSubid);
                }
            }
            message.setTag(FixConstants.FIX_TAG_SENDING_TIME, "");
            message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, TargetCompid);
            
            if( TargetSubid != null )
            {
                if(TargetSubid.Length > 0)
                {
                    message.setTag(FixConstants.FIX_TAG_TARGET_SUBID, TargetSubid);
                }
            }
            return message;
        }
 
        public FixMessage getLogonMessage()
        {
            FixMessage message = getBaseMessage(FixConstants.FIX_MESSAGE_LOG_ON);
            message.setTag(FixConstants.FIX_TAG_ENCRYPT_METHOD, EncryptionMethod);
            message.setTag(FixConstants.FIX_TAG_HEARTBEAT_INTERVAL, HeartbeatInterval);
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
 
        public bool connect(FixMessage logonMessage)
        {
            FixMessage message = null;
            // Not supporting timeouts in accept and connect methods
            var originalNetworkTimeoutInSeconds = NetworkTimeoutInSeconds;
            NetworkTimeoutInSeconds = 0;
            
            while(true)
            {   
                try
                {
                    m_socket.Connect(TargetAddress, TargetPort);

                    if( RestoreSequenceNumbersFromFile )
                    {
                        restoreSequenceNumberFromFile();
                    }
     
                    if(logonMessage == null)
                    {
                        logonMessage = getLogonMessage();
                    }

                    send(logonMessage);

                    message = recv();

                    if( message.getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE) != FixConstants.FIX_MESSAGE_LOG_ON )
                    {
                        throw new Exception("Incoming message was not a logon response");
                    }

                    if( RestoreSequenceNumbersFromFile == false)
                    {
                        IncomingSequenceNumber = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));
                    }

                    m_heartbeatTimer.Interval = HeartbeatInterval * 1000;
                    m_heartbeatTimer.Elapsed += new System.Timers.ElapsedEventHandler(heartbeatTimerFunction);
                    m_heartbeatTimer.Start();
                    Connected = true;

                    break;
                }
                catch(System.Net.Sockets.SocketException e)
                {
                    if(e.ErrorCode == 10061) // WSAECONNREFUSED
                    {
                        continue;
                    }
                }
                catch (System.Exception e)
                {
                    string exceptionMessage = "Error during a connection attempt : " ;
                    exceptionMessage += e.Message;
                    
                    if( message != null )
                    {
                        exceptionMessage += System.Environment.NewLine;
                        exceptionMessage += message.toString();
                    }

                    System.Console.WriteLine(exceptionMessage);
                    break;
                }
            }
            
            NetworkTimeoutInSeconds = originalNetworkTimeoutInSeconds;
            return Connected;
        }
 
        public bool accept(FixMessage logOnResponse)
        {
            FixMessage message = null;
            // Not supporting timeouts in accept and connect methods
            var originalNetworkTimeoutInSeconds = NetworkTimeoutInSeconds;
            NetworkTimeoutInSeconds = 0;
            try
            {
                System.Net.IPHostEntry ipHostInfo = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName());
                System.Net.IPAddress ipAddress = ipHostInfo.AddressList[0];
                System.Net.IPEndPoint localEndPoint = new System.Net.IPEndPoint(Convert.ToInt32(TargetAddress), TargetPort);
 
                m_serverSocket.Bind(localEndPoint);
                m_serverSocket.Listen(100);
                m_socket = m_serverSocket.Accept();
 
                message = recv();
                    
                if( message.getTagValue(FixConstants.FIX_TAG_MESSAGE_TYPE) != FixConstants.FIX_MESSAGE_LOG_ON )
                {
                    throw new Exception("Incoming message was not a logon message");
                }
                
                if( RestoreSequenceNumbersFromFile == false )
                {
                    IncomingSequenceNumber = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));
                }
                else
                {
                    restoreSequenceNumberFromFile();
                }
                
                HeartbeatInterval = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_HEARTBEAT_INTERVAL));
                TargetCompid = message.getTagValue(FixConstants.FIX_TAG_SENDER_COMPID);
                
                if( message.hasTag(FixConstants.FIX_TAG_SENDER_SUBID) )
                {
                    SenderSubid = message.getTagValue(FixConstants.FIX_TAG_SENDER_SUBID);
                }

                FixVersion = message.getTagValue(FixConstants.FIX_TAG_VERSION);
                if (message.hasTag(FixConstants.FIX_TAG_ENCRYPT_METHOD))
                {
                    EncryptionMethod = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_ENCRYPT_METHOD));
                    
                }
                if(logOnResponse == null)
                {
                    logOnResponse = getLogonMessage();
                }
                send(logOnResponse);
                Connected = true;
            }
            catch (System.Exception e)
            {
                string exceptionMessage = "Error during a connection attempt : " ;
                exceptionMessage += e.Message;
                
                if( message != null )
                {
                    exceptionMessage += System.Environment.NewLine;
                    exceptionMessage += message.toString();
                }

                System.Console.WriteLine(exceptionMessage);
            }
            NetworkTimeoutInSeconds = originalNetworkTimeoutInSeconds;
            return Connected;
        }
 
        private void heartbeatTimerFunction(object sender, System.EventArgs e)
        {
            try
            {
                send(getHeartbeatMessage());
            }
            catch { }
        }
 
        public bool send(FixMessage message)
        {
            message.setTag(FixConstants.FIX_TAG_VERSION, FixVersion);
            message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, SenderCompid);
            message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, TargetCompid);
            int sentBytes = 0;
            
            m_mutex.WaitOne();
            
            if(NetworkTimeoutInSeconds > 0)
            {
                m_socket.Blocking = false;
                m_socket.SendTimeout = NetworkTimeoutInSeconds * 1000;
            }
            
            message.setTag(FixConstants.FIX_TAG_SEQUENCE_NUMBER, OutgoingSequenceNumber);
            var str = message.toString(true, true);
            byte[] bytes = System.Text.Encoding.ASCII.GetBytes(str);
            
            try
            {
                sentBytes = m_socket.Send(bytes);
                if(sentBytes > 0)
                {
                    OutgoingSequenceNumber += 1;
                }
            }
            catch
            {
                sentBytes = 0;
            }
            
            enableSocketBlocking();
            m_mutex.ReleaseMutex();
            return sentBytes > 0;
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
        
        private void enableSocketBlocking()
        {
            m_socket.Blocking = true;
            m_socket.ReceiveTimeout = 0;
            m_socket.SendTimeout = 0;
        }
 
        public FixMessage recv()
        {
            FixMessage message = null;
            m_mutex.WaitOne();
            
            if(NetworkTimeoutInSeconds > 0)
            {
                m_socket.Blocking = false;
                m_socket.ReceiveTimeout = NetworkTimeoutInSeconds * 1000;
            }
            
            try
            {
                string initialBuffer = recvString(20); // Length of 8=FIX.4.2@9=7000@35= so we always get 35=A
     
                if (initialBuffer == null)
                {
                    throw new Exception("Receive failed");
                }
     
                if (initialBuffer.Length == 0)
                {
                    throw new Exception("Receive failed");
                }
     
                int allBytes = System.Convert.ToInt32(initialBuffer.Split((char)1)[1].Split('=')[1]);
                int remainingBytes = allBytes - (20 - initialBuffer.IndexOf("35="));
                remainingBytes += 7; // 7 is because of 10=081@
     
                string restOfBuffer = recvString(remainingBytes);
                
                if( restOfBuffer == null)
                {
                    throw new Exception("Receive failed");
                }
                
                if( remainingBytes > 0 && restOfBuffer.Length == 0)
                {
                    throw new Exception("Receive failed");
                }
     
                message = new FixMessage();
     
                message.loadFromString(initialBuffer + restOfBuffer);
               
                IncomingSequenceNumber = System.Convert.ToInt32(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER));
            }
            catch
            {
            }
            enableSocketBlocking();
            m_mutex.ReleaseMutex();
            return message;
        }
 
        public void disconnect(FixMessage logoffMessage)
        {
            if(Connected)
            {
                m_heartbeatTimer.Stop();
                try
                {   
                    if(logoffMessage == null)
                    {
                        logoffMessage = getLogoffMessage();
                    }
                    send(logoffMessage);
                    FixMessage logoffResponse = recv();
                    m_socket.Close();
                    m_serverSocket.Close();
                }
                catch { }
                saveSequenceNumberToFile();
                Connected = false;
            }
        }
 
        public void restoreSequenceNumberFromFile()
        {
            var fileName = getSequenceFileName();
            if (System.IO.File.Exists(fileName))
            {
                int outgoingSequenceNumber = 1;
                int incomingSequenceNumber = 1;
                try
                {
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
                catch
                {
                    Console.WriteLine("Warning : Error during opening sequence number file , sequence numbers set to 1");
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
        
 
        public FixSession FixSession { get { return m_session; } }
 
        public void initialise(string fixVersion, string address, int port, string compId, string subId, string targetCompid, string targetSubid, int heartbeatInterval = 30, int encryptionMethod = 0)
        {
            m_session.TargetAddress = address;
            m_session.TargetPort = port;
            m_session.SenderCompid = compId;
            m_session.SenderSubid = subId;
            m_session.TargetCompid = targetCompid;
            m_session.TargetSubid = targetSubid;
            m_session.HeartbeatInterval = heartbeatInterval;
            m_session.EncryptionMethod = encryptionMethod;
            m_session.FixVersion = fixVersion;
        }
 
        public bool connect(FixMessage logonMessage = null)
        {
            return m_session.connect(logonMessage);
        }
 
        public void disconnect(FixMessage logoffMessage = null)
        {
            m_session.disconnect(logoffMessage);
        }
 
        public bool send(FixMessage message, bool sendClientOrderId=true, int timeoutMilliseconds = 0)
        {
			if(sendClientOrderId)
			{
				m_orderId++;
				message.setTag(FixConstants.FIX_TAG_CLIENT_ORDER_ID, m_orderId);
			}
			
			bool ret = false;
			
			System.Diagnostics.Stopwatch stopWatch = new System.Diagnostics.Stopwatch();
			stopWatch.Start();
			while(true)
			{
				ret = m_session.send(message);
				
				if(ret)
				{
					break;
				}
				
				// Timeout check
				if(timeoutMilliseconds > 0)
				{
					if( stopWatch.ElapsedMilliseconds >= timeoutMilliseconds)
					{
						break;
					}
				}
				else
				{
					break;
				}
				
			}
			stopWatch.Stop();
			return ret;
        }
 
        public FixMessage recv()
        {
            return m_session.recv();
        }
    }
    
    public class FixAutomationClient : FixClient
    {
        private System.Collections.Generic.List<FixMessage> m_ordersToSend = new System.Collections.Generic.List<FixMessage>();
        private System.Collections.Generic.List<FixMessage> m_executionReports = new System.Collections.Generic.List<FixMessage>();
        
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
    }

    public class FixClientAutomation
    {
        private System.Collections.Generic.List<FixAutomationClient> m_fixClients = new System.Collections.Generic.List<FixAutomationClient>();
        private System.Collections.Generic.List<System.Threading.Thread> m_fixClientThreads = new System.Collections.Generic.List<System.Threading.Thread>();

        static void fixThreadClient(object client)
        {
            FixAutomationClient fixClient = (FixAutomationClient)client;
			fixClient.FixSession.RestoreSequenceNumbersFromFile = true;
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
                    fixClient.send(actualOrder, true, 600000);
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
			Console.WriteLine("0");
            for (int i = 0; i < numberOfClients; i++)
            {
				Console.WriteLine(fixOrdersFile);
                var orders = FixMessage.loadFromFile(fixOrdersFile);
				Console.WriteLine("1.2");
                string currentCompId = compIdBase + (i + 1).ToString();
				Console.WriteLine("1.3");
                FixAutomationClient currentFixClient = new FixAutomationClient();
				Console.WriteLine("2");
                currentFixClient.initialise(fixVersion, address, port, currentCompId, "", targetCompid, "");
				Console.WriteLine("3");
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