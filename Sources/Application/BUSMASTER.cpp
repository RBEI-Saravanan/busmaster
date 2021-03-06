/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file      BUSMASTER.cpp
 * \brief     CCANMonitorApp class implementation file
 * \authors   Amitesh Bharti, Ratnadip Choudhury, Anish kumar
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * CCANMonitorApp class implementation file
 */
#include "stdafx.h"
#include <initguid.h>
#include "HashDefines.h"
#include "common.h"
#include "SectionNames.h"
#include "Datatypes/ProjConfig_DataTypes.h"
#include "BUSMASTER.h"
#include "MainFrm.h"
#include "MessageAttrib.h"
#include "Splash.h"             // splash screen implementation file
#include "Replay/Replay_Extern.h"
#include "ConfigData.h"
#include "ConfigAdapter.h"
#include "InterfaceGetter.h"

//#include "ExecuteManager.h"
//disable the deprecated Enable3dControls() function warning
#pragma warning(disable : 4996)

#include "BUSMASTER_Interface.h"
#include "BUSMASTER_Interface.c"

 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern DWORD GUI_dwThread_MsgDisp;
extern BOOL g_bStopSendMultMsg;
extern BOOL g_bStopKeyHandlers;
extern BOOL g_bStopErrorHandlers;
extern BOOL g_bStopSelectedMsgTx;
extern BOOL g_bStopMsgBlockTx;
extern PSTXMSG g_psTxMsgBlockList;
// To Kill Message Handler Threads
extern void gvStopMessageHandlerThreads();


#ifdef FOR_ETIN
//static function used for getting erg values
static LONG GetEntitlementID(CString& omEntitlementId)
{
    char SubKey[] = "SOFTWARE\\Classes\\BoschIndia\\BUSMASTER tool\\ES581";
	HKEY hCurrKey = NULL;
	LONG Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			(LPCTSTR) SubKey, 0, KEY_READ, &hCurrKey);

	char ValueName[32] = "Identifier";
	char Data[128] = {'\0'};
	DWORD Size = 128;

	Result = RegQueryValueEx(hCurrKey, (LPCTSTR) ValueName,
			NULL, (LPDWORD) NULL, (LPBYTE) Data, 
			&Size);
    omEntitlementId = Data;
    return Result;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}
BOOL CAboutDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();    
    CString omVerStr(_T(""));
    omVerStr.Format(IDS_VERSION);
    GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(omVerStr);

#ifdef FOR_ETIN
    // Set Entitlement ID
    CString omEntitlementId(_T(""));
    GetEntitlementID(omEntitlementId);
    GetDlgItem(IDC_EDIT1)->SetWindowText(omEntitlementId);
#endif // FOR_ETIN

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCANMonitorApp

