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
 * \file      ChangeRegisters_ES581.cpp
 * \brief     This file contain definition of all function of 
 * \author    Pradeep Kadoor
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * This file contain definition of all function of 
 */
// For all standard header file include
#include "CongDlg_stdafx.h"
#include "ContrConfigPeakUsbDefs.h"
#include "ConfigDlg_Resource.h"
#include "ChangeRegisters_ES581.h"
#include "API_Dialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/******************************************************************************/
/*  Function Name    :  CChangeRegisters_ES581                                      */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Constructor is called when user create an object of   */
/*                      this class. Initialisation of all data members        */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :  Raja N on 13.09.2004, Added init of member variables  */
/*  Modifications    :  Raja N on 14.03.2005, Added init of member variable   */
/*                      m_pControllerDetails                                  */
/******************************************************************************/
CChangeRegisters_ES581::CChangeRegisters_ES581(CWnd* pParent /*=NULL*/, PSCONTROLER_DETAILS psControllerDetails, UINT nHardwareCount)
    : CDialog(CChangeRegisters_ES581::IDD, pParent)
    //, m_nClock(32)
    , m_omStrPropDelay(_T(""))
    , m_omStrSJW(_T(""))
{
    //{{AFX_DATA_INIT(CChangeRegisters_ES581)
    //m_omStrEditBTR0 = _T("");
    //m_omStrEditBTR1 = _T("");
    m_omStrEditCNF1 = _T("");
    m_omStrEditCNF2 = _T("");
    m_omStrEditCNF3 = _T("");
    m_omStrComboSampling = _T("");
    m_omStrEditBaudRate = _T("");
    m_omStrComboClock = _T("32");
    m_omStrEditWarningLimit = _T("");
    //}}AFX_DATA_INIT
    m_unCombClock      = 32;
    m_bDialogCancel    = FALSE;
    memset(&m_sAccFilterInfo, 0, sizeof(m_sAccFilterInfo));
    m_ucWarningLimit    = defWARNING_LIMIT_MIN;
    m_ucControllerMode  = defCONTROLLER_MODE;
    m_usBTR0BTR1 = defDEFAUT_BAUDRATE;
    // Update controller data
    m_pControllerDetails = psControllerDetails;
    m_nPropDelay = 0;
    m_nSJWCurr = 0;
    m_bOption = NO_DEF;
    m_nNoHardware = nHardwareCount;
    m_pControllerDetails = psControllerDetails;
    m_nDataConfirmStatus = WARNING_NOTCONFIRMED;
}

/******************************************************************************/
/*  Function Name    :  DoDataExchange                                        */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Called by the framework to exchange and validate      */
/*                         dialog data                                        */
/*                                                                            */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :  Raja N on 14.03.2005                                  */
/*                      Added list variable to include channel information    */
/******************************************************************************/
void CChangeRegisters_ES581::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChangeRegisters_ES581)
    DDX_Control(pDX, IDC_LIST_CHANNELS, m_omChannelList);
    DDX_Control(pDX, IDC_EDIT_WARNING_LIMIT, m_omEditWarningLimit);
    DDX_Control(pDX, IDC_COMB_SAMPLING, m_omCombSampling);
    DDX_Control(pDX, IDC_LSTC_BTR_LIST, m_omListCtrlBitTime);
    DDX_Control(pDX, IDC_EDIT_BAUD_RATE, m_omEditBaudRate);
    DDX_CBString(pDX, IDC_COMB_SAMPLING, m_omStrComboSampling);
    DDV_MaxChars(pDX, m_omStrComboSampling, 1);
    DDX_Text(pDX, IDC_EDIT_BAUD_RATE, m_omStrEditBaudRate);
    DDX_CBString(pDX, IDC_COMB_CLOCK, m_omStrComboClock);
    DDX_Text(pDX, IDC_EDIT_WARNING_LIMIT, m_omStrEditWarningLimit);
    DDX_Control(pDX, IDC_COMB_PROPDELAY, m_omCtrlPropDelay);
    DDX_Text(pDX, IDC_COMB_SJW, m_omStrSJW);
    DDX_Text(pDX,IDC_COMB_PROPDELAY, m_omStrPropDelay);
    DDX_Control(pDX, IDC_COMB_SJW, m_omCtrlSJW);
    DDX_Control(pDX, IDC_COMB_CLOCK, m_omCtrlClock);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeRegisters_ES581, CDialog)
    //{{AFX_MSG_MAP(CChangeRegisters_ES581)
    ON_EN_KILLFOCUS(IDC_EDIT_BAUD_RATE, OnKillfocusEditBaudRate)
    ON_CBN_SELCHANGE(IDC_COMB_SAMPLING, OnSelchangeCombSampling)
    ON_EN_SETFOCUS(IDC_EDIT_BAUD_RATE, OnSetfocusEditBaudRate)
    ON_BN_CLICKED(IDC_ButtonOK, OnClickedOK)
    ON_CBN_SETFOCUS(IDC_COMB_SAMPLING, OnSetfocusCombSampling)
    ON_WM_HELPINFO()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTC_BTR_LIST, OnItemchangedLstcBtrList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_CHANNELS, OnClickListChannels)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CHANNELS, OnItemchangedListChannels)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_CHANNELS, OnDblclkListChannels)
    ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickLstcBtrList)
    ON_CBN_SELCHANGE(IDC_COMB_SJW, OnCbnSelchangeCombSjw)
    ON_CBN_SELCHANGE(IDC_COMB_PROPDELAY, OnCbnSelchangeCombPropdelay)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/
/*  Function Name    :  OnInitDialog                                          */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  This function is called by the framework in response  */
/*                      to the WM_INITDIALOG message. This message is sent to */
/*                      the dialog box during DoModal calls,which occur       */
/*                      immediately before the dialog box is displayed.       */
/*                      All controls of dialog are initialised in this func.  */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */
/*                      Review comment incorporated                           */
/*                      13.11.2002, Gopi                                      */
/*                      Changed to usage of configuration file to read the    */
/*                      values.                                               */
/*                      18.12.2002, Edit box will not take negative value     */
/*                      28.03.2003, Changes for acceptance filter and warning */
/*                      limit.                                                */
/*                      23.10.2003, Moved updating fields on selection of     */
/*                      items in listctrl to OnItemchangedLstcBtrList function*/
/*  Modifications    :  31.08.2004, Raja N                                    */
/*                      Changes for USB devices specific code                 */
/*                      Added code to disable warning limit and to set default*/
/*                      value 96                                              */
/*  Modifications    :  14.03.2005, Raja N                                    */
/*                      Changes for Multi channel support to configure        */
/*                      multiple controllers                                  */
/*  Modifications    :  14.03.2005, Raja N                                    */
/*                      Implemented code review comments                      */
/*  Modifications    :  09.05.2008, Pradeep Kadoor                            */
/*                      Modification done for getting values of PropDelay and */
/*                      SJW from configuration file and display list          */ 
/*                      accordingly                                           */
/*  Modifications    :  09.05.2008, Pradeep Kadoor                            */
/*                      Modifications for setting two newly added combo box   */
/******************************************************************************/

