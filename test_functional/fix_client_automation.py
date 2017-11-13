#!/usr/bin/python
import sys
import socket
import os
import os.path
from sys import platform as _platform
#As Cpython ( default python engine) uses GIL ( https://wiki.python.org/moin/GlobalInterpreterLock )
#using process instead to benefit from multicore : http://stackoverflow.com/questions/1182315/python-multicore-processing
from multiprocessing import Process, Queue
import threading
import time
from datetime import datetime

class Utility:
    CONSOLE_RED = '\033[91m'
    CONSOLE_BLUE = '\033[94m'
    CONSOLE_YELLOW = '\033[93m'
    CONSOLE_END = '\033[0m'

    @staticmethod
    def getCurrentTimeInMilliseconds():
        return int(round(time.time() * 1000))

    @staticmethod
    def pressAnyKey():
        if _platform == "linux" or _platform == "linux2":
            os.system('read -s -n 1 -p "Press any key to continue..."')
        elif _platform == "win32":
            os.system('pause')

    @staticmethod
    def changeWorkingDirectoryToScriptLocation():
        absolutePath = os.path.abspath(__file__)
        dirName = os.path.dirname(absolutePath)
        os.chdir(dirName)

    @staticmethod
    def askQuestion(questionText, defaultAnswer = ""):
        actualQuestionText = questionText
        if len(defaultAnswer) == 0:
            actualQuestionText += " ( Press enter for " + defaultAnswer + " ) "
        actualQuestionText += " : "
        answer = raw_input(actualQuestionText)
        if not answer:
            answer = defaultAnswer
        return answer

    @staticmethod
    def yesNoQuestion(questionText):
        actualQuestionText = questionText + " ( Y/y or N/n ) : "
        retVal = False
        while True:
            answer = raw_input(actualQuestionText)
            answer = answer.lower()
            if not answer:
                continue

            if answer == "y" or answer == "yes":
                retVal = True
                break

            if answer == "n" or answer == "no":
                break

        return  retVal


    @staticmethod
    def writeColorMessage(message, colorCode):
        if _platform == "linux" or _platform == "linux2":
            print(colorCode + message + Utility.CONSOLE_END)
        elif _platform == "win32":
            os.system("echo " + message)

    @staticmethod
    def writeMessage(message):
        Utility.writeColorMessage(message, Utility.CONSOLE_BLUE)

    @staticmethod
    def writeErrorMessage(message):
        Utility.writeColorMessage(message, Utility.CONSOLE_RED)

    @staticmethod
    def writeInfoMessage(message):
        Utility.writeColorMessage(message, Utility.CONSOLE_YELLOW)
        
class StopWatch:
    def __init__(self):
        self.startTime = 0
        self.endTime = 0

    def start(self):
        self.startTime = Utility.getCurrentTimeInMilliseconds()

    def stop(self):
        self.endTime = Utility.getCurrentTimeInMilliseconds()

    def elapsedTimeInMilliseconds(self):
        return self.endTime - self.startTime