BEGIN_MESSAGE_MAP(CCANMonitorApp, CWinApp)
    //{{AFX_MSG_MAP(CCANMonitorApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    //ON_COMMAND(ID_FILE_NEW,  OnFileNew)
    //ON_COMMAND( ID_FILE_OPEN, OnFileOpen ) 
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/******************************************************************************/
/*  Function Name    :  CCANMonitorApp                                        */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :                                                        */
/*                                                                            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath Shastry,Amitesh Bharti                       */
/*  Date Created     :  20.02.2002                                            */
/*  Modification date:  30.12.2002,                                           */
/*  Modification By  :  Amitesh Bharti,                                       */
/*                      Initialise the thread structure.                      */
/*  Modification date:  22.07.2004,                                           */
/*  Modification By  :  Amitesh Bharti,                                       */
/*                      Modofiction due to new DLL Unload thread              */
/******************************************************************************/
CCANMonitorApp::CCANMonitorApp()
{
	
    // TODO: add construction code here,
    m_pouFlags = NULL;
    m_bIsMRU_CreatedInOpen = FALSE;
	//m_pDocTemplate = NULL;
	m_bFromAutomation = FALSE;
    GetCurrentDirectory(MAX_PATH, m_acApplicationDirectory);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCANMonitorApp object

CCANMonitorApp theApp;

static const CLSID clsid =
{ 0xc4eff9ad, 0x8f4b, 0x42bf, { 0xb6, 0xf5, 0x2f, 0xb3, 0xcd, 0x70, 0xa2, 0x2b } };

const GUID CDECL BASED_CODE _tlid =
{ 0xf710b25d, 0x7abf, 0x4545, { 0xa9, 0x69, 0x53, 0x74, 0xc3, 0xec, 0x8b, 0x8a } };

const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


/******************************************************************************/
/*  Function Name    :  InitInstance                                          */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :                                                        */
/*                                                                            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath Shastry,Amitesh Bharti                       */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :  Ratnadip Choudhury                                    */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modifications on :  23.03.2002, Interface for message filter list is chang*/
/*  Modification date:  28.11.2002,                                           */
/*  Modification By  :  Amarnath S                                            */
/*                      CFlags data member allocated before loading....       */
/*                      and initialisation of toolbar button members from     */
/*                      configuration file moved to nLoadConfiguration(...)   */
/*                      member function.                                      */
/*  Modification date:  18.12.2002,                                           */
/*  Modification By  :  Amarnath S                                            */
/*                      Updates member of CMainFrame if number of msgs        */
/*                      configured is more than zero in SEND msg module       */
/*                      to reflect the state of SEND toolbar button.          */
/*  Modification date:  18.12.2002,                                           */
/*  Modification By  :  Amitesh Bharti, Ratnadip Choudhury                    */
/*                      Post a message to message window thread with log file */
/*                      and status of log flag after getting it from          */
/*                      configuration module.                                 */
/*  Modification date:  30.12.2002,                                           */
/*  Modification By  :  Amitesh Bharti,                                       */
/*                      Re-arranged the sequence of code to take care of      */
/*                      double click of configuration file in explore. It will*/
/*                      get the configuration file name from user double click*/
/*                      and load that file. If it is null the configuration   */
/*                      store in registry will be loaded.                     */
/*  Modification date:  02.05.2003                                            */
/*  Modification By  :  Amitesh Bharti                                        */
/*                      Call function bInitialiseConfiguration() to initialise*/
/*                      setting as stored in configuration file.              */
/*  Modification By  :  Raja N on 01.08.2004                                  */
/*                      Added filter init code after the creation of msgwnd   */
/*                      This will load filter and pass the pointer to msgwnd  */
/*                      and added code to start logging it is enabled in the  */
/*                      configuration file                                    */
/*  Modification By  :  Raja N on 12.12.2004                                  */
/*                      Added API call AfxEnableControlContainer to enable OLE*/
/*                      object use in the application.                        */
/*  Modification By  :  Raja N on 20.07.2005                                  */
/*                      Added code to init display filter and new log manager */
/*                      architecture                                          */
/*  Modification By  :  Pradeep Kadoor on 12.06.2009                          */
/*                      1. Added code to load a sample database file if       */
/*                      no configuration file is found.                       */
/*                      2. Main Frame GUI is created before the initialzation */
/*                      of configuration                                      */
/******************************************************************************/
BOOL CCANMonitorApp::InitInstance()
{
	InitCommonControls(); 
	// START CHANGES MADE FOR AUTOMATION
	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox("Fail to Intilaize OLE");
		return FALSE;
	}

	// END CHANGES MADE FOR AUTOMATION

    // Enable OLE/ActiveX objects support
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need. DEBUG 


#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("RBIN"));

	// START CHANGES MADE FOR AUTOMATION
	COleTemplateServer::RegisterAll();
	// END CHANGES MADE FOR AUTOMATION

    LoadStdProfileSettings(0); // Load standard INI file options (including MRU)
  
   // Enable drag/drop open
    

    // Enable DDE Execute open
    //EnableShellOpen();
    //RegisterShellFileTypes(TRUE);

	// Display splash screen
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

	short shRegServer = -1; 
	short shUnRegServer = -1; 
	if (__argc > 1)
	{
		shRegServer = strcmpi(__targv[1],"/regserver");	
		shUnRegServer = strcmpi(__targv[1],"/unregserver");	
	}


	// Don't display a new MDI child window during startup
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew
		|| cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		  cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;          
	}
	

	// START CHANGES MADE FOR AUTOMATION

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		m_bFromAutomation = TRUE;
//		return TRUE;
	} else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister){
		AfxOleUnregisterTypeLib(LIBID_CAN_MonitorApp);
	} else {
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), LIBID_CAN_MonitorApp);
	}
	
	if (  shRegServer == 0  || shUnRegServer == 0 )	//If command line argument match
	{		
		 return FALSE;
	}


    if (!m_bFromAutomation)
    {
        CSplashScreen::ActivateSplashScreen(cmdInfo.m_bShowSplash);
    }
	
	// Allocate memory for CFlags
    m_pouFlags = &(CFlags::ouGetFlagObj());

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if ( pMainFrame == NULL )
    {
        ::PostQuitMessage(0);
    }

   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        return FALSE;
    }
    m_pMainWnd = pMainFrame;
    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
    {
        return FALSE;
    }
	m_pMainWnd->DragAcceptFiles();

    
    // show main frame    
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
    
    //// Create message window
    pMainFrame->bCreateMsgWindow();

    // In-Active Database
    m_pouMsgSgInactive  = new CMsgSignal(sg_asDbParams[CAN], m_bFromAutomation);
    if(m_pouMsgSgInactive == NULL )
    {
		if(m_bFromAutomation==FALSE)
        MessageBox(NULL,MSG_MEMORY_CONSTRAINT,
                    "BUSMASTER", MB_OK|MB_ICONINFORMATION);
        ::PostQuitMessage(0);
    }

    BOOL bResult;
    bResult = m_aomState[UI_THREAD].SetEvent();

    // get the information of the last used configuration file..
    // initialize the flag that indicates if the configuratin file has been
    // loaded..
    m_bIsConfigFileLoaded = FALSE;
    CString ostrCfgFilename = _T("");
    // If user has double click the .cfg file then assign that file name else
    // read from registry.
    if(cmdInfo.m_strFileName.IsEmpty() == TRUE)
    {
        ostrCfgFilename = 
            GetProfileString(SECTION, defCONFIGFILENAME, STR_EMPTY);
    }
    else
    {
        ostrCfgFilename = cmdInfo.m_strFileName;
    }	
    if(ostrCfgFilename.IsEmpty() == FALSE)
    {		
        bInitialiseConfiguration(m_bFromAutomation);		
        // load the configuration information
        if(pMainFrame->nLoadConfigFile(ostrCfgFilename) != defCONFIG_FILE_SUCCESS)
        {
            //m_oConfigDetails.vInitDefaultValues();
            m_ostrConfigFilename = STR_EMPTY;
            
        }
        else
        {
            m_ostrConfigFilename = ostrCfgFilename;
        }
        m_bIsConfigFileLoaded = TRUE;		
    }
    else
    {
        BOOL bReturn = bInitialiseConfiguration(m_bFromAutomation);		
        if(bReturn == FALSE )
        {
            ::PostQuitMessage(0);
        }
        // Load a default database file
        //CStringArray omDatabaseArray;
        CString omSampleDatabasePath;
        omSampleDatabasePath.Format("%s\\Samples\\SampleDB.dbf",m_acApplicationDirectory);
        DWORD dRetVal = pMainFrame->dLoadDataBaseFile(omSampleDatabasePath, FALSE);
        
        if (dRetVal == S_OK)
        {     
            //omDatabaseArray.Add(omSampleDatabasePath);
            //Store in configdetails
            //bSetData(DATABASE_FILE_NAME, &omDatabaseArray);
            bWriteIntoTraceWnd(MSG_DEFAULT_DATABASE);
            bWriteIntoTraceWnd(MSG_CREATE_UNLOAD_DATABASE);
        }
    }

    // ********  Filter workaround  ********
    // Filter list is initialised before msg wnd creation. So update display
    // filter here
    // Update Message Display Filter List
    //::PostThreadMessage(GUI_dwThread_MsgDisp, TM_UPDATE_FILTERLIST, NULL, NULL );
    // ********  Filter workaround  ********	
    pMainFrame->bUpdatePopupMenuDIL();
    // Start Logging if is enabled
    // Get the Flag Pointer
    CFlags* pomFlag =  pouGetFlagsPtr();
    if( pomFlag != NULL )
    {
        // Get the Logging Status
        BOOL bLogON = pomFlag->nGetFlagStatus(LOGTOFILE);
        // If it is on then post a message to display thread to start logging
        if(bLogON == TRUE )
        {
            // Start Logging
            //CLogManager::ouGetLogManager().vStartStopLogging( TRUE );
        }
    }
    //CExecuteManager::ouGetExecuteManager().vStartDllReadThread();	
    return TRUE;
}

