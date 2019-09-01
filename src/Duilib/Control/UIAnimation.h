#ifndef __UIANIMATION_H__
#define __UIANIMATION_H__

#pragma once

namespace DuiLib {

	class CControlUI;

	class UILIB_API IUIAnimation
	{
	public:
		virtual ~IUIAnimation() { NULL; }

		virtual CControlUI* OwnerControlUI() = 0;
		virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false) = 0;
		virtual void StopAnimation(int nAnimationID = 0) = 0;
		virtual bool IsAnimationRunning(int nAnimationID) = 0;
		virtual int GetCurrentFrame(int nAnimationID = 0) = 0;
		virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0) = 0;

		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) = 0;
		virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) = 0;
		virtual void OnAnimationStop(int nAnimationID) = 0;

		virtual void OnAnimationElapse(int nAnimationID) = 0;
	};

	class UILIB_API CAnimationData
	{
	public:
		CAnimationData(int nElipse, int nFrame, int nID, bool bLoop)
		{
			m_bFirstLoop = true;
			m_nCurFrame = 0;
			m_nElapse = nElipse;
			m_nTotalFrame = nFrame;
			m_bLoop = bLoop;
			m_nAnimationID = nID;
		}

	//protected:
	public:
		friend class CDUIAnimation;

		int m_nAnimationID;
		int m_nElapse;

		int m_nTotalFrame;
		int m_nCurFrame;

		bool m_bLoop;
		bool m_bFirstLoop;
	};

	class UILIB_API CUIAnimation: public IUIAnimation
	{
	public:
		CUIAnimation();
		virtual ~CUIAnimation();
		virtual CControlUI* OwnerControlUI() = 0;
		virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false);
		virtual void StopAnimation(int nAnimationID = 0);
		virtual bool IsAnimationRunning(int nAnimationID);
		virtual int GetCurrentFrame(int nAnimationID = 0);
		virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0);

		virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) {};
		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) {};
		virtual void OnAnimationStop(int nAnimationID) {};

		virtual void OnAnimationElapse(int nAnimationID);

	protected:
		CAnimationData* GetAnimationDataByID(int nAnimationID, int& index);

	protected:
		CStdPtrArray m_arAnimations;

	};

} // namespace DuiLib

#endif // __UIANIMATION_H__