BOOL CChangeRegisters_ES581::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    TCHAR acColumnName[defNUMBER_OF_COLUMNS_ES581][50] = { 
                                 defSTR_CNF1_COL_HEADING,
                                 defSTR_CNF2_COL_HEADING,
                                 defSTR_CNF3_COL_HEADING,
                                 defSTR_SAMPLE_POINT_COL_HEADING,
                                 defSTR_NBT_COL_HEADING,
                                 defSTR_BRP_COL_HEADING };
    CString omStrClock          = defCLOCK;
    CString omStrBaudRate       = _T("");
    CString omStrAcceptanceMask = _T("");
    CString omStrAcceptanceCode = _T("");
    CString omStrBrp            = _T("");
    CString omStrBtr0           = _T("");
    CString omStrBtr1           = _T("");

    INT nColumnSize             = 0;
    INT nTotalColunmSize        = 0;
    INT nTotalStrLengthPixel    = 0;
    RECT rListCtrlRect          ;

    // Init Channel List box

    // Create Image List for Channel List Control
    m_omChannelImageList.Create( defCHANNEL_ICON_SIZE,
                                 defCHANNEL_ICON_SIZE,
                                 ILC_COLOR24,
                                 defCHANNEL_LIST_INIT_SIZE,
                                 defCHANNEL_LIST_GROW_SIZE);
    // Load Channel Icon
    CWinApp* pWinApp = (CWinApp*)this;
    m_omChannelImageList.Add(pWinApp->LoadIcon(IDR_PGM_EDTYPE));
    // Assign the image list to the control
    m_omChannelList.SetImageList(&m_omChannelImageList, LVSIL_NORMAL);
    // Insert empty column
    m_omChannelList.InsertColumn( 0, _T(""));
    // Insert all channel information
    // Insert only for available channel information
    int nAvailableHardware = m_nNoHardware;//g_podHardwareInterface->nGetNoOfHardware();
    for (int nChannel = 0 ;
         nChannel < nAvailableHardware;
         nChannel++)
    {
        CString omStrChannel(_T(""));
        // Create Channel String
        omStrChannel.Format( defSTR_CHANNEL_NAME_FORMAT,
                        defSTR_CHANNEL_NAME,
                        nChannel + 1);
        // Insert channel item
        m_omChannelList.InsertItem( nChannel, omStrChannel);
    }

    // Set the selected item index to zero
    m_nLastSelection = 0;

    m_omEditBaudRate.vSetBase( BASE_DECIMAL);
    m_omEditBaudRate.vSetSigned(FALSE);
    m_omEditBaudRate.vAcceptFloatingNum( TRUE);
   
    m_omEditWarningLimit.vSetBase( BASE_DECIMAL);
    m_omEditWarningLimit.vSetSigned(FALSE);
    m_omEditWarningLimit.vAcceptFloatingNum( FALSE);

    //Calculate the total size of all column header
    m_omListCtrlBitTime.GetWindowRect( &rListCtrlRect);
    nTotalColunmSize     = rListCtrlRect.right - rListCtrlRect.left;
    nTotalStrLengthPixel = 0;

    for (INT j=0; j<defNUMBER_OF_COLUMNS;j++)
    {
         nTotalStrLengthPixel +=
             m_omListCtrlBitTime.GetStringWidth(acColumnName[j]);
    }
    //Insert each column name after calculating the size for the same.
    for (INT i=0; i<defNUMBER_OF_COLUMNS;i++)
    {
         nColumnSize  = m_omListCtrlBitTime.GetStringWidth(acColumnName[i]) ;
         nColumnSize +=
             (nTotalColunmSize-nTotalStrLengthPixel)/defNUMBER_OF_COLUMNS;
         m_omListCtrlBitTime.InsertColumn(i,acColumnName[i],
                                        LVCFMT_CENTER, nColumnSize);
    }

    //Set extended style to show selection for all subitems   
    m_omListCtrlBitTime.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    m_omCtrlPropDelay.InsertString(0,_T("ALL"));
    m_omCtrlSJW.InsertString(0,_T("ALL"));
    m_omCtrlClock.InsertString(0,defCLOCK);
    // Add an entry in each of the two combo boxes FindStringExact
    int nIndex = m_omCtrlPropDelay.FindStringExact(-1, 
                           m_pControllerDetails->m_omStrPropagationDelay);
    m_omStrPropDelay = m_pControllerDetails->m_omStrPropagationDelay;
    m_omStrSJW = m_pControllerDetails->m_omStrSjw;
    //UpdateData();
   if (CB_ERR != nIndex)
    {
        m_omCtrlPropDelay.SetCurSel (nIndex);
    }
    else
    {
        m_omCtrlPropDelay.SetCurSel (0);
    }
    nIndex = m_omCtrlSJW.FindStringExact (-1, m_pControllerDetails->m_omStrSjw);
    if (CB_ERR != nIndex)
    {
        m_omCtrlSJW.SetCurSel (nIndex);
    }
    else
    {
        m_omCtrlSJW.SetCurSel (0);
    }
    // List values having prop delay
    int nPropDelay = nGetValueFromComboBox(m_omCtrlPropDelay);
    if (nPropDelay != m_nPropDelay)
    {
        m_nPropDelay = nPropDelay;
        bDecideCalculatingOption();
        vChangeListBoxValues(-1);
    }
    // List Values having SJW
    int nSJWCurr = nGetValueFromComboBox(m_omCtrlSJW);
    if (nSJWCurr != m_nSJWCurr)
    {
        m_nSJWCurr = nSJWCurr;
        bDecideCalculatingOption();
        vChangeListBoxValues(-1);
    }

    m_omEditWarningLimit.SetReadOnly(TRUE);
    CWnd* pWndFilter = GetDlgItem(IDC_CBTN_ACCEPTANCE);
    if (pWndFilter != NULL)
    {
        pWndFilter->EnableWindow(FALSE);
    }
    //Initialise the index for number of items in list box before passing it is
    //function to calculate the same.
    
    // Set the Focus to the First Item
    m_omChannelList.SetItemState( 0,
                                  LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
/******************************************************************************/
/*  Function Name    :  OnCancel                                              */    
/*                                                                            */    
/*  Input(s)         :                                                        */    
/*  Output           :                                                        */    
/*  Functionality    :  message handlers on CANCEL request                    */    
/*                                                                            */    
/*  Member of        :  CChangeRegisters_ES581                                      */    
/*  Friend of        :      -                                                 */    
/*                                                                            */    
/*  Author(s)        :  Amitesh Bharti                                        */    
/*  Date Created     :  19.02.2002                                            */    
/*  Modifications    :                                                        */    
/*                                                                            */    
/******************************************************************************/
void CChangeRegisters_ES581::OnCancel() 
{
    // Flag to be checked while validating the edit control input on kill focus
    m_bDialogCancel = TRUE;
    m_nDataConfirmStatus = INFO_RETAINED_CONFDATA;
    CDialog::OnCancel();
}
/******************************************************************************/
/*  Function Name    :  OnOK                                                  */    
/*                                                                            */    
/*  Input(s)         :                                                        */    
/*  Output           :                                                        */    
/*  Functionality    :  Message handlers on Enter Button ( Default OK button) */    
/*                      Every press of enter key, focus is to next control    */    
/*  Member of        :  CChangeRegisters_ES581                                      */    
/*  Friend of        :      -                                                 */    
/*                                                                            */    
/*  Author(s)        :  Amitesh Bharti                                        */    
/*  Date Created     :  15.02.2002                                            */    
/*  Modifications    :                                                        */    
/*                                                                            */    
/******************************************************************************/
void CChangeRegisters_ES581::OnOK() 
{

 // Dummy virtual function to avoid closing the dialog when ENTER key is 
//  pressed. Instead next conrol gets focus in tab order
    NextDlgCtrl();
}

/*****************************************************************************/
/*  Function Name    : omGetFormattedRegVal                                  */
/*                                                                           */
/*  Input(s)         :  Register value in UCHAR                              */
/*  Output           :  CString                                              */
/*  Functionality    :  Formats the input register value as 0xYY             */
/*  Member of        :  CChangeRegisters_ES581                                     */
/*  Friend of        :      -                                                */
/*                                                                           */
/*  Author(s)        :  Ratnadip Choudhury                                   */
/*  Date Created     :  19.04.2008                                           */
/*****************************************************************************/
CString CChangeRegisters_ES581::omGetFormattedRegVal(UCHAR ucRegVal)
{
    CString omStr = _T("");
    omStr.Format(TEXT("0x%X"), ucRegVal);
    // Insert one zero to format the sigle digit value to 0x05 etc.
    if (omStr.GetLength() == 3)
    {
        omStr.Insert(2, '0');
    }
    return omStr;
}

/******************************************************************************/
/*  Function Name    : vDisplayListBox                                        */
/*                                                                            */
/*  Input(s)         :  Total number of item and item last on fucus           */
/*  Output           :  -                                                     */
/*  Functionality    :  Insert columns of each row in the list control        */
/*                      Calls function vSelSetFocusItemList to set focus      */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  19.02.2002                                            */
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */
/*                      Review comment incorporated                           */
/*  Modifications    :  Raja N on 14.03.2005, Added code to clear list box    */
/*                      items before inserting the new items as this function */
/*                      will be called multiple times                         */
/*  Modifications    :  19.04.2008, Ratnadip Choudhury                        */
/*                      Added code for two more columns; the helper function  */
/*                      omGetFormattedRegVal is being used to format register */
/*                      value. Also, proper indentation / formatting is done  */
/******************************************************************************/
void CChangeRegisters_ES581:: vDisplayListBox(INT nEntries, INT nItemFocus)
{
    CString cStrText    = _T("");

    // Clear list box first
    m_omListCtrlBitTime.DeleteAllItems();

    // Get the total number of Columns in the List View Header
    INT nColumnCount = m_omListCtrlBitTime.GetHeaderCtrl()->GetItemCount();

    // Insert items and subitems after Formating the strings in the list view 
    // control.
    for (INT i = 0; i < nEntries; i++)
    {
        // First column contains value of CNF1 register
        cStrText = omGetFormattedRegVal(m_asColListCtrl[i].uCNFReg1.ucCNF1);
        // Add the first column entry of the current row
        m_omListCtrlBitTime.InsertItem(LVIF_TEXT | LVIF_STATE, i, cStrText,
                            (i == 0) ? LVIS_SELECTED : 0, LVIS_SELECTED, 0, 0);

        // Set item data for sorting on column value basis
        m_omListCtrlBitTime.SetItemData(i, (LPARAM) &(m_asColListCtrl[i]));
       
        // Format and Initialize the text of the subitems.
        for (INT j = 1; j < nColumnCount; j++)
        {
            // Format the text of the subitems.
            if (1 == j) // Second column: CNF2 register value
            {
                cStrText = omGetFormattedRegVal(m_asColListCtrl[i].uCNFReg2.ucCNF2);
            }
            else if (2 == j) // Third column: CNF3 register value
            {
                cStrText = omGetFormattedRegVal(m_asColListCtrl[i].uCNFReg3.ucCNF3);
            }
            else if (3 == j) // Fourth column: sampling value
            {
                cStrText.Format(TEXT("%d%%"), m_asColListCtrl[i].sBRPNBTSampNSJW.usSampling);
            }
            else if (4 == j) // Fifth column: value of NBT
            {
                cStrText.Format(TEXT("%d"), m_asColListCtrl[i].sBRPNBTSampNSJW.usNBT);
                
            }
            else if (5 == j) // Sixth column: BRP value
            {
                cStrText.Format(TEXT("%d"), m_asColListCtrl[i].sBRPNBTSampNSJW.usBRP);
            }

            // Initialize the text of the subitems.
            if (m_omListCtrlBitTime.SetItemText(i, j, cStrText) == 0)
            {
                    AfxMessageBox(defERRORMSG_INSERT);
            }
        }
    }

    //  Set the focus on item
    vSelSetFocusItemList(nEntries, nItemFocus);
}

/******************************************************************************/
/*  Function Name    :  OnKillfocusEditBaudRate                               */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Validate the buadrate on kill focus of this edit      */
/*                      control                                               */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  19.02.2002                                            */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  22.03.2002, Validation for hexadecimal in CRadixEdit  */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  27.05.2002, Validation for zero value entered         */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  29.05.2002,Don't validate if CANCEL button is clicked */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  12.12.2002, negative value of baudrate will not be    */
/*                      acceptect.Also the range is fixed to 1000kbps         */
/*  Modification By  :  Raja N                                                */
/*  Modification on  :  14.03.2005, Added code to update list control for the */
/*                      values updated in the baud rate edit control          */
/******************************************************************************/
void CChangeRegisters_ES581::OnKillfocusEditBaudRate() 
{
    CString omStrBaudRate   =_T("");
    CString omStrValid      =_T("");
    INT     nLength         = 0;
    DOUBLE  dBaudRate       = 0;

    m_omEditBaudRate.GetWindowText(omStrBaudRate);    
    nLength             = omStrBaudRate.GetLength();
    
    CButton* pomButtonCancel = (CButton*) GetDlgItem(IDCANCEL);
    // To get the state of CANCEL button. A non zero value if the button is 
    // clicked.
    UINT unButtonState       = pomButtonCancel->GetState();
    // Validate only if next command is not ESC Button
    if (m_bDialogCancel != TRUE )
    {
        // Don't validate if CANCEL button is clicked.
        if (unButtonState ==0)
        {
            // Validate for empty string and if zero value is entered.
            dBaudRate           = (FLOAT)_tstof(omStrBaudRate);
            if (nLength == 0 || dBaudRate <= 0 || dBaudRate > 1000.0)
            {   
                m_omEditBaudRate.SetWindowText(m_omStrEditBaudRate);
                AfxMessageBox(defVALIDATION_MSG_BAUD_RATE);
                m_omEditBaudRate.SetFocus();
                m_omEditBaudRate.SetSel(0, -1,FALSE);
            }
            else
            {
                
                m_dEditBaudRate     = (FLOAT)_tstof(m_omStrEditBaudRate);

                // Call if string is valid to validate the baud rate value and 
                // suggest  a next valid baud rate
                //Validate only if previous value in edit control is not the
                //  same as the one changed by user
                if (m_dEditBaudRate != dBaudRate && dBaudRate>0 
                    && m_dEditBaudRate > 0 )
                {
                    vValidateBaudRate();
                    // Update List items only it is from edit box
                    vChangeListBoxValues(-1); 
                    CButton* pomButtonoK = (CButton*) GetDlgItem(IDC_ButtonOK);
                    CButton* pomFocusWnd     = (CButton*)GetFocus();

                    if (pomButtonoK ==pomFocusWnd)
                    {
                        // Close the dialog if the user 
                        // has pressed OK button
                        OnClickedOK();
                    }
                }
            }
        }
    }
    else
    {
      m_omEditBaudRate.SetWindowText(m_omStrEditBaudRate);
    }
}

/******************************************************************************/
/*  Function Name    :  OnSelchangeCombSampling                               */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Change the content of list control on change in       */
/*                      selection of number of sampling combo box.            */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  19.02.2002                                            */
/*  Modifications    :                                                        */
/******************************************************************************/
void CChangeRegisters_ES581::OnSelchangeCombSampling()
{ 
    INT nGetValue               = 0;
    CString omStrComboEditItem  =_T("");

    nGetValue =  m_omCombSampling.GetCurSel();
    if (nGetValue != CB_ERR)
    {
        m_omCombSampling.GetLBText(nGetValue, omStrComboEditItem);
    }
    if (m_omStrComboSampling != omStrComboEditItem)
    {
        vChangeListBoxValues(-1);
        m_omStrComboSampling = omStrComboEditItem;
    }
}

/******************************************************************************/
/*  Function Name    :  vChangeListBoxValues                                  */
/*                                                                            */
/*  Input(s)         :  Flag to indicate from where this function is called   */
/*  Output           :                                                        */
/*  Functionality    :  Change the content of list control on change in       */
/*                      selection of number of sampling or clock, or BTR0     */
/*                      or BTR1 or baudrate combo box.                        */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  19.02.2002                                            */
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */
/*                      Review comment incorporated                           */
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  29.05.2002, Update the data member for clock freq.    */
/*  Modification     :  19.04.2008, Ratnadip Choudhury                        */
/*                      Code optimization done.                               */
/******************************************************************************/
void CChangeRegisters_ES581::vChangeListBoxValues(INT nflag) 
{
    UINT unIndex = 0;

    // Call function to calculate the list of BTR0, BTR1, SJW,NBT and Sampling.
    if (nListBoxValues(m_asColListCtrl, m_dEditBaudRate, (WORD)m_unCombClock, 
                &unIndex, nGetValueFromComboBox(m_omCombSampling)) != -1)
    {
        // Remove all the items in the list box.
        m_omListCtrlBitTime.DeleteAllItems();

        // Display all the new items in the list box
        vDisplayListBox(unIndex, nflag);
    }
    else
    {
        vValidateBaudRate();
    }
    
}
/******************************************************************************/
/*  Function Name    :  OnSetfocusEditBaudRate                                */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Called when focus is set on baudrate edit box control */
/*                      Update all data members associated with Dialog        */
/*                      control.                                              */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :                                                        */
/*                                                                            */
/******************************************************************************/
void CChangeRegisters_ES581::OnSetfocusEditBaudRate() 
{
    // To update the data members before editing it and use it in kill focus
    UpdateData(TRUE);

}

/******************************************************************************/
/*  Function Name    :  vSelSetFocusItemList                                  */
/*                                                                            */
/*  Input(s)         : Total number of item in list control and item number   */
/*                     for selection from list control                        */ 
/*  Output           :                                                        */
/*  Functionality    :  Called when focus is set on baudrate edit box control */
/*                      Update all data members associated with Dialog        */
/*                      control.                                              */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :                                                        */
/*                                                                            */
/******************************************************************************/
void CChangeRegisters_ES581::vSelSetFocusItemList(INT nItemCount,INT nItem)
{
  LVITEM sItem;
  // If there is no defualt or last selected item, selection will be at
  // item number middle of the total item numbers.
  if (nItem ==-1 || nItem>=nItemCount)
  {
      nItem = 0;
  }
  sItem.mask      = LVIF_STATE;
  sItem.iItem     = nItem;
  sItem.state     = LVIS_FOCUSED|LVIS_SELECTED;
  sItem.stateMask = LVIS_SELECTED |LVIS_FOCUSED;
  sItem.iSubItem  = 0;

  m_omListCtrlBitTime.SetItem(&sItem);
  m_omListCtrlBitTime.EnsureVisible(nItem, FALSE);
}

/******************************************************************************/
/*  Function Name    :  vValidateBaudRate                                     */    
/*                                                                            */    
/*  Input(s)         :                                                        */    
/*  Output           :                                                        */    
/*  Functionality    :  This function will validate the user input value of   */    
/*                      baud rate. A valid baud rate will be calculated       */    
/*  Member of        :  CChangeRegisters_ES581                                      */    
/*  Friend of        :      -                                                 */    
/*                                                                            */    
/*  Author(s)        :  Amitesh Bharti                                        */    
/*  Date Created     :  18.02.2002                                            */    
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */    
/*                      Incorporated review comments                          */    
/*  Modification By  :  Amitesh Bharti                                        */
/*  Modification on  :  22.03.2002, If user changes clock freq. and select no */
/*                      for changing valid baudrate, change the clock freq. to*/
/*                      previous value.                                       */
/*  Modification By  :  Raja N                                                */
/*  Modification on  :  14.03.2005, Removed message box for asking the user to*/
/*                      change the baudrate to nearest possible value. Now it */
/*                      will automatically change the values                  */
/******************************************************************************/
void CChangeRegisters_ES581::vValidateBaudRate()
{
    CString omStrBaudRate       = _T("");
    CString omStrPrvBaudRate    = _T("");
    CString omStrClockFreq      = _T("");
    DOUBLE  dBaudRate           = 0;
    FLOAT   fNbt                = 0;
    //UINT    unClockFreq         = 0;
    //UINT    unClockPrevValue    = 0 ;
    UINT    unNbt               = 0;
    UINT    unProductNbtNBrp    = 0;
    DOUBLE  dProductNbtNBrp     = 0; 
    INT     i                   = 0;
    INT     nFlag               = 0;
    CString omStrMessage        = _T("");
   
   
    m_omEditBaudRate.GetWindowText(omStrBaudRate);
    dBaudRate           = (FLOAT)_tstof(omStrBaudRate);
    m_dEditBaudRate     = (FLOAT)_tstof(m_omStrEditBaudRate);

    //m_omCombClock.GetWindowText(omStrClockFreq);
    //unClockFreq          = _tstoi(omStrClockFreq);

    dProductNbtNBrp     = (DOUBLE)(m_unCombClock/dBaudRate)/2.0 * 
                                (defFACT_FREQUENCY / defFACT_BAUD_RATE);
    unProductNbtNBrp    = (UINT)(dProductNbtNBrp + 0.5);

    if ((fabs((dProductNbtNBrp - unProductNbtNBrp)) > defVALID_DECIMAL_VALUE) 
          ||(unProductNbtNBrp > (defMAX_NBT * defMAX_BRP)) 
          || (unProductNbtNBrp < defMIN_NBT))
    {
        unProductNbtNBrp =defmcROUND5(dProductNbtNBrp);
        nFlag = defRESET;

        while (nFlag == defRESET)
        {
            i = 1;
            unNbt = unProductNbtNBrp / i;
            fNbt  = (FLOAT)unProductNbtNBrp / i;
            
            while ((unNbt >= 1) && (i <= defMAX_BRP) && (nFlag == defRESET))
            {
                if ((unNbt == fNbt) && (unNbt >= defMIN_NBT)
                                     && (unNbt <=defMAX_NBT))
                {
                    nFlag =defSET;
                }
                else
                {
                    i++;
                    unNbt    = unProductNbtNBrp / i;
                    fNbt     = (FLOAT)unProductNbtNBrp / i;
                }
            } //end while( unNbt >=1 && i<=MAX_BRP)
            
            if ((nFlag == defRESET) && (unProductNbtNBrp < (defMIN_NBT *defMIN_BRP)))
            {
                unProductNbtNBrp = defMIN_NBT * defMIN_BRP;
            }
            else if ((unProductNbtNBrp > ( defMAX_NBT * defMAX_BRP)) 
                                            && (nFlag == defRESET))
            {
                unProductNbtNBrp = defMAX_NBT*defMAX_BRP;
            }
            else if (nFlag == defRESET)
            {
                unProductNbtNBrp++;
            }
        }//end while(nFlag==RESET)
        dBaudRate = (DOUBLE)((m_unCombClock/2.0)* 
                    ( defFACT_FREQUENCY / defFACT_BAUD_RATE))/unProductNbtNBrp;

        FLOAT  fTempBaudRate;
        fTempBaudRate = (FLOAT)((INT)(dBaudRate * 100000));
        fTempBaudRate = fTempBaudRate/100000;
        omStrBaudRate.Format(_T("%.4f"),fTempBaudRate);   
        
        omStrMessage.Format(defBAUD_RATE_MESSAGE,omStrBaudRate);
        omStrPrvBaudRate = m_omStrEditBaudRate;
        //unClockPrevValue = m_unCombClock;
        
        // set the baudrate
        m_omEditBaudRate.SetWindowText(omStrBaudRate);
        }// End if
    // Change the list of BTR0, BTR1, SJW, NBT and sampling if user selected YES
    m_dEditBaudRate     = dBaudRate;
    m_omStrEditBaudRate = omStrBaudRate;
    //m_unCombClock       = unClockFreq;
}
/******************************************************************************/
/*  Function Name    :  OnClickedOK                                           */    
/*                                                                            */    
/*  Input(s)         :  User Selects OK Button                                */    
/*  Output           :  All user input field entry is written into            */
/*                      Registry/.ini file                                    */    
/*  Functionality    :  Message handlers on OK Button.To Remove control       */
/*                      to close when Enter Button is pressed                 */
/*                                                                            */    
/*  Member of        :  CChangeRegisters_ES581                                      */    
/*  Friend of        :      -                                                 */    
/*                                                                            */    
/*  Author(s)        :  Amitesh Bharti                                        */    
/*  Date Created     :  18.02.2002                                            */    
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */
/*                      Incorporated review comments                          */
/*                      13.11.2002, Gopi                                      */
/*                      Changed to usage of configuration file                */
/*                      14.12.2002, put the tool in same state after baudrate */
/*                      is initialise. i.e. connected or disconnected state   */
/*                      28.03.2003, Changes for acceptance filter and warning */
/*                      limit.                                                */
/*  Modifications    :  Raja N on 07.09.2004. Modified code to refer HI layer */
/*                      for Hardware related functions. Removed code to refer */
/*                      CChangeRegisters_ES581 static functions as they are moved in*/
/*                      to HI layer                                           */
/*  Modifications    :  Raja N on 09.03.2005                                  */
/*                   :  Added code to support multiple contoller information  */
/*                      in the configuration module                           */
/******************************************************************************/
void CChangeRegisters_ES581::OnClickedOK() 
{
    // Update modified data
    UpdateData( TRUE);
    // Validate Baud rate and find the nearest match
    vValidateBaudRate();
    // Update data members associated with the controller
    if (bUpdateControllerDataMembers() == FALSE)
    {
        return;
    }
    // Save the changes in to the local data structure
    vUpdateControllerDetails();    
    // Close the dialog
    m_nDataConfirmStatus = INFO_INIT_DATA_CONFIRMED;
    CDialog::OnOK();
}

/******************************************************************************/
/*  Function Name    :  OnSetfocusCombSampling                                */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  Called when focus is set on Number of sampling        */
/*                      combo box control. Updates all data members           */
/*                      associated with Dialog control.                       */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  15.02.2002                                            */
/*  Modifications    :                                                        */
/*                                                                            */
/******************************************************************************/
void CChangeRegisters_ES581::OnSetfocusCombSampling() 
{
    UpdateData(TRUE);
}

/******************************************************************************/
/*  Function Name    :  dCalculateBaudRateFromBTRs                            */
/*                                                                            */
/*  Input(s)         :  CNF1, CNF2 and CNF3                                   */
/*  Output           :  baudrate in kbps                                      */
/*  Functionality    :  This function is called to calcualte the baudrate for */
/*                      the provided CNF1, CNF2 and CNF3 values.              */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  19.02.2002                                            */
/*  Modifications    :  25.02.2002, Amitesh Bharti                            */
/*                      Incorporated review comments                          */
/*  Modifications    :  Pradeep Kadoor, 10.04.2008                            */
/*                      Changed the prototype adequate for ES581              */
/******************************************************************************/
DOUBLE CChangeRegisters_ES581::dCalculateBaudRateFromBTRs(CString omStrCNF1, CString omStrCNF2,
                                                    CString omStrCNF3)
{
    uCNF1 uCNF1val; 
    uCNF2 uCNF2val;
    uCNF3 uCNF3val;
    DOUBLE dBaudRate = 0;
    BYTE   bTSEG2 = 0;
    TCHAR* pcStopStr = NULL;
    
    uCNF1val.ucCNF1 = 
        static_cast <UCHAR >(_tcstol((LPCTSTR)omStrCNF1.GetBuffer(MAX_PATH),&pcStopStr,defHEXADECIMAL));
    uCNF2val.ucCNF2 = 
    static_cast <UCHAR >(_tcstol((LPCTSTR)omStrCNF2.GetBuffer(MAX_PATH),&pcStopStr,defHEXADECIMAL));
    uCNF3val.ucCNF3 = 
    static_cast <UCHAR >(_tcstol((LPCTSTR)omStrCNF3.GetBuffer(MAX_PATH),&pcStopStr,defHEXADECIMAL));

    BYTE bBRP = static_cast <BYTE> (uCNF1val.sCNF1Bit.ucBRPbit + 1);
    BYTE bTSEG1 = static_cast <BYTE> (uCNF2val.sCNF2Bit.ucTSEG1bit + 1);
    BYTE bPROPDELAY = static_cast <BYTE> (uCNF2val.sCNF2Bit.ucPropDelaybit + 1);
    BYTE bFLAG = static_cast <BYTE> (uCNF2val.sCNF2Bit.ucFLAGbit + 1);

    if (bFLAG)
    {
        bTSEG2 = static_cast <BYTE> (uCNF3val.sCNF3Bit.ucTSEG2bit + 1);
    }
    else
    {
        bTSEG2 = bTSEG1 ;
    }

    BYTE bNBT = static_cast <BYTE> (bTSEG1 + bTSEG2 + bPROPDELAY + 1);

    dBaudRate = (DOUBLE)((defES581Frequency) / (2.0 * bBRP * bNBT));

    return dBaudRate / 1000;
}

/******************************************************************************/
/*  Function Name    : bFillControllerConfig                                  */
/*                                                                            */
/*  Input(s)         : -                                                      */
/*  Output           :                                                        */
/*  Functionality    : This function is called to read registry  or ini file  */
/*                     and return the baudrate ( BTR0 and BTR1). If there is */
/*                     no entry return the default value                      */
/*  Member of        : CChangeRegisters_ES581                                       */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        : Amitesh Bharti                                         */
/*  Date Created     : 26.03.2003                                             */
/*  Modifications    : Raja N on 13.09.2004                                   */
/*                     Modified the function as member function and changed   */
/*                     name as per coding standards                           */
/*  Modifications    : Raja N on 09.03.2005                                   */
/*                   : Added code to support multiple contoller information   */
/*                     in the configuration module                            */
/******************************************************************************/
BOOL CChangeRegisters_ES581::bFillControllerConfig()
{
    BOOL bReturn = FALSE;
    // If successful then set the result to pass
    if (m_pControllerDetails != NULL)
    {
        bReturn = TRUE;
    }
    // Return the result
    return bReturn;
}

/******************************************************************************/
/*  Function Name    :  ~CChangeRegisters_ES581                                     */
/*                                                                            */
/*  Input(s)         :                                                        */
/*  Output           :                                                        */
/*  Functionality    :  This is destructor of the class. It is called when    */
/*                      object of this class is being destroyed. All memory   */
/*                      allocation is deleted here.                           */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  07.04.2003                                            */
/*  Modifications    :  Raja N on 14.03.2005, Added code to clear memory used */
/*                      to get controller information                         */
/******************************************************************************/
CChangeRegisters_ES581::~CChangeRegisters_ES581()
{

}
/******************************************************************************/
/*  Function Name    :  OnItemchangedLstcBtrList                              */
/*                                                                            */
/*  Input(s)         :  NMHDR* pNMHDR, LRESULT* pResult                       */
/*  Output           :  LRESULT* pResult                                      */
/*  Functionality    :  This function will be called by framework when item in*/
/*                      list control is changed. Based on the index of        */
/*                      selected and focused item, the other field will be    */
/*                      updated by calling vUpdateBTRsBRPEditWindow(..)       */
/*  Member of        :  CChangeRegisters_ES581                                      */
/*  Friend of        :      -                                                 */
/*                                                                            */
/*  Author(s)        :  Amitesh Bharti                                        */
/*  Date Created     :  23.10.2003                                            */
/*  Modifications    :                                                        */
/*                                                                            */
/******************************************************************************/
void CChangeRegisters_ES581::OnItemchangedLstcBtrList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    TRACE("OnItemchangedLstcBtrList --- %d\n", pNMListView->iItem);
    *pResult = 0;
}

