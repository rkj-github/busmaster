
/******************************************************************************
  Project       :  Auto-SAT_Tools
  FileName      :  LogObjectJ1939.cpp
  Description   :  Source file for CLogObjectJ1939 class.
  $Log:   X:/Archive/Sources/FrameProcessor/LogObjectJ1939.cpv  $
   
      Rev 1.9   02 Dec 2011 20:16:50   rac2kor
   Removed hard coding of version information 
   string in the log file by accepting it as a parameter
   from the application / client.
   
      Rev 1.8   30 Nov 2011 20:46:44   CANMNTTM
   Version number is changed
   
      Rev 1.7   13 Sep 2011 11:50:44   CANMNTTM
    
   
      Rev 1.6   07 Sep 2011 14:33:38   rac2kor
   Updated application version number
   
      Rev 1.5   06 Sep 2011 20:06:12   CANMNTTM
   CANvas Name is changed to BUSMASTER
   
      Rev 1.4   15 Apr 2011 19:20:38   rac2kor
   Inserted RBEI Copyright information text into the file header.
   
      Rev 1.3   13 Dec 2010 21:24:36   rac2kor
   Added message type field in the log file.
   Also, corrected the ordering of fields PGN and CANID.
   
      Rev 1.2   10 Dec 2010 17:40:54   rac2kor
   Updated version information
   
      Rev 1.1   10 Dec 2010 11:58:46   rac2kor
    
   
      Rev 1.0   06 Dec 2010 18:53:02   rac2kor
    
   

  Author(s)     :  Ratnadip Choudhury
  Date Created  :  30.11.2010
  Modified By   :  
  Copyright (c) 2011, Robert Bosch Engineering and Business Solutions.  All rights reserved.
******************************************************************************/

#include "FrameProcessor_stdafx.h"
#include "LogObjectJ1939.h"            // For CLogObjectJ1939 class declaration


#define J1939_LOG_COLUMNS       _T("***<Time><Channel><CAN ID><PGN><Type><Source Node><Destination Node><Priority><Tx/Rx><DLC><DataBytes>***")



CLogObjectJ1939::CLogObjectJ1939(CString omVersion):CBaseLogObject(omVersion)
{
    // Initialise the filtering block
    m_sFilterApplied.vClear();
}

CLogObjectJ1939::~CLogObjectJ1939()
{
}

void CLogObjectJ1939::Der_CopySpecificData(const CBaseLogObject* pouLogObjRef)
{
    const CLogObjectJ1939 *pouLobObjCANRef = static_cast <const CLogObjectJ1939 *> (pouLogObjRef);
    pouLobObjCANRef->GetFilterInfo(m_sFilterApplied);
}

BOOL CLogObjectJ1939::bLogData(const SFORMATTEDATA_J1939& sDataJ1939)
{
    // Multiple return statements are used to keep the code precise.

    SFRAMEINFO_BASIC_J1939 J1939Info_Basic = 
    {
        sDataJ1939.m_dwPGN
    };
    
    // Assign appropriate values to FrameInfo_Basic

    if (bToBeLogged(J1939Info_Basic) == FALSE)
    {
        return FALSE;
    }

    CString omLogText = _T("");

    TCHAR* pTimeData = NULL;
    TCHAR acID[16] = {'\0'};
    TCHAR* pPGN = NULL;
    TCHAR* pData = NULL;
    TCHAR* psSrcNode = NULL;
    TCHAR* psDestNode = NULL;

    switch (m_sLogInfo.m_eLogTimerMode) // Time Mode
    {
        case TIME_MODE_ABSOLUTE: 
        {
            pTimeData = (TCHAR *) (sDataJ1939.m_acTimeAbs);
        }
        break;
        case TIME_MODE_RELATIVE: 
        {
            pTimeData = (TCHAR *) (sDataJ1939.m_acTimeRel);
        }
        break;
        case TIME_MODE_SYSTEM:
        {
            pTimeData = (TCHAR *) (sDataJ1939.m_acTimeSys);
        }
        break;
        default: ASSERT(FALSE); break;
    }

    switch (m_sLogInfo.m_eNumFormat)
    {
        case HEXADECIMAL: 
        {
            sprintf(acID, "%x", sDataJ1939.m_dwMsgID);
            pPGN = (TCHAR *) (sDataJ1939.m_acPGNHex);
            pData = (TCHAR *) (sDataJ1939.m_pcDataHex);
            psSrcNode = (TCHAR *) (sDataJ1939.m_acSrcHex);
            psDestNode = (TCHAR *) (sDataJ1939.m_acDestHex);
        }
        break;
        case DEC: 
        {
            sprintf(acID, "%d", sDataJ1939.m_dwMsgID);
            pPGN = (TCHAR *) (sDataJ1939.m_acPGNDec);
            pData = (TCHAR *) (sDataJ1939.m_pcDataDec);
            psSrcNode = (TCHAR *) (sDataJ1939.m_acSrcDec);
            psDestNode = (TCHAR *) (sDataJ1939.m_acDestDec);
        }
        break;
        default: ASSERT(FALSE); break;
    }

    // First put everything in a string to get the length
    // <Time> <Channel> <ID> <PGN> <Type> <Src Node> <Dest Node> <Priority> <Direction> <DLC> <Data>
    omLogText.Format(  _T("%s %s %s %s %s %s %s %s %s %s %s\n"), 
                       pTimeData,
                       sDataJ1939.m_acChannel,
                       acID,
                       pPGN,
                       sDataJ1939.m_acMsgType,
                       psSrcNode,
                       psDestNode,
                       sDataJ1939.m_acPriority,
                       sDataJ1939.m_acMsgDir,
                       sDataJ1939.m_acDataLen,
                       pData);

    vWriteTextToFile(omLogText);

    return TRUE;
}

