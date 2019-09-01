#include "stdafx.h"
#include "UIAnimation.h"
#include <algorithm>

namespace DuiLib {

	CUIAnimation::CUIAnimation()
	{
	}

	CUIAnimation::~CUIAnimation()
	{
	}

	bool CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, 
									  int nAnimationID /*= 0*/, bool bLoop/* = false*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL != pData 
			|| nElapse <= 0
			|| nTotalFrame <= 0
			|| NULL == OwnerControlUI())
		{
			ASSERT(false);
			return false;
		}

		CAnimationData* pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
		if( NULL == pAnimation ) return false;
		
		if(OwnerControlUI()->GetManager()->SetTimer( OwnerControlUI(), 
			nAnimationID, nElapse ))
		{
			m_arAnimations.Add(pAnimation);
			return true;
		}
		else
		{
			delete pAnimation;
		}
		return false;
	}

	void CUIAnimation::StopAnimation(int nAnimationID /*= 0*/)
	{
		if(OwnerControlUI() == NULL) return;

		if(nAnimationID  != 0)
		{
			int index = -1;
			CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
			if( NULL != pData )
			{
				OwnerControlUI()->GetManager()->KillTimer(
					OwnerControlUI(), nAnimationID );
				m_arAnimations.Remove(index);
				delete pData;
				pData = NULL;
				return;
			}
		}
		else
		{
			int nCount = m_arAnimations.GetSize();
			for(int i=0; i<nCount; ++i)
			{
				CAnimationData* pData = (CAnimationData*)m_arAnimations[i];
				OwnerControlUI()->GetManager()->KillTimer(OwnerControlUI(), 
													pData->m_nAnimationID );
				delete pData;
			}
			m_arAnimations.Empty();
		}
	}

	bool CUIAnimation::IsAnimationRunning(int nAnimationID)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		return NULL != pData;
	}

	int CUIAnimation::GetCurrentFrame(int nAnimationID/* = 0*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData )
		{
			ASSERT(false);
			return -1;
		}
		return pData->m_nCurFrame;
	}

	bool CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID/* = 0*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData)
		{
			ASSERT(false);
			return false;
		}

		if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
		{
			pData->m_nCurFrame = nFrame;
			return true;
		}
		else
		{
			ASSERT(false);
		}
		return false;
	}

	void CUIAnimation::OnAnimationElapse(int nAnimationID)
	{
		if(OwnerControlUI() == NULL) return;

		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData ) return;

		int nCurFrame = pData->m_nCurFrame;
		if(nCurFrame == 0)
		{
			OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
			pData->m_bFirstLoop = false;
		}

		OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);

		if(nCurFrame >= pData->m_nTotalFrame)
		{
			OnAnimationStop(nAnimationID);
			if(pData->m_bLoop)
			{
				pData->m_nCurFrame = 0;
			}
			else
			{
				OwnerControlUI()->GetManager()->KillTimer(
					OwnerControlUI(), nAnimationID );
				m_arAnimations.Remove(index);
				delete pData;
				pData = NULL;
			}
		}

		if( NULL != pData )
		{
			++(pData->m_nCurFrame);
		}
	}

	CAnimationData* CUIAnimation::GetAnimationDataByID(int nAnimationID, int& index)
	{
		CAnimationData* pRet = NULL;
		index = -1;
		int nCount = m_arAnimations.GetSize();
		for(int i=0; i<nCount; ++i)
		{
			if(((CAnimationData*)m_arAnimations[i])->m_nAnimationID == nAnimationID)
			{
				pRet = (CAnimationData*)m_arAnimations[i];
				index = i;
				break;
			}
		}

		return pRet;
	}

} // namespace DuiLib