/*******************************************************************************
  Function Name  : OnClickListChannels
  Input(s)       : pNMHDR - Pointer to Notification Block
                   pResult - Pointer to the result
  Output         : -
  Functionality  : This function will be called when the user clicks the channel
                   list item. This will set the focus to the last selected item
                   if user clicks outside
  Member of      : CChangeRegisters_ES581
  Author(s)      : Raja N
  Date Created   : 14.3.2005
  Modifications  : 
*******************************************************************************/
void CChangeRegisters_ES581::OnClickListChannels(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
    // Get the selection mask
    UINT unItemStateMask = LVNI_SELECTED|LVNI_FOCUSED;
    // Get the selected item index
    int nSel = m_omChannelList.GetNextItem( -1, LVNI_SELECTED);
    // If nothing is selected then set the selection to the last saved index
    if (nSel == -1)
    {
        m_omChannelList.SetItemState( m_nLastSelection,
                                      unItemStateMask,
                                      unItemStateMask);
    }
    *pResult = 0;
}

/*******************************************************************************
  Function Name  : OnItemchangedListChannels
  Input(s)       : pNMHDR - Pointer to the list item struct
                   pResult - Pointer to the result value
  Output         : -
  Functionality  : This function will update baudrate information of selected
                   channel
  Member of      : CChangeRegisters_ES581
  Author(s)      : Raja N
  Date Created   : 14.03.2005
  Modifications  :
*******************************************************************************/
void CChangeRegisters_ES581::OnItemchangedListChannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // Get the List item data from the notification
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // Create selection mask
    UINT unItemStateMask = LVIS_SELECTED | LVIS_FOCUSED;
    // If new state is selected then show selected channel details
    if (pNMListView->uNewState == unItemStateMask)
    {
        // Set the selection
        m_nLastSelection = pNMListView->iItem;
        // Update the UI Controls with the
        vFillControllerConfigDetails();

    }
    // If it is lose of focus then save the user changes
    else if (pNMListView->uChanged  == LVIF_STATE && 
             pNMListView->uOldState == LVIS_SELECTED)
    {
        // Update modified data
        UpdateData( TRUE);
        if (bUpdateControllerDataMembers() == FALSE)
        {
            return;
        }
        // Validate Baud rate and find the nearest match
        vValidateBaudRate();
        // Save the changes in to the local data structure
        vUpdateControllerDetails();
    }
    *pResult = 0;
}

