#include "stdafx.h"
#include "MainDialog.h"

#include <Commdlg.h>
#include <shellapi.h>
#include <string>
bool g_bUpdateDir = false;


void MainDialog::Init()
{
	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));   
    m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
    m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	m_pClose = static_cast<CButtonUI*>(m_pm.FindControl(_T("close")));
	m_pAddItem = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_AddItem")));
	m_pAddNode = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_AddNode")));
	m_pDelNode = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_DelNode")));
	m_pInputNodeText = static_cast<CEditUI*>(m_pm.FindControl(_T("Edit_InputNodeName")));
	m_pCurrentNodeText = static_cast<CTextUI*>(m_pm.FindControl(_T("Label_CurrentNodeName")));
	m_pSaveAndExit = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_SaveAndExit")));
	m_pTree = static_cast<CTreeViewUI*>(m_pm.FindControl(_T("tree")));
			
	if(NULL == m_pTree) return;
	
	errno_t err;
	FILE* pFile = NULL;
	err = fopen_s(&pFile,"resource.txt","r");
	if (NULL == pFile)
	{
		MessageBox(NULL,"no txt",_T("error"),MB_OK);
		return;
	}

	char szTemp[MAX_PATH]={0};

	while (1 == fscanf_s(pFile,"%[^\n]",szTemp,sizeof(szTemp)))
	{			
		fgetc(pFile);			
		
		string str(szTemp);
		string::size_type pos = str.find(' ');

		string szItemName(str.substr(0, pos));

		int nItemExist = -1;
		bool bItemExist = FindInItems(szItemName,nItemExist);

		if (bItemExist)
		{		
			//resources
			str = str.substr(pos + 1);
			while (string::npos != pos )
			{
				pos = str.find(' ');
				string szResourceName(str.substr(0, pos));
				
				str = str.substr(pos + 1);

				Item* pExistItem = m_vItem[nItemExist];
				pExistItem->AddNodeInLoad(str);

				CTreeNodeUI* pItemNode = pExistItem->GetItemNode();
				if(NULL == pItemNode) continue;

				CTreeNodeUI* pResourceNode = new CTreeNodeUI;		
				bool bb = pItemNode->AddChildNode(pResourceNode);				

				TCHAR stResourceName[MAX_PATH] = {0};			
				strcpy( stResourceName, szResourceName.c_str());
			
				pResourceNode->SetAttribute(_T("folderattr"),_T("padding=\"0,1,0,0\" width=\"36\" height=\"16\" normalimage=\"file='node.png' source='0,0,36,16'\" hotimage=\"file='node.png' source='0,0,36,16'\" selectedimage=\"file='node.png' source='0,0,36,16' selectedhotimage=\"file='node.png' source='0,0,36,16'\""));
				pResourceNode->SetItemText(stResourceName);			
				pResourceNode->SetName(stResourceName);			
			}		
			continue;
		}
				
		Item *item = new Item();
		item->SetName(szItemName);	
			
		CTreeNodeUI* pItemNode = new CTreeNodeUI;
		pItemNode->SetAttribute(_T("folderattr"),_T("padding=\"0,1,0,0\" width=\"36\" height=\"16\" normalimage=\"file='item_expand.png' source='0,0,36,16'\" hotimage=\"file='item_expand.png' source='0,0,36,16'\" selectedimage=\"file='item_normal.png' source='0,0,36,16' selectedhotimage=\"file='item_normal.png' source='0,0,36,16'\""));
				
		bool bbb=m_pTree->Add(pItemNode);
		pItemNode->SetItemText(szItemName.c_str());
		pItemNode->SetUserData(szItemName.c_str());		

		pItemNode->SetMaxWidth(230);
		pItemNode->SetMinWidth(40);

		item->SetItemNode(pItemNode); 

		CTextUI* pUseable = new CTextUI;
		pUseable->SetText("usable");	
		pUseable->SetAttribute("align","right");
		pUseable->SetAttribute("textcolor","#ff238e23");
					
		pItemNode->Add(pUseable);		
		item->SetUseableText(pUseable);

		if(string::npos == pos)
		{			
			item->SetName(str);				
			m_vItem.push_back(item);
			continue;
		}			

		//resources
		str = str.substr(pos + 1);
		while (string::npos != pos )
		{
			pos = str.find(' ');
			string szResourceName(str.substr(0, pos));

			//父子是否同名
			if (szResourceName == szItemName)	
			{
				str = str.substr(pos + 1);
				continue;		
			}

			//查看是否已存在
			bool bfind = item->FindInCurrent(szResourceName);
			if (bfind)
			{
				str = str.substr(pos + 1);
				continue;	
			}

			item->AddNodeInLoad(szResourceName);
			str = str.substr(pos + 1);
									
			CTreeNodeUI* pResourceNode = new CTreeNodeUI;
			int nn=pItemNode->GetCountChild();
			bool bb = pItemNode->AddChildNode(pResourceNode);
			nn=pItemNode->GetCountChild();

			pResourceNode->SetAttribute(_T("folderattr"),_T("padding=\"0,1,0,0\" width=\"36\" height=\"16\" normalimage=\"file='node.png' source='0,0,36,16'\" hotimage=\"file='node.png' source='0,0,36,16'\" selectedimage=\"file='node.png' source='0,0,36,16' selectedhotimage=\"file='node.png' source='0,0,36,16'\""));
					
			pResourceNode->SetItemText(szResourceName.c_str());			
			pResourceNode->SetName(szResourceName.c_str());		
		}
		
		m_vItem.push_back(item);	
		CCheckBoxUI* pFolder	= pItemNode->GetFolderButton();
		pFolder->SetCheck(false);	
	}

	fclose(pFile);	
}
void MainDialog::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("windowinit") ) 
		OnPrepare(msg);
	else if( msg.sType == _T("click") ) 
	{		
		if( msg.pSender == m_pCloseBtn ) 
		{			
			SendMessage(WM_DESTROY);		
			return; 
		}
		else if( msg.pSender == m_pMinBtn ) 
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return; 
		}	
		else if( msg.pSender == m_pRestoreBtn ) 
		{ 
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
		}
		else if( msg.pSender == m_pAddNode ) 
		{ 		
			CDuiString strInputNode = m_pInputNodeText->GetText();			
			CTreeNodeUI* pCurSel = m_pTree->GetCurSelNode();
			string szName=strInputNode.GetData();
			if(0 == strInputNode.GetLength() || " " == szName)
			{
				CDuiString str;
				str.Format("please input a name.");
				MessageBox(NULL,str ,_T("error"),MB_OK);
				return;	
			}

			if (NULL == pCurSel)
			{
				MessageBox(NULL,"please select a item.",_T("Tip"),MB_OK);
				return;
			}

			CTreeNodeUI* pParent = pCurSel->GetParentNode();
			if (NULL != pParent)
			{
				MessageBox(NULL,"please select a item.",_T("Tip"),MB_OK);
				return;
			}

			//先找父节点
			string szItem = pCurSel->GetItemText();
			int nItem = m_vItem.size();
			int i=0;			
			Item* pItem = NULL;

			for (i=0;i<nItem;i++)
			{
				if (szItem == m_vItem[i]->GetName())
					pItem = m_vItem[i];				
			}
			
			if (NULL == pItem ) return;


			//查item重名
			if (szItem == strInputNode.GetData())
			{
				CDuiString str;
				str.Format("'%s' has same name with its parent node, please input another name.",strInputNode.GetData(),szItem.c_str());
				MessageBox(NULL,str ,_T("error"),MB_OK);
				return;				
			}

			//查node重名						
			bool bFind = pItem->FindInCurrent(strInputNode.GetData());
			if(bFind)
			{				
				CDuiString str;
				str.Format("'%s' node already existed in '%s', please input another name.",strInputNode.GetData(),szItem.c_str());
				MessageBox(NULL,str ,_T("error"),MB_OK);
				return;				
			}

			//加入map
			pItem->AddNodeInUse(strInputNode.GetData());		

			//加入tree
			AddNodeInTree(strInputNode.GetData(),pCurSel);

			pItem->SetUseableState();
			m_pTree->SelectItem(-1,true);
			return; 
		}		
		else if( msg.pSender == m_pAddItem ) 
		{ 			
			CDuiString strInputNode = m_pInputNodeText->GetText();			
			string szName=strInputNode.GetData();
			if(0 == strInputNode.GetLength() || " " == szName)			
			{
				CDuiString str;
				str.Format("please input a name.");
				MessageBox(NULL,str ,_T("error"),MB_OK);
				return;	
			}

			//查item重名	
			int nItem = m_vItem.size();
			int i=0;			
			bool bFind = false;

			for (i=0;i<nItem;i++)
			{
				if (strInputNode.GetData() == m_vItem[i]->GetName())
					bFind = true;			
			}

			if (true == bFind ) 
			{
				CDuiString str;
				str.Format("'%s' item already existed , please input another name.",strInputNode.GetData());
				MessageBox(NULL,str ,_T("error"),MB_OK);
				return;	
			}
			
			//加入map
			Item* pItem = new Item;
			pItem->SetName(strInputNode.GetData()); 		
			m_vItem.push_back(pItem);
			AddNodeInTree(strInputNode.GetData(),NULL);
			m_pTree->SelectItem(-1,true);
			return; 
		}		
		else if( msg.pSender == m_pDelNode ) 
		{ 					
			CTreeNodeUI* pCurSel = m_pTree->GetCurSelNode();
			if (NULL == pCurSel)
			{
				MessageBox(NULL,"please select a node.",_T("Tip"),MB_OK);
				return;
			}
								
			string szItem = pCurSel->GetItemText();
				
			//删自己
			DelInItems(szItem);

			//删别人depend里的自己
			for (vector<Item*>::iterator it = m_vItem.begin();it!=m_vItem.end();it++)
			{
				(*it)->DelNode(szItem);		
				(*it)->SetUseableState();		
			}	

			DeleteNodeInTree(szItem);	

			//hot
			m_pTree->SelectItem(-1,true);

			return; 
		}	
	}	
	else if(msg.pSender == m_pSaveAndExit)
	{
		SendMessage(WM_DESTROY);		
		return; 
	}	
}