class FixConstants:
    # GENERAL
    FIX_EQUALS = '='
    FIX_DELIMITER = chr(1)
    # TAGS
    FIX_TAG_VERSION = 8
    FIX_TAG_BODY_LENGTH = 9
    FIX_TAG_BODY_CHECKSUM = 10
    FIX_TAG_CLIENT_ORDER_ID = 11
    FIX_TAG_EXEC_INST = 18
    FIX_TAG_HAND_INST = 21
    FIX_TAG_SEQUENCE_NUMBER = 34
    FIX_TAG_MESSAGE_TYPE = 35
    FIX_TAG_ORDER_QUANTITY = 38
    FIX_TAG_ORDER_STATUS = 39
    FIX_TAG_ORDER_TYPE = 40
    FIX_TAG_ORDER_PRICE = 44
    FIX_TAG_SECURITY_ID = 48
    FIX_TAG_SENDER_COMPID = 49
    FIX_TAG_SENDING_TIME = 52
    FIX_TAG_ORDER_SIDE = 54
    FIX_TAG_SYMBOL = 55
    FIX_TAG_TARGET_COMPID = 56
    FIX_TAG_TIME_IN_FORCE = 59
    FIX_TAG_TRANSACTION_TIME = 60
    FIX_TAG_ENCRYPT_METHOD = 98
    FIX_TAG_HEARBEAT_INTERVAL = 108
    FIX_TAG_EXEC_TYPE = 150
    FIX_TAG_LEAVES_QTY = 151
    # MESSAGE TYPES
    FIX_MESSAGE_HEARTBEAT = '0'
    FIX_MESSAGE_LOG_ON = 'A'
    FIX_MESSAGE_LOG_OFF = '5'
    FIX_MESSAGE_EXECUTION_REPORT = '8'
    FIX_MESSAGE_NEW_ORDER = 'D'
    FIX_MESSAGE_AMEND_ORDER = 'G'
    FIX_MESSAGE_CANCEL_ORDER = 'F'
    # ORDER STATUS
    FIX_ORDER_STATUS_NEW = '0'
    FIX_ORDER_STATUS_PARTIALLY_FILLED = '1'
    FIX_ORDER_STATUS_FILLED = '2'
    FIX_ORDER_STATUS_DONE_FOR_DAY = '3'
    FIX_ORDER_STATUS_CANCELED = '4'
    FIX_ORDER_STATUS_REPLACED = '5'
    FIX_ORDER_STATUS_PENDING_CANCEL = '6'
    FIX_ORDER_STATUS_STOPPED = '7'
    FIX_ORDER_STATUS_REJECTED = '8'
    # ORDER TYPE
    FIX_ORDER_TYPE_MARKET= '1'
    FIX_ORDER_TYPE_LIMIT = '2'
    # SIDE
    FIX_ORDER_SIDE_BUY = '1'
    FIX_ORDER_SIDE_SELL = '2'
    # TIME IN FORCE
    FIX_ORDER_TIF_DAY = '0'
    # ENCRYPTION METHODS
    FIX_ENCRYPTION_NONE = '0'