/*******************************************************************************
  Function Name  : OnDblclkListChannels
  Input(s)       : pNMHDR - Pointer to Notification Block
                   pResult - Pointer to the result
  Output         : -
  Functionality  : This function will be called wher the user double clicks the
                   channel list item. This will set the focus to the last
                   selected item if user clicks outside
  Member of      : CChangeRegisters_ES581
  Author(s)      : Raja N
  Date Created   : 14.3.2005
  Modifications  : 
*******************************************************************************/
void CChangeRegisters_ES581::OnDblclkListChannels(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
    // Create selection mask
    UINT unItemStateMask = LVNI_SELECTED | LVNI_FOCUSED;
    // Get current selection
    int nSel = m_omChannelList.GetNextItem( -1, LVNI_SELECTED);
    // If nothing got selected restore last selection
    if (nSel == -1)
    {
        m_omChannelList.SetItemState( m_nLastSelection,
                                      unItemStateMask,
                                      unItemStateMask);
    }
    *pResult = 0;
}

/*******************************************************************************
  Function Name  : vFillControllerConfigDetails
  Input(s)       : -
  Output         : -
  Functionality  : This function will fill details of selected channel in to the
                   member variables used. This will also update the BTR0 and
                   BTR1 registers value and list box of possible values for the
                   selected baudrate.
  Member of      : CChangeRegisters_ES581
  Author(s)      : Raja N
  Date Created   : 14.3.2005
  Modifications  : 
*******************************************************************************/
void CChangeRegisters_ES581::vFillControllerConfigDetails()
{
    int nIndex = m_nLastSelection;
    /* Add hardware info to the description field */
    CWnd* pWnd = GetDlgItem(IDC_EDIT_CHANNEL_DESC);
    if (pWnd != NULL)
    {
        pWnd->SetWindowText(m_pControllerDetails[nIndex].m_omHardwareDesc);
    }

    m_omStrEditBaudRate     = m_pControllerDetails[ nIndex ].m_omStrBaudrate;
    m_omStrEditCNF1         = m_pControllerDetails[ nIndex ].m_omStrCNF1;
    m_omStrEditCNF2         = m_pControllerDetails[ nIndex ].m_omStrCNF2;
    m_omStrEditCNF3         = m_pControllerDetails[ nIndex ].m_omStrCNF3;
    //m_omStrComboClock       = m_pControllerDetails[ nIndex ].m_omStrClock;
    m_omStrComboSampling    = m_pControllerDetails[ nIndex ].m_omStrSampling;
    m_omStrEditWarningLimit = m_pControllerDetails[ nIndex ].m_omStrWarningLimit;
    m_omStrPropDelay = m_pControllerDetails[ nIndex ].m_omStrPropagationDelay;
    m_omStrSJW = m_pControllerDetails[ nIndex ].m_omStrSjw;

    
    int nSample             = _tstoi(m_omStrComboSampling);
    //omStrInitComboBox(ITEM_SAMPLING,1,m_omCombSampling));
    //Assign edit box string value to CString member variable of Edit control
    // for Baudrate Convert String into float or INT to be used to make a list
    // of all possible  of BTRi, SJW, Sampling Percentage, and NBT values
    //m_unCombClock       = (UINT)_tstoi(m_omStrComboClock);

    // TO BE FIXED LATER
    m_dEditBaudRate = 
        dCalculateBaudRateFromBTRs( m_omStrEditCNF1, m_omStrEditCNF2, m_omStrEditCNF3);

    UpdateData(FALSE);
    unsigned int unIndex = 0;
    int nReturn = nListBoxValues(m_asColListCtrl, m_dEditBaudRate, 
                                 (WORD)m_unCombClock, &unIndex,nSample);

    // if Function returns Success display the item and set the focus to last
    // saved item or at item which is at the mid of the list. Update edit boxes
    // for BRP, BTRi by calling function vUpdateBtriBrpEditWindow(..,..)
    if (nReturn == defSUCCESS)
    {   
        // read the item to set the focus from the configuration
        if (m_pControllerDetails != NULL)
        {
            nReturn = m_pControllerDetails[m_nLastSelection].m_nItemUnderFocus;
        }
        vDisplayListBox(unIndex, nReturn);
    }

}