LRESULT MainDialog::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return FALSE;

}
LRESULT MainDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0, this, &m_pm);
	if (pRoot == NULL)
	{
	//	MessageBox(NULL,"no skin file",_T("error"),MB_OK);
	}
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	Init();
	
	return 0;
}
LRESULT MainDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
LRESULT MainDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SaveToFile();		

	for (int i=0;i<m_vItem.size();i++)
	{
		SAFE_DELETE(m_vItem[i]);
	}

	::PostQuitMessage(0L);	
	return 0;
}
LRESULT MainDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}
LRESULT MainDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT MainDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT MainDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    ::GetClientRect(*this, &rcClient);

    RECT rcCaption = m_pm.GetCaptionRect();
    if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
        && pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
            CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
            if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
                _tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
                _tcscmp(pControl->GetClass(), _T("TextUI")) != 0
				)
                return HTCAPTION;
    }

    return HTCLIENT;
}
LRESULT MainDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		HRGN hRgn = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom + 1, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}
LRESULT MainDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT MainDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE )
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed )
	{
		if( !bZoomed )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

LRESULT MainDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) 
	{
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}
//item
void MainDialog::DeleteNodeInTree(string szNode)
{	
	if (NULL == m_pTree) return;

	//删自己
	CTreeNodeUI* pCurSel = m_pTree->GetCurSelNode();
	if (NULL == pCurSel) return;

	CDuiString szCurSel = pCurSel->GetItemText();		
				
	int nCountAll = m_pTree->GetCount();
	for (int i=0;i<nCountAll;i++)
	{
		CTreeNodeUI* pItem = (CTreeNodeUI*)m_pTree->GetItemAt(i);
		if (NULL == pItem) continue;
		CDuiString szItem = pItem->GetItemText();				
						
		CTreeNodeUI* pParent = pItem->GetParentNode();
		if (NULL == pParent && szCurSel == szItem)//选中的item,先删自己子节点
		{			
			if (pCurSel->GetParentNode()) continue;			
			
			int nCount = pCurSel->GetCountChild();
			for (int m=0;m<nCount;m++)
			{
				CTreeNodeUI* pChild = pCurSel->GetChildNode(0);
				if (NULL == pChild) continue;
				bool bDel = pCurSel->Remove(pChild);		
			}
			bool bb = m_pTree->Remove(pCurSel);
			i--;
		}
		else if(NULL != pParent && szCurSel == szItem)//node	
		{					
			pParent->Remove(pItem);	
			i--;
		}			
		nCountAll = m_pTree->GetCount();
	}

	m_pTree->SetCurSelNode(NULL);
}
//node
void MainDialog::AddNodeInTree(string szNode,CTreeNodeUI* pParent)
{		
	if (NULL == m_pTree) return;
	if (NULL == pParent) 
	{
		CTreeNodeUI* pNode = new CTreeNodeUI;
		pNode->SetAttribute(_T("folderattr"),_T("padding=\"0,1,0,0\" width=\"36\" height=\"16\" normalimage=\"file='item_expand.png' source='0,0,36,16'\" hotimage=\"file='item_expand.png' source='0,0,36,16'\" selectedimage=\"file='item_normal.png' source='0,0,36,16' selectedhotimage=\"file='item_normal.png' source='0,0,36,16'\""));
		bool bbb=m_pTree->Add(pNode);
		pNode->SetItemText(szNode.c_str());				
	}
	else
	{	
		CTreeNodeUI* pNode = new CTreeNodeUI;		
		bool bb = pParent->AddChildNode(pNode);		
		pNode->SetAttribute(_T("folderattr"),_T("padding=\"0,1,0,0\" width=\"36\" height=\"16\" normalimage=\"file='node.png' source='0,0,36,16'\" hotimage=\"file='node.png' source='0,0,36,16'\" selectedimage=\"file='node.png' source='0,0,36,16' selectedhotimage=\"file='node.png' source='0,0,36,16'\""));
		pNode->SetItemText(szNode.c_str());					
	}	
}