class FixMessage:
    def __init__(self):
        self.tagValuePairs = dict()
        self.messageAsString = ""

    def appendToMessageAsString(self, tag, value, appendDelimiter=True):
        self.messageAsString += str(tag) + FixConstants.FIX_EQUALS + value
        if appendDelimiter is True:
            self.messageAsString += FixConstants.FIX_DELIMITER

    @staticmethod
    def getCurrentUTCDateTime():
        retVal = datetime.utcnow().strftime("%Y%m%d-%H:%M:%S.%f")[:-3]
        return retVal

    def setFixVersion(self, fixVersion):
        self.setTag(FixConstants.FIX_TAG_VERSION, fixVersion)

    def hasTag(self, tag):
        if tag in self.tagValuePairs:
            return True
        return False

    def getTagValue(self, tag):
        return self.tagValuePairs[tag]

    def setTag(self, tag, value):
        self.tagValuePairs[tag] = str(value)

    def getMessageType(self):
        return self.tagValuePairs[FixConstants.FIX_TAG_MESSAGE_TYPE]

    def isAdminMessage(self):
        messageType = self.getMessageType()
        if messageType is FixConstants.FIX_MESSAGE_LOG_ON:
            return True
        if messageType is FixConstants.FIX_MESSAGE_LOG_OFF:
            return True
        if messageType is FixConstants.FIX_MESSAGE_HEARTBEAT:
            return True
        return False

    def loadFromString(self, input=""):
        tagValuePairs = input.split(FixConstants.FIX_DELIMITER)
        for tagValuePair in tagValuePairs:
            if len(tagValuePair) < 3:
                continue
            tokens = tagValuePair.split(FixConstants.FIX_EQUALS)
            tag = tokens[0]
            value = tokens[1]
            self.setTag(int(tag), value)

    @staticmethod
    def loadFromFile(fileName):
        fixMessages = []
        with open(fileName, "r") as fileContent:
            for line in fileContent:
                fixMessage = FixMessage()
                fixMessage.loadFromString(line)
                fixMessages.append(fixMessage)
        return fixMessages

    @staticmethod
    def calculateChecksum(message):
        checksum = ""
        sum = 0
        for c in message:
            sum += ord(c)
        sum = sum % 256
        checksum = str(sum)
        checksum.zfill(3)
        return checksum

    def calculateBodyLength(self):
        bodyLength = 0
        for tag in self.tagValuePairs:
            if tag is FixConstants.FIX_TAG_VERSION:
                continue
            if tag is FixConstants.FIX_TAG_BODY_LENGTH:
                continue
            if tag is FixConstants.FIX_TAG_BODY_CHECKSUM:
                continue
            bodyLength += len(str(tag)) + len( self.tagValuePairs[tag] ) + 2 # +2 is because of = and delimiter
        return bodyLength

    def toString(self, sendingAsMessage, updateTransactionTime = False):
        # FIX VERSION
        self.appendToMessageAsString(FixConstants.FIX_TAG_VERSION, self.getTagValue(FixConstants.FIX_TAG_VERSION))

        # FIX SENDING TIME AND TRANSACTION, have to be before body length calculation ,but not appended for the correct order
        if sendingAsMessage is True:
            currentUTCDateTime = FixMessage.getCurrentUTCDateTime()
            self.setTag(FixConstants.FIX_TAG_SENDING_TIME, currentUTCDateTime)

            if updateTransactionTime is True:
                if self.isAdminMessage() is False:
                    self.setTag(FixConstants.FIX_TAG_TRANSACTION_TIME, currentUTCDateTime)

        # FIX BODY LENGTH
        if sendingAsMessage is True:
            bodyLength = str(self.calculateBodyLength())
            self.appendToMessageAsString(FixConstants.FIX_TAG_BODY_LENGTH, bodyLength)

        # FIX MESSAGE TYPE
        self.appendToMessageAsString(FixConstants.FIX_TAG_MESSAGE_TYPE, self.getMessageType())

        # FIX SEQUENCE NUMBER
        self.appendToMessageAsString(FixConstants.FIX_TAG_SEQUENCE_NUMBER, self.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER))

        # FIX SENDER COMPID
        self.appendToMessageAsString(FixConstants.FIX_TAG_SENDER_COMPID, self.getTagValue(FixConstants.FIX_TAG_SENDER_COMPID))

        # FIX SENDING TIME
        self.appendToMessageAsString(FixConstants.FIX_TAG_SENDING_TIME, self.getTagValue(FixConstants.FIX_TAG_SENDING_TIME))

        # FIX TARGET COMPID
        self.appendToMessageAsString(FixConstants.FIX_TAG_TARGET_COMPID, self.getTagValue(FixConstants.FIX_TAG_TARGET_COMPID))

        for tag in self.tagValuePairs:
            if tag is FixConstants.FIX_TAG_VERSION:
                continue
            if tag is FixConstants.FIX_TAG_BODY_LENGTH:
                continue
            if tag is FixConstants.FIX_TAG_MESSAGE_TYPE:
                continue
            if tag is FixConstants.FIX_TAG_SEQUENCE_NUMBER:
                continue
            if tag is FixConstants.FIX_TAG_SENDING_TIME:
                continue
            if tag is FixConstants.FIX_TAG_SENDER_COMPID:
                continue
            if tag is FixConstants.FIX_TAG_TARGET_COMPID:
                continue
            if tag is FixConstants.FIX_TAG_BODY_CHECKSUM:
                continue
            self.appendToMessageAsString(tag, self.getTagValue(tag))

        # FIX CHECKSUM
        if sendingAsMessage is True:
            checksumValue = FixMessage.calculateChecksum(self.messageAsString)
            self.appendToMessageAsString(FixConstants.FIX_TAG_BODY_CHECKSUM, checksumValue)
        
        return self.messageAsString