/******************************************************************************/
/*  Function Name    :  WinHelp                                               */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :                                                        */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :  Raja N on 17.01.2005                                  */
/*                      Modified code to invoke HTML help                     */
/*                      Anish  on 27.04.2006                                  */
/*                      Modified code for porting to .net                     */
/******************************************************************************/

void CCANMonitorApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	CWinApp::WinHelp(dwData,  nCmd) ;
}
/******************************************************************************/
/*  Function Name    :  ExitInstance                                          */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :                                                        */
/*                                                                            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :  Ratnadip Choudhury                                    */
/*  Modifications    :  Amitesh Bharti, 31.12.2002                            */
/*                      remove thread termination code.                       */
/******************************************************************************/
int CCANMonitorApp::ExitInstance() 
{

    if (m_pouMsgSignal != NULL )
    {
        m_pouMsgSignal->bDeAllocateMemory(STR_EMPTY);

        delete m_pouMsgSignal;

        m_pouMsgSignal = NULL;
    }
    // if the user directly closes the appln when the database
    // is opened, 
    // Delete memory associated with the in-active data structure.
    if ( m_pouMsgSgInactive != NULL )
    {
		m_pouMsgSgInactive->bDeAllocateMemoryInactive();

        delete m_pouMsgSgInactive;

        m_pouMsgSgInactive = NULL;
    }

    DWORD dwResult = WaitForSingleObject(m_aomState[UI_THREAD], MAX_TIME_LIMIT);
    switch (dwResult) 
    {
        case WAIT_ABANDONED: 
            break;
        case WAIT_OBJECT_0:
            break;
        case WAIT_TIMEOUT:
            break;
        default:
            break;
    }
    return CWinApp::ExitInstance();
}

/******************************************************************************/
/*  Function Name    :  OnAppAbout                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :                                                        */
/*                                                                            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath Shastry,Amitesh Bharti                       */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :                                                        */
/******************************************************************************/
void CCANMonitorApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
/******************************************************************************/
/*  Function Name    :  pouGetFlagsPtr                                        */
/*  Input(s)         :                                                        */
/*  Output           :  Pointer to CFlags                                     */
/*  Functionality    :  Return m_pouFlags data member varible value           */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :                                                        */
/******************************************************************************/
CFlags* CCANMonitorApp::pouGetFlagsPtr()
{
    return m_pouFlags; 
}
/******************************************************************************/
/*  Function Name    :  vSetHelpID                                            */
/*  Input(s)         :  dwHelpID                                              */
/*  Output           :                                                        */
/*  Functionality    :  assign the value passed as parameter to m_dwHelpID    */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  20.02.2002                                            */
/*  Modifications    :                                                        */
/******************************************************************************/
void CCANMonitorApp::vSetHelpID(DWORD dwHelpID)
{
    m_dwHelpID = dwHelpID;
}

CWnd* CCANMonitorApp::GetMainWnd() 
{
    // TODO: Add your specialized code here and/or call the base class
    
    return CWinApp::GetMainWnd();
}
/******************************************************************************/
/*  Function Name    :  vPopulateCANIDList                                    */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Fills the CANIDList struct with dtabase message ID,
                        name and color for future use.    
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath Shastry                                      */
/*  Date Created     :  04.04.2002                                            */
/*  Modifications    :  03-05-2002                                            */
/*  Modification By  :  Anish,02.02.07                                        */
/*  Modification on  :  Removed memory leak due to pidArray                   */
/******************************************************************************/
void CCANMonitorApp::vPopulateCANIDList()
{
    CMessageAttrib& ouMsgAttr = CMessageAttrib::ouGetHandle(CAN);

    
    if ( m_pouMsgSignal != NULL )
    {
        CStringList omStrMsgNameList;
        UINT unNoOfMsgs = 
            m_pouMsgSignal->unGetNumerOfMessages();

        UINT* pIDArray = new UINT[unNoOfMsgs];

        m_pouMsgSignal->omStrListGetMessageNames(omStrMsgNameList);
        
        if (pIDArray != NULL )
        {
            m_pouMsgSignal->unListGetMessageIDs( pIDArray );

             SCanIDList sList;

            POSITION pos = omStrMsgNameList.GetHeadPosition();
        
            UINT unCount = 0;
            POSITION pos1 = pos;

            for ( pos1 = pos, unCount = (unNoOfMsgs - 1);
            ((pos1 != NULL) && (unCount >= 0)); 
            unCount--)
            {
                sList.nCANID        = pIDArray[unCount];
                sList.omCANIDName   = omStrMsgNameList.GetNext( pos1 );

                if (ouMsgAttr.nValidateNewID(sList.nCANID) == MSGID_DUPLICATE)
                {
                    sList.Colour = ouMsgAttr.GetCanIDColour(sList.nCANID);
                    ouMsgAttr.nModifyAttrib(sList);
                }
                else
                {
                    sList.Colour = DEFAULT_MSG_COLOUR;
                    ouMsgAttr.nAddNewAttrib( sList );
                }
            }
            
            ouMsgAttr.vDoCommit();
            delete [] pIDArray;
            pIDArray = NULL;
        }
    }
}
/******************************************************************************/
/*  Function Name    :  omStrGetUnionFilePath                                 */
/*  Input(s)         :  file name to change                                   */
/*  Output           :  CString[File path]
/*  Functionality    :  Returns file path of unions.h
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath Shastry                                      */
/*  Date Created     :  04.07.2002                                            */
/*  Modification     :  Anish,19.12.2006
/*						For MDB,it create union file name for each DB name
/******************************************************************************/
CString CCANMonitorApp::omStrGetUnionFilePath(CString omStrTemp)
{
	CString omStrHeaderFileName = omStrTemp.Left(omStrTemp.ReverseFind('.'));
    omStrHeaderFileName += defHEADER_FILE_NAME;
    return omStrHeaderFileName;
}
/******************************************************************************/
/*  Function Name    :  bSetData                                              */
/*  Input(s)         :  eParam : enumeration denoting the information that    */
/*                               needs to be stored into the config           */
/*                      lpVoid : pointer where the data should be written     */
/*  Output           :  TRUE : if the CConfigDetails object successfully      */
/*                             updates the information                        */
/*                      FALSE : if any error is encountered while updating the*/
/*                              information                                   */
/*  Functionality    :  This is a wrapper around the method                   */
/*                      CConfigDetails::bSetData(...). It is planned to use   */
/*                      only one global object, theApp as provided by the     */
/*                      Wizard.                                               */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Gopi                                                  */
/*  Date Created     :  11.11.2002                                            */
/******************************************************************************/
BOOL CCANMonitorApp::bSetData1(eCONFIGDETAILS /*eParam*/, LPVOID /*lpVoid*/)
{
    return FALSE;//m_oConfigDetails.bSetData(eParam, lpVoid);
}

