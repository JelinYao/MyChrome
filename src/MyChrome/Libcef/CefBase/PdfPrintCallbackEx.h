#pragma once
#include "include/cef_browser.h"
#include "CGlobalMgr.h"


class CPdfPrintCallbackEx
	: CefPdfPrintCallback
	, CefRunFileDialogCallback
{
public:
	CPdfPrintCallbackEx(CefRefPtr<CefBrowser> browser)
		: m_browser(browser)
	{
		RunDialog();
	}

	void RunDialog() 
	{
		static const char kDefaultFileName[] = "output.pdf";
		std::string path(kDefaultFileName);
		std::vector<CefString> accept_filters;
		accept_filters.push_back(".pdf");

		// Results in a call to OnFileDialogDismissed.
		m_browser->GetHost()->RunFileDialog(
			static_cast<cef_file_dialog_mode_t>(
			FILE_DIALOG_SAVE | FILE_DIALOG_OVERWRITEPROMPT_FLAG),
			CefString(),  // title
			path,
			accept_filters,
			0,  // selected_accept_filter
			this);
	}

	void OnFileDialogDismissed(int selected_accept_filter, const std::vector<CefString>& file_paths) OVERRIDE
	{
		if (!file_paths.empty()) 
		{
			CefPdfPrintSettings settings;
			// Show the URL in the footer.
			settings.header_footer_enabled = true;
			CefString(&settings.header_footer_url) =m_browser->GetMainFrame()->GetURL();
			// Print to the selected PDF file.
			m_browser->GetHost()->PrintToPDF(file_paths[0], settings, this);
		}
	}

	void OnPdfPrintFinished(const CefString& path, bool ok) OVERRIDE
	{
// 		Alert(browser_, "File \"" + path.ToString() + "\" " +
// 		(ok ? "saved successfully." : "failed to save."));
		std::wstring strText(L"网页保存为PDF：");
		strText.append(path.ToString16());
		strText.append(ok ? L"成功。":L"失败。");
		CGlobalMgr::Instance()->SetStateText(strText.c_str());
	}

private:
	CefRefPtr<CefBrowser> m_browser;
	IMPLEMENT_REFCOUNTING(CPdfPrintCallbackEx);
};