class FixSession:
    def __init__(self):
        self.connected = False
        self.targetAddress = ""
        self.targetPort = 0
        self.heartbeatInterval = 30
        self.encryptionMethod = FixConstants.FIX_ENCRYPTION_NONE
        self.incomingSequenceNumber = 1
        self.outgoingSequenceNumber = 1
        self.fixVersion = ""
        self.senderCompid = ""
        self.targetCompid = ""
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setblocking(True)
        self.socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self.heartbeatTimer = None

    def getBaseMessage(self, messageType):
        message = FixMessage()
        message.setFixVersion(self.fixVersion)
        message.setTag(FixConstants.FIX_TAG_MESSAGE_TYPE, messageType)
        # Sequence number will be added during send call
        message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, self.senderCompid)
        message.setTag(FixConstants.FIX_TAG_SENDING_TIME, "")
        message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, self.targetCompid)
        return  message

    def getLogonMessage(self):
        message = self.getBaseMessage(FixConstants.FIX_MESSAGE_LOG_ON)
        message.setTag(FixConstants.FIX_TAG_ENCRYPT_METHOD, self.encryptionMethod)
        message.setTag(FixConstants.FIX_TAG_HEARBEAT_INTERVAL, self.heartbeatInterval)
        return message

    def getLogoffMessage(self):
        message = self.getBaseMessage(FixConstants.FIX_MESSAGE_LOG_OFF)
        message.setTag(FixConstants.FIX_TAG_ENCRYPT_METHOD, self.encryptionMethod)
        message.setTag(FixConstants.FIX_TAG_HEARBEAT_INTERVAL, self.heartbeatInterval)
        return message

    def getHeartbeatMessage(self):
        message = self.getBaseMessage(FixConstants.FIX_MESSAGE_HEARTBEAT)
        return message

    def getSequenceFileName(self):
        ret = self.senderCompid + "_" + self.targetCompid + "_sequence.txt"
        return ret

    def saveSequenceNumberToFile(self):
        fileName = self.getSequenceFileName()
        if os.path.exists(fileName):
            os.remove(fileName)
        with open(fileName, "w") as textFile:
            textFile.write(str(self.outgoingSequenceNumber) + "," + str(self.incomingSequenceNumber))

    def restoreSequenceNumber(self):
        incomingNumber = 1
        outgoingNumber = 1
        fileName = self.getSequenceFileName()

        if os.path.exists(fileName):
            with open(fileName, "r") as fileContent:
                line = fileContent.readline()
                outgoingNumberString = line.split(',')[0]
                incomingNumberString = line.split(',')[1]

                if incomingNumberString.isdigit() is True:
                    incomingNumber = int(incomingNumberString)

                if outgoingNumberString.isdigit() is True:
                    outgoingNumber = int(outgoingNumberString)

        self.incomingSequenceNumber = incomingNumber
        self.outgoingSequenceNumber = outgoingNumber

    def connect(self):
        try:
            self.socket.connect((self.targetAddress, self.targetPort))
            self.send(self.getLogonMessage())
            message = self.recv()

            if message is not None:
                if message.hasTag(FixConstants.FIX_TAG_MESSAGE_TYPE):
                    messageType = message.getMessageType()
                    if messageType is FixConstants.FIX_MESSAGE_LOG_ON:
                        if message.hasTag(FixConstants.FIX_TAG_SEQUENCE_NUMBER):
                            self.connected = True
                            self.incomingSequenceNumber = int( message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER) )
                            self.heartbeatTimer = threading.Timer(self.heartbeatInterval, self.heartbeatFunction)
                            self.heartbeatTimer.start()
        except Exception as e:
            print("Socket connection failed %s:%d. Exception is %s" % (self.targetAddress, self.targetPort, e))

        return  self.connected

    def heartbeatFunction(self):
        self.send(self.getHeartbeatMessage())

    def disconnect(self):
        self.send(self.getLogoffMessage())
        logoffResponse = self.recv()
        print("Disconnected : " + logoffResponse.toString(False))
        self.heartbeatTimer.cancel()
        self.socket.close()
        self.connected = False
        self.saveSequenceNumberToFile()

    def send(self, message):
        message.setTag(FixConstants.FIX_TAG_VERSION, self.fixVersion)
        message.setTag(FixConstants.FIX_TAG_SEQUENCE_NUMBER, self.outgoingSequenceNumber)
        message.setTag(FixConstants.FIX_TAG_SENDER_COMPID, self.senderCompid)
        message.setTag(FixConstants.FIX_TAG_TARGET_COMPID, self.targetCompid)
        self.socket.send( message.toString(True, True))
        self.outgoingSequenceNumber += 1
        return

    def recvString(self, size):
        data = ""
        try:
            data = self.socket.recv(size)
        except socket.error:
            data = None
            print "Socket error"
        return data

    def recv(self):
        initialBuffer = self.recvString(20) # Length of 8=FIX.4.2@9=7000@35= so we always get 35=A

        if initialBuffer is None:
            return None

        if len(initialBuffer) is 0:
            return None

        # Find all bytes from tag body length
        allBytes = int ( initialBuffer.split(FixConstants.FIX_DELIMITER)[1].split(FixConstants.FIX_EQUALS)[1] )
        # Calculate remaining bytes
        remainingBytes = allBytes - 20 + initialBuffer.find("35=")
        remainingBytes += 7 #7 is because of 10=081@
        restOfBuffer = self.recvString(remainingBytes)

        message = FixMessage()
        message.loadFromString( initialBuffer + restOfBuffer )

        self.incomingSequenceNumber = int(message.getTagValue(FixConstants.FIX_TAG_SEQUENCE_NUMBER))

        return  message