/******************************************************************************/
/*  Function Name    :  bGetData                                              */
/*  Input(s)         :  eParam : enumeration denoting the information that    */
/*                               needs to be obtained from the config         */
/*                      lpVoid : source pointer for data                      */
/*  Output           :  TRUE : if the CConfigDetails object successfully      */
/*                             obtains the information                        */
/*                      FALSE : if any error is encountered while obtaining   */
/*                              the information                               */
/*  Functionality    :  This is a wrapper around the method                   */
/*                      CConfigDetails::bGetData(...). It is planned to use   */
/*                      only one global object, theApp as provided by the     */
/*                      Wizard.                                               */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Gopi                                                  */
/*  Date Created     :  11.11.2002                                            */
/******************************************************************************/
BOOL CCANMonitorApp::bGetData1(eCONFIGDETAILS /*eParam*/, LPVOID* /*lpData*/)
{
    return FALSE;//m_oConfigDetails.bGetData(eParam, lpData);
}

/******************************************************************************/
/*  Function Name    :  vRelease                                              */
/*  Input(s)         :  eParam : enumeration denoting the section for which   */
/*                               the memory should be released                */
/*                      lpDataPtr : pointer that should be released           */
/*  Output           :                                                        */
/*  Functionality    :  This is a wrapper around the method                   */
/*                      CConfigDetails::bRelease(...). It is planned to use   */
/*                      only one global object, theApp as provided by the     */
/*                      Wizard.                                               */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Gopi                                                  */
/*  Date Created     :  11.11.2002                                            */
/******************************************************************************/
void CCANMonitorApp::vRelease1(eCONFIGDETAILS /*eParam*/, LPVOID* /*lpDataPtr*/)
{
    //m_oConfigDetails.vRelease(eParam, lpDataPtr);
}

void CCANMonitorApp::vSetFileStorageInfo(CString oCfgFilename)
{    
	DATASTORAGEINFO stempDataInfo;
	FILESTORAGEINFO FileStoreInfo;
	strcpy (FileStoreInfo.m_FilePath, oCfgFilename.GetBuffer(MAX_PATH));
	stempDataInfo.FSInfo = &FileStoreInfo;
	stempDataInfo.m_Datastore = FILEMODE;
	CConfigData::ouGetConfigDetailsObject().SetConfigDatastorage(&stempDataInfo);
	CConfigData::ouGetConfigDetailsObject().vSetCurrProjName(DEFAULT_PROJECT_NAME);
    CMainFrame* pMainFrame= (CMainFrame*)m_pMainWnd;
    if (pMainFrame != NULL)
    {
        pMainFrame->vPushConfigFilenameDown(oCfgFilename);
    }
}

