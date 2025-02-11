// CNetMySql.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include "MySql.h"
#include "NetMySql.h"


//一个字段结构
typedef struct _FIELDS
{
	char field_name[DEFAULT_FIELD_SIZE];//字段名
	void * filed_value; //字段值的指针
	unsigned value_len; //字段的长度，只适用于字符串类型
}FIELDS,*PFIELDS;

//一行数据的结构数组
typedef struct _ROWS{
    FIELDS fields[DEFAULT_FIELD];   //字段,根据不同的应用程序可以修改此值
	int cnt;             //多少个字段，指示field中元素的有效个数
}ROWS,*PROWS;


CNetMySql::CNetMySql()
{
	result=NULL;
	memset(m_host,0,32);
	memset(m_name,0,32);
	memset(m_pwd,0,32);
	m_port=0;
	mysql_init(&myData);

	//设置连接超时
	unsigned timeout=5*60;
	mysql_options(&myData, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&timeout);
	
	//启动再连接
	my_bool reconnect=TRUE;
    mysql_options(&myData, MYSQL_OPT_RECONNECT, (char *)&reconnect);

	//初始化HashTable大小为100
	//res.InitHashTable(100);
}
CNetMySql::~CNetMySql()
{
	Close();
}

//连接MySql服务器
BOOL CNetMySql::Connect(char * host,char * name,char *pwd,char * db, unsigned port)
{
	BOOL bRet=TRUE;
	strcpy_s(m_host,host);
	strcpy_s(m_name,name);
	strcpy_s(m_pwd,pwd);
	m_port=port;
	do
	{
		//连接服务器
		if(mysql_real_connect(&myData,host,name,pwd,db,port,NULL,0)==NULL)
		{
			//TRACE(TEXT("TIPTOP-mysql_real_connect数据库连接失败，错误信息:%s\r\n"), mysql_error(&myData));
			MessageBoxA(NULL, mysql_error(&myData), "数据库连接错误信息", MB_OK);
			bRet=FALSE;
			break;
		}
		
		//设置连接字符集，默认为gb2312
		if(mysql_set_character_set(&myData, "gb2312"))
		{
			bRet=FALSE;
			//如果设置字符集不成功，则关闭连接
			TRACE(TEXT("TIPTOP-mysql_set_character_set设置连接字符集失败，错误信息:%s\r\n"), mysql_error(&myData));
			Close();
			break;
		}
		//设置允许多语句支持
		mysql_set_server_option(&myData, MYSQL_OPTION_MULTI_STATEMENTS_ON);
	}while(0);
	TRACE(TEXT("TIPTOP-数据库连接成功，mysql_error信息:%s\r\n"), mysql_error(&myData));
	return bRet;
}

//执行非查询语句
BOOL CNetMySql::ExecuteNoQuery(char ** sqlArray,int count)
{
	BOOL uRet=TRUE;
	//撤消自动提交模式
     mysql_autocommit(&myData, 0); 
	 for(int i=0;i<count;i++)
	 {
		 mysql_query(&myData,sqlArray[i]);
		 //查询受影响的行数
		 //uRet=uRet+(unsigned)mysql_affected_rows(&myData); 
	 }
	 //如果没有成功
	 if(mysql_commit(&myData))
	 {
		 //回滚事务res
		 mysql_rollback(&myData);
		 uRet=FALSE;
	 }
     //恢复自动提交模式
     mysql_autocommit(&myData, 0); 
	 return uRet;
}
//执行查询语句
unsigned CNetMySql::ExecuteQuery(char * sql)
{
	unsigned uLine=0,uField=0;
	PROWS rows;
	MYSQL_ROW row;
	MYSQL_FIELD *pField;
    //释放前一次的结果集
	FreeResult();
	//查询
	if (mysql_query(&myData,sql))
	{
		MessageBoxA(NULL, mysql_error(&myData), "查询出错", MB_OK);
		return uLine;
	}
    //存储结果集
    result=mysql_store_result(&myData);
    //如果结果集为空，则返回0
    if(result==NULL)return 0;
    //获得行数
    uLine=(unsigned)mysql_num_rows(result);
    //获得字段数
    uField=(unsigned)mysql_num_fields(result);
   
   //填充结果集
   for(int i=0;i<(int)uLine;i++)
   {
	   //为存储一行数据分配内存
      rows=(PROWS)malloc(sizeof(ROWS));
	  //一行有多少个字段
	  rows->cnt=uField;
	  //获得一行数据
	  row=mysql_fetch_row(result);
	  for(int j=0;j<(int)uField;j++)
	  {
		 //获得该列的相关信息
		  pField=mysql_fetch_field_direct(result, j);
		  //列名称
		  strcpy_s(rows->fields[j].field_name,pField->name);
		  //列值
		  rows->fields[j].filed_value=row[j];
		  //列宽度
		  rows->fields[j].value_len=pField->max_length;
	  }
	  //将一行数据存储到HashTable中
	  //res.SetAt(i,(void *)rows);
	  res.Add((void *)rows);
   }
   //返回行数
   return uLine;
}