/*******************************************************************************
  Function Name  : vUpdateControllerDetails
  Input(s)       : -
  Output         : -
  Functionality  : This function will save the user enter values for baud rate
                   into the controller configuration structure
  Member of      : CChangeRegisters_ES581
  Author(s)      : Raja N
  Date Created   : 14.3.2005
  Modifications  : 
*******************************************************************************/
void CChangeRegisters_ES581::vUpdateControllerDetails()
{
    INT   nItemUnderFocus           = 0;
    UCHAR  ucBtr0                   = 0;
    UCHAR  ucBtr1                   = 0;
    UCHAR ucCNF1 = 0, ucCNF2 = 0, ucCNF3 = 0;
    TCHAR*    pcStopStr              = NULL;
    CString omStrComboSampling      = _T("");
    CString omStrEditBtr0           = _T("");
    CString omStrEditBtr1           = _T("");
    CString omStrEditAcceptanceCode = _T("");
    CString omStrEditAcceptanceMask = _T("");
    
    // Update the data members before writing into ini file or registry.
    UpdateData(TRUE);

    // Get the warning limit.
    UINT unWarningLimit = 0;
    unWarningLimit = static_cast <UINT>(_tcstol((LPCTSTR)
                                               m_omStrEditWarningLimit,
                                               &pcStopStr,defBASE_DEC));

    UINT unWarningLimtMin = static_cast <UINT> (defWARNING_LIMIT_MIN);
    UINT unWarningLimtMax = static_cast <UINT> (defWARNING_LIMIT_MAX);

    if (  ( unWarningLimit >= unWarningLimtMin) 
       && ( unWarningLimit <= unWarningLimtMax))
    {
        nItemUnderFocus = m_omListCtrlBitTime.GetNextItem(-1, LVNI_SELECTED);
        m_ucWarningLimit = static_cast <UCHAR> (unWarningLimit);
        ucCNF1 = static_cast<UCHAR>( _tcstol(( LPCTSTR)m_omStrEditCNF1,
                                        &pcStopStr,defHEXADECIMAL));
        ucCNF2 = static_cast<UCHAR>( _tcstol(( LPCTSTR)m_omStrEditCNF2,
                                        &pcStopStr,defHEXADECIMAL));
        ucCNF3 = static_cast<UCHAR>( _tcstol(( LPCTSTR)m_omStrEditCNF3,
                                        &pcStopStr,defHEXADECIMAL));
        // Pack the BTR0 and BTR1 values in two bytes before calling DIL fuction 
        // to initialise.
        m_usBTR0BTR1 = static_cast <USHORT>(((ucBtr0 << 8)| ucBtr1) & 0xffff);
        
        m_pControllerDetails[ m_nLastSelection ].m_nItemUnderFocus   =
                                                            nItemUnderFocus;
        _tcscpy(m_pControllerDetails[ m_nLastSelection ].m_omStrBaudrate, m_omStrEditBaudRate.GetBuffer(MAX_PATH));
        //m_pControllerDetails[ m_nLastSelection ].m_omStrClock        =
        //                                                    m_omStrComboClock;
        _tcscpy(m_pControllerDetails[m_nLastSelection].m_omStrCNF1, m_omStrEditCNF1.GetBuffer(MAX_PATH));
        _tcscpy(m_pControllerDetails[m_nLastSelection].m_omStrCNF2, m_omStrEditCNF2.GetBuffer(MAX_PATH));
        _tcscpy(m_pControllerDetails[m_nLastSelection].m_omStrCNF3, m_omStrEditCNF3.GetBuffer(MAX_PATH));
        _stprintf(m_pControllerDetails[m_nLastSelection].m_omStrClock, _T("%d"), m_unCombClock);
        _tcscpy(m_pControllerDetails[ m_nLastSelection ].m_omStrSampling, m_omStrComboSampling.GetBuffer(MAX_PATH));
        _tcscpy(m_pControllerDetails[ m_nLastSelection ].m_omStrWarningLimit, m_omStrEditWarningLimit.GetBuffer(MAX_PATH));
        _tcscpy(m_pControllerDetails[ m_nLastSelection ].m_omStrPropagationDelay, m_omStrPropDelay.GetBuffer(MAX_PATH));
        _tcscpy(m_pControllerDetails[ m_nLastSelection ].m_omStrSjw, m_omStrSJW.GetBuffer(MAX_PATH));
    }
    else
    {
        // Invalid Warning Limit Error Message
        CString omStrMsg = _T("");
        omStrMsg.Format( defWARNINGLIMIT_MSG, m_omStrEditWarningLimit,
                         defWARNING_LIMIT_MIN,
                         defWARNING_LIMIT_MAX);
        m_omEditWarningLimit.SetFocus();
        m_omEditWarningLimit.SetSel(0, -1,FALSE);
    }

}