// To format the header 
void CLogObjectJ1939::vFormatHeader(CString& omHeader)
{
    CBaseLogObject::vFormatHeader(omHeader);
    omHeader += J1939_LOG_COLUMNS;
    omHeader += L'\n';
}

// To format the footer 
void CLogObjectJ1939::vFormatFooter(CString& omFooter)
{
    CBaseLogObject::vFormatFooter(omFooter);
}

/*******************************************************************************
//  Function Name  : bLogData
//  Input(s)       : none
//  Output         : none
//  Description    : Logs data. This will open the file in
//                   appropriate mode and will insert header if it is new.
//  Member of      : CBaseLogObject
//	Friend of	   : None
//	Author		   : Arun Kumar
//	Creation Date  : 10/11/06
//	Modifications  : Anish, the message structure is changed
//  Modifications  : Ratnadip Choudhury.
                     1. Changed the prototype
                     2. Shifted the formatting codes into vWriteTextToFile(...)
*******************************************************************************/
BOOL CLogObjectJ1939::bToBeLogged(SFRAMEINFO_BASIC_J1939& J1939Info_Basic)
{
    // Multiple return statements are used to keep the code precise.

    if (m_sLogInfo.m_bEnabled == FALSE) 
    {
        return FALSE;
    }

    if (NULL == m_pLogFile)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    if (m_sFilterApplied.m_bEnabled == TRUE)
    {
        if (m_sFilterApplied.bToBeBlocked(J1939Info_Basic) == TRUE)
        {
            return FALSE;
        }
    }

    // Check for the triggering conditions
    switch (m_CurrTriggerType)
    {
        case NONE: break;
        case STOPPED:
        {
            //If the log file is stopped then don't log
            return FALSE;
        }
        break;
        case START:
        {
            if (m_sLogInfo.m_sLogTrigger.m_unStartID == J1939Info_Basic.m_dwPGN)
            {
                m_CurrTriggerType = NONE;
            }
            else 
            {
                return FALSE;
            }
        }
        break;
        case STOP:
        {
            if (m_sLogInfo.m_sLogTrigger.m_unStopID == J1939Info_Basic.m_dwPGN)
            {
                m_CurrTriggerType = STOPPED;
            }
        }
        break;

        case BOTH:
        {
            if (m_sLogInfo.m_sLogTrigger.m_unStartID == J1939Info_Basic.m_dwPGN)
            {
                m_CurrTriggerType = STOP;
            }
            else
            {
                return FALSE;
            }
        }
        break;
        default: ASSERT(FALSE); break;
    }

    return TRUE;
}

BYTE* CLogObjectJ1939::Der_SetConfigData(BYTE* pvDataStream)
{
    bool bResult = false;
    BYTE* pbSStream = pvDataStream;

    pbSStream = m_sFilterApplied.pbSetConfigData(pbSStream, bResult);

    return pbSStream;
}

BYTE* CLogObjectJ1939::Der_GetConfigData(BYTE* pvDataStream) const
{
    BYTE* pbTStream = pvDataStream;

    pbTStream = m_sFilterApplied.pbGetConfigData(pbTStream);

    return pbTStream;
}

UINT CLogObjectJ1939::Der_unGetBufSize(void) const
{
	return m_sFilterApplied.unGetSize();
}

void CLogObjectJ1939::EnableFilter(BOOL bEnable)
{
    m_sFilterApplied.m_bEnabled = bEnable;
}

void CLogObjectJ1939::GetFilterInfo(SFILTERAPPLIED_J1939& sFilterInfo) const
{
    sFilterInfo.bClone(m_sFilterApplied);
}

void CLogObjectJ1939::SetFilterInfo(const SFILTERAPPLIED_J1939& sFilterInfo)
{
    m_sFilterApplied.bClone(sFilterInfo);
}