void CCANMonitorApp::GetLoadedConfigFilename(CString &roStrCfgFile)
{
    roStrCfgFile = m_ostrConfigFilename;
}
/******************************************************************************/
/*  Function Name    :  OnFileOpen                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  This is a message handler for ID_FILE_OPEN            */
/*                      This function will be called when user selects        */
/*                      File -> Function Editor -> Open menu option.          */
/*                      Displays open file dialog initialised with *.c filter,*/
/*                      and the previous selected C file.                     */
/*                      Opens the selected file if the file is found and      */
/*                      saves back the selected file into the configuration   */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath S                                            */
/*  Date Created     :  22.11.2002                                            */
/*  Modifications    :  Amitesh Bharti                                        */
/*                      13.03.2003, If function editor is open and CTRL o is  */
/*                      pressed, don't call OnFileNew function. Check for flag*/
/*  Modifications    :  Raja N                                                */
/*                      16.03.2004, Moved the cration of MRU for MDI child to */
/*                      If block to avoide duplicate entries during cancel to */
/*                      file selection dialog or file not found               */
/*  Modifications    :  Anish kumar                                           */
/*                      13.01.2005, Enable the user to open multiple file at  */
/*                      a time                                                */
/******************************************************************************/
void CCANMonitorApp::OnFileOpen()
{
    // Display open dialog box with *.c filter
    // and select the C file by default
    CFileDialog fileDlg( TRUE,      // Open File dialog
                            "c",       // Default Extension,
                            m_omMRU_C_FileName,                        
                            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                            "C File(s)(*.c)|*.c||",
                            NULL );

    // Set Title
    fileDlg.m_ofn.lpstrTitle  = _T("Select BUSMASTER Source Filename...");

    if ( IDOK == fileDlg.DoModal() )
    {
        CString strExtName  = fileDlg.GetFileExt();
        CString omStrNewCFileName   = fileDlg.GetPathName();

        if ( omStrNewCFileName.ReverseFind('.') )
        {
            omStrNewCFileName = omStrNewCFileName.
                Left( omStrNewCFileName.ReverseFind('.') + 1);
            omStrNewCFileName.TrimRight();
            omStrNewCFileName += strExtName;
        }

        // file-attribute information
        struct _finddata_t fileinfo;
        // Check if file exists
        if (_findfirst( omStrNewCFileName.GetBuffer(MAX_PATH), &fileinfo) != -1L)
        {
            // Now open the selected file
            CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
            GetICANNodeSim()->FE_OpenFunctioneditorFile(omStrNewCFileName, pMainFrame->GetSafeHwnd(),
                pMainFrame->m_sExFuncPtr[CAN]);
            // Save the selected filename into the configuration details
            // if it is is not the same C file
            
            // Since opening of the document 
            // loads another menu,
            // the menu created for the MRU
            // would have destroyed. So create 
            // the same.
            // And this should be created only once.
            if ( pMainFrame != NULL && m_bIsMRU_CreatedInOpen == FALSE )
            {
                pMainFrame->vCreateMRU_Menus();
                m_bIsMRU_CreatedInOpen = TRUE;
            }

        }
        else
        {
            MessageBox(NULL,"Specified filename not found!", 
                        "BUSMASTER",MB_OK|MB_ICONINFORMATION);
        }
    }
}
/******************************************************************************/
/*  Function Name    :  vDisplayConfigErrMsgbox                               */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Dispaly an appropriate message for the error code     */
/*                      passed to this method. The message is user friendly.  */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Gopi                                                  */
/*  Date Created     :  24.11.2002                                            */
/*  Modifications    :  Pradeep Kadoor on 12.06.2009.                         */                                            
/*                      Error message displayed in trace window instead of    */
/*                      message box.                                          */
/******************************************************************************/
void CCANMonitorApp::vDisplayConfigErrMsgbox(UINT unErrorCode, 
                                             BOOL bOperation)
{    
    CString omStrSuffixMessage(STR_EMPTY);

    if ( bOperation == defCONFIG_FILE_LOADING )
    {
        omStrSuffixMessage = _T(" while loading.\nOperation unsuccessful.");
    }
    else if ( bOperation == defCONFIG_FILE_SAVING )
    {
        omStrSuffixMessage = _T(" while saving.\nOperation unsuccessful.");
    }
    else
    {
        omStrSuffixMessage = _T(".\nOperation unsuccessful.");
    }

    // Get actual error message
    switch(unErrorCode)
    {
    case defCONFIG_FILE_ERROR:
        {
            m_omConfigErr = _T("File error occured");
        }
        break;
    case defCONFIG_FILE_NOT_FOUND:
        {
            m_omConfigErr = _T("Configuration file not found");
        }
        break;
    case defCONFIG_FILE_OPEN_FAILED:
        {
            m_omConfigErr = _T("Unable to open configuration file");
        }
        break;
    case defCONFIG_FILE_READ_FAILED:
        {
            m_omConfigErr = _T("Unable to read configuration file");
        }
        break;
    case defCONFIG_FILE_WRITE_FAILED:
        {
            m_omConfigErr = _T("Unable to write into configuration file");
        }
        break;
    case defCONFIG_FILE_CLOSE_FAILED:
        {
            m_omConfigErr = _T("Unable to close configuration file successfully");
        }
        break;
    case defCONFIG_FILE_INVALID_FILE_EXTN:
        {
            m_omConfigErr = _T("Invalid configuration file extension found");
        }
        break;
    case defCONFIG_PATH_NOT_FOUND:
        {
            m_omConfigErr = _T("Configuration file path not found");
        }
        break;
    case defCONFIG_FILE_ACCESS_DENIED:
        {
            m_omConfigErr = _T("Configuration file access was denied");
        }
        break;
    case defCONFIG_FILE_HANDLE_INVALID:
        {
            m_omConfigErr = _T("Invalid file handle obtained");
        }
        break;
    case defCONFIG_DRIVE_NOT_FOUND:
        {
            m_omConfigErr = _T("Specified drive not found");
        }
        break;
    case defCONFIG_FILE_CORRUPT:
        {
            m_omConfigErr = _T("An attempt\
 to edit the file has been made from outside the application.\n\
Corrupt configuration file found");
        }
        break;
    case defCONFIG_FILE_HDR_CORRUPT:
        {
            m_omConfigErr = _T("Corrupt configuration file header found");
        }
        break;
    default:
        {
            m_omConfigErr = _T("Unknown error encountered");
        }
        break;
    }
    m_omConfigErr += omStrSuffixMessage;
    //UINT unMsgboxType = MB_OK | MB_ICONEXCLAMATION;
	if(m_bFromAutomation==FALSE)
    {
        bWriteIntoTraceWnd(m_omConfigErr.GetBuffer(MAX_PATH));
        //MessageBox(NULL, m_omConfigErr, _T("BUSMASTER"), unMsgboxType);
    }
}
/******************************************************************************/
/*  Function Name    :  bIsConfigurationModified                              */
/*  Input(s)         :                                                        */
/*  Output           :  BOOL                                                  */
/*  Functionality    :  This method returns the value of  m_bIsDirty          */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amarnath S                                            */
/*  Date Created     :  29.10.2002                                            */
/*  Modifications on :                                                        */
/******************************************************************************/
BOOL CCANMonitorApp::bIsConfigurationModified()
{
    return FALSE;//m_oConfigDetails.bIsConfigurationModified();
}