/*******************************************************************************
  Function Name  : bSetBaudRateFromCom
  Input(s)       : -
  Output         : -
  Functionality  : This function will be called from COM function to set baud rate
  Member of      : CChangeRegisters_ES581
  Author(s)      : Anish
  Date Created   : 21.06.06
  Modifications  : 
*******************************************************************************/
BOOL CChangeRegisters_ES581::bSetBaudRateFromCom(int nChannel,BYTE bBTR0,BYTE bBTR1)
{
    BOOL bReturn = FALSE;
    CString omStrBtr0;
    CString omStrBtr1;
    CString omStrBaudRate;
    USHORT m_usBTR0BTR1;
    UINT unIndex                    = 0;

    UINT unClock                    = 0;
    UINT unSample                   = 1;

    // TO BE FIXED LATER
    double dBaudRate = dCalculateBaudRateFromBTRs(omStrBtr0,omStrBtr1, omStrBtr1);
    omStrBaudRate.Format(_T("%f"),dBaudRate);
    m_usBTR0BTR1 = static_cast <USHORT>(((bBTR0 << 8)| bBTR1) & 0xffff);
    //Save the changes for the channels
    unClock       = (UINT)_tstoi(m_pControllerDetails[ nChannel-1 ].m_omStrClock);
    if ((bBTR1 & 0x80) != 0)
    {
        unSample          =  3 ;
    }

    UINT nReturn  = nListBoxValues(m_asColListCtrl, dBaudRate, (WORD)unClock,
                                   &unIndex, unSample);
    if (nReturn != -1)
    {
        for (UINT i = 0; i<unIndex; i++)
        {
            //if (( bBTR0 == (m_asColListCtrl[i].uBTRReg0.ucBTR0))&&
            //    ( bBTR1 == (m_asColListCtrl[i].uBTRReg1.ucBTR1)))
            {
                m_pControllerDetails[nChannel-1 ].m_nItemUnderFocus = i;    
            }
        }
    }
    m_pControllerDetails[ nChannel-1 ].m_nBTR0BTR1 = m_usBTR0BTR1;
    
    _tcscpy(m_pControllerDetails[ nChannel-1 ].m_omStrBaudrate, omStrBaudRate.GetBuffer(MAX_PATH));
    _tcscpy(m_pControllerDetails[nChannel - 1].m_omStrCNF1, m_omStrEditCNF1.GetBuffer(MAX_PATH));
    _tcscpy(m_pControllerDetails[nChannel - 1].m_omStrCNF2, m_omStrEditCNF2.GetBuffer(MAX_PATH));
    _tcscpy(m_pControllerDetails[nChannel - 1].m_omStrCNF3, m_omStrEditCNF3.GetBuffer(MAX_PATH));
    _stprintf(m_pControllerDetails[ nChannel-1  ].m_omStrSampling, _T("%d"), unSample);

    m_nLastSelection = nChannel-1;

    // Update Configuration file
    //kadoor theApp.bSetData( CONTROLLER_DETAILS, m_pControllerDetails);
    //// Update Hardware Interface Layer
    //if (g_podHardwareInterface->bLoadDataFromConfig() == TRUE)
    //{
    //    int nApply = g_podHardwareInterface->nSetApplyConfiguration();
    //    if (nApply ==defERR_OK)
    //    {
    //        bReturn =TRUE;
    //    }
    //}
    return bReturn;
    
}
/*******************************************************************************
  Function Name  : vGetBaudRateFromCom
  Input(s)       : -
  Output         : -
  Functionality  : This function will be called from COM function to set baud rate
  Member of      : CChangeRegisters_ES581
  Author(s)      : Anish
  Date Created   : 21.06.06
  Modifications  : 
*******************************************************************************/
BOOL CChangeRegisters_ES581::bGetBaudRateFromCom(int nChannel,BYTE &bBTR0,BYTE &bBTR1)
{
    BOOL bReturn =FALSE;
    if (m_pControllerDetails != NULL)
    {
         int nTempBTR0BTR1 = m_pControllerDetails[ nChannel-1 ].m_nBTR0BTR1;
         bBTR1 = (BYTE)(nTempBTR0BTR1 & 0XFF);
         bBTR0 = (BYTE)((nTempBTR0BTR1>>defBITS_IN_BYTE ) & 0XFF);

         bReturn=TRUE;
    }
    return bReturn;
}
/*******************************************************************************
 Function Name  : bSetFilterFromCom
 Input(s)       : long  nExtended,\\for extended msg or not 
                  DWORD  dBeginMsgId, \\filter's msg id start
                  DWORD dEndMsgId \\filter's msg id stop
 Output         : int - Operation Result. 0 incase of no errors. Failure Error
                  codes otherwise.
 Functionality  : This function will set the filter information if called using 
                  com interface.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Anish kr
 Date Created   : 05.06.06

*******************************************************************************/
BOOL CChangeRegisters_ES581::bSetFilterFromCom(BOOL  bExtended, DWORD  dBeginMsgId, 
                                   DWORD dEndMsgId)
{
    BOOL bReturn = FALSE;
    // for getting separate byte
    DWORD dTemp=0XFF;

    for (UINT unIndex = 0;
    unIndex < defNO_OF_CHANNELS;
    unIndex++)
    {
        // To set no. shifts
        int nShift = sizeof( UCHAR) * defBITS_IN_BYTE;
        
        //to convert all acceptance and mask byets into string
        CString omStrTempByte;
        // Create Code
        omStrTempByte.Format(_T("%02X"),(dTemp & ( dBeginMsgId)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccCodeByte4, 
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dBeginMsgId >> nShift)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccCodeByte3, 
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dBeginMsgId >> nShift * 2)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccCodeByte2,
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dBeginMsgId >> nShift * 3)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccCodeByte1,
            omStrTempByte.GetBuffer(MAX_PATH));

        // Create Mask
        omStrTempByte.Format(_T("%02X"),(dTemp & ( dEndMsgId)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccMaskByte4,
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dEndMsgId >> nShift)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccMaskByte3,
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dEndMsgId >> nShift * 2)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccMaskByte2,
            omStrTempByte.GetBuffer(MAX_PATH));

        omStrTempByte.Format(_T("%02X"),(dTemp & ( dEndMsgId >> nShift * 3)));
        _tcscpy(m_pControllerDetails[ unIndex ].m_omStrAccMaskByte1,
            omStrTempByte.GetBuffer(MAX_PATH));

        m_pControllerDetails[ unIndex ].m_bAccFilterMode = bExtended;
    }
     

    //kadoor // Update Configuration file
    //theApp.bSetData( CONTROLLER_DETAILS, m_pControllerDetails);
    //// Update Hardware Interface Layer
    //if (g_podHardwareInterface->bLoadDataFromConfig() == TRUE)
    //{
    //    int nApply = g_podHardwareInterface->nSetApplyConfiguration();
    //    if (nApply ==defERR_OK)
    //    {
    //        bReturn =TRUE;
    //    }
    //}
    
    return bReturn;
    
}