bool MainDialog::SaveToFile()
{
	errno_t err;
	FILE* pFile = NULL;
	err = fopen_s(&pFile,"resource.txt","w");
	if (NULL == pFile) return false;

	int nItem = m_vItem.size();
	int i=0;			
	bool bFind = false;

	for (i=0;i<nItem;i++)
	{
		Item* item = m_vItem[i];
		if (NULL == item) continue;
		
		fwrite(item->GetName().c_str(),item->GetName().length(),1,pFile);
		
		vector<string> vNode = item->GetCurrentNodes();
		int nCount = vNode.size();
		for (int i=0;i<nCount;i++)
		{
			fwrite(" ",1,1,pFile);
			fwrite(vNode[i].c_str(),vNode[i].length(),1,pFile);			
		}

		fwrite("\n",1,1,pFile);
	}	

	fclose(pFile);
	return true;
}

void MainDialog::DelInItems(string str)
{
	for (vector<Item*>::iterator it = m_vItem.begin();it!=m_vItem.end();it++)
	{
		Item* pItem = (*it);
		if (str == (*it)->GetName())
		{
			m_vItem.erase(it);	
			SAFE_DELETE(pItem);
			break;
		}						
	}			
}

bool MainDialog::FindInItems(string str,int& n)
{
	int nCount = m_vItem.size();
	for (int i=0;i<nCount;i++)
	{
		if (m_vItem[i]->GetName() == str)
		{
			n = i;
			return true;
		}
	}
	return false;
}