/******************************************************************************/
/*  Function Name    :  vSetConfigurationModified                             */
/*  Input(s)         :                                                        */
/*  Output           :  BOOL                                                  */
/*  Functionality    :  This method sets the value of                         */
/*                      m_bIsConfigurationModified                            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amarnath S                                            */
/*  Date Created     :  02.12.2002                                            */
/*  Modifications on :                                                        */
/******************************************************************************/
void CCANMonitorApp::vSetConfigurationModified(BOOL /*bModified = TRUE*/)
{
    //m_oConfigDetails.vSetConfigurationModified( bModified );
}
BOOL CCANMonitorApp::bGetDefaultSplitterPostion(eCONFIGDETAILS /*eParam*/,
                                                CRect /*omWindowSize*/,
                                                LPVOID* /*pData*/)
{
    return FALSE;
}
/******************************************************************************/
/*  Function Name    :  OnFileNew                                             */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  This is a message handler for ID_FILE_NEW             */
/*                      This function will be called when user selects        */
/*                      File -> Function Editor -> New  menu option.          */
/*                      Creates the MRU menu only once                        */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amarnath S                                            */
/*  Date Created     :  14.12.2002                                            */
/*  Modifications    :  Amitesh Bharti                                        */
/*                      13.03.2003, If function editor is open and CTRL n is  */
/*                      pressed, don't call OnFileNew function. Check for flag*/
/******************************************************************************/
void CCANMonitorApp::OnFileNew()
{
    BOOL bOneChildWndOpen = FALSE;
    bOneChildWndOpen = m_pouFlags->nGetFlagStatus(FUNCEDITOR);
    if(bOneChildWndOpen != TRUE )
    {
        struct _tfinddata_t fileinfo;
        // Find if the current directory has some .c file already created.
        // Check if it has name "NewEdn" if yes, the new file name will be 
        // NewEdx where value of x = n+1;
        char cBuffer[_MAX_PATH];
        CString omStrCFileName;
        CString strFilePath;
        _getcwd( cBuffer, _MAX_PATH );

        BOOL bStop = FALSE;
        UINT unCount = 0;
        while (bStop == FALSE)
        {
            omStrCFileName.Format(_T("%s%d%s"),_T("NewEd"), ++unCount, _T(".c"));
            // Search for the file name and if it is not present, set
            // the flag to TRUE to break the loop.
            if (_tfindfirst( omStrCFileName.GetBuffer(MAX_PATH), &fileinfo) == -1L) 
            {
                strFilePath = cBuffer ;
                strFilePath += _T("\\")+ omStrCFileName ;
                bStop = TRUE;
            }
        }
        CMainFrame* pMainFrame= (CMainFrame*)m_pMainWnd;

        GetICANNodeSim()->FE_OpenFunctioneditorFile(strFilePath, pMainFrame->GetSafeHwnd(),
                    pMainFrame->m_sExFuncPtr[CAN]);
        //CWinApp::OnFileNew();
        // Since creating of the document 
        // loads another menu,
        // the menu created for the MRU
        // would have destroyed. So create 
        // the same.
        // And this should be created only once.
        
       if ( pMainFrame != NULL &&
             m_bIsMRU_CreatedInOpen == FALSE )
        {
            pMainFrame->vCreateMRU_Menus();
            m_bIsMRU_CreatedInOpen = TRUE;
        }
    }
}
/******************************************************************************/
/*  Function Name    :  vDestroyUtilThreads                                   */
/*  Input(s)         :  unMaxWaitTime : Maximum time to wait for event        */
/*                      byThreadCode  : which thread to terminate             */
/*                      byThreadCode = 0x01 : Message Handler Thread          */
/*                      byThreadCode = 0x02 : Key Handler Thread              */
/*                      byThreadCode = 0x04 : Replay Thread                   */
/*                      byThreadCode = 0x08 : Selected Message Thread         */
/*                      byThreadCode = 0x10 : DLL handler Thread.             */
/*                      byThreadCode = 0x20 : Error Handler Thread.           */
/*                      byThreadCode = 0x40 : Transmission Msg block thread.  */
/*  Output           :                                                        */
/*  Functionality    :  This is a function called from CMainFrm class. It is  */
/*                      called from OnDestroy() function. This will wait for  */
/*                      all four thread (Key Handler, Message Handler, Replay */
/*                      and Send Multiple Message) to terminate. If it does   */
/*                      each of this thread does not terminate till           */
/*                      unMaxWaitTime ms then terminate. Before wait,         */
/*                      respective global flag is set to terminate thread     */
/*                      Normal termination of thread is indicated by          */
/*                      event signaled before exit of thread function. Any    */
/*                      dynamically allocated memory inside thread function   */
/*                      is deleted here in case it is not deleted in that func*/
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  30.12.2002                                            */
/*  Modifications    :  Amitesh Bharti                                        */
/*                      25.02.2003, Added thread termination code for Error   */
/*                      handlers                                              */
/*  Modifications    :  Amitesh Bharti                                        */
/*                      30.04.2003, Added thread termination code for DLL     */
/*                      handlers and SendMsg. Corrected some problems.        */
/*  Modifications    :  Amitesh Bharti                                        */
/*                      08.01.2004, Added thread termination code for TX msg  */
/*                      blocks and selected message.                          */
/*  Modifications    :  Raja N                                                */
/*                      10.05.2004, Removed thread termination code for  msg  */
/*                      handlers as it has been done separately               */
/*  Modifications    :  Raja N                                                */
/*                      01.06.2004, Merged thread termination code for  msg   */
/*                      handlers and added check for thread hadle before wait */
/*                      and initialising the handle to NULL after terminate   */
/*  Modifications    :  Amitesh Bharti & Raja N                               */
/*                      22.07.2004, Added code for dll Unload thread killing  */
/*                      and destroying key handler threads                    */
/*  Modifications    :  Raja N                                                */
/*                      01.08.2004, modified code as per code review. Timer   */
/*                      thread is killed after waiting for key thread. This   */
/*                      will reduce TerminateThread calls.                    */
/*  Modifications    :  Raja N                                                */
/*                      16.07.2005, modified code to call replay manager to   */
/*                      kill replay threads                                   */
/*  Modifications	 :  Anish kumar 
                        29.12.05 removed node specific thread code            */