/*******************************************************************************
 Function Name  : bGetFilterFromCom
 Input(s)       : long  nExtended,\\for extended msg or not 
                  DWORD  dBeginMsgId, \\acceptance code
                  DWORD dEndMsgId \\mask code
 Output         : int - Operation Result. 0 incase of no errors. Failure Error
                  codes otherwise.
 Functionality  : This function will set the filter information if called using 
                  com interface.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Anish kr
 Date Created   : 05.06.06

*******************************************************************************/
BOOL CChangeRegisters_ES581::bGetFilterFromCom(BOOL  &bExtended, double  &dBeginMsgId, 
                                   double &dEndMsgId)
{
    BOOL bReturn = FALSE;
    int nAccCodeByte1;
    int nAccCodeByte2;
    int nAccCodeByte3;
    int nAccCodeByte4;
    int nMaskCodeByte1;
    int nMaskCodeByte2;
    int nMaskCodeByte3;
    int nMaskCodeByte4;
    if (m_pControllerDetails != NULL)
    {
        
        TCHAR* pcStopStr ;
        //Change to separate integer value for each byte
        nAccCodeByte1 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccCodeByte1,
                        &pcStopStr,defHEXADECIMAL);
        nAccCodeByte2 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccCodeByte2,
                        &pcStopStr,defHEXADECIMAL);
        nAccCodeByte3 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccCodeByte3,
                        &pcStopStr,defHEXADECIMAL);
        nAccCodeByte4 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccCodeByte4,
                        &pcStopStr,defHEXADECIMAL);
        nMaskCodeByte1 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccMaskByte1,
                        &pcStopStr,defHEXADECIMAL);
        nMaskCodeByte2 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccMaskByte2,
                        &pcStopStr,defHEXADECIMAL); 
        nMaskCodeByte3 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccMaskByte3,
                        &pcStopStr,defHEXADECIMAL); 
        nMaskCodeByte4 = _tcstol((LPCTSTR)m_pControllerDetails[ 0 ].m_omStrAccMaskByte4,
                        &pcStopStr,defHEXADECIMAL);
        //now make them as dword in decimal
        dBeginMsgId = (ULONG)(nAccCodeByte1*0X1000000+nAccCodeByte2*0X10000+
            nAccCodeByte3*0X100+nAccCodeByte4);
        dEndMsgId = (ULONG)(nMaskCodeByte1*0X1000000+nMaskCodeByte2*0X10000+
            nMaskCodeByte3*0X100+nMaskCodeByte4);
        
        
        bExtended=  m_pControllerDetails[ 0 ].m_bAccFilterMode;
        bReturn=TRUE;
    }
    return bReturn;
}