//释放查询的结果集
void CNetMySql::FreeResult()
{
	int len=0;
	PROWS rows=NULL;

	//len=res.GetCount();
	len = res.GetCount();
	//释放结果集的内存
	for(int i=0;i<len;i++)
	{
		//rows=(PROWS)res[i];
		rows = (PROWS)res.GetAt(i);
		if(rows!=NULL)
		{
			free(rows);
			rows=NULL;
		}
	}
   //清空HashTable
	res.RemoveAll();

	//释放result
    if(result!=NULL)
	{
	   mysql_free_result(result);
	}
	result=NULL;
}

//执行带参数的SQL语句函数
BOOL CNetMySql::ExecutePrepare(char * sql)
{
	BOOL bRet=TRUE;
	do
	{
		//初始化句柄
		myStmt=mysql_stmt_init(&myData);
		if (!myStmt)
		{
			bRet=FALSE;
			break;
		}
		//准备SQL
		if (mysql_stmt_prepare(myStmt, sql, strlen(sql)))
		{
			bRet=FALSE;
			break;
		}
	}while(0);
	return bRet;
}
BOOL CNetMySql::BindParam(MYSQL_BIND * bind)
{
	BOOL bRet=TRUE;
	if (mysql_stmt_bind_param(myStmt, bind))
	{
       bRet=FALSE;
	}
	return bRet;

}
unsigned CNetMySql::Execute()
{
	unsigned uRet=0;
	do
	{
		if (mysql_stmt_execute(myStmt))
		{
			uRet=0;
			break;
		}
		//获得受影响的行数 
		uRet=(unsigned)mysql_stmt_affected_rows(myStmt);
	}while(0);
    return uRet;
}
BOOL CNetMySql::ClosePrepare()
{
	BOOL bRet=TRUE;
	//关闭句柄
	if (mysql_stmt_close(myStmt))
	{
       bRet=FALSE;
	}
	return bRet;
}

//改变用户
BOOL CNetMySql::ChangeUser(char * name,char *pwd,char *db)
{
	BOOL bRet=TRUE;
	//如果失败，返回FALSE
	if(mysql_change_user(&myData, name, pwd, db) )
	{
		bRet=FALSE;
	}
	return bRet;
}
//选择数据库
BOOL CNetMySql::SelectDB(char * db)
{
	BOOL bRet=TRUE;
	if(mysql_select_db(&myData, db))
	{
		bRet=FALSE;
	}
	return bRet;
}


//关闭
void CNetMySql::Close()
{
   //清空结果集
    FreeResult();
	//关闭连接
	mysql_close(&myData);
}


//获取查询的值
int CNetMySql::GetIntValue(int row, char * fd_name)
{
	PROWS rows=NULL;
	int len=0;
	int iRet=0;
	char * tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			//获得数据的字符串表示
			tmp=(char *)rows->fields[i].filed_value;
			//转换成数字
			iRet=atoi(tmp);
			break;
		}
	}
	return iRet;
}
int CNetMySql::GetIntValue(int row, int fd_num)
{
    PROWS rows=NULL;
	int len=0,iRet=0;
	char * tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

    //获得数字的字符串表示
	tmp=(char *)rows->fields[fd_num].filed_value;
	//转换成数字
	iRet=atoi(tmp);
	return iRet;
}