class FixClient:
    def __init__(self):
        self.orderId = 0
        self.fixSession = FixSession()
        self.ordersToSend = []
        self.executionReports = []

    def initialise(self, fixVersion, address, port, compId, targetCompid, heartbeatInterval=30, encryptionMethod=0):
        self.fixSession.fixVersion = fixVersion
        self.fixSession.targetAddress = address
        self.fixSession.targetPort = port
        self.fixSession.senderCompid = compId
        self.fixSession.targetCompid = targetCompid
        self.fixSession.heartbeatInterval = heartbeatInterval
        self.fixSession.encryptionMethod = encryptionMethod
        self.fixSession.restoreSequenceNumber()

    def connect(self):
        self.fixSession.connect()

    def disconnect(self):
        self.fixSession.disconnect()

    def recv(self):
        return self.fixSession.recv()

    def send(self, fixMessage):
        self.orderId += 1
        fixMessage.setTag(FixConstants.FIX_TAG_CLIENT_ORDER_ID, self.orderId)
        self.fixSession.send(fixMessage)

    def setOrders(self, orders):
        self.ordersToSend = orders

    def getOrders(self):
        return self.ordersToSend

    def getExecutionReports(self):
        return self.executionReports

    def addExecutionReport(self, fixMessage):
        self.executionReports.append(fixMessage)

def fixClientAutomationClientThread(resultsQueue, ordersFile, fixVersion, address, port, baseCompId, clientIndex, targetCompid):
    senderCompId = baseCompId + str(clientIndex)

    fixClient = FixClient()
    fixClient.initialise(fixVersion, address, port, senderCompId, targetCompid)

    orders = FixMessage.loadFromFile(ordersFile)
    ordersCount = len(orders)

    fixClient.connect()
    connected = fixClient.fixSession.connected

    if connected is True:
        print( senderCompId + " connected" )

        filledCount = 0

        for order  in orders:
            fixClient.send(order)

        print(senderCompId + " fired all orders")

        # Collect execution reports
        while True:
            message = fixClient.recv()

            if message is None:
                continue

            if message.getMessageType() is FixConstants.FIX_MESSAGE_HEARTBEAT:
                continue

            resultsQueue.put( str(clientIndex) +  "," +  message.toString(False) )

            if message.hasTag(FixConstants.FIX_TAG_ORDER_STATUS):
                orderStatus = message.getTagValue(FixConstants.FIX_TAG_ORDER_STATUS)
                if orderStatus is FixConstants.FIX_ORDER_STATUS_FILLED:
                    filledCount += 1
                    print( senderCompId + " received a fill :" + str(filledCount) + " of " + str(ordersCount) )

            if filledCount == ordersCount:
                break

        fixClient.disconnect()
        print(senderCompId + " disconnected")

class FixClientAutomation:
    def __init__(self):
        self.resultsQueue = Queue()
        self.fixClientThreads = []
        self.fixVersion = ""
        self.numberOfClients = 0
        self.ordersFile = ""
        self.address = ""
        self.port = 0
        self.compIdBase = ""
        self.targetCompId = ""

    def __del__(self):
        self.shutdown()

    def initialise(self, numberOfClients, ordersFile, fixVersion, address, port, compdIdBase, targetCompid):
        self.numberOfClients = numberOfClients
        self.ordersFile = ordersFile
        self.fixVersion = fixVersion
        self.address = address
        self.port = port
        self.compIdBase = compdIdBase
        self.targetCompId = targetCompid

    def shutdown(self):
        for thread in self.fixClientThreads:
            if thread.is_alive():
                thread.join()

    def start(self):
        for i in range(0, self.numberOfClients):
            fixClientThread = Process(target=fixClientAutomationClientThread,
                                      args=[self.resultsQueue, self.ordersFile, self.fixVersion, self.address, self.port, self.compIdBase, i+1, self.targetCompId ])
            self.fixClientThreads.append(fixClientThread)
            self.fixClientThreads[len(self.fixClientThreads) - 1].start()

    def join(self):
        for thread in self.fixClientThreads:
            thread.join()

    def report(self, reportFileName):
        report = ""

        numberOfClients = len(self.fixClientThreads)
        clientExecReports = []

        for i in range(0, numberOfClients):
            list = []
            clientExecReports.append(list)

        while not self.resultsQueue.empty():
            result = self.resultsQueue.get()
            clientIndex = int(result.split(',')[0])-1
            actualResult = result.split(',')[1]
            clientExecReports[clientIndex-1].append(actualResult)

        for i in range(0, numberOfClients):
            # Sender comp id
            currentClientFirstExecReport = FixMessage()
            currentClientFirstExecReport.loadFromString( clientExecReports[i][0] )
            report += currentClientFirstExecReport.getTagValue(FixConstants.FIX_TAG_TARGET_COMPID)

            report += "\n"
            report += "\n"

            for execReport in clientExecReports[i]:
                report += execReport
                report += "\n"

            report += "\n"
            report += "\n"

        if os.path.exists(reportFileName):
            os.remove(reportFileName)

        with open(reportFileName, "w") as textFile:
            textFile.write(report)