/*******************************************************************************
 Function Name  : CallbackSortFunction
 Input(s)       : LPARAM lParam1 - One of the two rows in the list box
                  LPARAM lParam2 - The other row in the list box
                  LPARAM lColumn - Column information
 Output         : void
 Functionality  : Callback function when the user clicks on the list control
                  header.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 19.04.2008

*******************************************************************************/
int CALLBACK CallbackSortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lColumn)
{
    int Result = 0;

    sCOLUMNS& sEntry1 = *((sCOLUMNS *) lParam1);
    sCOLUMNS& sEntry2 = *((sCOLUMNS *) lParam2);

    switch (lColumn)
    {
        case 0:
        {
            Result = sEntry1.uCNFReg1.ucCNF1 - sEntry2.uCNFReg1.ucCNF1;
        }
        break;
        case 1:
        {
            Result = sEntry1.uCNFReg2.ucCNF2 - sEntry2.uCNFReg2.ucCNF2;
        }
        break;
        case 2:
        {
            Result = sEntry1.uCNFReg3.ucCNF3 - sEntry2.uCNFReg3.ucCNF3;
        }
        break;
        default:
        break;
    }

    return Result;
}

/*******************************************************************************
 Function Name  : OnHdnItemclickLstcBtrList
 Input(s)       : NMHDR *pNMHDR - Contains header information
                  LRESULT *pResult - Result (out parameter)
 Output         : void
 Functionality  : Handler for the event of the user's click on the list control
                  header.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 19.04.2008

*******************************************************************************/
void CChangeRegisters_ES581::OnHdnItemclickLstcBtrList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
    m_omListCtrlBitTime.SortItems(CallbackSortFunction, pLV->iItem);

    *pResult = 0;
}

/*******************************************************************************
 Function Name  : OnCbnSelchangeCombSjw
 Input(s)       : void
 Output         : void
 Functionality  : Handler when the user selects a specific value in the SJW list
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 19.04.2008

*******************************************************************************/
void CChangeRegisters_ES581::OnCbnSelchangeCombSjw()
{
    int nSJWCurr = nGetValueFromComboBox(m_omCtrlSJW);
    if (nSJWCurr != m_nSJWCurr)
    {
        m_nSJWCurr = nSJWCurr;
        bDecideCalculatingOption();
        vChangeListBoxValues(-1);
    }
}

/*******************************************************************************
 Function Name  : OnCbnSelchangeCombPropdelay
 Input(s)       : void
 Output         : void
 Functionality  : Handler when the user selects a specific value in the PD list
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 19.04.2008

*******************************************************************************/
void CChangeRegisters_ES581::OnCbnSelchangeCombPropdelay()
{
    int nPropDelay = nGetValueFromComboBox(m_omCtrlPropDelay);
    if (nPropDelay != m_nPropDelay)
    {
        m_nPropDelay = nPropDelay;
        bDecideCalculatingOption();
        vChangeListBoxValues(-1);
    }
}

/*******************************************************************************
 Function Name  : bDecideCalculatingOption
 Input(s)       : void
 Output         : void
 Functionality  : Based on the present values SJW and PD, this function decides
                  the necessary parameter calculating operation.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 19.04.2008

*******************************************************************************/
void CChangeRegisters_ES581::bDecideCalculatingOption(void)
{
    if ((m_nSJWCurr == 0) && (m_nPropDelay == 0))
    {
        // All possible values of SJW and PD
        m_bOption = NO_DEF;
    }
    else if ((m_nSJWCurr == 0) && (m_nPropDelay != 0))
    {
        // All possible values of SJW and a specific value of PD
        m_bOption = SJW_TS1_TS2;
    }
    else if ((m_nSJWCurr != 0) && (m_nPropDelay == 0))
    {
        // All possible values of PD and a specific value of SJW
        m_bOption = PD_TS1_TS2;
    }
    else // if ((m_nSJWCurr != 0) && (m_nPropDelay != 0))
    {
        // Only specific values of SJW and PD
        m_bOption = TS1_TS2;
    }
}

/*******************************************************************************
 Function Name  : GetRegisterValues_ES581
 Input(s)       : Out parameters; BYTE& bCNF1, BYTE& bCNF2, BYTE& bCNF3
 Output         : BOOL
 Functionality  : This function retrieves the values of the registers CNF1, 
                  CNF2 and CNF3 associated with the current selected entry
                  of the register entry list control.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 21.04.2008

*******************************************************************************/
BOOL CChangeRegisters_ES581::GetRegisterValues_ES581(BYTE& bCNF1, BYTE& bCNF2, BYTE& bCNF3)
{
    BOOL bResult = FALSE;
    int nIndexSel = 0;

    // First of all - get the current selection
    if ((nIndexSel = GetSelectedEntryIndex()) != -1)
    {
        sCOLUMNS* psCurrEntry = 
                    (sCOLUMNS *) (m_omListCtrlBitTime.GetItemData(nIndexSel));

        if (psCurrEntry != NULL)
        {
            bResult = TRUE;
            bCNF1 = psCurrEntry->uCNFReg1.ucCNF1;
            bCNF2 = psCurrEntry->uCNFReg2.ucCNF2;
            bCNF3 = psCurrEntry->uCNFReg3.ucCNF3;
        }
    }

    return bResult;
}

/*******************************************************************************
 Function Name  : GetSelectedEntryIndex
 Input(s)       : void
 Output         : For a selected entry, its 0 based index; else -1
 Functionality  : This function retrieves the 0 based index of the selected
                  entry in the controller register entry list control.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 21.04.2008

*******************************************************************************/
int CChangeRegisters_ES581::GetSelectedEntryIndex(void)
{
    int nResult = -1;
    // First of all - get the current selection
    POSITION Pos = m_omListCtrlBitTime.GetFirstSelectedItemPosition();
    if (Pos != NULL)
    {
        nResult = m_omListCtrlBitTime.GetNextSelectedItem(Pos);
    }

    return nResult;
}

/*******************************************************************************
 Function Name  : bUpdateControllerDataMembers
 Input(s)       : void
 Output         : TRUE if successful, else FALSE
 Functionality  : This function updates the controller data members with the
                  present selected combination value in the list control
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 21.04.2008

*******************************************************************************/
BOOL CChangeRegisters_ES581::bUpdateControllerDataMembers(void)
{
    BYTE bCNF1, bCNF2, bCNF3;

    BOOL Result = GetRegisterValues_ES581(bCNF1, bCNF2, bCNF3);
    if (Result)
    {
        m_omStrEditCNF1.Format(_T("%x"), bCNF1);
        m_omStrEditCNF2.Format(_T("%x"), bCNF2);
        m_omStrEditCNF3.Format(_T("%x"), bCNF3);
    }
    return Result;
}

/*******************************************************************************
 Function Name  : nGetValueFromComboBox
 Input(s)       : void
 Output         : TRUE if successful, else FALSE
 Functionality  : This function returns value of the selected entry in a combo
                  box. Although helper in broader sense, this assumes the 
                  entries to be 1 based integers and returns 0 when the entry
                  contains the string 'ALL'.
 Member of      : CChangeRegisters_ES581
 Author(s)      : Ratnadip Choudhury
 Date Created   : 21.04.2008

*******************************************************************************/
int CChangeRegisters_ES581::nGetValueFromComboBox(CComboBox& omComboBox)
{
    int nResult = 0;
    int nCurrSel =  omComboBox.GetCurSel();
    if (nCurrSel != CB_ERR)
    {
        CString omCurText = _T("");
        omComboBox.GetLBText(nCurrSel, omCurText);
        if (omCurText != _T("ALL"))
        {
            nResult = _tstoi(omCurText);
        }
    }
    return nResult;
}

INT CChangeRegisters_ES581::nGetInitStatus()
{
    return m_nDataConfirmStatus;
}
