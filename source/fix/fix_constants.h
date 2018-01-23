#ifndef _FIX_CONSTANTS_
#define _FIX_CONSTANTS_

namespace fix
{

class FixConstants
{
    public:
        // GENERAL
        static const char FIX_EQUALS = '=';
        static const char FIX_DELIMITER = ((char)1);
        // TAGS
        static const int FIX_TAG_VERSION = 8;
        static const int FIX_TAG_BODY_LENGTH = 9;
        static const int FIX_TAG_BODY_CHECKSUM = 10;
        static const int FIX_TAG_CLIENT_ORDER_ID = 11;
        static const int FIX_TAG_EXEC_ID = 17;
        static const int FIX_TAG_EXEC_INST = 18;
        static const int FIX_TAG_HAND_INST = 21;
        static const int FIX_TAG_SEQUENCE_NUMBER = 34;
        static const int FIX_TAG_MESSAGE_TYPE = 35;
        static const int FIX_TAG_ORDER_QUANTITY = 38;
        static const int FIX_TAG_ORDER_STATUS = 39;
        static const int FIX_TAG_ORDER_TYPE = 40;
        static const int FIX_TAG_ORDER_PRICE = 44;
        static const int FIX_TAG_SECURITY_ID = 48;
        static const int FIX_TAG_SENDER_COMPID = 49;
        static const int FIX_TAG_SENDING_TIME = 52;
        static const int FIX_TAG_ORDER_SIDE = 54;
        static const int FIX_TAG_SYMBOL = 55;
        static const int FIX_TAG_TARGET_COMPID = 56;
        static const int FIX_TAG_TIME_IN_FORCE = 59;
        static const int FIX_TAG_TRANSACTION_TIME = 60;
        static const int FIX_TAG_ENCRYPT_METHOD = 98;
        static const int FIX_TAG_HEARBEAT_INTERVAL = 108;
        static const int FIX_TAG_EXEC_TYPE = 150;
        static const int FIX_TAG_LEAVES_QTY = 151;
        // MESSAGE TYPES
        static const char FIX_MESSAGE_HEARTBEAT = '0';
        static const char FIX_MESSAGE_LOG_ON = 'A';
        static const char FIX_MESSAGE_LOG_OFF = '5';
        static const char FIX_MESSAGE_EXECUTION_REPORT = '8';
        static const char FIX_MESSAGE_NEW_ORDER = 'D';
        static const char FIX_MESSAGE_AMEND_ORDER = 'G';
        static const char FIX_MESSAGE_CANCEL_ORDER = 'F';
        // ORDER STATUS
        static const int FIX_ORDER_STATUS_NEW = 0;
        static const int FIX_ORDER_STATUS_PARTIALLY_FILLED = 1;
        static const int FIX_ORDER_STATUS_FILLED = 2;
        static const int FIX_ORDER_STATUS_DONE_FOR_DAY = 3;
        static const int FIX_ORDER_STATUS_CANCELED = 4;
        static const int FIX_ORDER_STATUS_REPLACED = 5;
        static const int FIX_ORDER_STATUS_PENDING_CANCEL = 6;
        static const int FIX_ORDER_STATUS_STOPPED = 7;
        static const int FIX_ORDER_STATUS_REJECTED = 8;
        // ORDER TYPE
        static const int FIX_ORDER_TYPE_MARKET = 1;
        static const int FIX_ORDER_TYPE_LIMIT = 2;
        // SIDE
        static const int FIX_ORDER_SIDE_BUY = 1;
        static const int FIX_ORDER_SIDE_SELL = 2;
        // TIME IN FORCE
        static const int FIX_ORDER_TIF_DAY = 0;
        // ENCRYPTION METHODS
        static const int FIX_ENCRYPTION_NONE = 0;
};

}

#endif