#include "StdAfx.h"
#include "Patch.h"
#include "csmtp.h"
#include "registry.h"
#include "unicodeutils.h"
#include "hwsmtp.h"
#include "Windns.h"
#include "Git.h"

CPatch::CPatch()
{
	
}

CPatch::~CPatch()
{
	

}

void CPatch::ConvertToArray(CString &to,CStringArray &Array)
{
	int start=0;
	while(start>=0)
	{
		CString str=to.Tokenize(_T(";"),start);
		if(!str.IsEmpty())
			Array.Add(str);
	}
}

int CPatch::Send(CString &pathfile,CString &TO,CString &CC,bool bAttachment)
{
	CHwSMTP mail;
	if(this->Parser(pathfile)	)
		return -1;

	CStringArray attachments,CCArray;
	if(bAttachment)
	{
		attachments.Add(pathfile);
	}
	
	//ConvertToArray(CC,CCArray);

	CString sender;
	sender.Format(_T("%s <%s> "),g_Git.GetUserName(),g_Git.GetUserEmail());

	if(mail.SendSpeedEmail(this->m_Author,TO,this->m_Subject,this->m_strBody,NULL,&attachments,CC,25,sender))
		return 0;
	else
	{
		this->m_LastError=mail.GetLastErrorText();
		return -1;
	}
#if 0
	CRegString server(REG_SMTP_SERVER);
	CRegDWORD  port(REG_SMTP_PORT,25);
	CRegDWORD  bAuth(REG_SMTP_ISAUTH);
	CRegString  user(REG_SMTP_USER);
	CRegString  password(REG_SMTP_PASSWORD);

	mail.SetSMTPServer(CUnicodeUtils::GetUTF8(server),port);

	AddRecipient(mail,TO,false);
	AddRecipient(mail,CC,true);

	if( bAttachment )
		mail.AddAttachment(CUnicodeUtils::GetUTF8(pathfile));

	CString name,address;
	GetNameAddress(this->m_Author,name,address);
	mail.SetSenderName(CUnicodeUtils::GetUTF8(name));
	mail.SetSenderMail(CUnicodeUtils::GetUTF8(address));

	mail.SetXPriority(XPRIORITY_NORMAL);
	mail.SetXMailer("The Bat! (v3.02) Professional");

	mail.SetSubject(CUnicodeUtils::GetUTF8(this->m_Subject));

	mail.SetMessageBody((char*)&this->m_Body[0]);

	if(bAuth)
	{
		mail.SetLogin(CUnicodeUtils::GetUTF8((CString&)user));
		mail.SetPassword(CUnicodeUtils::GetUTF8((CString&)password));
	}

	return !mail.Send();
#endif


}

int CPatch::Parser(CString &pathfile)
{
	CString str;

	CStdioFile PatchFile;

	m_PathFile=pathfile;
	if( ! PatchFile.Open(pathfile,CFile::modeRead) )
		return -1;
	
	int i=0;
	while(PatchFile.ReadString(str))
	{
		if(i==1)
			this->m_Author=str.Right( str.GetLength() - 6 );
		if(i==2)
			this->m_Date = str.Right( str.GetLength() - 6 );
		if(i==3)
			this->m_Subject = str.Right( str.GetLength() - 8 );
		if(i==4)
			break;
		i++;		
	}

	m_Body.resize(PatchFile.GetLength() - PatchFile.GetPosition());
	PatchFile.Read(&m_Body.at(0),PatchFile.GetLength() - PatchFile.GetPosition());
	m_Body.push_back(0);

	PatchFile.Close();

	g_Git.StringAppend(&m_strBody,&m_Body[0],CP_ACP);
	
	return 0;
}

void CPatch::GetNameAddress(CString &in, CString &name,CString &address)
{
	int start,end;
	start=in.Find(_T('<'));
	end=in.Find(_T('>'));

	if(start >=0 && end >=0)
	{
		name=in.Left(start);
		address=in.Mid(start+1,end-start-1);
	}
	else
		address=in;
}
#if 0
void CPatch::AddRecipient(CSmtp &mail, CString &tolist, bool isCC)
{
	int pos=0;
	while(pos>=0)
	{
		CString one=tolist.Tokenize(_T(";"),pos);
		int start=one.Find(_T('<'));
		int end = one.Find(_T('>'));
		CStringA name;
		CStringA address;
		if( start>=0 && end >=0)
		{
			name=CUnicodeUtils::GetUTF8(one.Left(start));
			address=CUnicodeUtils::GetUTF8(one.Mid(start+1,end-start-1));
			if(address.IsEmpty())
				continue;
			if(isCC)
				mail.AddCCRecipient(address,name);
			else
				mail.AddRecipient(address,name);

		}else
		{
			if(one.IsEmpty())
				continue;
			if(isCC)
				mail.AddCCRecipient(CUnicodeUtils::GetUTF8(one));
			else
				mail.AddRecipient(CUnicodeUtils::GetUTF8(one));
		}
	}
}
#endif