def main():
    try:
        Utility.changeWorkingDirectoryToScriptLocation()

        fixClientAutomation = FixClientAutomation()
        numberOfClients = 1
        ordersFile = "test_cases_simple.txt"
        fixVersion = "FIX.4.2"
        server = "127.0.0.1"
        serverPort = 5001
        clientCompIdBase = "TEST_CLIENT"
        serverCompId = "OME"

        reportFile = "report.txt"

        # GATHER ARGUMENTS
        while True :
            Utility.writeInfoMessage("Current automation parameters are : ")
            print("")
            Utility.writeInfoMessage("1 Number of concurrent clients : " + str(numberOfClients))
            Utility.writeInfoMessage("2 Test case fix file : " + ordersFile)
            Utility.writeInfoMessage("3 Fix version : " + fixVersion)
            Utility.writeInfoMessage("4 Server address : " + server)
            Utility.writeInfoMessage("5 Server port : " + str(serverPort))
            Utility.writeInfoMessage("6 Server comp id : " + clientCompIdBase)
            Utility.writeInfoMessage("7 Client comp id base value : " + serverCompId)

            print("")
            answer = Utility.yesNoQuestion("Do you want to change any parameters ? ")
            print("")

            if answer is False:
                break

            if answer is True:
                try:
                    parameterIndexToChange = int( Utility.askQuestion("Enter a number between 1 and 7 :") )
                except:
                    continue
                if parameterIndexToChange >= 1 and parameterIndexToChange <= 7:
                    parameterNewValue = Utility.askQuestion("Enter value for the parameter you selected")

                    try:
                        if parameterIndexToChange == 1:
                            numberOfClients = int(parameterNewValue)
                        elif parameterIndexToChange == 2:
                            ordersFile = parameterNewValue
                        elif parameterIndexToChange == 3:
                            fixVersion = parameterNewValue
                        elif parameterIndexToChange == 4:
                            server = parameterNewValue
                        elif parameterIndexToChange == 5:
                            serverPort = int(parameterNewValue)
                        elif parameterNewValue == 6:
                            serverCompId = parameterNewValue
                        elif parameterNewValue == 7:
                            clientCompIdBase = parameterNewValue
                    except:
                        continue

        # RUN AUTOMATION
        print("")
        Utility.writeInfoMessage("Client automation starting")
        print("")

        stopwatch = StopWatch()
        stopwatch.start()

        fixClientAutomation.initialise(numberOfClients, ordersFile, fixVersion, server, serverPort, clientCompIdBase, serverCompId)
        fixClientAutomation.start()
        fixClientAutomation.join()

        stopwatch.stop()

        # DISPLAY RESULTS
        print("")
        Utility.writeInfoMessage("Client automation took " + str(stopwatch.elapsedTimeInMilliseconds()) + " milliseconds" )
        print("")

        fixClientAutomation.report(reportFile)

        if _platform == "linux" or _platform == "linux2":
            Utility.writeInfoMessage(reportFile + " created")
        elif _platform == "win32":
            os.system("notepad " + reportFile )

    except ValueError as err:
        Utility.writeErrorMessage(err.args)

# Entry point
if __name__ == "__main__":
    main()