/******************************************************************************/
VOID CCANMonitorApp::vDestroyUtilThreads(UINT unMaxWaitTime, BYTE byThreadCode)
{
    DWORD dwResult = WAIT_ABANDONED;

    BYTE bySelectThread = 0;

    bySelectThread  = static_cast<UCHAR>( byThreadCode & BIT_REPLAY_THREAD );
    // if thread for message replay exists
    if ( bySelectThread != 0 )
    {
        vREP_StopReplayThread();
    }

    bySelectThread  = 
        static_cast<UCHAR>( byThreadCode & BIT_TX_SEL_MSG_THREAD );
    // if thread for tx selected message exists
    if ( bySelectThread != 0 &&
         m_asUtilThread[defTX_SEL_MSG_THREAD].m_hThread != NULL)
    {
        // Set the flag to exit from thread.
        g_bStopSelectedMsgTx = TRUE;
        // Wait for thread to exit.
        dwResult = WaitForSingleObject(m_aomState[defTX_SEL_MSG_THREAD  
                                                        + defOFFSET_TXMSG], 
                                                   unMaxWaitTime);
        // If time out, terminate the thread and delete the memory
        // if it is allocated inside the thread function and not
        // deleted
        if( dwResult == WAIT_TIMEOUT )
        {
            TerminateThread(m_asUtilThread[defTX_SEL_MSG_THREAD].m_hThread, 0);
            // Set the thread handle to NULL
            m_asUtilThread[defTX_SEL_MSG_THREAD].m_hThread = NULL;

            if( m_asUtilThread[defTX_SEL_MSG_THREAD].m_pvThread !=NULL )
            {
                PSTXSELMSGDATA psTxCanMsg = static_cast <PSTXSELMSGDATA> 
                             (m_asUtilThread[defTX_SEL_MSG_THREAD].m_pvThread);
                delete [](psTxCanMsg->m_psTxMsg);
                delete psTxCanMsg;
                psTxCanMsg->m_psTxMsg = NULL;
                psTxCanMsg = NULL;
                m_asUtilThread[defTX_SEL_MSG_THREAD].m_pvThread = NULL;
            }
        }
    }
    
    bySelectThread  = static_cast<UCHAR>( byThreadCode & BIT_MULTI_MSG_THREAD);
    // If message block thread has to be terminated.
    if(bySelectThread != 0 )
    {
        TX_vSetTxStopFlag(TRUE);
        //g_bStopMsgBlockTx = TRUE;
        PSTXMSG psTxMsg = g_psTxMsgBlockList;
        // Store wait return value of timer and key threads seprately
        DWORD   dwTimerThreadStatus = WAIT_ABANDONED;
        DWORD   dwKeyThreadStatus = WAIT_ABANDONED;

        while(psTxMsg != NULL )
        {
            // Set the event for thread waiting for key event to set.
            // This is only if a block is trigger on key and so
            // the m_pomKeyEvent will not be null.
            if( psTxMsg->m_sKeyThreadInfo.m_hThread != NULL)
            {
                // Set the Key Event so that it will terminate by iteself
                psTxMsg->m_omKeyEvent.SetEvent();
            }
            // If timer tharead is active then wait for a while
            if(psTxMsg->m_sTimerThreadInfo.m_hThread != NULL )
            {
                // Wait for thread to exit.
                dwTimerThreadStatus =
                    WaitForSingleObject( psTxMsg->m_omTxBlockTimerEvent,
                                         unMaxWaitTime );
            }
            // If key thread is active then wait for key thread termination
            if(psTxMsg->m_sKeyThreadInfo.m_hThread != NULL )
            {
                // Wait for thread to exit.
                dwKeyThreadStatus =
                    WaitForSingleObject( psTxMsg->m_omTxBlockKeyEvent,
                                         unMaxWaitTime );
            }
            // If time out, terminate the thread and delete the memory
            // if it is allocated inside the thread function and not
            // deleted
            if( dwTimerThreadStatus == WAIT_TIMEOUT &&
                psTxMsg->m_sTimerThreadInfo.m_hThread != NULL)
            {
                TerminateThread(psTxMsg->m_sTimerThreadInfo.m_hThread, 0);
                // Invalidate the handle
                psTxMsg->m_sTimerThreadInfo.m_hThread = NULL;
                // Delete if any memory is allocated for this thread.
                // Currently there are all global data so not required 
                // to be deleted here.
                /***********************************************************/
                // Right now Tx timer thread isn't using any dynamic memory
                // So this is commented
                /*if( psTxMsg->m_sTimerThreadInfo.m_pvThread !=NULL )
                {
                    m_asUtilThread[defTX_SEL_MSG_THREAD].m_pvThread = NULL;
                }*/
                /***********************************************************/
            }
            // If the key handler is not terminated yet then kill the thread
            if( dwKeyThreadStatus == WAIT_TIMEOUT &&
                psTxMsg->m_sKeyThreadInfo.m_hThread != NULL)
            {
                TerminateThread(psTxMsg->m_sKeyThreadInfo.m_hThread, 0);
                // Invalidate the handle
                psTxMsg->m_sKeyThreadInfo.m_hThread = NULL;
                /***********************************************************/
                // Delete if any memory is allocated for this thread.
                // Currently there are all global data so not required 
                // to be deleted here.
                /*if( psTxMsg->m_sThreadInfo1.m_pvThread !=NULL )
                {
                    m_asUtilThread[defTX_SEL_MSG_THREAD].m_pvThread = NULL;
                }*/
                /***********************************************************/
            }
            // Go to the next thread node
            psTxMsg = psTxMsg->m_psNextTxMsgInfo;
        }
    }
}
/******************************************************************************/
/*  Function Name    :  bInitialiseConfiguration                              */
/*  Input(s)         :                                                        */
/*  Output           :  TRUE or FALSE                                         */
/*  Functionality    :  This method will initialise user selection from       */
/*                      a configuration module to respective module.          */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  30.04.2003                                            */
/*  Modification date:  17.06.2003, Amitesh Bharti, updation of toolbar status*/
/*                      to other done irrespective of if the configuration    */
/*                      file has been selected or not.                        */
/*  Modification date:  19.06.2003, Amitesh Bharti, If database file in       */
/*                      configuration file is deleted from the disk, prompt   */
/*                      the user and set the file name as empty in config file*/
/*  Modifications    :  Amitesh Bharti                                        */
/*                      08.01.2004, Interface for getting message block count */
/*                      is changed.                                           */
/*  Modifications    :  Raja N                                                */
/*                      10.03.2004 Modified to include check while loading the*/
/*                      configuration to clear signal watch list and to show  */
/*                      warnning if DLL is loaded                             */
/*  Modifications    :  Raja N                                                */
/*                      05.04.2004 Modified to refer latest signal watch list */
/*                      structure while checking                              */
/*  Modifications    :  Raja N                                                */
/*                      22.07.2004 Modified to create message buffer while    */
/*                      initialising the confguration                         */
/*  Modifications    :  Raja N                                                */
/*                      02.08.2004 Implemented code review changes. The return*/
/*                      value will be set to FALSE in case of buffer creation */
/*                      failure                                               */
/*  Modifications    :  Raja N                                                */
/*                      02.12.2004 Added code to init graph list              */
/*  Modifications    :  Raja N                                                */
/*                      02.12.2004 Added code to new filter list and for      */
/*                      backward compatiblity of filter,log and replay        */
/*  Modifications    :  Pradeep Kadoor on 12.06.2009.                         */
/*                      Load database error message is displayed in           */
/*                      trace window instead of message box.                  */  
/******************************************************************************/
BOOL CCANMonitorApp::bInitialiseConfiguration(BOOL bFromCom)
{
    BOOL bReturn = TRUE;
    CMainFrame* pMainFrame= (CMainFrame*)m_pMainWnd;	
    if(pMainFrame != NULL )
    {
        BOOL bIsDatabaseFoundInConfigFile = FALSE;		
        if(m_pouMsgSignal != NULL)
        {
            m_pouMsgSignal->bDeAllocateMemory(STR_EMPTY);
        }
        else
        {
            m_pouMsgSignal = new CMsgSignal(sg_asDbParams[CAN], m_bFromAutomation);
        }
        if ( m_pouMsgSignal != NULL )
        {			
			//Get the Database names
			CStringArray aomOldDatabases;
			//To keep all the files which are successfully imported
			CStringArray aomNewDatabases;
			aomNewDatabases.RemoveAll();
			m_pouMsgSignal->vGetDataBaseNames(&aomOldDatabases);
			int nFileCount = aomOldDatabases.GetSize();
			if(nFileCount == 0)
			{				
				bIsDatabaseFoundInConfigFile = FALSE;
				// Reset corresponding flag
				m_pouFlags->vSetFlagStatus( SELECTDATABASEFILE, FALSE );
			}
			else
			{
				CString omStrDatabase;
				int nDatabaseNotFound = 0;
				for(int nCount = 0;nCount < nFileCount;nCount++)
				{
					omStrDatabase  = aomOldDatabases.GetAt(nCount);

					if (omStrDatabase.IsEmpty())
					{
						nDatabaseNotFound++;
						aomOldDatabases.RemoveAt(nCount);
						--nCount;
						--nFileCount;
					}
					else
					{
						bIsDatabaseFoundInConfigFile = TRUE;
						// Check if the file really exists
						struct _finddata_t fileinfo;
                        if (_findfirst(omStrDatabase.GetBuffer(MAX_PATH) ,&fileinfo) == -1L)
						{
							CString omStrMsg = "Database File: ";
							omStrMsg += omStrDatabase;
							omStrMsg += " not found!";
							if(bFromCom==FALSE)
								MessageBox(NULL,omStrMsg,"BUSMASTER",MB_OK|MB_ICONERROR);
							// Remove the file name from configuration file.
							nDatabaseNotFound++;
							aomOldDatabases.RemoveAt(nCount);
							--nCount;
							--nFileCount;
						}
						else
						{
							// Reset corresponding flag
							m_pouFlags->vSetFlagStatus( SELECTDATABASEFILE, TRUE );
							m_pouMsgSignal->
								bFillDataStructureFromDatabaseFile(omStrDatabase);
							// Create Unions.h in local directory
							// and fill the file with the structure
							m_pouMsgSignal->bWriteDBHeader(omStrDatabase);

							vPopulateCANIDList();							
							aomNewDatabases.Add(omStrDatabase);
						}
					}
				}
				if(nDatabaseNotFound > 0)
				{
                    BYTE* pbyConfigData = NULL;
                    UINT unSize = 0;

					unSize += (sizeof (UINT) + ((sizeof(TCHAR) *MAX_PATH) * aomNewDatabases.GetSize()));
                    pbyConfigData = new BYTE[unSize];
                    BYTE* pbyTemp = pbyConfigData;
                    UINT nCount = 0;
                    COPY_DATA(pbyTemp, &nCount, sizeof(UINT));

                    for (UINT i = 0; i < nCount; i++)
                    {
                        TCHAR acName[MAX_PATH] = {_T('\0')};
                        CString omDBName = aomNewDatabases.GetAt(i);
                        _tcscpy(acName, omDBName.GetBuffer(MAX_PATH));
                        COPY_DATA(pbyTemp, acName, sizeof(TCHAR) * MAX_PATH);
                    }
                    CConfigData::ouGetConfigDetailsObject().bSetData(pbyTemp, unSize, SectionName[DATABASE_SECTION_ID]);

                    delete[] pbyConfigData;
                    pbyConfigData = NULL;
				}
				if(aomNewDatabases.GetSize()== 0)
				{
					// Reset the flag and prompt user of file not in disk.
					m_pouFlags->vSetFlagStatus( SELECTDATABASEFILE, FALSE );
				}
			}			
		}
		else
		{
			if(bFromCom==FALSE)
            // Display a message and quit the application
            MessageBox(NULL,
                       MSG_MEMORY_CONSTRAINT,
                       "BUSMASTER", 
                       MB_OK|MB_ICONINFORMATION);
            bReturn = FALSE;
        }
        //Finally load the default configuration		
        pMainFrame->nLoadConfigFile(_T(""));		
    }
    return bReturn;
}
/******************************************************************************/
/*  Function Name    :  psReturnMsgBlockPointer                               */
/*  Input(s)         :                                                        */
/*  Output           :  Pointer to  SMSGBLOCKLIST structure                   */
/*  Functionality    :  This method will call psReturnMsgBlockPointer()       */
/*                      a member function of CConfigDetails class.            */
/*  Member of        :  CCANMonitorApp                                        */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  08.01.2004                                            */
/*  Modification date:                                                        */
/******************************************************************************/
PSMSGBLOCKLIST CCANMonitorApp::psReturnMsgBlockPointer()
{
    return NULL;//m_oConfigDetails.psReturnMsgBlockPointer();
}