//获得符点型值
float CNetMySql::GetFloatValue(int row,char * fd_name)
{
	PROWS rows=NULL;
	int len=0;
	char * tmp=NULL;
	float iRet=0.0;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			tmp=(char *)rows->fields[i].filed_value;
			iRet=(float)atof(tmp);
			break;
		}
	}
	return iRet;
}
float CNetMySql::GetFloatValue(int row,int fd_num)
{
	PROWS rows=NULL;
	int len=0;
	float iRet=0.0;
	char * tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

    tmp=(char *)rows->fields[fd_num].filed_value;
    iRet=(float)atof(tmp);
	return iRet;
}

//获得高精度符点型值
double CNetMySql::GetDoubleValue(int row,char * fd_name)
{
	PROWS rows=NULL;
	int len=0;
	char * tmp=NULL;
	double iRet=0.0;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			tmp=(char *)rows->fields[i].filed_value;
		    iRet=atof(tmp);
			break;
		}
	}
	return iRet;
}
double CNetMySql::GetDoubleValue(int row,int fd_num)
{
	PROWS rows=NULL;
	int len=0;
	char * tmp=NULL;
	double iRet=0.0;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

	tmp=(char *)rows->fields[fd_num].filed_value;
	iRet=atof(tmp);
	return iRet;
}

//获得布尔型值
BOOL CNetMySql::GetBoolValue(int row,char * fd_name)
{
   	PROWS rows=NULL;
	int len=0;
	char * tmp=NULL;
	BOOL iRet=0;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			tmp=(char *)rows->fields[i].filed_value;
			iRet=(BOOL)atoi(tmp);
			break;
		}
	}
	return iRet;
}
BOOL CNetMySql::GetBoolValue(int row,int fd_num)
{
	PROWS rows=NULL;
	int len=0;
	BOOL iRet=0;
	char * tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

	tmp=(char *)rows->fields[fd_num].filed_value;
	iRet=(BOOL)atoi(tmp);
	return iRet;
}

char * CNetMySql::GetStringValue(int row,char * fd_name,unsigned * length)
{
	PROWS rows=NULL;
	int len=0;
	char *p_Tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			*length=(unsigned)(rows->fields[i].value_len);
			p_Tmp=(char *)(rows->fields[i].filed_value);
			break;
		}
	}
	return p_Tmp;
}
char * CNetMySql::GetStringValue(int row,int fd_num, unsigned * length)
{
	PROWS rows=NULL;
	int len=0;
	char * p_Tmp=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

	//获得列长度
	*length=(unsigned)rows->fields[fd_num].value_len;
	//直接返回此列的值
	p_Tmp=(char *)(rows->fields[fd_num].filed_value);
	return p_Tmp;
}

//获得日期时间型数据
char * CNetMySql::GetDataTimeValue(int row,char * fd_name)
{
	PROWS rows=NULL;
	int len=0;
	char * pTime=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return NULL;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
			pTime=(char *)(rows->fields[i].filed_value);
			break;
		}
	}
	return pTime;
}
char * CNetMySql::GetDataTimeValue(int row, int fd_num)
{
   	PROWS rows=NULL;
	int len=0;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return NULL;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);

	//直接返回此列的值
	return (char *)(rows->fields[fd_num].filed_value);
}

//获得二进制数据的缓冲区
int CNetMySql::GetBinaryValue(int row,char * fd_name,char * pBuf)
{
	PROWS rows=NULL;
	int len=0;
	char * p=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return 0;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
	//查找列
	for(int i=0;i<rows->cnt;i++)
	{
		if(!strcmp(rows->fields[i].field_name,fd_name))
		{
		    len=(int)rows->fields[i].value_len;
			if(pBuf==NULL)
				break;
			p=(char *)(rows->fields[i].filed_value);
			memcpy(pBuf,p,len);
			break;
		}
	}
	return len;
}
int CNetMySql::GetBinaryValue(int row,int fd_num,char * pBuf)
{
	PROWS rows=NULL;
	int len=0;
	char * p=NULL;
	len=res.GetCount();
	//如果行号超过了范围，则返回0
	if(row>=len)return NULL;
	//rows=(PROWS)res[row];
	rows = (PROWS)res.GetAt(row);
    
	len=rows->fields[fd_num].value_len;

	if(pBuf==NULL)
		return len;
  
	//直接返回此列的值
	 p=(char *)(rows->fields[fd_num].filed_value);
	 memcpy(pBuf,p,len);
	 return len;
}
