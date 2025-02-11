/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 2.1.1
 * Author	: Bruce Liang
 * Porject	: https://code.google.com/p/ldcsaa
 * Bolg		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
 * QQ Group	: http://qun.qq.com/#jointhegroup/gid/75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

class CSEM
{
public:
	CSEM(LONG lMaximumCount, LONG lInitialCount = 0, LPCTSTR lpName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)
	{
		//创建信号量
		//第一个参数表示安全控制，一般直接传入NULL。
		//第二个参数表示初始资源数量。
		//第三个参数表示最大并发数量。
		//第四个参数表示信号量的名称，传入NULL表示匿名信号量。
		m_hsem = ::CreateSemaphore(pSecurity, lInitialCount, lMaximumCount, lpName);
		ASSERT(IsValid());
	}

	~CSEM()
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hsem));
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hsem));

		m_hsem = ::OpenSemaphore(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	void Wait(DWORD dwMilliseconds = INFINITE)
	{
		::WaitForSingleObject(m_hsem, dwMilliseconds);
	}

	BOOL Release(LONG lReleaseCount = 1, LPLONG lpPreviousCount = nullptr)
	{
		return ::ReleaseSemaphore(m_hsem, lReleaseCount, lpPreviousCount);
	}

	HANDLE& GetHandle	() 	{return m_hsem;}
	operator HANDLE		()	{return m_hsem;}
	HANDLE* operator &	()	{return &m_hsem;}
	BOOL IsValid		()	{return m_hsem != nullptr;}

private:
	CSEM(const CSEM& sem);
	CSEM operator = (const CSEM& sem);
private:
	HANDLE m_hsem;
};