/*******************************************************************************
  Function Name  : bGetDefaultValue
  Input(s)       : eParam - Window Identity
                   sPosition - Reference to Window Placement Structure
  Output         : -
  Functionality  : This function will call Config Details class member to get
                   default window size and position.
  Member of      : CCAN_Monitor
  Author(s)      : Raja N
  Date Created   : 29.4.2005
  Modifications  : 
*******************************************************************************/
BOOL CCANMonitorApp::bGetDefaultValue(eCONFIGDETAILS /*eParam*/,
                                      WINDOWPLACEMENT& /*sPosition*/)
{
    return FALSE;//m_oConfigDetails.bGetDefaultValue( eParam, sPosition );
}

/*******************************************************************************
  Function Name  : bWriteIntoTraceWnd
  Input(s)       : omText - Text to be displayed in trace window
  Output         : TRUE - Success, FALSE - Failure
  Functionality  : This function will write the text into trace window.
                   Since this function is asynchronous, caller should not immediately 
                   deallocate the omText. 
  Member of      : CCAN_Monitor
  Author(s)      : Pradeep Kadoor
  Date Created   : 13.06.2009
  Modifications  : 
*******************************************************************************/
BOOL CCANMonitorApp::bWriteIntoTraceWnd(char* omText)
{
    BOOL bResult = FALSE;
    CMainFrame* pMainFrame= (CMainFrame*)m_pMainWnd;
    if (pMainFrame != NULL)
    {   
        pMainFrame->SendMessage(IDM_TRACE_WND);                
        if (pMainFrame->m_podUIThread != NULL)
        {   
            pMainFrame->m_podUIThread->PostThreadMessage(WM_WRITE_TO_TRACE, NULL, (LPARAM)omText);
            bResult = TRUE;
        }
    }
    return bResult;
}

INT CCANMonitorApp::COM_nSaveConfiguration(CString omStrCfgFilename)
{
    vSetFileStorageInfo(omStrCfgFilename);
    CConfigData::ouGetConfigDetailsObject().vSaveConfigFile();
    return defCONFIG_FILE_SUCCESS;
}
